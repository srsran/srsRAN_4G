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

#include "srsran/phy/phch/uci_nr.h"
#include "srsran/phy/fec/block/block.h"
#include "srsran/phy/fec/polar/polar_chanalloc.h"
#include "srsran/phy/phch/csi.h"
#include "srsran/phy/phch/uci_cfg.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/vector.h"

#define UCI_NR_INFO_TX(...) INFO("UCI-NR Tx: " __VA_ARGS__)
#define UCI_NR_INFO_RX(...) INFO("UCI-NR Rx: " __VA_ARGS__)

#define UCI_NR_MAX_L 11U
#define UCI_NR_POLAR_MAX 2048U
#define UCI_NR_POLAR_RM_IBIL 1
#define UCI_NR_PUCCH_POLAR_N_MAX 10
#define UCI_NR_BLOCK_DEFAULT_CORR_THRESHOLD 0.5f
#define UCI_NR_ONE_BIT_CORR_THRESHOLD 0.5f

uint32_t srsran_uci_nr_crc_len(uint32_t A)
{
  return (A <= 11) ? 0 : (A < 20) ? 6 : 11;
}

static inline int uci_nr_pusch_cfg_valid(const srsran_uci_nr_pusch_cfg_t* cfg)
{
  // No data pointer
  if (cfg == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Unset configuration is unset
  if (cfg->nof_re == 0 && cfg->nof_layers == 0 && !isnormal(cfg->R)) {
    return SRSRAN_SUCCESS;
  }

  // Detect invalid number of layers
  if (cfg->nof_layers == 0) {
    ERROR("Invalid number of layers %d", cfg->nof_layers);
    return SRSRAN_ERROR;
  }

  // Detect invalid number of RE
  if (cfg->nof_re == 0) {
    ERROR("Invalid number of RE %d", cfg->nof_re);
    return SRSRAN_ERROR;
  }

  // Detect invalid Rate
  if (!isnormal(cfg->R)) {
    ERROR("Invalid R %f", cfg->R);
    return SRSRAN_ERROR;
  }

  // Otherwise it is set and valid
  return 1;
}

int srsran_uci_nr_init(srsran_uci_nr_t* q, const srsran_uci_nr_args_t* args)
{
  if (q == NULL || args == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  srsran_polar_encoder_type_t polar_encoder_type = SRSRAN_POLAR_ENCODER_PIPELINED;
  srsran_polar_decoder_type_t polar_decoder_type = SRSRAN_POLAR_DECODER_SSC_C;
#ifdef LV_HAVE_AVX2
  if (!args->disable_simd) {
    polar_encoder_type = SRSRAN_POLAR_ENCODER_AVX2;
    polar_decoder_type = SRSRAN_POLAR_DECODER_SSC_C_AVX2;
  }
#endif // LV_HAVE_AVX2

  if (srsran_polar_code_init(&q->code)) {
    ERROR("Initialising polar code");
    return SRSRAN_ERROR;
  }

  if (srsran_polar_encoder_init(&q->encoder, polar_encoder_type, NMAX_LOG) < SRSRAN_SUCCESS) {
    ERROR("Initialising polar encoder");
    return SRSRAN_ERROR;
  }

  if (srsran_polar_decoder_init(&q->decoder, polar_decoder_type, NMAX_LOG) < SRSRAN_SUCCESS) {
    ERROR("Initialising polar encoder");
    return SRSRAN_ERROR;
  }

  if (srsran_polar_rm_tx_init(&q->rm_tx) < SRSRAN_SUCCESS) {
    ERROR("Initialising polar RM");
    return SRSRAN_ERROR;
  }

  if (srsran_polar_rm_rx_init_c(&q->rm_rx) < SRSRAN_SUCCESS) {
    ERROR("Initialising polar RM");
    return SRSRAN_ERROR;
  }

  if (srsran_crc_init(&q->crc6, SRSRAN_LTE_CRC6, 6) < SRSRAN_SUCCESS) {
    ERROR("Initialising CRC");
    return SRSRAN_ERROR;
  }

  if (srsran_crc_init(&q->crc11, SRSRAN_LTE_CRC11, 11) < SRSRAN_SUCCESS) {
    ERROR("Initialising CRC");
    return SRSRAN_ERROR;
  }

  // Allocate bit sequence with space for the CRC
  q->bit_sequence = srsran_vec_u8_malloc(SRSRAN_UCI_NR_MAX_NOF_BITS);
  if (q->bit_sequence == NULL) {
    ERROR("Error malloc");
    return SRSRAN_ERROR;
  }

  // Allocate c with space for a and the CRC
  q->c = srsran_vec_u8_malloc(SRSRAN_UCI_NR_MAX_NOF_BITS + UCI_NR_MAX_L);
  if (q->c == NULL) {
    ERROR("Error malloc");
    return SRSRAN_ERROR;
  }

  q->allocated = srsran_vec_u8_malloc(UCI_NR_POLAR_MAX);
  if (q->allocated == NULL) {
    ERROR("Error malloc");
    return SRSRAN_ERROR;
  }

  q->d = srsran_vec_u8_malloc(UCI_NR_POLAR_MAX);
  if (q->d == NULL) {
    ERROR("Error malloc");
    return SRSRAN_ERROR;
  }

  if (isnormal(args->block_code_threshold)) {
    q->block_code_threshold = args->block_code_threshold;
  } else {
    q->block_code_threshold = UCI_NR_BLOCK_DEFAULT_CORR_THRESHOLD;
  }
  if (isnormal(args->one_bit_threshold)) {
    q->one_bit_threshold = args->one_bit_threshold;
  } else {
    q->one_bit_threshold = UCI_NR_ONE_BIT_CORR_THRESHOLD;
  }

  return SRSRAN_SUCCESS;
}

void srsran_uci_nr_free(srsran_uci_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  srsran_polar_code_free(&q->code);
  srsran_polar_encoder_free(&q->encoder);
  srsran_polar_decoder_free(&q->decoder);
  srsran_polar_rm_tx_free(&q->rm_tx);
  srsran_polar_rm_rx_free_c(&q->rm_rx);

  if (q->bit_sequence != NULL) {
    free(q->bit_sequence);
  }
  if (q->c != NULL) {
    free(q->c);
  }
  if (q->allocated != NULL) {
    free(q->allocated);
  }
  if (q->d != NULL) {
    free(q->d);
  }

  SRSRAN_MEM_ZERO(q, srsran_uci_nr_t, 1);
}

static int uci_nr_pack_ack_sr(const srsran_uci_cfg_nr_t* cfg, const srsran_uci_value_nr_t* value, uint8_t* sequence)
{
  int A = 0;

  // Append ACK bits
  srsran_vec_u8_copy(&sequence[A], value->ack, cfg->ack.count);
  A += cfg->ack.count;

  // Append SR bits
  uint8_t* bits = &sequence[A];
  srsran_bit_unpack(value->sr, &bits, cfg->o_sr);
  A += cfg->o_sr;

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    UCI_NR_INFO_TX("Packed UCI bits: ");
    srsran_vec_fprint_byte(stdout, sequence, A);
  }

  return A;
}

static int uci_nr_unpack_ack_sr(const srsran_uci_cfg_nr_t* cfg, uint8_t* sequence, srsran_uci_value_nr_t* value)
{
  int A = 0;

  // Append ACK bits
  srsran_vec_u8_copy(value->ack, &sequence[A], cfg->ack.count);
  A += cfg->ack.count;

  // Append SR bits
  uint8_t* bits = &sequence[A];
  value->sr     = srsran_bit_pack(&bits, cfg->o_sr);
  A += cfg->o_sr;

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    UCI_NR_INFO_RX("Unpacked UCI bits: ");
    srsran_vec_fprint_byte(stdout, sequence, A);
  }

  return A;
}

