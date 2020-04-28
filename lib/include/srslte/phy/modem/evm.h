/*
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

#ifndef SRSLTE_EVM_H_
#define SRSLTE_EVM_H_

#include <srslte/config.h>
#include <srslte/phy/phch/ra.h>
#include <srslte/phy/utils/debug.h>
#include <srslte/phy/utils/vector.h>

/** @struct srslte_evm_buffer_t
 * This structure carries the necessary temporary data required for calculating the EVM.
 *
 * @var max_bits maximum of bits which can support
 * @var hard_bits vector that stores hard bits after decision
 * @var symbols vector that stores the modulated symbols from the hard bits
 */
typedef struct {
  uint32_t max_bits;
  uint8_t* hard_bits;
  cf_t*    symbols;
} srslte_evm_buffer_t;

/**
 * Allocates the EVM calculation buffer
 * @param nof_prb that provides the maximum number of bits
 * @return EVM buffer pointer
 */
static inline srslte_evm_buffer_t* srslte_evm_buffer_alloc(uint32_t nof_prb)
{
  srslte_evm_buffer_t* q = (srslte_evm_buffer_t*)srslte_vec_malloc(sizeof(srslte_evm_buffer_t));

  // Check allocation result and number of PRB
  if (!q || !nof_prb) {
    ERROR("Malloc");
    return q;
  }

  // Zero memory the buffer fields
  memset(q, 0, sizeof(srslte_evm_buffer_t));

  // Set max number of bits
  q->max_bits = srslte_ra_tbs_from_idx(SRSLTE_RA_NOF_TBS_IDX - 1, nof_prb);

  // Allocate hard bits
  q->hard_bits = srslte_vec_u8_malloc(q->max_bits);
  if (!q->hard_bits) {
    ERROR("Malloc");
    return q;
  }

  // Allocate symbols assuming BPSK
  q->symbols = srslte_vec_cf_malloc(q->max_bits);
  if (!q->symbols) {
    ERROR("Malloc");
    return q;
  }

  return q;
}

/**
 * Allocates the EVM calculation buffer
 * @param nof_prb that provides the maximum number of bits
 * @return EVM buffer pointer
 */
static inline void srslte_evm_buffer_resize(srslte_evm_buffer_t* q, uint32_t nof_prb)
{
  // Assert pointer and number of PRB
  if (!q || !nof_prb) {
    ERROR("Invalid inputs");
    return;
  }

  // Get new number of max bits
  uint32_t new_max_bits = srslte_ra_tbs_from_idx(SRSLTE_RA_NOF_TBS_IDX - 1, nof_prb);

  // Return if no resize is required
  if (q->max_bits >= new_max_bits) {
    return;
  }

  // Update with greater value
  q->max_bits = new_max_bits;

  // Free hard bits if it was allocated
  if (q->hard_bits) {
    free(q->hard_bits);
  }

  // Allocate hard bits again
  q->hard_bits = srslte_vec_u8_malloc(q->max_bits);
  if (!q->hard_bits) {
    ERROR("Malloc");
    return;
  }

  // Free symbols if it was allocated
  if (q->symbols) {
    free(q->symbols);
  }

  // Allocate symbols again
  q->symbols = srslte_vec_cf_malloc(q->max_bits);
  if (!q->symbols) {
    ERROR("Malloc");
    return;
  }
}

/**
 * Template for hard decision taking
 */
#define HARD_DECISION(LLR_T, SOFTBITS, HARDBITS, NOF_SOFTBITS)                                                         \
  do {                                                                                                                 \
    /* Typecasts pointer type to minimum width */                                                                      \
    uint8_t* ptr = (uint8_t*)SOFTBITS;                                                                                 \
                                                                                                                       \
    /* Big endian compensation, ptr needs to point at the MSB */                                                       \
    ptr += sizeof(LLR_T) - 1UL;                                                                                        \
                                                                                                                       \
    for (uint32_t i = 0; i < NOF_SOFTBITS / 8; i++) {                                                                  \
      /* Default mask */                                                                                               \
      uint8_t w = 0xff;                                                                                                \
                                                                                                                       \
      /* For each soft bit, take MSB ad collocate in right position */                                                 \
      w ^= (*ptr & 0x80);                                                                                              \
      ptr += sizeof(LLR_T);                                                                                            \
      w ^= (*ptr & 0x80) >> 1;                                                                                         \
      ptr += sizeof(LLR_T);                                                                                            \
      w ^= (*ptr & 0x80) >> 2;                                                                                         \
      ptr += sizeof(LLR_T);                                                                                            \
      w ^= (*ptr & 0x80) >> 3;                                                                                         \
      ptr += sizeof(LLR_T);                                                                                            \
      w ^= (*ptr & 0x80) >> 4;                                                                                         \
      ptr += sizeof(LLR_T);                                                                                            \
      w ^= (*ptr & 0x80) >> 5;                                                                                         \
      ptr += sizeof(LLR_T);                                                                                            \
      w ^= (*ptr & 0x80) >> 6;                                                                                         \
      ptr += sizeof(LLR_T);                                                                                            \
      w ^= (*ptr & 0x80) >> 7;                                                                                         \
      ptr += sizeof(LLR_T);                                                                                            \
      HARDBITS[i] = w;                                                                                                 \
    }                                                                                                                  \
  } while (false)

