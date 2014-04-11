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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "lte/common/base.h"
#include "lte/phch/regs.h"
#include "lte/utils/debug.h"

regs_reg_t *regs_find_reg(regs_t *h, int k, int l);




/***************************************************************
 *
 * PDCCH REG ALLOCATION
 *
 ***************************************************************/

/** Initialize REGs for PDCCH
 * 36.211 10.3 section 6.8.5
 */
int regs_pdcch_init(regs_t *h) {
	return 0;
}

void regs_pdcch_free(regs_t *h) {
	if (h->pdcch) {
		free(h->pdcch);
	}
}





/***************************************************************
 *
 * PHICH REG ALLOCATION
 *
 ***************************************************************/

/** Initialize REGs for PHICH
 * 36.211 10.3 section 6.9.3
 */
int regs_phich_init(regs_t *h) {
	float ng;
	int i,ni,li,n[3],nreg,mi;
	regs_reg_t **regs_phich[3];
	int ret = -1;

	switch(h->phich_res) {
	case R_1_6:
		ng = (float) 1/6;
		break;
	case R_1_2:
		ng = (float) 1/2;
		break;
	case R_1:
		ng = 1;
		break;
	case R_2:
		ng = 2;
		break;
	}
	h->ngroups_phich = (int) ceilf(ng * ((float) h->nof_prb/8));
	h->phich = malloc(sizeof(regs_ch_t) * h->ngroups_phich);
	if (!h->phich) {
		perror("malloc");
		return -1;
	}
	INFO("Creating %d PHICH mapping units. %s length, Ng=%.2f\n",h->ngroups_phich,
			h->phich_len==PHICH_EXT?"Extended":"Normal",ng);
	for (i=0;i<h->ngroups_phich;i++) {
		h->phich[i].nof_regs = REGS_PHICH_REGS_X_GROUP;
		h->phich[i].regs = malloc(sizeof(regs_reg_t*) * REGS_PHICH_REGS_X_GROUP);
		if (!h->phich[i].regs) {
			perror("malloc");
			goto clean_and_exit;
		}
	}

	/** Here begins the mapping algorithm */

	/* Step 2. Count REGs not assigned to PCFICH */
	bzero(n, 3*sizeof(int));
	for (i=0;i<h->nof_regs;i++) {
		if (h->regs[i].l < 3 && !h->regs[i].assigned) {
			n[h->regs[i].l]++;
		}
	}

	bzero(regs_phich, sizeof(regs_reg_t*) * 3);
	for (i=0;i<3;i++) {
		regs_phich[i] = malloc(n[i] * sizeof(regs_reg_t*));
		if (!regs_phich[i]) {
			perror("malloc");
			goto clean_and_exit;
		}
	}

	bzero(n, 3 * sizeof(int));
	/* Step 3. Number REGs not assigned to PCFICH */
	for (i=0;i<h->nof_regs;i++) {
		// they are already sorted starting from the REG with the lowest frequency-domain index
		if (h->regs[i].l < 3 && !h->regs[i].assigned) {
			regs_phich[h->regs[i].l][n[h->regs[i].l]++] = &h->regs[i];
		}
	}

	nreg=0;
	for (mi=0;mi<h->ngroups_phich;mi++) { // here ngroups is the number of mapping units
		for (i=0;i<3;i++) {
			li=h->phich_len==PHICH_EXT?i:0; // Step 7
			ni=((h->cell_id*n[li]/n[0])+mi+i*n[li]/3) % n[li]; // Step 8
			h->phich[mi].regs[i] = regs_phich[li][ni];
			h->phich[mi].regs[i]->assigned = true;
			INFO("Assigned PHICH REG#%d (%d,%d)\n",nreg,h->phich[mi].regs[i]->k0,li);
			nreg++;
		}
	}

	// now the number of mapping units = number of groups for normal cp. For extended cp
	// ngroups = 2 * number mapping units
	if (CP_ISEXT(h->cp)) {
		h->ngroups_phich *= 2;
	}
	ret = 0;

clean_and_exit:
	if (ret == -1) {
		if (h->phich) {
			for (i=0;i<h->ngroups_phich;i++) {
				if (h->phich[i].regs) {
					free(h->phich[i].regs);
				}
			}
			free(h->phich);
		}
	}
	for (i=0;i<3;i++) {
		if (regs_phich[i]) {
			free(regs_phich[i]);
		}
	}
	return ret;
}

void regs_phich_free(regs_t *h) {
	int i;
	if (h->phich) {
		if (CP_ISEXT(h->cp)) {
			h->ngroups_phich /= 2;
		}
		for (i=0;i<h->ngroups_phich;i++) {
			if (h->phich[i].regs) {
				free(h->phich[i].regs);
			}
		}
		free(h->phich);
	}
}

