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

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/utils/vector.h"
#include <strings.h>

/**
 * 36.211 6.6.1
 */
int srsran_sequence_pbch(srsran_sequence_t* seq, srsran_cp_t cp, uint32_t cell_id)
{
  return srsran_sequence_LTE_pr(seq, SRSRAN_CP_ISNORM(cp) ? 1920 : 1728, cell_id);
}

/**
 * 36.211 6.7.1
 */
int srsran_sequence_pcfich(srsran_sequence_t* seq, uint32_t nslot, uint32_t cell_id)
{
  return srsran_sequence_LTE_pr(seq, 32, (nslot / 2 + 1) * (2 * cell_id + 1) * 512 + cell_id);
}

/**
 * 36.211 6.9.1
 */
int srsran_sequence_phich(srsran_sequence_t* seq, uint32_t nslot, uint32_t cell_id)
{
  return srsran_sequence_LTE_pr(seq, 12, (nslot / 2 + 1) * (2 * cell_id + 1) * 512 + cell_id);
}

/**
 * 36.211 6.8.2
 */
int srsran_sequence_pdcch(srsran_sequence_t* seq, uint32_t nslot, uint32_t cell_id, uint32_t len)
{
  return srsran_sequence_LTE_pr(seq, len, (nslot / 2) * 512 + cell_id);
}

/**
 * 36.211 6.3.1
 */
static inline uint32_t sequence_pdsch_seed(uint16_t rnti, int q, uint32_t nslot, uint32_t cell_id)
{
  return (rnti << 14) + (q << 13) + ((nslot / 2) << 9) + cell_id;
}

int srsran_sequence_pdsch(srsran_sequence_t* seq, uint16_t rnti, int q, uint32_t nslot, uint32_t cell_id, uint32_t len)
{
  return srsran_sequence_LTE_pr(seq, len, sequence_pdsch_seed(rnti, q, nslot, cell_id));
}

void srsran_sequence_pdsch_apply_pack(const uint8_t* in,
                                      uint8_t*       out,
                                      uint16_t       rnti,
                                      int            q,
                                      uint32_t       nslot,
                                      uint32_t       cell_id,
                                      uint32_t       len)
{
  srsran_sequence_apply_packed(in, out, len, sequence_pdsch_seed(rnti, q, nslot, cell_id));
}

void srsran_sequence_pdsch_apply_f(const float* in,
                                   float*       out,
                                   uint16_t     rnti,
                                   int          q,
                                   uint32_t     nslot,
                                   uint32_t     cell_id,
                                   uint32_t     len)
{
  srsran_sequence_apply_f(in, out, len, sequence_pdsch_seed(rnti, q, nslot, cell_id));
}

void srsran_sequence_pdsch_apply_s(const int16_t* in,
                                   int16_t*       out,
                                   uint16_t       rnti,
                                   int            q,
                                   uint32_t       nslot,
                                   uint32_t       cell_id,
                                   uint32_t       len)
{
  srsran_sequence_apply_s(in, out, len, sequence_pdsch_seed(rnti, q, nslot, cell_id));
}

void srsran_sequence_pdsch_apply_c(const int8_t* in,
                                   int8_t*       out,
                                   uint16_t      rnti,
                                   int           q,
                                   uint32_t      nslot,
                                   uint32_t      cell_id,
                                   uint32_t      len)
{
  srsran_sequence_apply_c(in, out, len, sequence_pdsch_seed(rnti, q, nslot, cell_id));
}

/**
 * 36.211 5.3.1
 */
static inline uint32_t sequence_pusch_seed(uint16_t rnti, uint32_t nslot, uint32_t cell_id)
{
  return (rnti << 14) + ((nslot / 2) << 9) + cell_id;
}

int srsran_sequence_pusch(srsran_sequence_t* seq, uint16_t rnti, uint32_t nslot, uint32_t cell_id, uint32_t len)
{
  return srsran_sequence_LTE_pr(seq, len, sequence_pusch_seed(rnti, nslot, cell_id));
}

void srsran_sequence_pusch_apply_pack(const uint8_t* in,
                                      uint8_t*       out,
                                      uint16_t       rnti,
                                      uint32_t       nslot,
                                      uint32_t       cell_id,
                                      uint32_t       len)
{
  srsran_sequence_apply_packed(in, out, len, sequence_pusch_seed(rnti, nslot, cell_id));
}

void srsran_sequence_pusch_apply_s(const int16_t* in,
                                   int16_t*       out,
                                   uint16_t       rnti,
                                   uint32_t       nslot,
                                   uint32_t       cell_id,
                                   uint32_t       len)
{
  srsran_sequence_apply_s(in, out, len, sequence_pusch_seed(rnti, nslot, cell_id));
}