static int uci_nr_pack_ack_sr_csi(const srsran_uci_cfg_nr_t* cfg, const srsran_uci_value_nr_t* value, uint8_t* sequence)
{
  int A = 0;

  // Append ACK bits
  srsran_vec_u8_copy(&sequence[A], value->ack, cfg->ack.count);
  A += cfg->ack.count;

  // Append SR bits
  uint8_t* bits = &sequence[A];
  srsran_bit_unpack(value->sr, &bits, cfg->o_sr);
  A += cfg->o_sr;

  // Append CSI bits
  int n = srsran_csi_part1_pack(cfg->csi, value->csi, cfg->nof_csi, bits, SRSRAN_UCI_NR_MAX_NOF_BITS - A);
  if (n < SRSRAN_SUCCESS) {
    ERROR("Packing CSI part 1");
    return SRSRAN_ERROR;
  }
  A += n;

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    UCI_NR_INFO_TX("Packed UCI bits: ");
    srsran_vec_fprint_byte(stdout, sequence, A);
  }

  return A;
}

static int uci_nr_unpack_ack_sr_csi(const srsran_uci_cfg_nr_t* cfg, uint8_t* sequence, srsran_uci_value_nr_t* value)
{
  int A = 0;

  // Append ACK bits
  srsran_vec_u8_copy(value->ack, &sequence[A], cfg->ack.count);
  A += cfg->ack.count;

  // Append SR bits
  uint8_t* bits = &sequence[A];
  value->sr     = srsran_bit_pack(&bits, cfg->o_sr);
  A += cfg->o_sr;

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    UCI_NR_INFO_RX("Unpacked UCI bits: ");
    srsran_vec_fprint_byte(stdout, sequence, A);
  }

  // Append CSI bits
  int n = srsran_csi_part1_unpack(cfg->csi, cfg->nof_csi, bits, SRSRAN_UCI_NR_MAX_NOF_BITS - A, value->csi);
  if (n < SRSRAN_SUCCESS) {
    ERROR("Packing CSI part 1");
    return SRSRAN_ERROR;
  }

  return A;
}

static int uci_nr_A(const srsran_uci_cfg_nr_t* cfg)
{
  int o_csi = srsran_csi_part1_nof_bits(cfg->csi, cfg->nof_csi);

  // 6.3.1.1.1 HARQ-ACK/SR only UCI bit sequence generation
  if (o_csi == 0) {
    return cfg->ack.count + cfg->o_sr;
  }

  // 6.3.1.1.2 CSI only
  if (cfg->ack.count == 0 && cfg->o_sr == 0) {
    return o_csi;
  }

  // 6.3.1.1.3 HARQ-ACK/SR and CSI
  return cfg->ack.count + cfg->o_sr + o_csi;
}

static int uci_nr_pack_pucch(const srsran_uci_cfg_nr_t* cfg, const srsran_uci_value_nr_t* value, uint8_t* sequence)
{
  int o_csi = srsran_csi_part1_nof_bits(cfg->csi, cfg->nof_csi);

  // 6.3.1.1.1 HARQ-ACK/SR only UCI bit sequence generation
  if (o_csi == 0) {
    return uci_nr_pack_ack_sr(cfg, value, sequence);
  }

  // 6.3.1.1.2 CSI only
  if (cfg->ack.count == 0 && cfg->o_sr == 0) {
    return srsran_csi_part1_pack(cfg->csi, value->csi, cfg->nof_csi, sequence, SRSRAN_UCI_NR_MAX_NOF_BITS);
  }

  // 6.3.1.1.3 HARQ-ACK/SR and CSI
  return uci_nr_pack_ack_sr_csi(cfg, value, sequence);
}

