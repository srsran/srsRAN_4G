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
#include <strings.h>
#include <math.h>
#include "lte/common/base.h"
#include "lte/utils/bit.h"
#include "lte/utils/vector.h"
#include "lte/utils/debug.h"
#include "lte/phch/ra.h"
#include "lte/utils/bit.h"

#include "tbs_tables.h"

#define min(a,b) (a<b?a:b)

/* Returns the number of RE in a PRB in a slot and subframe */
int ra_re_x_prb(int nsubframe, int nslot, int prb_idx, int nof_prb, int nof_ports,
		int nof_ctrl_symbols, lte_cp_t cp) {

	int re;
	bool skip_refs = false;

	if (nslot == 0) {
		re = (CP_NSYMB(cp) - nof_ctrl_symbols) * RE_X_RB;
	} else {
		re = CP_NSYMB(cp) * RE_X_RB;
	}

	/* if it's the prb in the middle, there are less RE due to PBCH and PSS/SSS */
	if ((nsubframe == 0 || nsubframe == 5) &&
			(prb_idx >= nof_prb/2-3 && prb_idx <= nof_prb/2+3)) {
		if (nsubframe == 0) {
			if (nslot == 0) {
				re = (CP_NSYMB(cp) - nof_ctrl_symbols - 2) * RE_X_RB;
			} else {
				if (CP_ISEXT(cp)) {
					re = (CP_NSYMB(cp) - 4) * RE_X_RB;
					skip_refs = true;
				} else {
					re = (CP_NSYMB(cp) - 4) * RE_X_RB + 2*nof_ports;
				}
			}
		} else if (nsubframe == 5) {
			if (nslot == 0) {
				re = (CP_NSYMB(cp) - nof_ctrl_symbols - 2) * RE_X_RB;
			}
		}
		if ((nof_prb%2) && (prb_idx == nof_prb/2-3 || prb_idx == nof_prb/2+3)) {
			if (nslot == 0) {
				re += 2 * RE_X_RB / 2;
			} else if (nsubframe == 0) {
				re += 4 * RE_X_RB / 2 - nof_ports;
				if (CP_ISEXT(cp)) {
					re -= nof_ports>2?2:nof_ports;
				}
			}
		}
	}

	// remove references
	if (!skip_refs) {
		switch(nof_ports) {
		case 1:
		case 2:
			re -= 2 * (nslot + 1) * nof_ports;
			break;
		case 4:
			if (nslot == 1) {
				re -= 12;
			} else {
				re -= 4;
				if (nof_ctrl_symbols == 1) {
					re -= 4;
				}
			}
			break;
		}
	}

	return re;
}

/* Computes the number of RE for each PRB in the prb_dist structure */
void ra_prb_get_re(ra_prb_t *prb_dist, int nof_prb, int nof_ports, int nof_ctrl_symbols, lte_cp_t cp) {
	int i, j, s;

	/* Set start symbol according to Section 7.1.6.4 in 36.213 */
	prb_dist->lstart = nof_ctrl_symbols;
	// Compute number of RE per subframe
	for (i=0;i<NSUBFRAMES_X_FRAME;i++) {
		for (s=0;s<2;s++) {
			for (j=0;j<prb_dist->slot[s].nof_prb;j++) {
				prb_dist->re_sf[i] += ra_re_x_prb(i, s, prb_dist->slot[s].prb_idx[j], nof_prb,
						nof_ports, nof_ctrl_symbols, cp);
			}
		}
	}
}


void ra_prb_fprint(FILE *f, ra_prb_slot_t *prb) {
	int i, j, nrows;
	nrows = (prb->nof_prb - 1)/ 25 + 1;
	for (j=0;j<nrows;j++) {
		for (i=0;i<min(25, prb->nof_prb-j*25);i++) {
			fprintf(f, "%3d, ", prb->prb_idx[j*25+i]);
		}
		fprintf(f, "\n");
	}
}