int regs_phich_ngroups(regs_t *h) {
	return h->ngroups_phich;
}

/**
 * Adds the PHICH symbols to the resource grid pointed by slot_symbols.
 *
 * Each subframe, the user shall call the regs_phich_reset function before adding PHICH symbols.
 *
 * Returns the number of written symbols, or -1 on error
 */
int regs_phich_add(regs_t *h, cf_t phich_symbols[REGS_PHICH_NSYM], int ngroup, cf_t *slot_symbols) {
	int i;
	if (ngroup < 0 || ngroup > h->ngroups_phich) {
		fprintf(stderr, "Error invalid ngroup %d\n", ngroup);
		return -1;
	}
	if (CP_ISEXT(h->cp)) {
		ngroup /= 2;
	}
	regs_ch_t *rch = &h->phich[ngroup];
	for (i = 0; i < rch->nof_regs && i*REGS_RE_X_REG < REGS_PHICH_NSYM; i++) {
		regs_add_reg(rch->regs[i], &phich_symbols[i*REGS_RE_X_REG], slot_symbols, h->nof_prb);
	}
	return i*REGS_RE_X_REG;
}

/**
 * Resets the PHICH symbols
 *
 * Returns the number of written symbols, or -1 on error
 */
int regs_phich_reset(regs_t *h, cf_t *slot_symbols) {
	int i;
	int ngroup, ng;
	for (ngroup = 0;ngroup < h->ngroups_phich;CP_ISEXT(h->cp)?ngroup+=2:ngroup++) {
		if (CP_ISEXT(h->cp)) {
			ng = ngroup/2;
		} else {
			ng = ngroup;
		}
		regs_ch_t *rch = &h->phich[ng];
		for (i = 0; i < rch->nof_regs && i*REGS_RE_X_REG < REGS_PHICH_NSYM; i++) {
			regs_reset_reg(rch->regs[i], slot_symbols, h->nof_prb);
		}
	}
	return 0;
}

/**
 * Gets the PHICH symbols from the resource grid pointed by slot_symbols
 *
 * Returns the number of written symbols, or -1 on error
 */
int regs_phich_get(regs_t *h, cf_t *slot_symbols, cf_t phich_symbols[REGS_PHICH_NSYM], int ngroup) {
	int i;
	if (ngroup < 0 || ngroup > h->ngroups_phich) {
		fprintf(stderr, "Error invalid ngroup %d\n", ngroup);
		return -1;
	}
	if (CP_ISEXT(h->cp)) {
		ngroup /= 2;
	}
	regs_ch_t *rch = &h->phich[ngroup];
	for (i = 0; i < rch->nof_regs && i*REGS_RE_X_REG < REGS_PHICH_NSYM; i++) {
		regs_get_reg(rch->regs[i], slot_symbols, &phich_symbols[i*REGS_RE_X_REG], h->nof_prb);
	}
	return i*REGS_RE_X_REG;
}














/***************************************************************
 *
 * PCFICH REG ALLOCATION
 *
 ***************************************************************/


/** Initialize REGs for PCFICH
 * 36.211 10.3 section 6.7.4
 */
int regs_pcfich_init(regs_t *h) {
	int i, k_hat, k;
	regs_ch_t *ch = &h->pcfich;

	ch->regs = malloc(sizeof(regs_reg_t*) * REGS_PCFICH_NREGS);
	if (!ch->regs) {
		perror("malloc");
		return -1;
	}
	ch->nof_regs = REGS_PCFICH_NREGS;

	INFO("PCFICH allocating %d regs. CellID: %d, PRB: %d\n", ch->nof_regs, h->cell_id, h->nof_prb);

	k_hat = (RE_X_RB / 2) * (h->cell_id % (2 * h->nof_prb));
	for (i = 0; i < REGS_PCFICH_NREGS; i++) {

		k = (k_hat + (i * h->nof_prb / 2) * (RE_X_RB / 2))
				% (h->nof_prb * RE_X_RB);
		ch->regs[i] = regs_find_reg(h, k, 0);
		if (!ch->regs[i]) {
			fprintf(stderr, "Error allocating PCFICH: REG (%d,0) not found\n",
					k);
			return -1;
		} else if (ch->regs[i]->assigned) {
			fprintf(stderr,
					"Error allocating PCFICH: REG (%d,0) already allocated\n",
					k);
			return -1;
		} else {
			ch->regs[i]->assigned = true;
			INFO("Assigned PCFICH REG#%d (%d,0)\n", i, k);
		}
	}
	return 0;
}

void regs_pcfich_free(regs_t *h) {
	if (h->pcfich.regs) {
		free(h->pcfich.regs);
	}
}

/**
 * Maps the PCFICH symbols to the resource grid pointed by slot_symbols
 *
 * Returns the number of written symbols, or -1 on error
 */