static int uci_nr_unpack_pucch(const srsran_uci_cfg_nr_t* cfg, uint8_t* sequence, srsran_uci_value_nr_t* value)
{
  int o_csi = srsran_csi_part1_nof_bits(cfg->csi, cfg->nof_csi);

  // 6.3.1.1.1 HARQ-ACK/SR only UCI bit sequence generation
  if (o_csi == 0) {
    return uci_nr_unpack_ack_sr(cfg, sequence, value);
  }

  // 6.3.1.1.2 CSI only
  if (cfg->ack.count == 0 && cfg->o_sr == 0) {
    return srsran_csi_part1_unpack(cfg->csi, cfg->nof_csi, sequence, SRSRAN_UCI_NR_MAX_NOF_BITS, value->csi);
  }

  // 6.3.1.1.3 HARQ-ACK/SR and CSI
  return uci_nr_unpack_ack_sr_csi(cfg, sequence, value);
}

static int uci_nr_encode_1bit(srsran_uci_nr_t* q, const srsran_uci_cfg_nr_t* cfg, uint8_t* o, uint32_t E)
{
  uint32_t              i  = 0;
  srsran_uci_bit_type_t c0 = (q->bit_sequence[0] == 0) ? UCI_BIT_0 : UCI_BIT_1;

  switch (cfg->pusch.modulation) {
    case SRSRAN_MOD_BPSK:
      while (i < E) {
        o[i++] = c0;
      }
      break;
    case SRSRAN_MOD_QPSK:
      while (i < E) {
        o[i++] = c0;
        o[i++] = (uint8_t)UCI_BIT_REPETITION;
      }
      break;
    case SRSRAN_MOD_16QAM:
      while (i < E) {
        o[i++] = c0;
        o[i++] = (uint8_t)UCI_BIT_REPETITION;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
      }
      break;
    case SRSRAN_MOD_64QAM:
      while (i < E) {
        while (i < E) {
          o[i++] = c0;
          o[i++] = (uint8_t)UCI_BIT_REPETITION;
          o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
          o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
          o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
          o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        }
      }
      break;
    case SRSRAN_MOD_256QAM:
      while (i < E) {
        o[i++] = c0;
        o[i++] = (uint8_t)UCI_BIT_REPETITION;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
      }
      break;
    case SRSRAN_MOD_NITEMS:
    default:
      ERROR("Invalid modulation");
      return SRSRAN_ERROR;
  }

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    UCI_NR_INFO_TX("One bit encoded NR-UCI; o=");
    srsran_vec_fprint_b(stdout, o, E);
  }

  return E;
}

static int uci_nr_decode_1_bit(srsran_uci_nr_t*           q,
                               const srsran_uci_cfg_nr_t* cfg,
                               uint32_t                   A,
                               const int8_t*              llr,
                               uint32_t                   E,
                               bool*                      decoded_ok)
{
  uint32_t Qm = srsran_mod_bits_x_symbol(cfg->pusch.modulation);
  if (Qm == 0) {
    ERROR("Invalid modulation (%s)", srsran_mod_string(cfg->pusch.modulation));
    return SRSRAN_ERROR;
  }

  // Correlate LLR
  float    corr  = 0.0f;
  float    pwr   = 0.0f;
  uint32_t count = 0;
  for (uint32_t i = 0; i < E; i += Qm) {
    float t = (float)llr[i];
    corr += t;
    pwr += t * t;
    count++;
  }

  // Normalise correlation
  float norm_corr = fabsf(corr) / sqrtf(pwr * count);

  // Take decoded decision with threshold
  *decoded_ok = (norm_corr > q->one_bit_threshold);

  // Save decoded bit
  q->bit_sequence[0] = (corr < 0) ? 0 : 1;

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    UCI_NR_INFO_RX("One bit decoding NR-UCI llr=");
    srsran_vec_fprint_bs(stdout, llr, E);
    UCI_NR_INFO_RX("One bit decoding NR-UCI A=%d; E=%d; pwr=%f; corr=%f; norm=%f; thr=%f; %s",
                   A,
                   E,
                   pwr,
                   corr,
                   norm_corr,
                   q->block_code_threshold,
                   *decoded_ok ? "OK" : "KO");
  }

  return SRSRAN_SUCCESS;
}

static int uci_nr_encode_2bit(srsran_uci_nr_t* q, const srsran_uci_cfg_nr_t* cfg, uint8_t* o, uint32_t E)
{
  uint32_t i  = 0;
  uint8_t  c0 = (uint8_t)((q->bit_sequence[0] == 0) ? UCI_BIT_0 : UCI_BIT_1);
  uint8_t  c1 = (uint8_t)((q->bit_sequence[1] == 0) ? UCI_BIT_0 : UCI_BIT_1);
  uint8_t  c2 = (uint8_t)(((q->bit_sequence[0] ^ q->bit_sequence[1]) == 0) ? UCI_BIT_0 : UCI_BIT_1);

  switch (cfg->pusch.modulation) {
    case SRSRAN_MOD_BPSK:
    case SRSRAN_MOD_QPSK:
      while (i < E) {
        o[i++] = c0;
        o[i++] = c1;
        o[i++] = c2;
      }
      break;
    case SRSRAN_MOD_16QAM:
      while (i < E) {
        o[i++] = c0;
        o[i++] = c1;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = c2;
        o[i++] = c0;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = c1;
        o[i++] = c2;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
      }
      break;
    case SRSRAN_MOD_64QAM:
      while (i < E) {
        o[i++] = c0;
        o[i++] = c1;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = c2;
        o[i++] = c0;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = c1;
        o[i++] = c2;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
      }
      break;
    case SRSRAN_MOD_256QAM:

      while (i < E) {
        o[i++] = c0;
        o[i++] = c1;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = c2;
        o[i++] = c0;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = c1;
        o[i++] = c2;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
        o[i++] = (uint8_t)UCI_BIT_PLACEHOLDER;
      }
      break;
    case SRSRAN_MOD_NITEMS:
    default:
      ERROR("Invalid modulation");
      return SRSRAN_ERROR;
  }

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    UCI_NR_INFO_TX("Two bit encoded NR-UCI; E=%d; o=", E);
    srsran_vec_fprint_b(stdout, o, E);
  }

  return E;
}