/** Compute PRB allocation for Downlink as defined in 8.1 of 36.213 */
int ra_prb_get_ul(ra_prb_slot_t *prb, ra_pusch_t *ra, int nof_prb) {
	int i;
	if (ra->type2_alloc.mode != t2_loc) {
		fprintf(stderr, "Uplink only accepts type2 localized scheduling\n");
		return -1;
	}
	for (i=0;i<ra->type2_alloc.L_crb;i++) {
		prb->prb_idx[i] = i+ra->type2_alloc.RB_start;
		prb->nof_prb++;
	}
	return 0;
}


/** Compute PRB allocation for Downlink as defined in 7.1.6 of 36.213 */
int ra_prb_get_dl(ra_prb_t *prb_dist, ra_pdsch_t *ra, int nof_prb) {
	int i, j;
	uint32_t bitmask;
	int P = ra_type0_P(nof_prb);
	int n_rb_rbg_subset, n_rb_type1;

	bzero(prb_dist, sizeof(ra_prb_t));
	switch(ra->alloc_type) {
	case alloc_type0:
		bitmask = ra->type0_alloc.rbg_bitmask;
		int nb = (int) ceilf((float)nof_prb/P);
		for (i=0;i<nb;i++) {
			if (bitmask & (1<<(nb-i-1))) {
				for (j=0;j<P;j++) {
					prb_dist->slot[0].prb_idx[prb_dist->slot[0].nof_prb] = i*P+j;
					prb_dist->slot[0].nof_prb++;
				}
			}
		}
		memcpy(&prb_dist->slot[1], &prb_dist->slot[0], sizeof(ra_prb_slot_t));
		break;
	case alloc_type1:
		n_rb_type1 = ra_type1_N_rb(nof_prb);
		if (ra->type1_alloc.rbg_subset < (nof_prb/P) % P) {
			n_rb_rbg_subset = ((nof_prb-1)/(P*P)) * P + P;
		} else if (ra->type1_alloc.rbg_subset == ((nof_prb/P) % P)) {
			n_rb_rbg_subset = ((nof_prb-1)/(P*P)) * P + ((nof_prb-1)%P)+1;
		} else {
			n_rb_rbg_subset = ((nof_prb-1)/(P*P)) * P;
		}
		int shift = ra->type1_alloc.shift?(n_rb_rbg_subset-n_rb_type1):0;
		bitmask = ra->type1_alloc.vrb_bitmask;
		for (i=0;i<n_rb_type1;i++) {
			if (bitmask & (1<<(n_rb_type1-i-1))) {
				prb_dist->slot[0].prb_idx[prb_dist->slot[0].nof_prb] = ((i+shift)/P)*P*P+
						ra->type1_alloc.rbg_subset*P+(i+shift)%P;
				prb_dist->slot[0].nof_prb++;
			}
		}
		memcpy(&prb_dist->slot[1], &prb_dist->slot[0], sizeof(ra_prb_slot_t));
		break;
	case alloc_type2:
		if (ra->type2_alloc.mode == t2_loc) {
			for (i=0;i<ra->type2_alloc.L_crb;i++) {
				prb_dist->slot[0].prb_idx[i] = i+ra->type2_alloc.RB_start;
				prb_dist->slot[0].nof_prb++;
			}
			memcpy(&prb_dist->slot[1], &prb_dist->slot[0], sizeof(ra_prb_slot_t));
		} else {
			/* Mapping of Virtual to Physical RB for distributed type is defined in
			 * 6.2.3.2 of 36.211
			 */
			int N_gap, N_tilde_vrb, n_tilde_vrb, n_tilde_prb, n_tilde2_prb, N_null, N_row, n_vrb;
			int n_tilde_prb_odd, n_tilde_prb_even;
			if (ra->type2_alloc.n_gap == t2_ng1) {
				N_tilde_vrb = nof_prb;
				N_gap = ra_type2_ngap(nof_prb, true);
			} else {
				N_tilde_vrb = 2*nof_prb;
				N_gap = ra_type2_ngap(nof_prb, false);
			}
			N_row = (int) ceilf((float) N_tilde_vrb/(4*P))*P;
			N_null = 4*N_row-N_tilde_vrb;
			for (i=0;i<ra->type2_alloc.L_crb;i++) {
				n_vrb = i+ra->type2_alloc.RB_start;
				n_tilde_vrb = n_vrb%N_tilde_vrb;
				n_tilde_prb = 2*N_row*(n_tilde_vrb % 2)+n_tilde_vrb/2+N_tilde_vrb*(n_vrb/N_tilde_vrb);
				n_tilde2_prb = N_row*(n_tilde_vrb % 4)+n_tilde_vrb/4+N_tilde_vrb*(n_vrb/N_tilde_vrb);

				if (N_null != 0 && n_tilde_vrb >= (N_tilde_vrb - N_null) && (n_tilde_vrb%2) == 1) {
					n_tilde_prb_odd = n_tilde_prb-N_row;
				} else if (N_null != 0 && n_tilde_vrb >= (N_tilde_vrb - N_null) && (n_tilde_vrb%2) == 0) {
					n_tilde_prb_odd = n_tilde_prb-N_row+N_null/2;
				} else if (N_null != 0 && n_tilde_vrb < (N_tilde_vrb - N_null) && (n_tilde_vrb%4) >= 2) {
					n_tilde_prb_odd = n_tilde2_prb-N_null/2;
				} else {
					n_tilde_prb_odd = n_tilde2_prb;
				}
				n_tilde_prb_even = (n_tilde_prb_odd+N_tilde_vrb/2)%N_tilde_vrb+N_tilde_vrb*(n_vrb/N_tilde_vrb);

				if (n_tilde_prb_odd < N_tilde_vrb/2) {
					prb_dist->slot[0].prb_idx[i] = n_tilde_prb_odd;
				} else {
					prb_dist->slot[0].prb_idx[i] = n_tilde_prb_odd+N_gap-N_tilde_vrb/2;
				}
				prb_dist->slot[0].nof_prb++;
				if (n_tilde_prb_even < N_tilde_vrb/2) {
					prb_dist->slot[1].prb_idx[i] = n_tilde_prb_even;
				} else {
					prb_dist->slot[1].prb_idx[i] = n_tilde_prb_even+N_gap-N_tilde_vrb/2;
				}
				prb_dist->slot[1].nof_prb++;
			}
		}
		break;
	default:
		return -1;
	}

	return 0;
}

