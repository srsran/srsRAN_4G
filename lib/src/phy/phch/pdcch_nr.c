/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/phy/phch/pdcch_nr.h"
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/fec/polar/polar_chanalloc.h"
#include "srsran/phy/fec/polar/polar_interleaver.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#define PDCCH_NR_POLAR_RM_IBIL 0

#define PDCCH_INFO_TX(...) INFO("PDCCH Tx: " __VA_ARGS__)
#define PDCCH_INFO_RX(...) INFO("PDCCH Rx: " __VA_ARGS__)
#define PDCCH_DEBUG_RX(...) DEBUG("PDCCH Rx: " __VA_ARGS__)

/**
 * @brief Recursive Y_p_n function
 */
static uint32_t srsran_pdcch_calculate_Y_p_n(uint32_t coreset_id, uint16_t rnti, uint32_t n)
{
  static const uint32_t A_p[3] = {39827, 39829, 39839};
  const uint32_t        D      = 65537;

  uint32_t Y_p_n = (uint32_t)rnti;
  for (uint32_t i = 0; i <= n; i++) {
    Y_p_n = (A_p[coreset_id % 3] * Y_p_n) % D;
  }

  return Y_p_n;
}

/**
 * Calculates the Control Channnel Element As described in 3GPP 38.213 R15 10.1 UE procedure for determining physical
 * downlink control channel assignment
 *
 */
static int srsran_pdcch_nr_get_ncce(const srsran_coreset_t*      coreset,
                                    const srsran_search_space_t* search_space,
                                    uint16_t                     rnti,
                                    uint32_t                     aggregation_level,
                                    uint32_t                     slot_idx,
                                    uint32_t                     candidate)
{
  if (aggregation_level >= SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR) {
    ERROR("Invalid aggregation level %d;", aggregation_level);
    return SRSRAN_ERROR;
  }

  uint32_t L    = 1U << aggregation_level;                         // Aggregation level
  uint32_t n_ci = 0;                                               //  Carrier indicator field
  uint32_t m    = candidate;                                       // Selected PDDCH candidate
  uint32_t M    = search_space->nof_candidates[aggregation_level]; // Number of aggregation levels

  if (M == 0) {
    ERROR("Invalid number of candidates %d for aggregation level %d", M, aggregation_level);
    return SRSRAN_ERROR;
  }

  // Calculate CORESET bandiwth in physical resource blocks
  uint32_t coreset_bw = srsran_coreset_get_bw(coreset);

  // Every REG is 1PRB wide and a CCE is 6 REG. So, the number of N_CCE is a sixth of the bandwidth times the number of
  // symbols
  uint32_t N_cce = coreset_bw * coreset->duration / 6;

  if (N_cce < L) {
    ERROR("Error CORESET (total bandwidth of %d RBs and %d CCEs) cannot fit the aggregation level %d (%d)",
          coreset_bw,
          N_cce,
          L,
          aggregation_level);
    return SRSRAN_ERROR;
  }

  // Calculate Y_p_n for UE search space only
  uint32_t Y_p_n = 0;
  if (search_space->type == srsran_search_space_type_ue) {
    Y_p_n = srsran_pdcch_calculate_Y_p_n(coreset->id, rnti, slot_idx);
  }

  return (int)(L * ((Y_p_n + (m * N_cce) / (L * M) + n_ci) % (N_cce / L)));
}

