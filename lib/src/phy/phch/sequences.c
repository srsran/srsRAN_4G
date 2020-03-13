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

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/common/sequence.h"
#include <strings.h>

/**
 * 36.211 6.6.1
 */
int srslte_sequence_pbch(srslte_sequence_t* seq, srslte_cp_t cp, uint32_t cell_id)
{
  return srslte_sequence_LTE_pr(seq, SRSLTE_CP_ISNORM(cp) ? 1920 : 1728, cell_id);
}

/**
 * 36.211 6.7.1
 */
int srslte_sequence_pcfich(srslte_sequence_t* seq, uint32_t nslot, uint32_t cell_id)
{
  return srslte_sequence_LTE_pr(seq, 32, (nslot / 2 + 1) * (2 * cell_id + 1) * 512 + cell_id);
}

/**
 * 36.211 6.9.1
 */
int srslte_sequence_phich(srslte_sequence_t* seq, uint32_t nslot, uint32_t cell_id)
{
  return srslte_sequence_LTE_pr(seq, 12, (nslot / 2 + 1) * (2 * cell_id + 1) * 512 + cell_id);
}

/**
 * 36.211 6.8.2
 */
int srslte_sequence_pdcch(srslte_sequence_t* seq, uint32_t nslot, uint32_t cell_id, uint32_t len)
{
  return srslte_sequence_LTE_pr(seq, len, (nslot / 2) * 512 + cell_id);
}

/**
 * 36.211 6.3.1
 */
int srslte_sequence_pdsch(srslte_sequence_t* seq, uint16_t rnti, int q, uint32_t nslot, uint32_t cell_id, uint32_t len)
{
  return srslte_sequence_LTE_pr(seq, len, (rnti << 14) + (q << 13) + ((nslot / 2) << 9) + cell_id);
}

/**
 * 36.211 5.3.1
 */
int srslte_sequence_pusch(srslte_sequence_t* seq, uint16_t rnti, uint32_t nslot, uint32_t cell_id, uint32_t len)
{
  return srslte_sequence_LTE_pr(seq, len, (rnti << 14) + ((nslot / 2) << 9) + cell_id);
}

/**
 * 36.211 5.4.2
 */
int srslte_sequence_pucch(srslte_sequence_t* seq, uint16_t rnti, uint32_t nslot, uint32_t cell_id)
{
  return srslte_sequence_LTE_pr(seq, 12 * 4, ((((nslot / 2) + 1) * (2 * cell_id + 1)) << 16) + rnti);
}

int srslte_sequence_pmch(srslte_sequence_t* seq, uint32_t nslot, uint32_t mbsfn_id, uint32_t len)
{
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, len, (((nslot / 2) << 9) + mbsfn_id));
}

/**
 * 36.211 6.6.1 and 10.2.4.1 (13.2.0)
 */
int srslte_sequence_npbch(srslte_sequence_t* seq, srslte_cp_t cp, uint32_t cell_id)
{
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, SRSLTE_NBIOT_NPBCH_NOF_TOTAL_BITS, cell_id);
}

/**
 * 36.211 6.6.1 and 10.2.4.4 (14.2.0)
 */
int srslte_sequence_npbch_r14(srslte_sequence_t* seq, uint32_t n_id_ncell, uint32_t nf)
{
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq,
                                SRSLTE_NBIOT_NPBCH_NOF_BITS_SF,
                                (n_id_ncell + 1) * (((nf % 8) + 1) * ((nf % 8) + 1) * ((nf % 8) + 1)) * 512 +
                                    n_id_ncell);
}

/**
 * 36.211 6.3.1 and 10.2.3.1 (13.2.0)
 */
int srslte_sequence_npdsch(srslte_sequence_t* seq,
                           uint16_t           rnti,
                           int                q,
                           uint32_t           nf,
                           uint32_t           nslot,
                           uint32_t           cell_id,
                           uint32_t           len)
{
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, len, (rnti << 14) + ((nf % 2) << 13) + ((nslot / 2) << 9) + cell_id);
}

/**
 * 36.211 6.3.1 and 10.2.3.1 (14.2.0)
 */
int srslte_sequence_npdsch_bcch_r14(srslte_sequence_t* seq, uint32_t nf, uint32_t n_id_ncell, uint32_t len)
{
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, len, (0xffff << 15) + (n_id_ncell + 1) * ((nf % 61) + 1));
}

/**
 * 36.211 6.6.1 and 10.2.5.2 (13.2.0)
 */
int srslte_sequence_npdcch(srslte_sequence_t* seq, uint32_t nslot, uint32_t cell_id, uint32_t len)
{
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, len, (nslot / 2) * 512 + cell_id);
}

/**
 * 36.211 5.3.1 and 10.1.3.1 (13.2.0)
 */
int srslte_sequence_npusch(srslte_sequence_t* seq,
                           uint16_t           rnti,
                           uint32_t           nf,
                           uint32_t           nslot,
                           uint32_t           cell_id,
                           uint32_t           len)
{
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, len, (rnti << 14) + ((nf % 2) << 13) + ((nslot / 2) << 9) + cell_id);
}

/**
 * 36.211 7.2.0 and 10.1.6.1 (13.2.0)
 */
int srslte_sequence_nprach(srslte_sequence_t* seq, uint32_t cell_id)
{
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, 1600, cell_id);
}