/* Returns the number of allocated PRB for Uplink */
int ra_nprb_ul(ra_pusch_t *ra, int nof_prb) {
	return ra->type2_alloc.L_crb;
}

/* Returns the number of allocated PRB for Downlink */
int ra_nprb_dl(ra_pdsch_t *ra, int nof_prb) {
	int nprb;
	int nof_rbg, P;
	switch(ra->alloc_type) {
	case alloc_type0:
		// Get the number of allocated RBG except the last RBG
		nof_rbg = bit_count(ra->type0_alloc.rbg_bitmask & 0xFFFFFFFE);
		P = ra_type0_P(nof_prb);
		if (nof_rbg > (int) ceilf((float)nof_prb/P)) {
			fprintf(stderr, "Number of RGB (%d) can not exceed %d\n", nof_prb,
					(int) ceilf((float)nof_prb/P));
			return -1;
		}
		nprb = nof_rbg * P;

		// last RBG may have smaller size. Add if set
		int P_last = (nof_prb%P);
		if (!P_last) P_last = P;
		nprb += P_last*(ra->type0_alloc.rbg_bitmask&1);
		break;
	case alloc_type1:
		nprb = bit_count(ra->type1_alloc.vrb_bitmask);
		if (nprb > ra_type1_N_rb(nof_prb)) {
			fprintf(stderr, "Number of RB (%d) can not exceed %d\n", nprb,
					ra_type1_N_rb(nof_prb));
			return -1;
		}
		break;
	case alloc_type2:
		nprb = ra->type2_alloc.L_crb;
		break;
	default:
		return -1;
	}
	return nprb;
}