int regs_pcfich_put(regs_t *h, cf_t pcfich_symbols[REGS_PCFICH_NSYM], cf_t *slot_symbols) {
	regs_ch_t *rch = &h->pcfich;

	int i;
	for (i = 0; i < rch->nof_regs && i*REGS_RE_X_REG < REGS_PCFICH_NSYM; i++) {
		regs_put_reg(rch->regs[i], &pcfich_symbols[i*REGS_RE_X_REG], slot_symbols, h->nof_prb);
	}
	return i*REGS_RE_X_REG;
}

/**
 * Gets the PCFICH symbols from the resource grid pointed by slot_symbols
 *
 * Returns the number of written symbols, or -1 on error
 */
int regs_pcfich_get(regs_t *h, cf_t *slot_symbols, cf_t ch_data[REGS_PCFICH_NSYM]) {
	regs_ch_t *rch = &h->pcfich;
	int i;
	for (i = 0; i < rch->nof_regs && i*REGS_RE_X_REG < REGS_PCFICH_NSYM; i++) {
		regs_get_reg(rch->regs[i], slot_symbols, &ch_data[i*REGS_RE_X_REG], h->nof_prb);
	}
	return i*REGS_RE_X_REG;
}














/***************************************************************
 *
 * COMMON FUNCTIONS
 *
 ***************************************************************/

regs_reg_t *regs_find_reg(regs_t *h, int k, int l) {
	int i;
	for (i=0;i<h->nof_regs;i++) {
		if (h->regs[i].l == l && h->regs[i].k0 == k) {
			return &h->regs[i];
		}
	}
	return NULL;
}

/**
 * Returns the number of REGs in a PRB
 * 36.211 Section 6.2.4
 */
int regs_num_x_symbol(int symbol, int refs_in_symbol1, lte_cp_t cp) {

	switch (symbol) {
	case 0:
		return 2;
	case 1:
		switch (refs_in_symbol1) {
		case 1:
		case 2:
			return 3;
		case 4:
			return 2;
		default:
			return -1;
		}
		break;
	case 2:
		return 3;
	case 3:
		if (CP_ISNORM(cp)) {
			return 3;
		} else {
			return 2;
		}
	default:
		return -1;
	}
}

/**
 * Initializes the indices of a REG
 * 36.211 Section 6.2.4
 */
int regs_reg_init(regs_reg_t *reg, int symbol, int nreg, int k0, int maxreg, int vo) {
	int i, j, z;

	reg->l = symbol;
	reg->assigned = false;
	reg->k0 = k0 + nreg * 6;

	switch (maxreg) {
	case 2:
		/* there are two references in the middle */
		j = z = 0;
		for (i = 0; i < vo; i++) {
			reg->k[j] = k0 + nreg * 6 + i;
			j++;
		}
		for (i = 0; i < 2; i++) {
			reg->k[j] = k0 + nreg * 6 + i + vo + 1;
			j++;
		}
		z = j;
		for (i = 0; i < 4 - z; i++) {
			reg->k[j] = k0 + nreg * 6 + vo + 3 + i + 1;
			j++;
		}
		if (j != 4) {
			fprintf(stderr, "Something went wrong: expected 2 references\n");
			return -1;
		}
		break;

	case 3:
		/* there is no reference */
		for (i = 0; i < 4; i++) {
			reg->k[i] = k0 + nreg * 4 + i;
		}
		break;
	default:
		fprintf(stderr, "Invalid number of REGs per PRB: %d\n", maxreg);
		return -1;
	}
	return 0;
}

void regs_free(regs_t *h) {
	if (h->regs) {
		free(h->regs);
	}
	regs_pcfich_free(h);
	regs_phich_free(h);
	regs_pdcch_free(h);

	bzero(h, sizeof(regs_t));
}

/**
 * Initializes REGs structure.
 * Sets all REG indices and initializes PCFICH, PHICH and PDCCH REGs
 * Returns 0 if OK, -1 on error
 */
