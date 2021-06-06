/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

/**********************************************************************************************
 *  File:         sequence.h
 *
 *  Description:  Pseudo Random Sequence generation. Sequences are defined by a length-31 Gold
 *                sequence.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.2
 *********************************************************************************************/

#ifndef SRSRAN_SEQUENCE_H
#define SRSRAN_SEQUENCE_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"

#define SRSRAN_SEQUENCE_MOD(X) ((uint32_t)((X) & (uint64_t)INT32_MAX))

typedef struct SRSRAN_API {
  uint32_t x1;
  uint32_t x2;
} srsran_sequence_state_t;

SRSRAN_API void srsran_sequence_state_init(srsran_sequence_state_t* s, uint32_t seed);

SRSRAN_API void srsran_sequence_state_gen_f(srsran_sequence_state_t* s, float value, float* out, uint32_t length);

SRSRAN_API void srsran_sequence_state_apply_f(srsran_sequence_state_t* s, const float* in, float* out, uint32_t length);

SRSRAN_API void
srsran_sequence_state_apply_c(srsran_sequence_state_t* s, const int8_t* in, int8_t* out, uint32_t length);

SRSRAN_API
void srsran_sequence_state_apply_bit(srsran_sequence_state_t* s, const uint8_t* in, uint8_t* out, uint32_t length);

SRSRAN_API void srsran_sequence_state_advance(srsran_sequence_state_t* s, uint32_t length);

typedef struct SRSRAN_API {
  uint8_t* c;
  uint8_t* c_bytes;
  float*   c_float;
  short*   c_short;
  int8_t*  c_char;
  uint32_t cur_len;
  uint32_t max_len;
} srsran_sequence_t;

SRSRAN_API int srsran_sequence_init(srsran_sequence_t* q, uint32_t len);

SRSRAN_API void srsran_sequence_free(srsran_sequence_t* q);

SRSRAN_API int srsran_sequence_LTE_pr(srsran_sequence_t* q, uint32_t len, uint32_t seed);

SRSRAN_API int srsran_sequence_set_LTE_pr(srsran_sequence_t* q, uint32_t len, uint32_t seed);

SRSRAN_API void srsran_sequence_apply_f(const float* in, float* out, uint32_t length, uint32_t seed);

SRSRAN_API void srsran_sequence_apply_s(const int16_t* in, int16_t* out, uint32_t length, uint32_t seed);

SRSRAN_API void srsran_sequence_apply_c(const int8_t* in, int8_t* out, uint32_t length, uint32_t seed);

SRSRAN_API void srsran_sequence_apply_packed(const uint8_t* in, uint8_t* out, uint32_t length, uint32_t seed);

SRSRAN_API void srsran_sequence_apply_bit(const uint8_t* in, uint8_t* out, uint32_t length, uint32_t seed);

SRSRAN_API int srsran_sequence_pbch(srsran_sequence_t* seq, srsran_cp_t cp, uint32_t cell_id);

SRSRAN_API int srsran_sequence_pcfich(srsran_sequence_t* seq, uint32_t nslot, uint32_t cell_id);

SRSRAN_API int srsran_sequence_phich(srsran_sequence_t* seq, uint32_t nslot, uint32_t cell_id);

SRSRAN_API int srsran_sequence_pdcch(srsran_sequence_t* seq, uint32_t nslot, uint32_t cell_id, uint32_t len);

SRSRAN_API int
srsran_sequence_pdsch(srsran_sequence_t* seq, uint16_t rnti, int q, uint32_t nslot, uint32_t cell_id, uint32_t len);

SRSRAN_API void srsran_sequence_pdsch_apply_pack(const uint8_t* in,
                                                 uint8_t*       out,
                                                 uint16_t       rnti,
                                                 int            q,
                                                 uint32_t       nslot,
                                                 uint32_t       cell_id,
                                                 uint32_t       len);

SRSRAN_API void srsran_sequence_pdsch_apply_f(const float* in,
                                              float*       out,
                                              uint16_t     rnti,
                                              int          q,
                                              uint32_t     nslot,
                                              uint32_t     cell_id,
                                              uint32_t     len);

SRSRAN_API void srsran_sequence_pdsch_apply_s(const int16_t* in,
                                              int16_t*       out,
                                              uint16_t       rnti,
                                              int            q,
                                              uint32_t       nslot,
                                              uint32_t       cell_id,
                                              uint32_t       len);

SRSRAN_API void srsran_sequence_pdsch_apply_c(const int8_t* in,
                                              int8_t*       out,
                                              uint16_t      rnti,
                                              int           q,
                                              uint32_t      nslot,
                                              uint32_t      cell_id,
                                              uint32_t      len);

SRSRAN_API int
srsran_sequence_pusch(srsran_sequence_t* seq, uint16_t rnti, uint32_t nslot, uint32_t cell_id, uint32_t len);

SRSRAN_API void srsran_sequence_pusch_apply_pack(const uint8_t* in,
                                                 uint8_t*       out,
                                                 uint16_t       rnti,
                                                 uint32_t       nslot,
                                                 uint32_t       cell_id,
                                                 uint32_t       len);

SRSRAN_API void srsran_sequence_pusch_apply_c(const int8_t* in,
                                              int8_t*       out,
                                              uint16_t      rnti,
                                              uint32_t      nslot,
                                              uint32_t      cell_id,
                                              uint32_t      len);

SRSRAN_API void srsran_sequence_pusch_apply_s(const int16_t* in,
                                              int16_t*       out,
                                              uint16_t       rnti,
                                              uint32_t       nslot,
                                              uint32_t       cell_id,
                                              uint32_t       len);

SRSRAN_API void
srsran_sequence_pusch_gen_unpack(uint8_t* out, uint16_t rnti, uint32_t nslot, uint32_t cell_id, uint32_t len);

SRSRAN_API int srsran_sequence_pucch(srsran_sequence_t* seq, uint16_t rnti, uint32_t nslot, uint32_t cell_id);

SRSRAN_API int srsran_sequence_pmch(srsran_sequence_t* seq, uint32_t nslot, uint32_t mbsfn_id, uint32_t len);

SRSRAN_API int srsran_sequence_npbch(srsran_sequence_t* seq, srsran_cp_t cp, uint32_t cell_id);

SRSRAN_API int srsran_sequence_npbch_r14(srsran_sequence_t* seq, uint32_t n_id_ncell, uint32_t nf);

SRSRAN_API int srsran_sequence_npdsch(srsran_sequence_t* seq,
                                      uint16_t           rnti,
                                      int                q,
                                      uint32_t           nf,
                                      uint32_t           nslot,
                                      uint32_t           cell_id,
                                      uint32_t           len);

SRSRAN_API int srsran_sequence_npdsch_bcch_r14(srsran_sequence_t* seq, uint32_t nf, uint32_t n_id_ncell, uint32_t len);

SRSRAN_API int srsran_sequence_npdcch(srsran_sequence_t* seq, uint32_t nslot, uint32_t cell_id, uint32_t len);

SRSRAN_API int srsran_sequence_npusch(srsran_sequence_t* seq,
                                      uint16_t           rnti,
                                      uint32_t           nf,
                                      uint32_t           nslot,
                                      uint32_t           cell_id,
                                      uint32_t           len);

SRSRAN_API int srsran_sequence_nprach(srsran_sequence_t* seq, uint32_t cell_id);

#endif // SRSRAN_SEQUENCE_H