static int uci_nr_decode_2_bit(srsran_uci_nr_t*           q,
                               const srsran_uci_cfg_nr_t* cfg,
                               uint32_t                   A,
                               const int8_t*              llr,
                               uint32_t                   E,
                               bool*                      decoded_ok)
{
  uint32_t Qm = srsran_mod_bits_x_symbol(cfg->pusch.modulation);
  if (Qm == 0) {
    ERROR("Invalid modulation (%s)", srsran_mod_string(cfg->pusch.modulation));
    return SRSRAN_ERROR;
  }

  // Correlate LLR
  float corr[3] = {};
  if (Qm == 1) {
    for (uint32_t i = 0; i < E / Qm; i++) {
      corr[i % 3] = llr[i];
    }
  } else {
    for (uint32_t i = 0, j = 0; i < E; i += Qm) {
      corr[(j++) % 3] = llr[i + 0];
      corr[(j++) % 3] = llr[i + 1];
    }
  }

  // Take decoded decision
  bool c0 = corr[0] > 0.0f;
  bool c1 = corr[1] > 0.0f;
  bool c2 = corr[2] > 0.0f;

  // Check redundancy bit
  *decoded_ok = (c2 == (c0 ^ c1));

  // Save decoded bits
  q->bit_sequence[0] = c0 ? 1 : 0;
  q->bit_sequence[1] = c1 ? 1 : 0;

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    UCI_NR_INFO_RX("Two bit decoding NR-UCI llr=");
    srsran_vec_fprint_bs(stdout, llr, E);
    UCI_NR_INFO_RX("Two bit decoding NR-UCI A=%d; E=%d; Qm=%d; c0=%d; c1=%d; c2=%d %s",
                   A,
                   E,
                   Qm,
                   c0,
                   c1,
                   c2,
                   *decoded_ok ? "OK" : "KO");
  }

  return SRSRAN_SUCCESS;
}

static int
uci_nr_encode_3_11_bit(srsran_uci_nr_t* q, const srsran_uci_cfg_nr_t* cfg, uint32_t A, uint8_t* o, uint32_t E)
{
  srsran_block_encode(q->bit_sequence, A, o, E);

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    UCI_NR_INFO_TX("Block encoded UCI bits; o=");
    srsran_vec_fprint_b(stdout, o, E);
  }

  return E;
}

static int uci_nr_decode_3_11_bit(srsran_uci_nr_t*           q,
                                  const srsran_uci_cfg_nr_t* cfg,
                                  uint32_t                   A,
                                  const int8_t*              llr,
                                  uint32_t                   E,
                                  bool*                      decoded_ok)
{
  // Check E for avoiding zero division
  if (E < 1) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (A == 11 && E <= 16) {
    ERROR("NR-UCI Impossible to decode A=%d; E=%d", A, E);
    return SRSRAN_ERROR;
  }

  // Compute average LLR power
  float pwr = srsran_vec_avg_power_bf(llr, E);

  // If the power measurement is invalid (zero, NAN, INF) then consider it cannot be decoded
  if (!isnormal(pwr)) {
    *decoded_ok = false;
    return SRSRAN_SUCCESS;
  }

  // Decode
  float corr = (float)srsran_block_decode_i8(llr, E, q->bit_sequence, A);

  // Normalise correlation
  float norm_corr = corr / (sqrtf(pwr) * E);

  // Take decoded decision with threshold
  *decoded_ok = (corr > q->block_code_threshold);

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    UCI_NR_INFO_RX("Block decoding NR-UCI llr=");
    srsran_vec_fprint_bs(stdout, llr, E);
    UCI_NR_INFO_RX("Block decoding NR-UCI A=%d; E=%d; pwr=%f; corr=%f; norm=%f; thr=%f; %s",
                   A,
                   E,
                   pwr,
                   corr,
                   norm_corr,
                   q->block_code_threshold,
                   *decoded_ok ? "OK" : "KO");
  }

  return SRSRAN_SUCCESS;
}

static int
uci_nr_encode_11_1706_bit(srsran_uci_nr_t* q, const srsran_uci_cfg_nr_t* cfg, uint32_t A, uint8_t* o, uint32_t E_uci)
{
  // If ( A ≥ 360 and E ≥ 1088 ) or if A ≥ 1013 , I seg = 1 ; otherwise I seg = 0
  uint32_t I_seg = 0;
  if ((A >= 360 && E_uci >= 1088) || A >= 1013) {
    I_seg = 1;
  }

  // Select CRC
  uint32_t      L   = srsran_uci_nr_crc_len(A);
  srsran_crc_t* crc = (L == 6) ? &q->crc6 : &q->crc11;

  // Segmentation
  uint32_t C = 1;
  if (I_seg == 1) {
    C = 2;
  }
  uint32_t A_prime = SRSRAN_CEIL(A, C) * C;

  // Get polar code
  uint32_t K_r = A_prime / C + L;
  uint32_t E_r = E_uci / C;
  if (srsran_polar_code_get(&q->code, K_r, E_r, UCI_NR_PUCCH_POLAR_N_MAX) < SRSRAN_SUCCESS) {
    ERROR("Error computing Polar code");
    return SRSRAN_ERROR;
  }

  // Write codeword
  for (uint32_t r = 0, s = 0; r < C; r++) {
    uint32_t k = 0;

    // Prefix (A_prime - A) zeros for the first CB only
    if (r == 0) {
      for (uint32_t i = 0; i < (A_prime - A); i++) {
        q->c[k++] = 0;
      }
    }

    // Load codeword bits
    while (k < A_prime / C) {
      q->c[k++] = q->bit_sequence[s++];
    }

    // Attach CRC
    srsran_crc_attach(crc, q->c, A_prime / C);
    UCI_NR_INFO_TX("Attaching %d/%d CRC%d=%" PRIx64, r, C, L, srsran_crc_checksum_get(crc));

    if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
      UCI_NR_INFO_TX("Polar cb %d/%d c=", r, C);
      srsran_vec_fprint_byte(stdout, q->c, K_r);
    }

    // Allocate channel
    srsran_polar_chanalloc_tx(q->c, q->allocated, q->code.N, q->code.K, q->code.nPC, q->code.K_set, q->code.PC_set);

    if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
      UCI_NR_INFO_TX("Polar alloc %d/%d ", r, C);
      srsran_vec_fprint_byte(stdout, q->allocated, q->code.N);
    }

    // Encode bits
    if (srsran_polar_encoder_encode(&q->encoder, q->allocated, q->d, q->code.n) < SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }

    if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
      UCI_NR_INFO_TX("Polar encoded %d/%d ", r, C);
      srsran_vec_fprint_byte(stdout, q->d, q->code.N);
    }

    // Rate matching
    srsran_polar_rm_tx(&q->rm_tx, q->d, &o[E_r * r], q->code.n, E_r, K_r, UCI_NR_POLAR_RM_IBIL);

    if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
      UCI_NR_INFO_TX("Polar RM cw %d/%d ", r, C);
      srsran_vec_fprint_byte(stdout, &o[E_r * r], E_r);
    }
  }

  return E_uci;
}

