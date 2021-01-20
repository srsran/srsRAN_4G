/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/phy/phch/uci_nr.h"
#include "srslte/phy/fec/block/block.h"
#include "srslte/phy/fec/polar/polar_chanalloc.h"
#include "srslte/phy/phch/uci_cfg.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/vector.h"

// TS 38.212 section 5.2.1 Polar coding: The value of A is no larger than 1706.
#define UCI_NR_MAX_A 1706U
#define UCI_NR_MAX_L 11U
#define UCI_NR_POLAR_MAX 2048U
#define UCI_NR_POLAR_RM_IBIL 0
#define UCI_NR_BLOCK_CORR_THRESHOLD 0.5f

int srslte_uci_nr_init(srslte_uci_nr_t* q, const srslte_uci_nr_args_t* args)
{
  if (q == NULL || args == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  srslte_polar_encoder_type_t polar_encoder_type = SRSLTE_POLAR_ENCODER_PIPELINED;
  srslte_polar_decoder_type_t polar_decoder_type = SRSLTE_POLAR_DECODER_SSC_C;
#ifdef LV_HAVE_AVX2
  if (!args->disable_simd) {
    polar_encoder_type = SRSLTE_POLAR_ENCODER_AVX2;
    polar_decoder_type = SRSLTE_POLAR_DECODER_SSC_C_AVX2;
  }
#endif // LV_HAVE_AVX2

  if (srslte_polar_encoder_init(&q->encoder, polar_encoder_type, NMAX_LOG) < SRSLTE_SUCCESS) {
    ERROR("Initialising polar encoder\n");
    return SRSLTE_ERROR;
  }

  if (srslte_polar_decoder_init(&q->decoder, polar_decoder_type, NMAX_LOG) < SRSLTE_SUCCESS) {
    ERROR("Initialising polar encoder\n");
    return SRSLTE_ERROR;
  }

  if (srslte_polar_rm_tx_init(&q->rm) < SRSLTE_SUCCESS) {
    ERROR("Initialising polar RM\n");
    return SRSLTE_ERROR;
  }

  if (srslte_crc_init(&q->crc6, SRSLTE_LTE_CRC6, 6) < SRSLTE_SUCCESS) {
    ERROR("Initialising CRC\n");
    return SRSLTE_ERROR;
  }

  if (srslte_crc_init(&q->crc11, SRSLTE_LTE_CRC11, 11) < SRSLTE_SUCCESS) {
    ERROR("Initialising CRC\n");
    return SRSLTE_ERROR;
  }

  // Allocate bit sequence with space for the CRC
  q->bit_sequence = srslte_vec_u8_malloc(UCI_NR_MAX_A);
  if (q->bit_sequence == NULL) {
    ERROR("Error malloc\n");
    return SRSLTE_ERROR;
  }

  // Allocate c with space for a and the CRC
  q->c = srslte_vec_u8_malloc(UCI_NR_MAX_A + UCI_NR_MAX_L);
  if (q->c == NULL) {
    ERROR("Error malloc\n");
    return SRSLTE_ERROR;
  }

  q->allocated = srslte_vec_u8_malloc(UCI_NR_POLAR_MAX);
  if (q->allocated == NULL) {
    ERROR("Error malloc\n");
    return SRSLTE_ERROR;
  }

  q->d = srslte_vec_u8_malloc(UCI_NR_POLAR_MAX);
  if (q->d == NULL) {
    ERROR("Error malloc\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

void srslte_uci_nr_free(srslte_uci_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  srslte_polar_encoder_free(&q->encoder);
  srslte_polar_decoder_free(&q->decoder);

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

  SRSLTE_MEM_ZERO(q, srslte_uci_nr_t, 1);
}

static int uci_nr_pack_ack_sr(const srslte_uci_cfg_nr_t* cfg, const srslte_uci_value_nr_t* value, uint8_t* sequence)
{
  int A = 0;

  // Append ACK bits
  srslte_vec_u8_copy(&sequence[A], value->ack, cfg->o_ack);
  A += cfg->o_ack;

  // Append SR bits
  srslte_vec_u8_copy(&sequence[A], value->sr, cfg->o_sr);
  A += cfg->o_sr;

  return A;
}

static int uci_nr_unpack_ack_sr(const srslte_uci_cfg_nr_t* cfg, const uint8_t* sequence, srslte_uci_value_nr_t* value)
{
  int A = 0;

  // Append ACK bits
  srslte_vec_u8_copy(value->ack, &sequence[A], cfg->o_ack);
  A += cfg->o_ack;

  // Append SR bits
  srslte_vec_u8_copy(value->sr, &sequence[A], cfg->o_sr);
  A += cfg->o_sr;

  return A;
}

static int uci_nr_A(const srslte_uci_cfg_nr_t* cfg)
{
  // 6.3.1.1.1 HARQ-ACK/SR only UCI bit sequence generation
  if (cfg->o_csi1 == 0 && cfg->o_csi2 == 0) {
    return cfg->o_ack + cfg->o_sr;
  }

  // 6.3.1.1.2 CSI only
  if (cfg->o_ack == 0 && cfg->o_sr == 0) {
    ERROR("CSI only are not implemented\n");
    return SRSLTE_ERROR;
  }

  // 6.3.1.1.3 HARQ-ACK/SR and CSI
  ERROR("HARQ-ACK/SR and CSI encoding are not implemented\n");
  return SRSLTE_ERROR;
}

static int uci_nr_packing(const srslte_uci_cfg_nr_t* cfg, const srslte_uci_value_nr_t* value, uint8_t* sequence)
{
  // 6.3.1.1.1 HARQ-ACK/SR only UCI bit sequence generation
  if (cfg->o_csi1 == 0 && cfg->o_csi2 == 0) {
    return uci_nr_pack_ack_sr(cfg, value, sequence);
  }

  // 6.3.1.1.2 CSI only
  if (cfg->o_ack == 0 && cfg->o_sr == 0) {
    ERROR("CSI only are not implemented\n");
    return SRSLTE_ERROR;
  }

  // 6.3.1.1.3 HARQ-ACK/SR and CSI
  ERROR("HARQ-ACK/SR and CSI encoding are not implemented\n");
  return SRSLTE_ERROR;
}

static int uci_nr_unpacking(const srslte_uci_cfg_nr_t* cfg, const uint8_t* sequence, srslte_uci_value_nr_t* value)
{
  // 6.3.1.1.1 HARQ-ACK/SR only UCI bit sequence generation
  if (cfg->o_csi1 == 0 && cfg->o_csi2 == 0) {
    return uci_nr_unpack_ack_sr(cfg, sequence, value);
  }

  // 6.3.1.1.2 CSI only
  if (cfg->o_ack == 0 && cfg->o_sr == 0) {
    ERROR("CSI only are not implemented\n");
    return SRSLTE_ERROR;
  }

  // 6.3.1.1.3 HARQ-ACK/SR and CSI
  ERROR("HARQ-ACK/SR and CSI encoding are not implemented\n");
  return SRSLTE_ERROR;
}

static int uci_nr_encode_1bit(srslte_uci_nr_t* q, const srslte_uci_cfg_nr_t* cfg, uint8_t* o, uint32_t E)
{
  uint32_t              i  = 0;
  srslte_uci_bit_type_t c0 = (q->bit_sequence[0] == 0) ? UCI_BIT_0 : UCI_BIT_1;

  switch (cfg->modulation) {
    case SRSLTE_MOD_BPSK:
      while (i < E) {
        o[i++] = c0;
      }
      break;
    case SRSLTE_MOD_QPSK:
      while (i < E) {
        o[i++] = c0;
        o[i++] = UCI_BIT_REPETITION;
      }
      break;
    case SRSLTE_MOD_16QAM:
      while (i < E) {
        o[i++] = c0;
        o[i++] = UCI_BIT_REPETITION;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
      }
      break;
    case SRSLTE_MOD_64QAM:
      while (i < E) {
        while (i < E) {
          o[i++] = c0;
          o[i++] = UCI_BIT_REPETITION;
          o[i++] = UCI_BIT_PLACEHOLDER;
          o[i++] = UCI_BIT_PLACEHOLDER;
          o[i++] = UCI_BIT_PLACEHOLDER;
          o[i++] = UCI_BIT_PLACEHOLDER;
        }
      }
      break;
    case SRSLTE_MOD_256QAM:
      while (i < E) {
        o[i++] = c0;
        o[i++] = UCI_BIT_REPETITION;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
      }
      break;
    case SRSLTE_MOD_NITEMS:
    default:
      ERROR("Invalid modulation\n");
      return SRSLTE_ERROR;
  }

  return E;
}

static int uci_nr_encode_2bit(srslte_uci_nr_t* q, const srslte_uci_cfg_nr_t* cfg, uint8_t* o, uint32_t E)
{
  uint32_t              i  = 0;
  srslte_uci_bit_type_t c0 = (q->bit_sequence[0] == 0) ? UCI_BIT_0 : UCI_BIT_1;
  srslte_uci_bit_type_t c1 = (q->bit_sequence[1] == 0) ? UCI_BIT_0 : UCI_BIT_1;
  srslte_uci_bit_type_t c2 = ((q->bit_sequence[0] ^ q->bit_sequence[1]) == 0) ? UCI_BIT_0 : UCI_BIT_1;

  switch (cfg->modulation) {

    case SRSLTE_MOD_BPSK:
    case SRSLTE_MOD_QPSK:
      while (i < E) {
        o[i++] = c0;
        o[i++] = c1;
        o[i++] = c2;
      }
      break;
    case SRSLTE_MOD_16QAM:
      while (i < E) {
        o[i++] = c0;
        o[i++] = c1;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = c2;
        o[i++] = c0;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = c1;
        o[i++] = c2;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
      }
      break;
    case SRSLTE_MOD_64QAM:
      while (i < E) {
        o[i++] = c0;
        o[i++] = c1;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = c2;
        o[i++] = c0;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = c1;
        o[i++] = c2;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
      }
      break;
    case SRSLTE_MOD_256QAM:

      while (i < E) {
        o[i++] = c0;
        o[i++] = c1;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = c2;
        o[i++] = c0;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = c1;
        o[i++] = c2;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
        o[i++] = UCI_BIT_PLACEHOLDER;
      }
      break;
    case SRSLTE_MOD_NITEMS:
    default:
      ERROR("Invalid modulation\n");
      return SRSLTE_ERROR;
  }

  return E;
}

static int
uci_nr_encode_3_11_bit(srslte_uci_nr_t* q, const srslte_uci_cfg_nr_t* cfg, uint32_t A, uint8_t* o, uint32_t E)
{
  uint8_t encoded[SRSLTE_FEC_BLOCK_SIZE] = {};
  srslte_block_encode(q->bit_sequence, A, encoded, SRSLTE_FEC_BLOCK_SIZE);

  for (uint32_t i = 0; i < E; i++) {
    o[i] = (encoded[i % SRSLTE_FEC_BLOCK_SIZE] == 0) ? UCI_BIT_0 : UCI_BIT_1;
  }

  return E;
}

static int uci_nr_decode_3_11_bit(srslte_uci_nr_t*           q,
                                  const srslte_uci_cfg_nr_t* cfg,
                                  uint32_t                   A,
                                  const int8_t*              llr,
                                  uint32_t                   E,
                                  bool*                      decoded_ok)
{
  // Check E for avoiding zero division
  if (E < 1) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Compute average LLR power
  float pwr = sqrtf(srslte_vec_avg_power_bf(llr, E));
  if (!isnormal(pwr)) {
    return SRSLTE_ERROR;
  }

  // Decode
  float corr = (float)srslte_block_decode_i8(llr, E, q->bit_sequence, A);

  // Normalise correlation
  corr /= sqrtf(pwr) * E;

  // Take decoded decision with threshold
  *decoded_ok = (corr > UCI_NR_BLOCK_CORR_THRESHOLD);

  return SRSLTE_SUCCESS;
}

#define CEIL(NUM, DEN) (((NUM) + ((DEN)-1)) / (DEN))

static int
uci_nr_encode_11_1706_bit(srslte_uci_nr_t* q, const srslte_uci_cfg_nr_t* cfg, uint32_t A, uint8_t* o, uint32_t E_uci)
{
  // If ( A ≥ 360 and E ≥ 1088 ) or if A ≥ 1013 , I seg = 1 ; otherwise I seg = 0
  uint32_t I_seg = 0;
  if ((A >= 360 && E_uci >= 1088) || A >= 1013) {
    I_seg = 1;
  }

  // Select CRC
  srslte_crc_t* crc = &q->crc6;
  if (A >= 20) {
    crc = &q->crc11;
  }

  // Segmentation
  uint32_t C = 1;
  if (I_seg == 1) {
    C = 2;
  }
  uint32_t A_prime = CEIL(A, C) * C;

  // Get polar code
  uint32_t K_r = A_prime / C + crc->order;
  uint32_t E_r = E_uci / C;
  if (srslte_polar_code_get(&q->code, K_r, E_r, 9U) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
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
    srslte_crc_attach(crc, q->c, A_prime / C);

    // Allocate channel
    srslte_polar_chanalloc_tx(q->c, q->allocated, q->code.N, q->code.K, q->code.nPC, q->code.K_set, q->code.PC_set);

    // Encode bits
    if (srslte_polar_encoder_encode(&q->encoder, q->allocated, q->d, q->code.n) < SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }

    // Rate matching
    srslte_polar_rm_tx(&q->rm, q->d, &o[E_r * r], q->code.n, E_r, K_r, UCI_NR_POLAR_RM_IBIL);
  }

  return E_uci;
}

static int uci_nr_decode_11_1706_bit(srslte_uci_nr_t*           q,
                                     const srslte_uci_cfg_nr_t* cfg,
                                     uint32_t                   A,
                                     const int8_t*              llr,
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
  srslte_crc_t* crc = &q->crc6;
  if (A >= 20) {
    crc = &q->crc11;
  }

  // Segmentation
  uint32_t C = 1;
  if (I_seg == 1) {
    C = 2;
  }
  uint32_t A_prime = CEIL(A, C) * C;

  // Get polar code
  uint32_t K_r = A_prime / C + crc->order;
  uint32_t E_r = E_uci / C;
  if (srslte_polar_code_get(&q->code, K_r, E_r, 9U) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Write codeword
  for (uint32_t r = 0, s = 0; r < C; r++) {
    uint32_t k = 0;

    // Undo rate matching
    int8_t* d = (int8_t*)q->d;
    srslte_polar_rm_rx_c(&q->rm, &llr[E_r * r], d, q->code.n, E_r, K_r, UCI_NR_POLAR_RM_IBIL);

    // Decode bits
    if (srslte_polar_decoder_decode_c(&q->decoder, d, q->allocated, q->code.n, q->code.F_set, q->code.F_set_size) <
        SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }

    // Undo channel allocation
    srslte_polar_chanalloc_rx(q->allocated, q->c, q->code.K, q->code.nPC, q->code.K_set, q->code.PC_set);

    //
    uint8_t* ptr       = &q->c[q->code.K - crc->order];
    uint32_t checksum1 = srslte_crc_checksum(crc, q->c, q->code.K);
    uint32_t checksum2 = srslte_bit_pack(&ptr, crc->order);
    (*decoded_ok)      = ((*decoded_ok) && (checksum1 == checksum2));

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

  return SRSLTE_SUCCESS;
}

static int uci_nr_encode(srslte_uci_nr_t*             q,
                         const srslte_uci_cfg_nr_t*   uci_cfg,
                         const srslte_uci_value_nr_t* uci_value,
                         uint8_t*                     o,
                         uint32_t                     E_uci)
{
  if (q == NULL || uci_cfg == NULL || uci_value == NULL || o == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // 6.3.1.1 UCI bit sequence generation
  int A = uci_nr_packing(uci_cfg, uci_value, q->bit_sequence);
  if (A < SRSLTE_SUCCESS) {
    ERROR("Generating bit sequence");
    return SRSLTE_ERROR;
  }

  // 5.3.3.1 Encoding of 1-bit information
  if (A == 1) {
    return uci_nr_encode_1bit(q, uci_cfg, o, E_uci);
  }

  // 5.3.3.2 Encoding of 2-bit information
  if (A == 2) {
    return uci_nr_encode_2bit(q, uci_cfg, o, E_uci);
  }

  // 5.3.3.3 Encoding of other small block lengths
  if (A <= SRSLTE_FEC_BLOCK_MAX_NOF_BITS) {
    return uci_nr_encode_3_11_bit(q, uci_cfg, A, o, E_uci);
  }

  // Encoding of other sizes up to 1906
  if (A < UCI_NR_MAX_A) {
    return uci_nr_encode_11_1706_bit(q, uci_cfg, A, o, E_uci);
  }

  return SRSLTE_ERROR;
}

static int uci_nr_decode(srslte_uci_nr_t*           q,
                         const srslte_uci_cfg_nr_t* uci_cfg,
                         const int8_t*              llr,
                         uint32_t                   E_uci,
                         srslte_uci_value_nr_t*     uci_value)
{
  if (q == NULL || uci_cfg == NULL || uci_value == NULL || llr == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // 6.3.1.1 UCI bit sequence generation
  int A = uci_nr_A(uci_cfg);
  if (A < SRSLTE_SUCCESS) {
    ERROR("Error getting number of bits\n");
    return SRSLTE_ERROR;
  }

  // Decode LLR
  if (A == 1) {
    ERROR("Not implemented\n");
  } else if (A == 2) {
    ERROR("Not implemented\n");
  } else if (A <= 11) {
    if (uci_nr_decode_3_11_bit(q, uci_cfg, A, llr, E_uci, &uci_value->valid) < SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }
  } else if (A < UCI_NR_MAX_A) {
    if (uci_nr_decode_11_1706_bit(q, uci_cfg, A, llr, E_uci, &uci_value->valid) < SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }
  } else {
    ERROR("Invalid number of bits (A=%d)\n", A);
  }

  // Unpack bits
  if (uci_nr_unpacking(uci_cfg, q->bit_sequence, uci_value) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

// Implements TS 38.212 Table 6.3.1.4-1: Total rate matching output sequence length Etot
static int uci_nr_pucch_E_tot(const srslte_pucch_nr_resource_t* pucch_cfg, const srslte_uci_cfg_nr_t* uci_cfg)
{
  if (pucch_cfg == NULL || uci_cfg == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Compute total number of bits
  uint32_t nof_bits = uci_cfg->o_sr + uci_cfg->o_ack + uci_cfg->o_csi1 + uci_cfg->o_csi2;

  switch (pucch_cfg->format) {
    case SRSLTE_PUCCH_NR_FORMAT_1:
      if (nof_bits <= 2) {
        return nof_bits;
      }
      break;
    case SRSLTE_PUCCH_NR_FORMAT_2:
      if (uci_cfg->modulation == SRSLTE_MOD_QPSK) {
        return (int)(16 * pucch_cfg->nof_symbols * pucch_cfg->nof_prb);
      }
      break;
    case SRSLTE_PUCCH_NR_FORMAT_3:
      if (uci_cfg->modulation == SRSLTE_MOD_QPSK) {
        return (int)(24 * pucch_cfg->nof_symbols * pucch_cfg->nof_prb);
      }
      if (uci_cfg->modulation == SRSLTE_MOD_BPSK) {
        return (int)(12 * pucch_cfg->nof_symbols * pucch_cfg->nof_prb);
      }
      break;
    case SRSLTE_PUCCH_NR_FORMAT_4:
      if (pucch_cfg->occ_lenth != 1 && pucch_cfg->occ_lenth != 2) {
        ERROR("Invalid spreading factor (%d)\n", pucch_cfg->occ_lenth);
        return SRSLTE_ERROR;
      }
      if (uci_cfg->modulation == SRSLTE_MOD_QPSK) {
        return (int)(24 * pucch_cfg->nof_symbols / pucch_cfg->occ_lenth);
      }
      if (uci_cfg->modulation == SRSLTE_MOD_BPSK) {
        return (int)(12 * pucch_cfg->nof_symbols / pucch_cfg->occ_lenth);
      }
      break;
    default:
      return SRSLTE_ERROR;
  }
  return SRSLTE_ERROR;
}

// Implements TS 38.212 Table 6.3.1.4.1-1: Rate matching output sequence length E UCI
static int
uci_nr_pucch_E_uci(const srslte_pucch_nr_resource_t* pucch_cfg, const srslte_uci_cfg_nr_t* uci_cfg, uint32_t E_tot)
{
  if (uci_cfg->o_csi1 != 0 && uci_cfg->o_csi2) {
    ERROR("Simultaneous CSI part 1 and CSI part 2 is not implemented\n");
    return SRSLTE_ERROR;
  }

  return E_tot;
}

int srslte_uci_nr_encode_pucch(srslte_uci_nr_t*                  q,
                               const srslte_pucch_nr_resource_t* pucch_resource_cfg,
                               const srslte_uci_cfg_nr_t*        uci_cfg,
                               const srslte_uci_value_nr_t*      value,
                               uint8_t*                          o)
{
  int E_tot = uci_nr_pucch_E_tot(pucch_resource_cfg, uci_cfg);
  if (E_tot < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  int E_uci = uci_nr_pucch_E_uci(pucch_resource_cfg, uci_cfg, E_tot);
  if (E_uci < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  return uci_nr_encode(q, uci_cfg, value, o, E_uci);
}

int srslte_uci_nr_decode_pucch(srslte_uci_nr_t*                  q,
                               const srslte_pucch_nr_resource_t* pucch_resource_cfg,
                               const srslte_uci_cfg_nr_t*        uci_cfg,
                               const int8_t*                     llr,
                               srslte_uci_value_nr_t*            value)
{
  int E_tot = uci_nr_pucch_E_tot(pucch_resource_cfg, uci_cfg);
  if (E_tot < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  int E_uci = uci_nr_pucch_E_uci(pucch_resource_cfg, uci_cfg, E_tot);
  if (E_uci < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  return uci_nr_decode(q, uci_cfg, llr, E_uci, value);
}