int regs_init(regs_t *h, int cell_id, int nof_prb, int refs_in_symbol1,
		phich_resources_t phich_res, phich_length_t phich_len, lte_cp_t cp) {
	int ret = -1;
	int i, j, n, p, k;
	int vo = cell_id % 3;
	int max_ctrl_symbols = nof_prb<10?4:3;

	bzero(h, sizeof(regs_t));

	h->cell_id = cell_id;
	h->nof_prb = nof_prb;
	h->max_ctrl_symbols = max_ctrl_symbols;
	h->phich_res = phich_res;
	h->phich_len = phich_len;
	h->cp = cp;
	h->refs_in_symbol1 = refs_in_symbol1;

	h->nof_regs = 0;
	for (i = 0; i < max_ctrl_symbols; i++) {
		n = regs_num_x_symbol(i, refs_in_symbol1, cp);
		if (n == -1) {
			return -1;
		}
		h->nof_regs += nof_prb * n;
	}
	INFO("Indexing %d REGs. CellId: %d, %d PRB, CP: %s\n", h->nof_regs, h->cell_id, h->nof_prb,
			CP_ISNORM(cp)?"Normal":"Extended");
	h->regs = malloc(sizeof(regs_reg_t) * h->nof_regs);
	if (!h->regs) {
		perror("malloc");
		goto clean_and_exit;
	}

	k = 0;
	for (i = 0; i < max_ctrl_symbols; i++) {
		n = regs_num_x_symbol(i, refs_in_symbol1, cp);
		for (p = 0; p < nof_prb; p++) {
			for (j = 0; j < n; j++) {
				if (regs_reg_init(&h->regs[k], i, j, p * RE_X_RB, n, vo)) {
					fprintf(stderr, "Error initializing REGs\n");
					goto clean_and_exit;
				}
				DEBUG("Available REG #%3d: %d:%d:%d (k0=%d)\n", k, i, p, j,
						h->regs[k].k0);
				k++;
			}
		}
	}

	if (regs_pcfich_init(h)) {
		fprintf(stderr, "Error initializing PCFICH REGs\n");
		goto clean_and_exit;
	}
	if (regs_phich_init(h)) {
		fprintf(stderr, "Error initializing PHICH REGs\n");
		goto clean_and_exit;
	}
	if (regs_pdcch_init(h)) {
		fprintf(stderr, "Error initializing PDCCH REGs\n");
		goto clean_and_exit;
	}

	ret = 0;

	clean_and_exit: if (ret == -1) {
		regs_free(h);
	}
	return ret;
}

#define REG_IDX(r, i, n) r->k[i]+r->l*n*RE_X_RB

/**
 * Puts one REG data (4 symbols) in the slot symbols array
 */
int regs_put_reg(regs_reg_t *reg, cf_t *reg_data, cf_t *slot_symbols, int nof_prb) {
	int i;
	for (i = 0; i < REGS_RE_X_REG; i++) {
		if (reg->assigned) {
			DEBUG("PUT REG: i=%d, (k=%d,l=%d)\n", i, REG_IDX(reg, i, nof_prb),reg->l);
			slot_symbols[REG_IDX(reg, i, nof_prb)] = reg_data[i];
		} else {
			fprintf(stderr, "Error REG not assigned\n");
			return -1;
		}
	}
	return REGS_RE_X_REG;
}

/**
 * Adds one REG data (4 symbols) in the slot symbols array
 * Used by PHICH
 */
int regs_add_reg(regs_reg_t *reg, cf_t *reg_data, cf_t *slot_symbols, int nof_prb) {
	int i;
	for (i = 0; i < REGS_RE_X_REG; i++) {
		if (reg->assigned) {
			slot_symbols[REG_IDX(reg, i, nof_prb)] += reg_data[i];
			DEBUG("ADD REG: i=%d, (k=%d,l=%d): %.1f+%.1fi\n", i, REG_IDX(reg, i, nof_prb),reg->l,
					__real__ slot_symbols[REG_IDX(reg, i, nof_prb)],
					__imag__ slot_symbols[REG_IDX(reg, i, nof_prb)]);
		} else {
			fprintf(stderr, "Error REG not assigned\n");
			return -1;
		}
	}
	return REGS_RE_X_REG;
}


/**
 * Reset REG data (4 symbols) in the slot symbols array
 */
int regs_reset_reg(regs_reg_t *reg, cf_t *slot_symbols, int nof_prb) {
	int i;
	for (i = 0; i < REGS_RE_X_REG; i++) {
		if (reg->assigned) {
			DEBUG("RESET REG: i=%d, (k=%d,l=%d)\n", i, REG_IDX(reg, i, nof_prb),reg->l);
			slot_symbols[REG_IDX(reg, i, nof_prb)] = 0;
		} else {
			fprintf(stderr, "Error REG not assigned\n");
			return -1;
		}
	}
	return REGS_RE_X_REG;
}

/**
 * Gets one REG data (4 symbols) from the slot symbols array
 */
int regs_get_reg(regs_reg_t *reg, cf_t *slot_symbols, cf_t *reg_data, int nof_prb) {
	int i;
	for (i = 0; i < REGS_RE_X_REG; i++) {
		if (reg->assigned) {
			reg_data[i] = slot_symbols[REG_IDX(reg, i, nof_prb)];
			DEBUG("GET REG: i=%d, (k=%d,l=%d): %.1f+%.1fi\n", i, REG_IDX(reg, i, nof_prb),reg->l,
					__real__ reg_data[i], __imag__ reg_data[i]);
		} else {
			fprintf(stderr, "Error REG not assigned\n");
			return -1;
		}
	}
	return REGS_RE_X_REG;
}

