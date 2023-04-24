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

/******************************************************************************
 *  File:         regs.h
 *
 *  Description:  Resource element mapping functions.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10
 *****************************************************************************/

#ifndef SRSRAN_REGS_H
#define SRSRAN_REGS_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include <stdbool.h>

#define REGS_PHICH_NSYM 12
#define REGS_PHICH_REGS_X_GROUP 3

#define REGS_PCFICH_NSYM 16
#define REGS_PCFICH_NREGS 4

#define REGS_RE_X_REG 4

typedef struct SRSRAN_API {
  uint32_t k[4];
  uint32_t k0;
  uint32_t l;
  bool     assigned;
} srsran_regs_reg_t;

typedef struct SRSRAN_API {
  uint32_t            nof_regs;
  srsran_regs_reg_t** regs;
} srsran_regs_ch_t;

typedef struct SRSRAN_API {
  srsran_cell_t cell;
  uint32_t      max_ctrl_symbols;
  uint32_t      ngroups_phich;
  uint32_t      ngroups_phich_m1;

  srsran_phich_r_t      phich_res;
  srsran_phich_length_t phich_len;

  srsran_regs_ch_t  pcfich;
  srsran_regs_ch_t* phich;    // there are several phich
  srsran_regs_ch_t  pdcch[3]; /* PDCCH indexing, permutation and interleaving is computed for
             the three possible CFI value */

  uint32_t           phich_mi;
  uint32_t           nof_regs;
  srsran_regs_reg_t* regs;
} srsran_regs_t;

SRSRAN_API int srsran_regs_init(srsran_regs_t* h, srsran_cell_t cell);

SRSRAN_API int srsran_regs_init_opts(srsran_regs_t* h, srsran_cell_t cell, uint32_t phich_mi, bool mbsfn_or_sf1_6_tdd);

SRSRAN_API void srsran_regs_free(srsran_regs_t* h);

SRSRAN_API int srsran_regs_pdcch_nregs(srsran_regs_t* h, uint32_t cfi);

SRSRAN_API int srsran_regs_pdcch_ncce(srsran_regs_t* h, uint32_t cfi);

SRSRAN_API int srsran_regs_pcfich_put(srsran_regs_t* h, cf_t symbols[REGS_PCFICH_NSYM], cf_t* slot_symbols);

SRSRAN_API int srsran_regs_pcfich_get(srsran_regs_t* h, cf_t* slot_symbols, cf_t symbols[REGS_PCFICH_NSYM]);

SRSRAN_API uint32_t srsran_regs_phich_nregs(srsran_regs_t* h);

SRSRAN_API int
srsran_regs_phich_add(srsran_regs_t* h, cf_t symbols[REGS_PHICH_NSYM], uint32_t ngroup, cf_t* slot_symbols);

SRSRAN_API int
srsran_regs_phich_get(srsran_regs_t* h, cf_t* slot_symbols, cf_t symbols[REGS_PHICH_NSYM], uint32_t ngroup);

SRSRAN_API uint32_t srsran_regs_phich_ngroups(srsran_regs_t* h);

SRSRAN_API uint32_t srsran_regs_phich_ngroups_m1(srsran_regs_t* h);

SRSRAN_API int srsran_regs_phich_reset(srsran_regs_t* h, cf_t* slot_symbols);

SRSRAN_API int srsran_regs_pdcch_put(srsran_regs_t* h, uint32_t cfi, cf_t* d, cf_t* slot_symbols);

SRSRAN_API int srsran_regs_pdcch_put_offset(srsran_regs_t* h,
                                            uint32_t       cfi,
                                            cf_t*          d,
                                            cf_t*          slot_symbols,
                                            uint32_t       start_reg,
                                            uint32_t       nof_regs);

SRSRAN_API int srsran_regs_pdcch_get(srsran_regs_t* h, uint32_t cfi, cf_t* slot_symbols, cf_t* d);

SRSRAN_API int srsran_regs_pdcch_get_offset(srsran_regs_t* h,
                                            uint32_t       cfi,
                                            cf_t*          slot_symbols,
                                            cf_t*          d,
                                            uint32_t       start_reg,
                                            uint32_t       nof_regs);

#endif // SRSRAN_REGS_H