/* RBG size for type0 scheduling as in table 7.1.6.1-1 of 36.213 */
int ra_type0_P(int nof_prb) {
	if (nof_prb <= 10) {
		return 1;
	} else if (nof_prb <= 26) {
		return 2;
	} else if (nof_prb <= 63) {
		return 3;
	} else {
		return 4;
	}
}

/* Returns N_rb_type1 according to section 7.1.6.2 */
int ra_type1_N_rb(int nof_prb) {
	int P = ra_type0_P(nof_prb);
	return (int) ceilf((float) nof_prb/P) - (int) ceilf(log2f((float) P)) - 1;
}

/* Convert Type2 scheduling L_crb and RB_start to RIV value */
uint32_t ra_type2_to_riv(uint16_t L_crb, uint16_t RB_start, int nof_prb) {
	uint32_t riv;
	if (L_crb <= (int) nof_prb/2) {
		riv = nof_prb*(L_crb-1) + RB_start;
	} else {
		riv = nof_prb*(nof_prb-L_crb+1) + nof_prb - 1 - RB_start;
	}
	return riv;
}

/* Convert Type2 scheduling RIV value to L_crb and RB_start values */
void ra_type2_from_riv(uint32_t riv, uint16_t *L_crb, uint16_t *RB_start, int nof_prb, int nof_vrb) {
	*L_crb = (int) (riv/nof_prb) + 1;
	*RB_start = riv%nof_prb;
	if (*L_crb > nof_vrb - *RB_start) {
		*L_crb = nof_prb - (int) (riv/nof_prb) + 1;
		*RB_start = nof_prb - riv%nof_prb - 1;
	}
}


/* Table 6.2.3.2-1 in 36.211 */
int ra_type2_ngap(int nof_prb, bool ngap_is_1) {
	if (nof_prb <= 10) {
		return nof_prb/2;
	} else if (nof_prb == 11) {
		return 4;
	} else if (nof_prb <= 19) {
		return 8;
	} else if (nof_prb <= 26) {
		return 12;
	} else if (nof_prb <= 44) {
		return 18;
	} else if (nof_prb <= 49) {
		return 27;
	} else if (nof_prb <= 63) {
		return ngap_is_1?27:9;
	} else if (nof_prb <= 79) {
		return ngap_is_1?32:16;
	} else {
		return ngap_is_1?48:16;
	}
}


/* Table 7.1.6.3-1 in 36.213 */
int ra_type2_n_rb_step(int nof_prb) {
	if (nof_prb < 50) {
		return 2;
	} else {
		return 4;
	}
}


/* as defined in 6.2.3.2 of 36.211 */
int ra_type2_n_vrb_dl(int nof_prb, bool ngap_is_1) {
	int ngap = ra_type2_ngap(nof_prb, ngap_is_1);
	if (ngap_is_1) {
		return 2*(ngap<(nof_prb-ngap)?ngap:nof_prb-ngap);
	} else {
		return ((int) nof_prb/ngap)*2*ngap;
	}
}

/* Converts ra_mcs_t structure to MCS index for both Uplink and Downlink */
uint8_t ra_mcs_to_table_idx(ra_mcs_t *mcs) {
	switch (mcs->mod) {
	case QPSK:
		return mcs->tbs_idx;
	case QAM16:
		return mcs->tbs_idx + 1;
	case QAM64:
		return mcs->tbs_idx + 2;
	default:
		return 0;
	}
}