void srsran_sequence_pusch_gen_unpack(uint8_t* out, uint16_t rnti, uint32_t nslot, uint32_t cell_id, uint32_t len)
{
  srsran_vec_u8_zero(out, len);

  srsran_sequence_apply_bit(out, out, len, sequence_pusch_seed(rnti, nslot, cell_id));
}

void srsran_sequence_pusch_apply_c(const int8_t* in,
                                   int8_t*       out,
                                   uint16_t      rnti,
                                   uint32_t      nslot,
                                   uint32_t      cell_id,
                                   uint32_t      len)
{
  srsran_sequence_apply_c(in, out, len, sequence_pusch_seed(rnti, nslot, cell_id));
}

/**
 * 36.211 5.4.2
 */
int srsran_sequence_pucch(srsran_sequence_t* seq, uint16_t rnti, uint32_t nslot, uint32_t cell_id)
{
  return srsran_sequence_LTE_pr(seq, 12 * 4, ((((nslot / 2) + 1) * (2 * cell_id + 1)) << 16) + rnti);
}

int srsran_sequence_pmch(srsran_sequence_t* seq, uint32_t nslot, uint32_t mbsfn_id, uint32_t len)
{
  bzero(seq, sizeof(srsran_sequence_t));
  return srsran_sequence_LTE_pr(seq, len, (((nslot / 2) << 9) + mbsfn_id));
}

/**
 * 36.211 6.6.1 and 10.2.4.1 (13.2.0)
 */
int srsran_sequence_npbch(srsran_sequence_t* seq, srsran_cp_t cp, uint32_t cell_id)
{
  bzero(seq, sizeof(srsran_sequence_t));
  return srsran_sequence_LTE_pr(seq, SRSRAN_NBIOT_NPBCH_NOF_TOTAL_BITS, cell_id);
}

/**
 * 36.211 6.6.1 and 10.2.4.4 (14.2.0)
 */
int srsran_sequence_npbch_r14(srsran_sequence_t* seq, uint32_t n_id_ncell, uint32_t nf)
{
  bzero(seq, sizeof(srsran_sequence_t));
  return srsran_sequence_LTE_pr(seq,
                                SRSRAN_NBIOT_NPBCH_NOF_BITS_SF,
                                (n_id_ncell + 1) * (((nf % 8) + 1) * ((nf % 8) + 1) * ((nf % 8) + 1)) * 512 +
                                    n_id_ncell);
}

/**
 * 36.211 6.3.1 and 10.2.3.1 (13.2.0)
 */
int srsran_sequence_npdsch(srsran_sequence_t* seq,
                           uint16_t           rnti,
                           int                q,
                           uint32_t           nf,
                           uint32_t           nslot,
                           uint32_t           cell_id,
                           uint32_t           len)
{
  bzero(seq, sizeof(srsran_sequence_t));
  return srsran_sequence_LTE_pr(seq, len, (rnti << 14) + ((nf % 2) << 13) + ((nslot / 2) << 9) + cell_id);
}

/**
 * 36.211 6.3.1 and 10.2.3.1 (14.2.0)
 */
int srsran_sequence_npdsch_bcch_r14(srsran_sequence_t* seq, uint32_t nf, uint32_t n_id_ncell, uint32_t len)
{
  bzero(seq, sizeof(srsran_sequence_t));
  return srsran_sequence_LTE_pr(seq, len, (0xffff << 15) + (n_id_ncell + 1) * ((nf % 61) + 1));
}

/**
 * 36.211 6.6.1 and 10.2.5.2 (13.2.0)
 */
int srsran_sequence_npdcch(srsran_sequence_t* seq, uint32_t nslot, uint32_t cell_id, uint32_t len)
{
  bzero(seq, sizeof(srsran_sequence_t));
  return srsran_sequence_LTE_pr(seq, len, (nslot / 2) * 512 + cell_id);
}

/**
 * 36.211 5.3.1 and 10.1.3.1 (13.2.0)
 */
int srsran_sequence_npusch(srsran_sequence_t* seq,
                           uint16_t           rnti,
                           uint32_t           nf,
                           uint32_t           nslot,
                           uint32_t           cell_id,
                           uint32_t           len)
{
  bzero(seq, sizeof(srsran_sequence_t));
  return srsran_sequence_LTE_pr(seq, len, (rnti << 14) + ((nf % 2) << 13) + ((nslot / 2) << 9) + cell_id);
}

/**
 * 36.211 7.2.0 and 10.1.6.1 (13.2.0)
 */
int srsran_sequence_nprach(srsran_sequence_t* seq, uint32_t cell_id)
{
  bzero(seq, sizeof(srsran_sequence_t));
  return srsran_sequence_LTE_pr(seq, 1600, cell_id);
}