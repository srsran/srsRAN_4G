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

/******************************************************************************
 *  File:         regs.h
 *
 *  Description:  Resource element mapping functions.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10
 *****************************************************************************/

#ifndef SRSLTE_REGS_H
#define SRSLTE_REGS_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include <stdbool.h>

#define REGS_PHICH_NSYM 12
#define REGS_PHICH_REGS_X_GROUP 3

#define REGS_PCFICH_NSYM 16
#define REGS_PCFICH_NREGS 4

#define REGS_RE_X_REG 4

typedef struct SRSLTE_API {
  uint32_t k[4];
  uint32_t k0;
  uint32_t l;
  bool     assigned;
} srslte_regs_reg_t;

typedef struct SRSLTE_API {
  uint32_t            nof_regs;
  srslte_regs_reg_t** regs;
} srslte_regs_ch_t;

typedef struct SRSLTE_API {
  srslte_cell_t cell;
  uint32_t      max_ctrl_symbols;
  uint32_t      ngroups_phich;
  uint32_t      ngroups_phich_m1;

  srslte_phich_r_t      phich_res;
  srslte_phich_length_t phich_len;

  srslte_regs_ch_t  pcfich;
  srslte_regs_ch_t* phich;    // there are several phich
  srslte_regs_ch_t  pdcch[3]; /* PDCCH indexing, permutation and interleaving is computed for
             the three possible CFI value */

  uint32_t           phich_mi;
  uint32_t           nof_regs;
  srslte_regs_reg_t* regs;
} srslte_regs_t;

SRSLTE_API int srslte_regs_init(srslte_regs_t* h, srslte_cell_t cell);

SRSLTE_API int srslte_regs_init_opts(srslte_regs_t* h, srslte_cell_t cell, uint32_t phich_mi, bool mbsfn_or_sf1_6_tdd);

SRSLTE_API void srslte_regs_free(srslte_regs_t* h);

SRSLTE_API int srslte_regs_pdcch_nregs(srslte_regs_t* h, uint32_t cfi);

SRSLTE_API int srslte_regs_pdcch_ncce(srslte_regs_t* h, uint32_t cfi);

SRSLTE_API int srslte_regs_pcfich_put(srslte_regs_t* h, cf_t symbols[REGS_PCFICH_NSYM], cf_t* slot_symbols);

SRSLTE_API int srslte_regs_pcfich_get(srslte_regs_t* h, cf_t* slot_symbols, cf_t symbols[REGS_PCFICH_NSYM]);

SRSLTE_API uint32_t srslte_regs_phich_nregs(srslte_regs_t* h);

SRSLTE_API int
srslte_regs_phich_add(srslte_regs_t* h, cf_t symbols[REGS_PHICH_NSYM], uint32_t ngroup, cf_t* slot_symbols);

SRSLTE_API int
srslte_regs_phich_get(srslte_regs_t* h, cf_t* slot_symbols, cf_t symbols[REGS_PHICH_NSYM], uint32_t ngroup);

SRSLTE_API uint32_t srslte_regs_phich_ngroups(srslte_regs_t* h);

SRSLTE_API uint32_t srslte_regs_phich_ngroups_m1(srslte_regs_t* h);

SRSLTE_API int srslte_regs_phich_reset(srslte_regs_t* h, cf_t* slot_symbols);

SRSLTE_API int srslte_regs_pdcch_put(srslte_regs_t* h, uint32_t cfi, cf_t* d, cf_t* slot_symbols);

SRSLTE_API int srslte_regs_pdcch_put_offset(srslte_regs_t* h,
                                            uint32_t       cfi,
                                            cf_t*          d,
                                            cf_t*          slot_symbols,
                                            uint32_t       start_reg,
                                            uint32_t       nof_regs);

SRSLTE_API int srslte_regs_pdcch_get(srslte_regs_t* h, uint32_t cfi, cf_t* slot_symbols, cf_t* d);

SRSLTE_API int srslte_regs_pdcch_get_offset(srslte_regs_t* h,
                                            uint32_t       cfi,
                                            cf_t*          slot_symbols,
                                            cf_t*          d,
                                            uint32_t       start_reg,
                                            uint32_t       nof_regs);

#endif // SRSLTE_REGS_H