/* Converts MCS index to ra_mcs_t structure for Downlink as defined inTable 7.1.7.1-1 on 36.213 */
int ra_mcs_from_idx_dl(uint8_t idx, ra_mcs_t *mcs) {
	if (idx < 10) {
		mcs->mod = QPSK;
		mcs->tbs_idx = idx;
	} else if (idx < 17) {
		mcs->mod = QAM16;
		mcs->tbs_idx = idx-1;
	} else if (idx < 29) {
		mcs->mod = QAM64;
		mcs->tbs_idx = idx-2;
	} else if (idx == 29) {
		mcs->mod = QPSK;
		mcs->tbs_idx = 0;
	} else if (idx == 30) {
		mcs->mod = QAM16;
		mcs->tbs_idx = 0;
	} else if (idx == 31) {
		mcs->mod = QAM64;
		mcs->tbs_idx = 0;
	} else {
		mcs->mod = MOD_NULL;
		mcs->tbs_idx = 0;
		return -1;
	}
	return 0;
}


/* Converts MCS index to ra_mcs_t structure for Uplink as defined in Table 8.6.1-1 on 36.213 */
int ra_mcs_from_idx_ul(uint8_t idx, ra_mcs_t *mcs) {
	if (idx < 11) {
		mcs->mod = QPSK;
		mcs->tbs_idx = idx;
	} else if (idx < 21) {
		mcs->mod = QAM16;
		mcs->tbs_idx = idx-1;
	} else if (idx < 29) {
		mcs->mod = QAM64;
		mcs->tbs_idx = idx-2;
	} else {
		mcs->mod = MOD_NULL;
		mcs->tbs_idx = 0;
		return -1;
	}
	return 0;
}

/* Downlink Transport Block size for Format 1C as defined in 7.1.7.2.2-1 on 36.213 */
int ra_tbs_from_idx_format1c(uint8_t tbs_idx) {
	if (tbs_idx < 32) {
		return tbs_format1c_table[tbs_idx];
	} else {
		return -1;
	}
}

/* Returns lowest nearest index of TBS value in table 7.1.7.2.2-1 on 36.213
 * or -1 if the TBS value is not within the valid TBS values
 */
int ra_tbs_to_table_idx_format1c(int tbs) {
	int idx;
	if (tbs < tbs_format1c_table[0]) {
		return -1;
	}
	for (idx=1;idx<32;idx++) {
		if (tbs_format1c_table[idx-1] <= tbs &&
				tbs_format1c_table[idx] >= tbs) {
			return idx;
		}
	}
	return -1;
}

/* Downlink Transport Block size determination as defined in 7.1.7.2 on 36.213 */
int ra_tbs_from_idx(uint8_t tbs_idx, int n_prb ) {
	if (tbs_idx < 27 && n_prb > 0 && n_prb <= 110) {
		return tbs_table[tbs_idx][n_prb-1];
	} else {
		return -1;
	}
}

/* Returns lowest nearest index of TBS value in table 7.1.7.2 on 36.213
 * or -1 if the TBS value is not within the valid TBS values
 */
int ra_tbs_to_table_idx(int tbs, int n_prb) {
	int idx;
	if (n_prb > 0 && n_prb <= 110) {
		return -1;
	}
	if (tbs < tbs_table[0][n_prb]) {
		return -1;
	}
	for (idx=1;idx<28;idx++) {
		if (tbs_table[idx-1][n_prb] <= tbs &&
				tbs_table[idx][n_prb] >= tbs) {
			return idx;
		}
	}
	return -1;
}

char *ra_mod_string(ra_mod_t mod) {
	switch (mod) {
	case QPSK:
		return "QPSK";
	case QAM16:
		return "QAM16";
	case QAM64:
		return "QAM64";
	default:
		return "N/A";
	}
}

void ra_pusch_fprint(FILE *f, ra_pusch_t *ra, int nof_prb) {
	fprintf(f, "Frequency Hopping:\t");
	if (ra->freq_hop_fl == hop_disabled) {
		fprintf(f, "No");
	} else {
		fprintf(f, "Yes");

	}
}