static int uci_nr_decode_11_1706_bit(srsran_uci_nr_t*           q,
                                     const srsran_uci_cfg_nr_t* cfg,
                                     uint32_t                   A,
                                     int8_t*                    llr,
                                     uint32_t                   E_uci,
                                     bool*                      decoded_ok)
{
  *decoded_ok = true;

  // If ( A ≥ 360 and E ≥ 1088 ) or if A ≥ 1013 , I seg = 1 ; otherwise I seg = 0
  uint32_t I_seg = 0;
  if ((A >= 360 && E_uci >= 1088) || A >= 1013) {
    I_seg = 1;
  }

  // Select CRC
  uint32_t      L   = srsran_uci_nr_crc_len(A);
  srsran_crc_t* crc = (L == 6) ? &q->crc6 : &q->crc11;

  // Segmentation
  uint32_t C = 1;
  if (I_seg == 1) {
    C = 2;
  }
  uint32_t A_prime = SRSRAN_CEIL(A, C) * C;

  // Get polar code
  uint32_t K_r = A_prime / C + L;
  uint32_t E_r = E_uci / C;
  if (srsran_polar_code_get(&q->code, K_r, E_r, UCI_NR_PUCCH_POLAR_N_MAX) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Negate all LLR
  for (uint32_t i = 0; i < E_r; i++) {
    llr[i] *= -1;
  }

  // Write codeword
  for (uint32_t r = 0, s = 0; r < C; r++) {
    uint32_t k = 0;

    if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
      UCI_NR_INFO_RX("Polar LLR %d/%d ", r, C);
      srsran_vec_fprint_bs(stdout, &llr[E_r * r], q->code.N);
    }

    // Undo rate matching
    int8_t* d = (int8_t*)q->d;
    srsran_polar_rm_rx_c(&q->rm_rx, &llr[E_r * r], d, E_r, q->code.n, K_r, UCI_NR_POLAR_RM_IBIL);

    // Decode bits
    if (srsran_polar_decoder_decode_c(&q->decoder, d, q->allocated, q->code.n, q->code.F_set, q->code.F_set_size) <
        SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }

    if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
      UCI_NR_INFO_RX("Polar alloc %d/%d ", r, C);
      srsran_vec_fprint_byte(stdout, q->allocated, q->code.N);
    }

    // Undo channel allocation
    srsran_polar_chanalloc_rx(q->allocated, q->c, q->code.K, q->code.nPC, q->code.K_set, q->code.PC_set);

    if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
      UCI_NR_INFO_RX("Polar cb %d/%d c=", r, C);
      srsran_vec_fprint_byte(stdout, q->c, K_r);
    }

    // Calculate checksum
    uint8_t* ptr       = &q->c[A_prime / C];
    uint32_t checksum1 = srsran_crc_checksum(crc, q->c, A_prime / C);
    uint32_t checksum2 = srsran_bit_pack(&ptr, L);
    (*decoded_ok)      = ((*decoded_ok) && (checksum1 == checksum2));
    UCI_NR_INFO_RX("Checking %d/%d CRC%d={%02x,%02x}", r, C, L, checksum1, checksum2);

    // Prefix (A_prime - A) zeros for the first CB only
    if (r == 0) {
      for (uint32_t i = 0; i < (A_prime - A); i++) {
        k++;
      }
    }

    // Load codeword bits
    while (k < A_prime / C) {
      q->bit_sequence[s++] = q->c[k++];
    }
  }

  return SRSRAN_SUCCESS;
}

static int uci_nr_encode(srsran_uci_nr_t* q, const srsran_uci_cfg_nr_t* uci_cfg, uint32_t A, uint8_t* o, uint32_t E_uci)
{
  // 5.3.3.1 Encoding of 1-bit information
  if (A == 1) {
    return uci_nr_encode_1bit(q, uci_cfg, o, E_uci);
  }

  // 5.3.3.2 Encoding of 2-bit information
  if (A == 2) {
    return uci_nr_encode_2bit(q, uci_cfg, o, E_uci);
  }

  // 5.3.3.3 Encoding of other small block lengths
  if (A <= SRSRAN_FEC_BLOCK_MAX_NOF_BITS) {
    return uci_nr_encode_3_11_bit(q, uci_cfg, A, o, E_uci);
  }

  // Encoding of other sizes up to 1906
  if (A < SRSRAN_UCI_NR_MAX_NOF_BITS) {
    return uci_nr_encode_11_1706_bit(q, uci_cfg, A, o, E_uci);
  }

  return SRSRAN_ERROR;
}