int srsran_pdcch_nr_locations_coreset(const srsran_coreset_t*      coreset,
                                      const srsran_search_space_t* search_space,
                                      uint16_t                     rnti,
                                      uint32_t                     aggregation_level,
                                      uint32_t                     slot_idx,
                                      uint32_t                     locations[SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR])
{
  if (coreset == NULL || search_space == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  uint32_t nof_candidates = search_space->nof_candidates[aggregation_level];

  nof_candidates = SRSRAN_MIN(nof_candidates, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR);

  for (uint32_t candidate = 0; candidate < nof_candidates; candidate++) {
    int ret = srsran_pdcch_nr_get_ncce(coreset, search_space, rnti, aggregation_level, slot_idx, candidate);
    if (ret < SRSRAN_SUCCESS) {
      return ret;
    }

    locations[candidate] = ret;
  }

  return nof_candidates;
}

int srsran_pdcch_nr_max_candidates_coreset(const srsran_coreset_t* coreset, uint32_t aggregation_level)
{
  if (coreset == NULL) {
    return SRSRAN_ERROR;
  }

  uint32_t coreset_bw = srsran_coreset_get_bw(coreset);
  uint32_t nof_cce    = (coreset_bw * coreset->duration) / 6;

  uint32_t L              = 1U << aggregation_level;
  uint32_t nof_candidates = nof_cce / L;

  return SRSRAN_MIN(nof_candidates, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR);
}

static int pdcch_nr_init_common(srsran_pdcch_nr_t* q, const srsran_pdcch_nr_args_t* args)
{
  if (q == NULL || args == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  q->meas_time_en = args->measure_time;

  q->c = srsran_vec_u8_malloc(SRSRAN_PDCCH_MAX_RE * 2);
  if (q->c == NULL) {
    return SRSRAN_ERROR;
  }

  q->d = srsran_vec_u8_malloc(SRSRAN_PDCCH_MAX_RE * 2);
  if (q->d == NULL) {
    return SRSRAN_ERROR;
  }

  q->f = srsran_vec_u8_malloc(SRSRAN_PDCCH_MAX_RE * 2);
  if (q->f == NULL) {
    return SRSRAN_ERROR;
  }

  q->symbols = srsran_vec_cf_malloc(SRSRAN_PDCCH_MAX_RE);
  if (q->symbols == NULL) {
    return SRSRAN_ERROR;
  }

  q->allocated = srsran_vec_u8_malloc(NMAX);
  if (q->allocated == NULL) {
    return SRSRAN_ERROR;
  }

  if (srsran_crc_init(&q->crc24c, SRSRAN_LTE_CRC24C, 24) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_polar_code_init(&q->code) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  srsran_modem_table_lte(&q->modem_table, SRSRAN_MOD_QPSK);
  if (args->measure_evm) {
    srsran_modem_table_bytes(&q->modem_table);
  }

  return SRSRAN_SUCCESS;
}

int srsran_pdcch_nr_init_tx(srsran_pdcch_nr_t* q, const srsran_pdcch_nr_args_t* args)
{
  if (pdcch_nr_init_common(q, args) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }
  q->is_tx = true;

  srsran_polar_encoder_type_t encoder_type = SRSRAN_POLAR_ENCODER_PIPELINED;

#ifdef LV_HAVE_AVX2
  if (!args->disable_simd) {
    encoder_type = SRSRAN_POLAR_ENCODER_AVX2;
  }
#endif // LV_HAVE_AVX2

  if (srsran_polar_encoder_init(&q->encoder, encoder_type, NMAX_LOG) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_polar_rm_tx_init(&q->rm) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_pdcch_nr_init_rx(srsran_pdcch_nr_t* q, const srsran_pdcch_nr_args_t* args)
{
  if (pdcch_nr_init_common(q, args) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  srsran_polar_decoder_type_t decoder_type = SRSRAN_POLAR_DECODER_SSC_C;

#ifdef LV_HAVE_AVX2
  if (!args->disable_simd) {
    decoder_type = SRSRAN_POLAR_DECODER_SSC_C_AVX2;
  }
#endif // LV_HAVE_AVX2

  if (srsran_polar_decoder_init(&q->decoder, decoder_type, NMAX_LOG) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_polar_rm_rx_init_c(&q->rm) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (args->measure_evm) {
    q->evm_buffer = srsran_evm_buffer_alloc(SRSRAN_PDCCH_MAX_RE * 2);
  }

  return SRSRAN_SUCCESS;
}

void srsran_pdcch_nr_free(srsran_pdcch_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  srsran_polar_code_free(&q->code);

  if (q->is_tx) {
    srsran_polar_encoder_free(&q->encoder);
    srsran_polar_rm_tx_free(&q->rm);
  } else {
    srsran_polar_decoder_free(&q->decoder);
    srsran_polar_rm_rx_free_c(&q->rm);
  }

  if (q->c) {
    free(q->c);
  }

  if (q->d) {
    free(q->d);
  }

  if (q->f) {
    free(q->f);
  }

  if (q->allocated) {
    free(q->allocated);
  }

  if (q->symbols) {
    free(q->symbols);
  }

  srsran_modem_table_free(&q->modem_table);

  if (q->evm_buffer) {
    srsran_evm_free(q->evm_buffer);
  }

  SRSRAN_MEM_ZERO(q, srsran_pdcch_nr_t, 1);
}

int srsran_pdcch_nr_set_carrier(srsran_pdcch_nr_t*         q,
                                const srsran_carrier_nr_t* carrier,
                                const srsran_coreset_t*    coreset)
{
  if (q == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (carrier != NULL) {
    q->carrier = *carrier;
  }

  if (coreset != NULL) {
    q->coreset = *coreset;
  }

  return SRSRAN_SUCCESS;
}

static int pdcch_nr_cce_to_reg_mapping_non_interleaved(const srsran_coreset_t*      coreset,
                                                       const srsran_dci_location_t* dci_location,
                                                       bool                         rb_mask[SRSRAN_MAX_PRB_NR])
{
  uint32_t nof_cce        = 1U << dci_location->L;
  uint32_t L              = 6;
  uint32_t nof_reg_bundle = 6 / L;

  // For each CCE j in the PDCCH transmission
  for (uint32_t j = dci_location->ncce; j < dci_location->ncce + nof_cce; j++) {
    // For each REG bundle i in the CCE j
    for (uint32_t reg_bundle = 0; reg_bundle < nof_reg_bundle; reg_bundle++) {
      // Calculate x variable
      uint32_t x = (6 * j) / L + reg_bundle;

      // For non interleaved f(x) = x
      uint32_t i = x;

      // For each REG in the REG bundle
      uint32_t rb_start = (i * L) / coreset->duration;
      uint32_t rb_end   = ((i + 1) * L) / coreset->duration;
      for (uint32_t rb = rb_start; rb < rb_end; rb++) {
        rb_mask[rb] = true;
      }
    }
  }

  return SRSRAN_SUCCESS;
}

static int pdcch_nr_cce_to_reg_mapping_interleaved(const srsran_coreset_t*      coreset,
                                                   const srsran_dci_location_t* dci_location,
                                                   bool                         rb_mask[SRSRAN_MAX_PRB_NR])
{
  // Calculate CORESET constants
  uint32_t N_CORESET_REG = coreset->duration * srsran_coreset_get_bw(coreset);
  uint32_t L             = pdcch_nr_bundle_size(coreset->reg_bundle_size);
  uint32_t R             = pdcch_nr_bundle_size(coreset->interleaver_size);
  uint32_t C             = N_CORESET_REG / (L * R);
  uint32_t n_shift       = coreset->shift_index;

  // Validate
  if (N_CORESET_REG == 0 || N_CORESET_REG % (L * R) != 0 || L % coreset->duration != 0) {
    ERROR("Invalid CORESET configuration N=%d; L=%d; R=%d;", N_CORESET_REG, L, R);
    return 0;
  }

  uint32_t nof_cce        = 1U << dci_location->L;
  uint32_t nof_reg_bundle = 6 / L;

  // For each CCE j in the PDCCH transmission
  for (uint32_t j = dci_location->ncce; j < dci_location->ncce + nof_cce; j++) {
    // For each REG bundle i in the CCE j
    for (uint32_t reg_bundle = 0; reg_bundle < nof_reg_bundle; reg_bundle++) {
      // Calculate x variable
      uint32_t x = (6 * j) / L + reg_bundle;

      // For non interleaved f(x) = x
      uint32_t r = x % R;
      uint32_t c = x / R;
      uint32_t i = (r * C + c + n_shift) % (N_CORESET_REG / L);

      // For each REG in the REG bundle
      uint32_t rb_start = (i * L) / coreset->duration;
      uint32_t rb_end   = ((i + 1) * L) / coreset->duration;
      for (uint32_t rb = rb_start; rb < rb_end; rb++) {
        rb_mask[rb] = true;
      }
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_pdcch_nr_cce_to_reg_mapping(const srsran_coreset_t*      coreset,
                                       const srsran_dci_location_t* dci_location,
                                       bool                         rb_mask[SRSRAN_MAX_PRB_NR])
{
  if (coreset == NULL || dci_location == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Non-interleaved case
  if (coreset->mapping_type == srsran_coreset_mapping_type_non_interleaved) {
    return pdcch_nr_cce_to_reg_mapping_non_interleaved(coreset, dci_location, rb_mask);
  }

  // Interleaved case
  return pdcch_nr_cce_to_reg_mapping_interleaved(coreset, dci_location, rb_mask);
}

static uint32_t pdcch_nr_cp(const srsran_pdcch_nr_t*     q,
                            const srsran_dci_location_t* dci_location,
                            cf_t*                        slot_grid,
                            cf_t*                        symbols,
                            bool                         put)
{
  uint32_t offset_k = q->coreset.offset_rb * SRSRAN_NRE;

  // Compute REG list
  bool rb_mask[SRSRAN_MAX_PRB_NR] = {};
  if (srsran_pdcch_nr_cce_to_reg_mapping(&q->coreset, dci_location, rb_mask) < SRSRAN_SUCCESS) {
    return 0;
  }

  uint32_t count = 0;

  // Iterate over symbols
  for (uint32_t l = 0; l < q->coreset.duration; l++) {
    // Iterate over frequency resource groups
    uint32_t rb = 0;
    for (uint32_t r = 0; r < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; r++) {
      // Skip frequency resource if not set
      if (!q->coreset.freq_resources[r]) {
        continue;
      }

      // For each RB in the frequency resource
      for (uint32_t i = r * 6; i < (r + 1) * 6; i++, rb++) {
        // Skip if this RB is not marked as mapped
        if (!rb_mask[rb]) {
          continue;
        }

        // For each RE in the RB
        for (uint32_t k = i * SRSRAN_NRE; k < (i + 1) * SRSRAN_NRE; k++) {
          // Skip if it is a DMRS
          if (k % 4 == 1) {
            continue;
          }

          // Read or write in the grid
          if (put) {
            slot_grid[q->carrier.nof_prb * SRSRAN_NRE * l + k + offset_k] = symbols[count++];
          } else {
            symbols[count++] = slot_grid[q->carrier.nof_prb * SRSRAN_NRE * l + k + offset_k];
          }
        }
      }
    }
  }

  return count;
}

static uint32_t pdcch_nr_c_init(const srsran_pdcch_nr_t* q, const srsran_dci_msg_nr_t* dci_msg)
{
  uint32_t n_id   = (dci_msg->ctx.ss_type == srsran_search_space_type_ue && q->coreset.dmrs_scrambling_id_present)
                        ? q->coreset.dmrs_scrambling_id
                        : q->carrier.pci;
  uint32_t n_rnti = (dci_msg->ctx.ss_type == srsran_search_space_type_ue && q->coreset.dmrs_scrambling_id_present)
                        ? dci_msg->ctx.rnti
                        : 0U;
  return ((n_rnti << 16U) + n_id) & 0x7fffffffU;
}

int srsran_pdcch_nr_encode(srsran_pdcch_nr_t* q, const srsran_dci_msg_nr_t* dci_msg, cf_t* slot_symbols)
{
  if (q == NULL || dci_msg == NULL || slot_symbols == NULL) {
    return SRSRAN_ERROR;
  }

  struct timeval t[3];
  if (q->meas_time_en) {
    gettimeofday(&t[1], NULL);
  }

  // Calculate...
  q->K           = dci_msg->nof_bits + 24U;                                  // Payload size including CRC
  q->M           = (1U << dci_msg->ctx.location.L) * (SRSRAN_NRE - 3U) * 6U; // Number of RE
  q->E           = q->M * 2;                                                 // Number of Rate-Matched bits
  uint32_t cinit = pdcch_nr_c_init(q, dci_msg);                              // Pseudo-random sequence initiation

  // Get polar code
  if (srsran_polar_code_get(&q->code, q->K, q->E, 9U) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }
  PDCCH_INFO_TX("K=%d; E=%d; M=%d; n=%d; cinit=%08x;", q->K, q->E, q->M, q->code.n, cinit);

  // Set first L bits to ones, c will have an offset of 24 bits
  uint8_t* c = q->c;
  srsran_bit_unpack(UINT32_MAX, &c, 24U);

  // Copy DCI message
  srsran_vec_u8_copy(c, dci_msg->payload, dci_msg->nof_bits);

  // Append CRC
  srsran_crc_attach(&q->crc24c, q->c, q->K);

  PDCCH_INFO_TX("Append CRC %06x", (uint32_t)srsran_crc_checksum_get(&q->crc24c));

  // Unpack RNTI
  uint8_t  unpacked_rnti[16] = {};
  uint8_t* ptr               = unpacked_rnti;
  srsran_bit_unpack(dci_msg->ctx.rnti, &ptr, 16);

  // Scramble CRC with RNTI
  srsran_vec_xor_bbb(unpacked_rnti, &c[q->K - 16], &c[q->K - 16], 16);

  // Interleave
  uint8_t c_prime[SRSRAN_POLAR_INTERLEAVER_K_MAX_IL];
  srsran_polar_interleaver_run_u8(c, c_prime, q->K, true);

  // Print c and c_prime (after interleaving)
  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    PDCCH_INFO_TX("c=");
    srsran_vec_fprint_hex(stdout, c, q->K);
    PDCCH_INFO_TX("c_prime=");
    srsran_vec_fprint_hex(stdout, c_prime, q->K);
  }

  // Allocate channel
  srsran_polar_chanalloc_tx(c_prime, q->allocated, q->code.N, q->code.K, q->code.nPC, q->code.K_set, q->code.PC_set);

  // Encode bits
  if (srsran_polar_encoder_encode(&q->encoder, q->allocated, q->d, q->code.n) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Print d
  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    PDCCH_INFO_TX("d=");
    srsran_vec_fprint_byte(stdout, q->d, q->code.N);
  }

  // Rate matching
  srsran_polar_rm_tx(&q->rm, q->d, q->f, q->code.n, q->E, q->K, PDCCH_NR_POLAR_RM_IBIL);

  // Scrambling
  srsran_sequence_apply_bit(q->f, q->f, q->E, cinit);

  // Modulation
  srsran_mod_modulate(&q->modem_table, q->f, q->symbols, q->E);

  // Put symbols in grid
  uint32_t m = pdcch_nr_cp(q, &dci_msg->ctx.location, slot_symbols, q->symbols, true);
  if (q->M != m) {
    ERROR("Unmatch number of RE (%d != %d)", m, q->M);
    return SRSRAN_ERROR;
  }

  if (q->meas_time_en) {
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    q->meas_time_us = (uint32_t)t[0].tv_usec;
  }

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    char str[128] = {};
    srsran_pdcch_nr_info(q, NULL, str, sizeof(str));
    PDCCH_INFO_TX("%s", str);
  }

  return SRSRAN_SUCCESS;
}

int srsran_pdcch_nr_decode(srsran_pdcch_nr_t*      q,
                           cf_t*                   slot_symbols,
                           srsran_dmrs_pdcch_ce_t* ce,
                           srsran_dci_msg_nr_t*    dci_msg,
                           srsran_pdcch_nr_res_t*  res)
{
  if (q == NULL || dci_msg == NULL || ce == NULL || slot_symbols == NULL || res == NULL) {
    return SRSRAN_ERROR;
  }

  struct timeval t[3];
  if (q->meas_time_en) {
    gettimeofday(&t[1], NULL);
  }

  // Calculate...
  q->K = dci_msg->nof_bits + 24U;                                  // Payload size including CRC
  q->M = (1U << dci_msg->ctx.location.L) * (SRSRAN_NRE - 3U) * 6U; // Number of RE
  q->E = q->M * 2;                                                 // Number of Rate-Matched bits

  // Check number of estimates is correct
  if (ce->nof_re != q->M) {
    ERROR("Invalid number of channel estimates (%d != %d)", q->M, ce->nof_re);
    return SRSRAN_ERROR;
  }

  // Get polar code
  if (srsran_polar_code_get(&q->code, q->K, q->E, 9U) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }
  PDCCH_INFO_RX("K=%d; E=%d; M=%d; n=%d;", q->K, q->E, q->M, q->code.n);

  // Get symbols from grid
  uint32_t m = pdcch_nr_cp(q, &dci_msg->ctx.location, slot_symbols, q->symbols, false);
  if (q->M != m) {
    ERROR("Unmatch number of RE (%d != %d)", m, q->M);
    return SRSRAN_ERROR;
  }

  // Print channel estimates if enabled
  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
    PDCCH_DEBUG_RX("ce=");
    srsran_vec_fprint_c(stdout, ce->ce, q->M);
  }

  // Equalise
  srsran_predecoding_single(q->symbols, ce->ce, q->symbols, NULL, q->M, 1.0f, ce->noise_var);

  // Print symbols if enabled
  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
    PDCCH_DEBUG_RX("symbols=");
    srsran_vec_fprint_c(stdout, q->symbols, q->M);
  }

  // Demodulation
  int8_t* llr = (int8_t*)q->f;
  srsran_demod_soft_demodulate_b(SRSRAN_MOD_QPSK, q->symbols, llr, q->M);

  // Measure EVM if configured
  if (q->evm_buffer != NULL) {
    res->evm = srsran_evm_run_b(q->evm_buffer, &q->modem_table, q->symbols, llr, q->E);
  } else {
    res->evm = NAN;
  }

  // Negate all LLR
  for (uint32_t i = 0; i < q->E; i++) {
    llr[i] *= -1;
  }

  // Descrambling
  srsran_sequence_apply_c(llr, llr, q->E, pdcch_nr_c_init(q, dci_msg));

  // Un-rate matching
  int8_t* d = (int8_t*)q->d;
  if (srsran_polar_rm_rx_c(&q->rm, llr, d, q->E, q->code.n, q->K, PDCCH_NR_POLAR_RM_IBIL) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Print d
  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
    PDCCH_DEBUG_RX("d=");
    srsran_vec_fprint_bs(stdout, d, q->K);
  }

  // Decode
  if (srsran_polar_decoder_decode_c(&q->decoder, d, q->allocated, q->code.n, q->code.F_set, q->code.F_set_size) <
      SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // De-allocate channel
  uint8_t c_prime[SRSRAN_POLAR_INTERLEAVER_K_MAX_IL];
  srsran_polar_chanalloc_rx(q->allocated, c_prime, q->code.K, q->code.nPC, q->code.K_set, q->code.PC_set);

  // Set first L bits to ones, c will have an offset of 24 bits
  uint8_t* c = q->c;
  srsran_bit_unpack(UINT32_MAX, &c, 24U);

  // De-interleave
  srsran_polar_interleaver_run_u8(c_prime, c, q->K, false);

  // Print c
  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    PDCCH_INFO_RX("c_prime=");
    srsran_vec_fprint_hex(stdout, c_prime, q->K);
    PDCCH_INFO_RX("c=");
    srsran_vec_fprint_hex(stdout, c, q->K);
  }

  // Unpack RNTI
  uint8_t  unpacked_rnti[16] = {};
  uint8_t* ptr               = unpacked_rnti;
  srsran_bit_unpack(dci_msg->ctx.rnti, &ptr, 16);

  // De-Scramble CRC with RNTI
  srsran_vec_xor_bbb(unpacked_rnti, &c[q->K - 16], &c[q->K - 16], 16);

  // Check CRC
  ptr                = &c[q->K - 24];
  uint32_t checksum1 = srsran_crc_checksum(&q->crc24c, q->c, q->K);
  uint32_t checksum2 = srsran_bit_pack(&ptr, 24);
  res->crc           = checksum1 == checksum2;

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    PDCCH_INFO_RX("CRC={%06x, %06x}; msg=", checksum1, checksum2);
    srsran_vec_fprint_hex(stdout, c, dci_msg->nof_bits);
  }

  // Copy DCI message
  srsran_vec_u8_copy(dci_msg->payload, c, dci_msg->nof_bits);

  if (q->meas_time_en) {
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    q->meas_time_us = (uint32_t)t[0].tv_usec;
  }

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    char str[128] = {};
    srsran_pdcch_nr_info(q, res, str, sizeof(str));
    PDCCH_INFO_RX("%s", str);
  }

  return SRSRAN_SUCCESS;
}

uint32_t srsran_pdcch_nr_info(const srsran_pdcch_nr_t* q, const srsran_pdcch_nr_res_t* res, char* str, uint32_t str_len)
{
  int len = 0;

  if (q == NULL) {
    return len;
  }

  len = srsran_print_check(str, str_len, len, "K=%d,E=%d", q->K, q->E);

  if (res != NULL) {
    len = srsran_print_check(str, str_len, len, ",crc=%s", res->crc ? "OK" : "KO");

    if (q->evm_buffer && res) {
      len = srsran_print_check(str, str_len, len, ",evm=%.2f", res->evm);
    }
  }

  if (q->meas_time_en) {
    len = srsran_print_check(str, str_len, len, ",t=%d us", q->meas_time_us);
  }

  return len;
}