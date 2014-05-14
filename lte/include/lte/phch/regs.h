/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
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
#include "lte/common/base.h"

#define REGS_PHICH_NSYM			12
#define REGS_PHICH_REGS_X_GROUP	3

#define REGS_PCFICH_NSYM	16
#define REGS_PCFICH_NREGS	4

#define REGS_RE_X_REG		4


typedef _Complex float cf_t;

typedef struct {
	int k[4];
	int k0;
	int l;
	bool assigned;
}regs_reg_t;

typedef struct {
	int nof_regs;
	regs_reg_t **regs;
}regs_ch_t;

typedef struct {
	int cell_id;
	int nof_prb;
	int max_ctrl_symbols;
	int ngroups_phich;
	int refs_in_symbol1;
	lte_cp_t cp;
	phich_resources_t phich_res;
	phich_length_t phich_len;
	int nof_cce;
	regs_ch_t pcfich;
	regs_ch_t *phich; // there are several phich
	regs_ch_t *pdcch; // there are several pdcch
	int nof_regs;
	regs_reg_t *regs;
}regs_t;

int regs_init(regs_t *h, int cell_id, int nof_prb, int refs_in_symbol1,
		phich_resources_t phich_res, phich_length_t phich_len, lte_cp_t cp);
void regs_free(regs_t *h);

int regs_put_reg(regs_reg_t *reg, cf_t *reg_data, cf_t *slot_symbols, int nof_prb);
int regs_add_reg(regs_reg_t *reg, cf_t *reg_data, cf_t *slot_symbols, int nof_prb);
int regs_get_reg(regs_reg_t *reg, cf_t *slot_symbols, cf_t *reg_data, int nof_prb);
int regs_reset_reg(regs_reg_t *reg, cf_t *slot_symbols, int nof_prb);

int regs_pcfich_put(regs_t *h, cf_t pcfich_symbols[REGS_PCFICH_NSYM], cf_t *slot_symbols);
int regs_pcfich_get(regs_t *h, cf_t *slot_symbols, cf_t ch_data[REGS_PCFICH_NSYM]);

int regs_phich_add(regs_t *h, cf_t phich_symbols[REGS_PHICH_NSYM], int ngroup, cf_t *slot_symbols);
int regs_phich_get(regs_t *h, cf_t *slot_symbols, cf_t phich_symbols[REGS_PHICH_NSYM], int ngroup);
int regs_phich_ngroups(regs_t *h);
int regs_phich_reset(regs_t *h, cf_t *slot_symbols);

int regs_pdcch_nregs(regs_t *h);

#endif
