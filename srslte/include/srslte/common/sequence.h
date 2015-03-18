/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef LTESEQ_
#define LTESEQ_

#include "srslte/config.h"
#include "srslte/common/phy_common.h"

typedef struct SRSLTE_API {
  uint8_t *c;
  uint32_t len;
} sequence_t;

SRSLTE_API int sequence_init(sequence_t *q, uint32_t len);

SRSLTE_API void sequence_free(sequence_t *q);

SRSLTE_API int sequence_LTE_pr(sequence_t *q, 
                               uint32_t len, 
                               uint32_t seed);

SRSLTE_API void sequence_set_LTE_pr(sequence_t *q, 
                                    uint32_t seed); 

SRSLTE_API int sequence_pbch(sequence_t *seq, 
                             lte_cp_t cp, 
                             uint32_t cell_id);

SRSLTE_API int sequence_pcfich(sequence_t *seq, 
                               uint32_t nslot, 
                               uint32_t cell_id);

SRSLTE_API int sequence_phich(sequence_t *seq, 
                              uint32_t nslot, 
                              uint32_t cell_id);

SRSLTE_API int sequence_pdcch(sequence_t *seq, 
                              uint32_t nslot, 
                              uint32_t cell_id, 
                              uint32_t len);

SRSLTE_API int sequence_pdsch(sequence_t *seq, 
                              unsigned short rnti, 
                              int q,
                              uint32_t nslot, 
                              uint32_t cell_id, 
                              uint32_t len);

SRSLTE_API int sequence_pusch(sequence_t *seq, 
                              unsigned short rnti, 
                              uint32_t nslot, 
                              uint32_t cell_id, 
                              uint32_t len);

#endif
