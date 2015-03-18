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


#ifndef _REGS_H_
#define _REGS_H_

#include <stdbool.h>
#include "srslte/config.h"
#include "srslte/common/phy_common.h"

#define REGS_PHICH_NSYM      12
#define REGS_PHICH_REGS_X_GROUP  3

#define REGS_PCFICH_NSYM  16
#define REGS_PCFICH_NREGS  4

#define REGS_RE_X_REG    4


typedef _Complex float cf_t;

typedef struct SRSLTE_API {
  uint32_t k[4];
  uint32_t k0;
  uint32_t l;
  bool assigned;
}regs_reg_t;

typedef struct SRSLTE_API {
  uint32_t nof_regs;
  regs_reg_t **regs;
}regs_ch_t;

typedef struct SRSLTE_API {
  srslte_cell_t cell;
  uint32_t max_ctrl_symbols;
  uint32_t cfi;
  bool cfi_initiated;
  uint32_t ngroups_phich;
  
  srslte_phich_resources_t phich_res;
  srslte_phich_length_t phich_len;
  
  regs_ch_t pcfich;
  regs_ch_t *phich; // there are several phich
  regs_ch_t pdcch[3]; /* PDCCH indexing, permutation and interleaving is computed for
            the three possible CFI value */
  
  uint32_t nof_regs;
  regs_reg_t *regs;
}regs_t;

SRSLTE_API int regs_init(regs_t *h,                          
                         srslte_cell_t cell);

SRSLTE_API void regs_free(regs_t *h);
SRSLTE_API int regs_set_cfi(regs_t *h, 
                            uint32_t nof_ctrl_symbols);


SRSLTE_API uint32_t regs_pcfich_nregs(regs_t *h);
SRSLTE_API int regs_pcfich_put(regs_t *h, 
                               cf_t pcfich_symbols[REGS_PCFICH_NSYM], 
                               cf_t *slot_symbols);

SRSLTE_API int regs_pcfich_get(regs_t *h,
                               cf_t *slot_symbols, 
                               cf_t pcfich_symbols[REGS_PCFICH_NSYM]);

SRSLTE_API uint32_t regs_phich_nregs(regs_t *h);
SRSLTE_API int regs_phich_add(regs_t *h, 
                              cf_t phich_symbols[REGS_PHICH_NSYM], 
                              uint32_t ngroup, 
                              cf_t *slot_symbols);

SRSLTE_API int regs_phich_get(regs_t *h, 
                              cf_t *slot_symbols, 
                              cf_t phich_symbols[REGS_PHICH_NSYM], 
                              uint32_t ngroup);

SRSLTE_API uint32_t regs_phich_ngroups(regs_t *h);
SRSLTE_API int regs_phich_reset(regs_t *h, 
                                cf_t *slot_symbols);

SRSLTE_API int regs_pdcch_nregs(regs_t *h, uint32_t cfi);
SRSLTE_API int regs_pdcch_put(regs_t *h, 
                              cf_t *pdcch_symbols, 
                              cf_t *slot_symbols);

SRSLTE_API int regs_pdcch_put_offset(regs_t *h, 
                                     cf_t *pdcch_symbols, 
                                     cf_t *slot_symbols, 
                                     uint32_t start_reg, 
                                     uint32_t nof_regs);

SRSLTE_API int regs_pdcch_get(regs_t *h, 
                              cf_t *slot_symbols, 
                              cf_t *pdcch_symbols);

SRSLTE_API int regs_pdcch_get_offset(regs_t *h, 
                                     cf_t *slot_symbols, 
                                     cf_t *pdcch_symbols, 
                                     uint32_t start_reg, 
                                     uint32_t nof_regs);

#endif // REGS_H_