/**
 * Template RMS EVM calculation for different LLR
 */
#define EVM_RUN_TEMPLATE(LLR_T, SUFFIX)                                                                                \
  static inline float srslte_evm_run_##SUFFIX(srslte_evm_buffer_t*        q,                                           \
                                              const srslte_modem_table_t* modem_table,                                 \
                                              const cf_t*                 symbols,                                     \
                                              const LLR_T*                llr,                                         \
                                              uint32_t                    nof_bits)                                    \
  {                                                                                                                    \
    float evm_rms = NAN;                                                                                               \
                                                                                                                       \
    /* Return NAN if EVM buffers, modem table, LLR, symbols or bits missing*/                                          \
    if (!q || !modem_table || !modem_table->nbits_x_symbol || !llr || !symbols || !nof_bits) {                         \
      ERROR("Invalid inputs %p %p %p %p %d\n", q, modem_table, llr, symbols, nof_bits);                                \
      return evm_rms;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    /* Limit number of bits to the one supported by the buffer */                                                      \
    nof_bits = SRSLTE_MIN(q->max_bits, nof_bits);                                                                      \
                                                                                                                       \
    /* Calculate number of symbols */                                                                                  \
    uint32_t nsymbols = nof_bits / modem_table->nbits_x_symbol;                                                        \
                                                                                                                       \
    /* Hard decision */                                                                                                \
    HARD_DECISION(LLR_T, llr, q->hard_bits, nof_bits);                                                                 \
                                                                                                                       \
    /* Modulate */                                                                                                     \
    srslte_mod_modulate_bytes(modem_table, q->hard_bits, q->symbols, nof_bits);                                        \
                                                                                                                       \
    /* Compute symbol difference */                                                                                    \
    srslte_vec_sub_ccc(symbols, q->symbols, q->symbols, nsymbols);                                                     \
                                                                                                                       \
    /* Average squares */                                                                                              \
    float evm_pow = srslte_vec_avg_power_cf(q->symbols, nsymbols);                                                     \
                                                                                                                       \
    /* Convert measure to RMS */                                                                                       \
    evm_rms = sqrtf(evm_pow);                                                                                          \
                                                                                                                       \
    return evm_rms;                                                                                                    \
  }

/** @function srslte_evm_run_f
 * Calculates the Root Mean Squared EVM given a modulation table, complex symbols, floating point LLR (soft bits) and
 * the number of bits.
 *
 * @param q is the EVM buffers, need to be preallocated
 * @param modem_table points at the modulator table
 * @param symbols vector carrying the modulated complex symbols
 * @param llr softbits
 * @param nof_bits number of bits
 * @return the measured RMS EVM if no error occurs, otherwise it returns NAN
 */
EVM_RUN_TEMPLATE(float, f)

/** @function srslte_evm_run_s
 * Calculates the Root Mean Squared EVM given a modulation table, complex symbols, fixed integer 16 bit LLR (soft bits)
 * and the number of bits.
 *
 * @param q is the EVM buffers, need to be preallocated
 * @param modem_table points at the modulator table
 * @param symbols vector carrying the modulated complex symbols
 * @param llr softbits
 * @param nof_bits number of bits
 * @return the measured RMS EVM if no error occurs, otherwise it returns NAN
 */
EVM_RUN_TEMPLATE(int16_t, s)

/** @function srslte_evm_run_b
 * Calculates the Root Mean Squared EVM given a modulation table, complex symbols, fixed integer 8 bit LLR (soft bits)
 * and the number of bits.
 *
 * @param q is the EVM buffers, need to be preallocated
 * @param modem_table points at the modulator table
 * @param symbols vector carrying the modulated complex symbols
 * @param llr softbits
 * @param nof_bits number of bits
 * @return the measured RMS EVM if no error occurs, otherwise it returns NAN
 */
EVM_RUN_TEMPLATE(int8_t, b)

#undef EVM_RUN_TEMPLATE
#undef HARD_DECISION

static inline void srslte_evm_free(srslte_evm_buffer_t* q)
{
  // Check EVM buffer object exist
  if (q) {
    // Check hard bits were allocated
    if (q->hard_bits) {
      free(q->hard_bits);
    }

    // Check symbols were allocated
    if (q->symbols) {
      free(q->symbols);
    }

    // Free buffer object
    free(q);
  }
}

#endif // SRSLTE_EVM_H_
