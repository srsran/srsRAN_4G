/**
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/phy/phch/pdcch_nr.h"
#include "srslte/phy/fec/polar/polar_chanalloc.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define PDCCH_NR_POLAR_RM_IBIL 0

#define PDCCH_INFO_TX(...) INFO("PDCCH Tx: " __VA_ARGS__)
#define PDCCH_INFO_RX(...) INFO("PDCCH Rx: " __VA_ARGS__)

/**
 * @brief Recursive Y_p_n function
 */
static uint32_t srslte_pdcch_calculate_Y_p_n(uint32_t coreset_id, uint16_t rnti, int n)
{
  static const uint32_t A_p[3] = {39827, 39829, 39839};
  const uint32_t        D      = 65537;

  if (n < 0) {
    return rnti;
  }

  return (A_p[coreset_id % 3] * srslte_pdcch_calculate_Y_p_n(coreset_id, rnti, n - 1)) % D;
}

/**
 * Calculates the Control Channnel Element As described in 3GPP 38.213 R15 10.1 UE procedure for determining physical
 * downlink control channel assignment
 *
 */
static int srslte_pdcch_nr_get_ncce(const srslte_coreset_t*      coreset,
                                    const srslte_search_space_t* search_space,
                                    uint16_t                     rnti,
                                    uint32_t                     aggregation_level,
                                    uint32_t                     slot_idx,
                                    uint32_t                     candidate)
{
  if (aggregation_level >= SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR) {
    ERROR("Invalid aggregation level %d;\n", aggregation_level);
    return SRSLTE_ERROR;
  }

  uint32_t L    = 1U << aggregation_level;                         // Aggregation level
  uint32_t n_ci = 0;                                               //  Carrier indicator field
  uint32_t m    = candidate;                                       // Selected PDDCH candidate
  uint32_t M    = search_space->nof_candidates[aggregation_level]; // Number of aggregation levels

  if (M == 0) {
    ERROR("Invalid number of candidates %d for aggregation level %d\n", M, aggregation_level);
    return SRSLTE_ERROR;
  }

  // Every REG is 1PRB wide and a CCE is 6 REG. So, the number of N_CCE is a sixth of the bandwidth times the number of
  // symbols
  uint32_t N_cce = srslte_coreset_get_bw(coreset) * coreset->duration / 6;

  if (N_cce < L) {
    ERROR("Error number of CCE %d is lower than the aggregation level %d\n", N_cce, L);
    return SRSLTE_ERROR;
  }

  // Calculate Y_p_n for UE search space only
  uint32_t Y_p_n = 0;
  if (search_space->type == srslte_search_space_type_ue) {
    Y_p_n = srslte_pdcch_calculate_Y_p_n(coreset->id, rnti, slot_idx);
  }

  return (int)(L * ((Y_p_n + (m * N_cce) / (L * M) + n_ci) % (N_cce / L)));
}

