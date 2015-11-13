/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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


#include <strings.h>
#include "srslte/common/phy_common.h"
#include "srslte/common/sequence.h"

/**
 * 36.211 6.6.1
 */
int srslte_sequence_pbch(srslte_sequence_t *seq, srslte_cp_t cp, uint32_t cell_id) {
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, SRSLTE_CP_ISNORM(cp)?1920:1728, cell_id);
}

/**
 * 36.211 6.7.1
 */
int srslte_sequence_pcfich(srslte_sequence_t *seq, uint32_t nslot, uint32_t cell_id) {
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, 32, (nslot/2+1) * (2*cell_id + 1) * 512 + cell_id);
}


/**
 * 36.211 6.9.1
 */
int srslte_sequence_phich(srslte_sequence_t *seq, uint32_t nslot, uint32_t cell_id) {
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, 12, (nslot/2+1) * (2*cell_id + 1) * 512 + cell_id);
}

/**
 * 36.211 6.8.2
 */
int srslte_sequence_pdcch(srslte_sequence_t *seq, uint32_t nslot, uint32_t cell_id, uint32_t len) {
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, len, (nslot/2) * 512 + cell_id);
}

/**
 * 36.211 6.3.1
 */
int srslte_sequence_pdsch(srslte_sequence_t *seq, uint16_t rnti, int q, uint32_t nslot, uint32_t cell_id, uint32_t len) {
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, len, (rnti<<14) + (q<<13) + ((nslot/2)<<9) + cell_id);
}

/**
 * 36.211 5.3.1
 */
int srslte_sequence_pusch(srslte_sequence_t *seq, uint16_t rnti, uint32_t nslot, uint32_t cell_id, uint32_t len) {
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, len, (rnti<<14) + ((nslot/2)<<9) + cell_id);
}

/**
 * 36.211 5.4.2
 */
int srslte_sequence_pucch(srslte_sequence_t *seq, uint16_t rnti, uint32_t nslot, uint32_t cell_id) {
  bzero(seq, sizeof(srslte_sequence_t));
  return srslte_sequence_LTE_pr(seq, 20, ((((nslot/2)+1)*(2*cell_id+1))<<16)+rnti);
}