static int uci_nr_decode(srsran_uci_nr_t*           q,
                         const srsran_uci_cfg_nr_t* uci_cfg,
                         int8_t*                    llr,
                         uint32_t                   A,
                         uint32_t                   E_uci,
                         bool*                      valid)
{
  if (q == NULL || uci_cfg == NULL || valid == NULL || llr == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Decode LLR
  if (A == 1) {
    if (uci_nr_decode_1_bit(q, uci_cfg, A, llr, E_uci, valid) < SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  } else if (A == 2) {
    if (uci_nr_decode_2_bit(q, uci_cfg, A, llr, E_uci, valid) < SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  } else if (A <= 11) {
    if (uci_nr_decode_3_11_bit(q, uci_cfg, A, llr, E_uci, valid) < SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  } else if (A < SRSRAN_UCI_NR_MAX_NOF_BITS) {
    if (uci_nr_decode_11_1706_bit(q, uci_cfg, A, llr, E_uci, valid) < SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  } else {
    ERROR("Invalid number of bits (A=%d)", A);
  }

  return SRSRAN_SUCCESS;
}

int srsran_uci_nr_pucch_format_2_3_4_E(const srsran_pucch_nr_resource_t* resource)
{
  if (resource == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  switch (resource->format) {
    case SRSRAN_PUCCH_NR_FORMAT_2:
      return (int)(16 * resource->nof_symbols * resource->nof_prb);
    case SRSRAN_PUCCH_NR_FORMAT_3:
      if (!resource->enable_pi_bpsk) {
        return (int)(24 * resource->nof_symbols * resource->nof_prb);
      }
      return (int)(12 * resource->nof_symbols * resource->nof_prb);
    case SRSRAN_PUCCH_NR_FORMAT_4:
      if (resource->occ_lenth != 1 && resource->occ_lenth != 2) {
        ERROR("Invalid spreading factor (%d)", resource->occ_lenth);
        return SRSRAN_ERROR;
      }
      if (!resource->enable_pi_bpsk) {
        return (int)(24 * resource->nof_symbols / resource->occ_lenth);
      }
      return (int)(12 * resource->nof_symbols / resource->occ_lenth);
    default:
      ERROR("Invalid case");
  }
  return SRSRAN_ERROR;
}

// Implements TS 38.212 Table 6.3.1.4.1-1: Rate matching output sequence length E UCI
static int
uci_nr_pucch_E_uci(const srsran_pucch_nr_resource_t* pucch_cfg, const srsran_uci_cfg_nr_t* uci_cfg, uint32_t E_tot)
{
  //  if (uci_cfg->o_csi1 != 0 && uci_cfg->o_csi2) {
  //    ERROR("Simultaneous CSI part 1 and CSI part 2 is not implemented");
  //    return SRSRAN_ERROR;
  //  }

  return E_tot;
}

int srsran_uci_nr_encode_pucch(srsran_uci_nr_t*                  q,
                               const srsran_pucch_nr_resource_t* pucch_resource_cfg,
                               const srsran_uci_cfg_nr_t*        uci_cfg,
                               const srsran_uci_value_nr_t*      value,
                               uint8_t*                          o)
{
  int E_tot = srsran_uci_nr_pucch_format_2_3_4_E(pucch_resource_cfg);
  if (E_tot < SRSRAN_SUCCESS) {
    ERROR("Error calculating number of bits");
    return SRSRAN_ERROR;
  }

  int E_uci = uci_nr_pucch_E_uci(pucch_resource_cfg, uci_cfg, E_tot);
  if (E_uci < SRSRAN_SUCCESS) {
    ERROR("Error calculating number of bits");
    return SRSRAN_ERROR;
  }

  // 6.3.1.1 UCI bit sequence generation
  int A = uci_nr_pack_pucch(uci_cfg, value, q->bit_sequence);
  if (A < SRSRAN_SUCCESS) {
    ERROR("Generating bit sequence");
    return SRSRAN_ERROR;
  }

  return uci_nr_encode(q, uci_cfg, A, o, E_uci);
}

int srsran_uci_nr_decode_pucch(srsran_uci_nr_t*                  q,
                               const srsran_pucch_nr_resource_t* pucch_resource_cfg,
                               const srsran_uci_cfg_nr_t*        uci_cfg,
                               int8_t*                           llr,
                               srsran_uci_value_nr_t*            value)
{
  int E_tot = srsran_uci_nr_pucch_format_2_3_4_E(pucch_resource_cfg);
  if (E_tot < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  int E_uci = uci_nr_pucch_E_uci(pucch_resource_cfg, uci_cfg, E_tot);
  if (E_uci < SRSRAN_SUCCESS) {
    ERROR("Error calculating number of encoded PUCCH UCI bits");
    return SRSRAN_ERROR;
  }

  // 6.3.1.1 UCI bit sequence generation
  int A = uci_nr_A(uci_cfg);
  if (A < SRSRAN_SUCCESS) {
    ERROR("Error getting number of bits");
    return SRSRAN_ERROR;
  }

  if (uci_nr_decode(q, uci_cfg, llr, A, E_uci, &value->valid) < SRSRAN_SUCCESS) {
    ERROR("Error decoding UCI bits");
    return SRSRAN_ERROR;
  }

  // Unpack bits
  if (uci_nr_unpack_pucch(uci_cfg, q->bit_sequence, value) < SRSRAN_SUCCESS) {
    ERROR("Error unpacking PUCCH UCI bits");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

uint32_t srsran_uci_nr_total_bits(const srsran_uci_cfg_nr_t* uci_cfg)
{
  if (uci_cfg == NULL) {
    return 0;
  }

  uint32_t o_csi = srsran_csi_part1_nof_bits(uci_cfg->csi, uci_cfg->nof_csi);

  // According to 38.213 9.2.4 UE procedure for reporting SR
  // The UE transmits a PUCCH in the PUCCH resource for the corresponding SR configuration only when the UE transmits a
  // positive SR
  if (uci_cfg->ack.count == 0 && o_csi == 0 && !uci_cfg->sr_positive_present) {
    return 0;
  }

  return uci_cfg->ack.count + uci_cfg->o_sr + o_csi;
}

uint32_t srsran_uci_nr_info(const srsran_uci_data_nr_t* uci_data, char* str, uint32_t str_len)
{
  uint32_t len = 0;

  if (uci_data->cfg.ack.count > 0) {
    char str_ack[10];
    srsran_vec_sprint_bin(str_ack, (uint32_t)sizeof(str_ack), uci_data->value.ack, uci_data->cfg.ack.count);
    len = srsran_print_check(str, str_len, len, "ack=%s ", str_ack);
  }

  if (uci_data->cfg.nof_csi > 0) {
    len += srsran_csi_str(uci_data->cfg.csi, uci_data->value.csi, uci_data->cfg.nof_csi, &str[len], str_len - len);
  }

  if (uci_data->cfg.o_sr > 0) {
    len = srsran_print_check(str, str_len, len, "sr=%d ", uci_data->value.sr);
  }

  return len;
}

static int uci_nr_pusch_Q_prime_ack(const srsran_uci_nr_pusch_cfg_t* cfg, uint32_t O_ack)
{
  uint32_t L_ack = srsran_uci_nr_crc_len(O_ack);              // Number of CRC bits
  uint32_t Qm    = srsran_mod_bits_x_symbol(cfg->modulation); // modulation order of the PUSCH

  uint32_t M_uci_sum    = 0;
  uint32_t M_uci_l0_sum = 0;
  for (uint32_t l = 0; l < SRSRAN_NSYMB_PER_SLOT_NR; l++) {
    M_uci_sum += cfg->M_uci_sc[l];
    if (l >= cfg->l0) {
      M_uci_l0_sum += cfg->M_uci_sc[l];
    }
  }

  if (!isnormal(cfg->R)) {
    ERROR("Invalid Rate (%f)", cfg->R);
    return SRSRAN_ERROR;
  }

  if (cfg->K_sum == 0) {
    return (int)SRSRAN_MIN(ceilf(((O_ack + L_ack) * cfg->beta_harq_ack_offset) / (Qm * cfg->R)),
                           cfg->alpha * M_uci_l0_sum);
  }
  return (int)SRSRAN_MIN(ceilf(((O_ack + L_ack) * cfg->beta_harq_ack_offset * M_uci_sum) / cfg->K_sum),
                         cfg->alpha * M_uci_l0_sum);
}

int srsran_uci_nr_pusch_ack_nof_bits(const srsran_uci_nr_pusch_cfg_t* cfg, uint32_t O_ack)
{
  // Validate configuration
  int err = uci_nr_pusch_cfg_valid(cfg);
  if (err < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Configuration is unset
  if (err == 0) {
    return 0;
  }

  int Q_ack_prime = uci_nr_pusch_Q_prime_ack(cfg, O_ack);
  if (Q_ack_prime < SRSRAN_SUCCESS) {
    ERROR("Error calculating number of RE");
    return Q_ack_prime;
  }

  return (int)(Q_ack_prime * cfg->nof_layers * srsran_mod_bits_x_symbol(cfg->modulation));
}

int srsran_uci_nr_encode_pusch_ack(srsran_uci_nr_t*             q,
                                   const srsran_uci_cfg_nr_t*   cfg,
                                   const srsran_uci_value_nr_t* value,
                                   uint8_t*                     o)
{
  // Check inputs
  if (q == NULL || cfg == NULL || value == NULL || o == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  int A = cfg->ack.count;

  // 6.3.2.1 UCI bit sequence generation
  // 6.3.2.1.1 HARQ-ACK
  bool has_csi_part2 = srsran_csi_has_part2(cfg->csi, cfg->nof_csi);
  if (cfg->pusch.K_sum == 0 && cfg->nof_csi > 1 && !has_csi_part2 && A < 2) {
    q->bit_sequence[0] = (A == 0) ? 0 : value->ack[0];
    q->bit_sequence[1] = 0;
    A                  = 2;
  } else if (A == 0) {
    UCI_NR_INFO_TX("No HARQ-ACK to mux");
    return SRSRAN_SUCCESS;
  } else {
    srsran_vec_u8_copy(q->bit_sequence, value->ack, cfg->ack.count);
  }

  // Compute total of encoded bits according to 6.3.2.4 Rate matching
  int E_uci = srsran_uci_nr_pusch_ack_nof_bits(&cfg->pusch, A);
  if (E_uci < SRSRAN_SUCCESS) {
    ERROR("Error calculating number of encoded bits");
    return SRSRAN_ERROR;
  }

  return uci_nr_encode(q, cfg, A, o, E_uci);
}

int srsran_uci_nr_decode_pusch_ack(srsran_uci_nr_t*           q,
                                   const srsran_uci_cfg_nr_t* cfg,
                                   int8_t*                    llr,
                                   srsran_uci_value_nr_t*     value)
{
  // Check inputs
  if (q == NULL || cfg == NULL || llr == NULL || value == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  int A = cfg->ack.count;

  // 6.3.2.1 UCI bit sequence generation
  // 6.3.2.1.1 HARQ-ACK
  bool has_csi_part2 = srsran_csi_has_part2(cfg->csi, cfg->nof_csi);
  if (cfg->pusch.K_sum == 0 && cfg->nof_csi > 1 && !has_csi_part2 && cfg->ack.count < 2) {
    A = 2;
  }

  // Compute total of encoded bits according to 6.3.2.4 Rate matching
  int E_uci = srsran_uci_nr_pusch_ack_nof_bits(&cfg->pusch, A);
  if (E_uci < SRSRAN_SUCCESS) {
    ERROR("Error calculating number of encoded bits");
    return SRSRAN_ERROR;
  }

  // Decode
  if (uci_nr_decode(q, cfg, llr, A, E_uci, &value->valid) < SRSRAN_SUCCESS) {
    ERROR("Error decoding UCI");
    return SRSRAN_ERROR;
  }

  // Unpack
  srsran_vec_u8_copy(value->ack, q->bit_sequence, A);

  return SRSRAN_SUCCESS;
}

static int uci_nr_pusch_Q_prime_csi1(const srsran_uci_nr_pusch_cfg_t* cfg, uint32_t O_csi1, uint32_t O_ack)
{
  if (cfg == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  uint32_t L_ack = srsran_uci_nr_crc_len(O_csi1);             // Number of CRC bits
  uint32_t Qm    = srsran_mod_bits_x_symbol(cfg->modulation); // modulation order of the PUSCH

  int Q_prime_ack = uci_nr_pusch_Q_prime_ack(cfg, SRSRAN_MAX(2, O_ack));
  if (Q_prime_ack < SRSRAN_ERROR) {
    ERROR("Calculating Q_prime_ack");
    return SRSRAN_ERROR;
  }

  uint32_t M_uci_sum = 0;
  for (uint32_t l = 0; l < SRSRAN_NSYMB_PER_SLOT_NR; l++) {
    M_uci_sum += cfg->M_uci_sc[l];
  }

  if (!isnormal(cfg->R)) {
    ERROR("Invalid Rate (%f)", cfg->R);
    return SRSRAN_ERROR;
  }

  if (cfg->K_sum == 0) {
    if (cfg->csi_part2_present) {
      return (int)SRSRAN_MIN(ceilf(((O_csi1 + L_ack) * cfg->beta_csi1_offset) / (Qm * cfg->R)),
                             cfg->alpha * M_uci_sum - Q_prime_ack);
    }
    return (int)(M_uci_sum - Q_prime_ack);
  }
  return (int)SRSRAN_MIN(ceilf(((O_csi1 + L_ack) * cfg->beta_csi1_offset * M_uci_sum) / cfg->K_sum),
                         ceilf(cfg->alpha * M_uci_sum) - Q_prime_ack);
}

int srsran_uci_nr_pusch_csi1_nof_bits(const srsran_uci_cfg_nr_t* cfg)
{
  // Validate configuration
  int err = uci_nr_pusch_cfg_valid(&cfg->pusch);
  if (err < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Configuration is unset
  if (err == 0) {
    return 0;
  }

  int O_csi1 = srsran_csi_part1_nof_bits(cfg->csi, cfg->nof_csi);
  if (O_csi1 < SRSRAN_SUCCESS) {
    ERROR("Errpr calculating CSI part 1 number of bits");
    return SRSRAN_ERROR;
  }
  uint32_t O_ack = SRSRAN_MAX(2, cfg->ack.count);

  int Q_csi1_prime = uci_nr_pusch_Q_prime_csi1(&cfg->pusch, (uint32_t)O_csi1, O_ack);
  if (Q_csi1_prime < SRSRAN_SUCCESS) {
    ERROR("Error calculating number of RE");
    return Q_csi1_prime;
  }

  return (int)(Q_csi1_prime * cfg->pusch.nof_layers * srsran_mod_bits_x_symbol(cfg->pusch.modulation));
}

int srsran_uci_nr_encode_pusch_csi1(srsran_uci_nr_t*             q,
                                    const srsran_uci_cfg_nr_t*   cfg,
                                    const srsran_uci_value_nr_t* value,
                                    uint8_t*                     o)
{
  // Check inputs
  if (q == NULL || cfg == NULL || value == NULL || o == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  int A = srsran_csi_part1_pack(cfg->csi, value->csi, cfg->nof_csi, q->bit_sequence, SRSRAN_UCI_NR_MAX_NOF_BITS);
  if (A < SRSRAN_SUCCESS) {
    ERROR("Error packing CSI part 1 report");
    return SRSRAN_ERROR;
  }

  if (A == 0) {
    UCI_NR_INFO_TX("No CSI part 1 to mux");
    return SRSRAN_SUCCESS;
  }

  // Compute total of encoded bits according to 6.3.2.4 Rate matching
  int E_uci = srsran_uci_nr_pusch_csi1_nof_bits(cfg);
  if (E_uci < SRSRAN_SUCCESS) {
    ERROR("Error calculating number of encoded bits");
    return SRSRAN_ERROR;
  }

  return uci_nr_encode(q, cfg, A, o, E_uci);
}

int srsran_uci_nr_decode_pusch_csi1(srsran_uci_nr_t*           q,
                                    const srsran_uci_cfg_nr_t* cfg,
                                    int8_t*                    llr,
                                    srsran_uci_value_nr_t*     value)
{
  // Check inputs
  if (q == NULL || cfg == NULL || llr == NULL || value == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Compute total of encoded bits according to 6.3.2.4 Rate matching
  int E_uci = srsran_uci_nr_pusch_csi1_nof_bits(cfg);
  if (E_uci < SRSRAN_SUCCESS) {
    ERROR("Error calculating number of encoded bits");
    return SRSRAN_ERROR;
  }

  int A = srsran_csi_part1_nof_bits(cfg->csi, cfg->nof_csi);
  if (A < SRSRAN_SUCCESS) {
    ERROR("Error getting number of CSI part 1 bits");
    return SRSRAN_ERROR;
  }

  // Decode
  if (uci_nr_decode(q, cfg, llr, (uint32_t)A, (uint32_t)E_uci, &value->valid) < SRSRAN_SUCCESS) {
    ERROR("Error decoding UCI");
    return SRSRAN_ERROR;
  }

  // Unpack
  if (srsran_csi_part1_unpack(cfg->csi, cfg->nof_csi, q->bit_sequence, A, value->csi) < SRSRAN_SUCCESS) {
    ERROR("Error unpacking CSI");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}