int srslte_pdcch_nr_locations_coreset(const srslte_coreset_t*      coreset,
                                      const srslte_search_space_t* search_space,
                                      uint16_t                     rnti,
                                      uint32_t                     aggregation_level,
                                      uint32_t                     slot_idx,
                                      uint32_t                     locations[SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR])
{
  if (coreset == NULL || search_space == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  uint32_t nof_candidates = search_space->nof_candidates[aggregation_level];

  nof_candidates = SRSLTE_MIN(nof_candidates, SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR);

  for (uint32_t candidate = 0; candidate < nof_candidates; candidate++) {
    int ret = srslte_pdcch_nr_get_ncce(coreset, search_space, rnti, aggregation_level, slot_idx, candidate);
    if (ret < SRSLTE_SUCCESS) {
      return ret;
    }

    locations[candidate] = ret;
  }

  return nof_candidates;
}

int srslte_pdcch_nr_max_candidates_coreset(const srslte_coreset_t* coreset, uint32_t aggregation_level)
{
  if (coreset == NULL) {
    return SRSLTE_ERROR;
  }

  uint32_t coreset_bw = srslte_coreset_get_bw(coreset);
  uint32_t nof_cce    = (coreset_bw * coreset->duration) / 6;

  uint32_t L              = 1U << aggregation_level;
  uint32_t nof_candidates = nof_cce / L;

  return SRSLTE_MIN(nof_candidates, SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR);
}

static int pdcch_nr_init_common(srslte_pdcch_nr_t* q, const srslte_pdcch_nr_args_t* args)
{
  if (q == NULL || args == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  q->meas_time_en = args->measure_time;

  q->c = srslte_vec_u8_malloc(SRSLTE_PDCCH_MAX_RE * 2);
  if (q->c == NULL) {
    return SRSLTE_ERROR;
  }

  q->d = srslte_vec_u8_malloc(SRSLTE_PDCCH_MAX_RE * 2);
  if (q->d == NULL) {
    return SRSLTE_ERROR;
  }

  q->f = srslte_vec_u8_malloc(SRSLTE_PDCCH_MAX_RE * 2);
  if (q->f == NULL) {
    return SRSLTE_ERROR;
  }

  q->symbols = srslte_vec_cf_malloc(SRSLTE_PDCCH_MAX_RE);
  if (q->symbols == NULL) {
    return SRSLTE_ERROR;
  }

  q->allocated = srslte_vec_u8_malloc(NMAX);
  if (q->allocated == NULL) {
    return SRSLTE_ERROR;
  }

  if (srslte_crc_init(&q->crc24c, SRSLTE_LTE_CRC24C, 24) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_polar_code_init(&q->code) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  srslte_modem_table_lte(&q->modem_table, SRSLTE_MOD_QPSK);
  if (args->measure_evm) {
    srslte_modem_table_bytes(&q->modem_table);
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdcch_nr_init_tx(srslte_pdcch_nr_t* q, const srslte_pdcch_nr_args_t* args)
{
  if (pdcch_nr_init_common(q, args) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }
  q->is_tx = true;

  srslte_polar_encoder_type_t encoder_type =
      (args->disable_simd) ? SRSLTE_POLAR_ENCODER_PIPELINED : SRSLTE_POLAR_ENCODER_AVX2;

  if (srslte_polar_encoder_init(&q->encoder, encoder_type, NMAX_LOG) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_polar_rm_tx_init(&q->rm) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdcch_nr_init_rx(srslte_pdcch_nr_t* q, const srslte_pdcch_nr_args_t* args)
{
  if (pdcch_nr_init_common(q, args) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  srslte_polar_decoder_type_t decoder_type =
      (args->disable_simd) ? SRSLTE_POLAR_DECODER_SSC_C : SRSLTE_POLAR_DECODER_SSC_C_AVX2;

  if (srslte_polar_decoder_init(&q->decoder, decoder_type, NMAX_LOG) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_polar_rm_rx_init_c(&q->rm) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (args->measure_evm) {
    q->evm_buffer = srslte_evm_buffer_alloc(SRSLTE_PDCCH_MAX_RE * 2);
  }

  return SRSLTE_SUCCESS;
}

void srslte_pdcch_nr_free(srslte_pdcch_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  srslte_polar_code_free(&q->code);

  if (q->is_tx) {
    srslte_polar_encoder_free(&q->encoder);
    srslte_polar_rm_tx_free(&q->rm);
  } else {
    srslte_polar_decoder_free(&q->decoder);
    srslte_polar_rm_rx_free_c(&q->rm);
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

  srslte_modem_table_free(&q->modem_table);

  if (q->evm_buffer) {
    srslte_evm_free(q->evm_buffer);
  }

  SRSLTE_MEM_ZERO(q, srslte_pdcch_nr_t, 1);
}

int srslte_pdcch_nr_set_carrier(srslte_pdcch_nr_t*         q,
                                const srslte_carrier_nr_t* carrier,
                                const srslte_coreset_t*    coreset)
{
  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (carrier != NULL) {
    q->carrier = *carrier;
  }

  if (coreset != NULL) {
    q->coreset = *coreset;
  }

  return SRSLTE_SUCCESS;
}

static uint32_t pdcch_nr_cp(const srslte_pdcch_nr_t*     q,
                            const srslte_dci_location_t* dci_location,
                            cf_t*                        slot_grid,
                            cf_t*                        symbols,
                            bool                         put)
{
  uint32_t L = 1U << dci_location->L;

  // Calculate begin and end sub-carrier index for the selected candidate
  uint32_t k_begin = (dci_location->ncce * SRSLTE_NRE * 6) / q->coreset.duration;
  uint32_t k_end   = k_begin + (L * 6 * SRSLTE_NRE) / q->coreset.duration;

  uint32_t count = 0;

  // Iterate over symbols
  for (uint32_t l = 0; l < q->coreset.duration; l++) {
    // Iterate over frequency resource groups
    uint32_t k = 0;
    for (uint32_t r = 0; r < SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE; r++) {
      if (q->coreset.freq_resources[r]) {
        for (uint32_t i = r * 6 * SRSLTE_NRE; i < (r + 1) * 6 * SRSLTE_NRE; i++, k++) {
          if (k >= k_begin && k < k_end && k % 4 != 1) {
            if (put) {
              slot_grid[q->carrier.nof_prb * SRSLTE_NRE * l + i] = symbols[count++];
            } else {
              symbols[count++] = slot_grid[q->carrier.nof_prb * SRSLTE_NRE * l + i];
            }
          }
        }
      }
    }
  }

  return count;
}

uint32_t pdcch_nr_c_init(const srslte_pdcch_nr_t* q, const srslte_dci_msg_nr_t* dci_msg)
{
  uint32_t n_id = (dci_msg->search_space == srslte_search_space_type_ue && q->coreset.dmrs_scrambling_id_present)
                      ? q->coreset.dmrs_scrambling_id
                      : q->carrier.id;
  uint32_t n_rnti = (dci_msg->search_space == srslte_search_space_type_ue && q->coreset.dmrs_scrambling_id_present)
                        ? dci_msg->rnti
                        : 0U;
  return ((n_rnti << 16U) + n_id) & 0x7fffffffU;
}

int srslte_pdcch_nr_encode(srslte_pdcch_nr_t* q, const srslte_dci_msg_nr_t* dci_msg, cf_t* slot_symbols)
{

  if (q == NULL || dci_msg == NULL || slot_symbols == NULL) {
    return SRSLTE_ERROR;
  }

  struct timeval t[3];
  if (q->meas_time_en) {
    gettimeofday(&t[1], NULL);
  }

  // Calculate...
  q->K = dci_msg->nof_bits + 24U;                              // Payload size including CRC
  q->M = (1U << dci_msg->location.L) * (SRSLTE_NRE - 3U) * 6U; // Number of RE
  q->E = q->M * 2;                                             // Number of Rate-Matched bits

  // Get polar code
  if (srslte_polar_code_get(&q->code, q->K, q->E, 9U) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }
  PDCCH_INFO_TX("K=%d; E=%d; M=%d; n=%d;\n", q->K, q->E, q->M, q->code.n);

  // Copy DCI message
  srslte_vec_u8_copy(q->c, dci_msg->payload, dci_msg->nof_bits);

  // Append CRC
  srslte_crc_attach(&q->crc24c, q->c, dci_msg->nof_bits);

  PDCCH_INFO_TX("Append CRC %06x\n", (uint32_t)srslte_crc_checksum_get(&q->crc24c));

  // Unpack RNTI
  uint8_t  unpacked_rnti[16] = {};
  uint8_t* ptr               = unpacked_rnti;
  srslte_bit_unpack(dci_msg->rnti, &ptr, 16);

  // Scramble CRC with RNTI
  srslte_vec_xor_bbb(unpacked_rnti, &q->c[q->K - 16], &q->c[q->K - 16], 16);

  // Print c
  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    PDCCH_INFO_TX("c=");
    srslte_vec_fprint_hex(stdout, q->c, q->K);
  }

  // Allocate channel
  srslte_polar_chanalloc_tx(q->c, q->allocated, q->code.N, q->code.K, q->code.nPC, q->code.K_set, q->code.PC_set);

  // Encode bits
  if (srslte_polar_encoder_encode(&q->encoder, q->allocated, q->d, q->code.n) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Print d
  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    PDCCH_INFO_TX("d=");
    srslte_vec_fprint_byte(stdout, q->d, q->K);
  }

  // Rate matching
  srslte_polar_rm_tx(&q->rm, q->d, q->f, q->code.n, q->E, q->K, PDCCH_NR_POLAR_RM_IBIL);

  // Scrambling
  srslte_sequence_apply_bit(q->f, q->f, q->E, pdcch_nr_c_init(q, dci_msg));

  // Modulation
  srslte_mod_modulate(&q->modem_table, q->f, q->symbols, q->E);

  // Put symbols in grid
  uint32_t m = pdcch_nr_cp(q, &dci_msg->location, slot_symbols, q->symbols, true);
  if (q->M != m) {
    ERROR("Unmatch number of RE (%d != %d)\n", m, q->M);
    return SRSLTE_ERROR;
  }

  if (q->meas_time_en) {
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    q->meas_time_us = (uint32_t)t[0].tv_usec;
  }

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    char str[128] = {};
    srslte_pdcch_nr_info(q, NULL, str, sizeof(str));
    PDCCH_INFO_TX("%s\n", str);
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdcch_nr_decode(srslte_pdcch_nr_t*      q,
                           cf_t*                   slot_symbols,
                           srslte_dmrs_pdcch_ce_t* ce,
                           srslte_dci_msg_nr_t*    dci_msg,
                           srslte_pdcch_nr_res_t*  res)
{
  if (q == NULL || dci_msg == NULL || ce == NULL || slot_symbols == NULL || res == NULL) {
    return SRSLTE_ERROR;
  }

  struct timeval t[3];
  if (q->meas_time_en) {
    gettimeofday(&t[1], NULL);
  }

  // Calculate...
  q->K = dci_msg->nof_bits + 24U;                              // Payload size including CRC
  q->M = (1U << dci_msg->location.L) * (SRSLTE_NRE - 3U) * 6U; // Number of RE
  q->E = q->M * 2;                                             // Number of Rate-Matched bits

  // Check number of estimates is correct
  if (ce->nof_re != q->M) {
    ERROR("Invalid number of channel estimates (%d != %d)\n", q->M, ce->nof_re);
    return SRSLTE_ERROR;
  }

  // Get polar code
  if (srslte_polar_code_get(&q->code, q->K, q->E, 9U) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }
  PDCCH_INFO_RX("K=%d; E=%d; M=%d; n=%d;\n", q->K, q->E, q->M, q->code.n);

  // Get symbols from grid
  uint32_t m = pdcch_nr_cp(q, &dci_msg->location, slot_symbols, q->symbols, false);
  if (q->M != m) {
    ERROR("Unmatch number of RE (%d != %d)\n", m, q->M);
    return SRSLTE_ERROR;
  }

  // Print channel estimates if enabled
  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    PDCCH_INFO_RX("ce=");
    srslte_vec_fprint_c(stdout, ce->ce, q->M);
  }

  // Equalise
  srslte_predecoding_single(q->symbols, ce->ce, q->symbols, NULL, q->M, 1.0f, ce->noise_var);

  // Print symbols if enabled
  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    PDCCH_INFO_RX("symbols=");
    srslte_vec_fprint_c(stdout, q->symbols, q->M);
  }

  // Demodulation
  int8_t* llr = (int8_t*)q->f;
  srslte_demod_soft_demodulate_b(SRSLTE_MOD_QPSK, q->symbols, llr, q->M);

  // Measure EVM if configured
  if (q->evm_buffer != NULL) {
    res->evm = srslte_evm_run_b(q->evm_buffer, &q->modem_table, q->symbols, llr, q->E);
  } else {
    res->evm = NAN;
  }

  // Negate all LLR
  for (uint32_t i = 0; i < q->E; i++) {
    llr[i] *= -1;
  }

  // Descrambling
  srslte_sequence_apply_c(llr, llr, q->E, pdcch_nr_c_init(q, dci_msg));

  // Un-rate matching
  int8_t* d = (int8_t*)q->d;
  if (srslte_polar_rm_rx_c(&q->rm, llr, d, q->E, q->code.n, q->K, PDCCH_NR_POLAR_RM_IBIL) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Print d
  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    PDCCH_INFO_RX("d=");
    srslte_vec_fprint_bs(stdout, d, q->K);
  }

  // Decode
  if (srslte_polar_decoder_decode_c(&q->decoder, d, q->allocated, q->code.n, q->code.F_set, q->code.F_set_size) <
      SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // De-allocate channel
  srslte_polar_chanalloc_rx(q->allocated, q->c, q->code.K, q->code.nPC, q->code.K_set, q->code.PC_set);

  // Print c
  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    PDCCH_INFO_RX("c=");
    srslte_vec_fprint_hex(stdout, q->c, q->K);
  }

  // Unpack RNTI
  uint8_t  unpacked_rnti[16] = {};
  uint8_t* ptr               = unpacked_rnti;
  srslte_bit_unpack(dci_msg->rnti, &ptr, 16);

  // De-Scramble CRC with RNTI
  ptr = &q->c[q->K - 24];
  srslte_vec_xor_bbb(unpacked_rnti, &q->c[q->K - 16], &q->c[q->K - 16], 16);

  // Check CRC
  uint32_t checksum1 = srslte_crc_checksum(&q->crc24c, q->c, dci_msg->nof_bits);
  uint32_t checksum2 = srslte_bit_pack(&ptr, 24);
  res->crc           = checksum1 == checksum2;

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    PDCCH_INFO_RX("CRC={%06x, %06x}; msg=", checksum1, checksum2);
    srslte_vec_fprint_hex(stdout, q->c, dci_msg->nof_bits);
  }

  // Copy DCI message
  srslte_vec_u8_copy(dci_msg->payload, q->c, dci_msg->nof_bits);

  if (q->meas_time_en) {
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    q->meas_time_us = (uint32_t)t[0].tv_usec;
  }

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    char str[128] = {};
    srslte_pdcch_nr_info(q, res, str, sizeof(str));
    PDCCH_INFO_RX("%s\n", str);
  }

  return SRSLTE_SUCCESS;
}

uint32_t srslte_pdcch_nr_info(const srslte_pdcch_nr_t* q, const srslte_pdcch_nr_res_t* res, char* str, uint32_t str_len)
{
  int len = 0;

  if (q == NULL) {
    return len;
  }

  len = srslte_print_check(str, str_len, len, "K=%d,E=%d", q->K, q->E);

  if (res != NULL) {
    len = srslte_print_check(str, str_len, len, ",crc=%s", res->crc ? "OK" : "KO");

    if (q->evm_buffer && res) {
      len = srslte_print_check(str, str_len, len, ",evm=%.2f", res->evm);
    }
  }

  if (q->meas_time_en) {
    len = srslte_print_check(str, str_len, len, ",t=%d us", q->meas_time_us);
  }

  return len;
}