char *ra_type_string(ra_type_t alloc_type) {
	switch(alloc_type) {
	case alloc_type0:
		return "Type 0";
	case alloc_type1:
		return "Type 1";
	case alloc_type2:
		return "Type 2";
	default:
		return "N/A";
	}
}

void ra_pdsch_set_mcs_index(ra_pdsch_t *ra, uint8_t mcs_idx) {
	ra->mcs.mod = MOD_NULL;
	ra->mcs.mcs_idx = mcs_idx;
}
void ra_pdsch_set_mcs(ra_pdsch_t *ra, ra_mod_t mod, uint8_t tbs_idx) {
	ra->mcs.mod = mod;
	ra->mcs.tbs_idx = tbs_idx;
}


void ra_pdsch_fprint(FILE *f, ra_pdsch_t *ra, int nof_prb) {
	fprintf(f, " - Resource Allocation Type:\t\t%s\n",ra_type_string(ra->alloc_type));
	switch(ra->alloc_type) {
	case alloc_type0:
		fprintf(f, "   + Resource Block Group Size:\t\t%d\n",ra_type0_P(nof_prb));
		fprintf(f, "   + RBG Bitmap:\t\t\t0x%x\n",ra->type0_alloc.rbg_bitmask);
		break;
	case alloc_type1:
		fprintf(f, "   + Resource Block Group Size:\t\t%d\n",ra_type0_P(nof_prb));
		fprintf(f, "   + RBG Bitmap:\t\t\t0x%x\n",ra->type1_alloc.vrb_bitmask);
		fprintf(f, "   + RBG Subset:\t\t\t%d\n",ra->type1_alloc.rbg_subset);
		fprintf(f, "   + RBG Shift:\t\t\t\t%s\n",ra->type1_alloc.shift?"Yes":"No");
		break;
	case alloc_type2:
		fprintf(f, "   + Type:\t\t\t\t%s\n",
				ra->type2_alloc.mode==t2_loc?"Localized":"Distributed");
		fprintf(f, "   + Resource Indicator Value:\t\t%d\n",ra->type2_alloc.riv);
		if (ra->type2_alloc.mode == t2_loc) {
			fprintf(f, "   + VRB Assignment:\t\t\t%d VRB starting with VRB %d\n",
				ra->type2_alloc.L_crb, ra->type2_alloc.RB_start);
		} else {
			fprintf(f, "   + VRB Assignment:\t\t\t%d VRB starting with VRB %d\n",
				ra->type2_alloc.L_crb, ra->type2_alloc.RB_start);
			fprintf(f, "   + VRB gap selection:\t\t\tGap %d\n",
				ra->type2_alloc.n_gap == t2_ng1?1:2);
			fprintf(f, "   + VRB gap:\t\t\t\t%d\n",
					ra_type2_ngap(nof_prb, ra->type2_alloc.n_gap == t2_ng1));
		}
		break;
	}

	ra_prb_t alloc;
	ra_prb_get_dl(&alloc, ra, nof_prb);
	for (int s=0;s<2;s++) {
		fprintf(f, " - PRB Bitmap Assignment %dst slot:\n", s);
		ra_prb_fprint(f, &alloc.slot[s]);
	}

	fprintf(f, " - Number of PRBs:\t\t\t%d\n", ra_nprb_dl(ra, nof_prb));
	fprintf(f, " - Modulation and coding scheme index:\t%d\n", ra->mcs.mcs_idx);
	fprintf(f, " - Modulation type:\t\t\t%s\n", ra_mod_string(ra->mcs.mod));
	fprintf(f, " - Transport block size:\t\t%d\n", ra->mcs.tbs);
	fprintf(f, " - HARQ process:\t\t\t%d\n", ra->harq_process);
	fprintf(f, " - New data indicator:\t\t\t%s\n", ra->ndi?"Yes":"No");
	fprintf(f, " - Redundancy version:\t\t\t%d\n", ra->rv_idx);
	fprintf(f, " - TPC command for PUCCH:\t\t--\n");
}

