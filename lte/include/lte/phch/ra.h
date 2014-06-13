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

#ifndef RB_ALLOC_H_
#define RB_ALLOC_H_

#include <stdint.h>
#include <stdbool.h>

#include "lte/config.h"

/** Structures and utility functions for DL/UL resource
 * allocation.
 */

typedef enum {
	MOD_NULL = 0, BPSK = 1, QPSK = 2, QAM16 = 4, QAM64 = 16
} ra_mod_t;

typedef struct LIBLTE_API {
	ra_mod_t mod; // By default, mod = MOD_NULL and the mcs_idx value is taken by the packing functions
				// otherwise mod + tbs values are used to generate the mcs_idx automatically.
	uint8_t tbs_idx;
	uint8_t mcs_idx;
	int tbs;	// If tbs<=0, the tbs_idx value is taken by the packing functions to generate the DCI
			// message. Otherwise the tbs_idx corresponding to the lower nearest TBS is taken.
}ra_mcs_t;


typedef enum {
	alloc_type0 = 0, alloc_type1 = 1, alloc_type2 = 2
}ra_type_t;

typedef struct LIBLTE_API {
	uint32_t rbg_bitmask;
}ra_type0_t;

typedef struct LIBLTE_API {
	uint32_t vrb_bitmask;
	uint8_t rbg_subset;
	bool shift;
}ra_type1_t;

typedef struct LIBLTE_API {
	uint32_t riv;  // if L_crb==0, DCI message packer will take this value directly
	uint16_t L_crb;
	uint16_t RB_start;
	enum {nprb1a_2 = 0, nprb1a_3 = 1} n_prb1a;
	enum {t2_ng1 = 0, t2_ng2 = 1} n_gap;
	enum {t2_loc = 0, t2_dist = 1} mode;
}ra_type2_t;

typedef struct LIBLTE_API {
	unsigned short rnti;
	ra_type_t alloc_type;
	union {
		ra_type0_t type0_alloc;
		ra_type1_t type1_alloc;
		ra_type2_t type2_alloc;
	};
	ra_mcs_t mcs;
	uint8_t harq_process;
	uint8_t rv_idx;
	bool ndi;
} ra_pdsch_t;

typedef struct LIBLTE_API {
	/* 36.213 Table 8.4-2: hop_half is 0 for < 10 Mhz and 10 for > 10 Mh.
	 * hop_quart is 00 for > 10 Mhz and hop_quart_neg is 01 for > 10 Mhz.
	 */
	enum {
		hop_disabled = -1,
		hop_quart = 0,
		hop_quart_neg = 1,
		hop_half = 2,
		hop_type_2 = 3
	} freq_hop_fl;

	ra_type2_t type2_alloc;
	ra_mcs_t mcs;
	uint8_t rv_idx; 	// If set to non-zero, a retransmission is requested with the same modulation
				// than before (Format0 message, see also 8.6.1 in 36.2313).
	bool ndi;
	bool cqi_request;

} ra_pusch_t;

typedef struct LIBLTE_API {
	uint8_t prb_idx[110];
	int nof_prb;
}ra_prb_slot_t;

typedef struct LIBLTE_API {
	ra_prb_slot_t slot1;
	ra_prb_slot_t slot2;
	bool is_dist;
}ra_prb_t;


LIBLTE_API void ra_prb_fprint(FILE *f, ra_prb_slot_t *prb);
LIBLTE_API int ra_prb_get_dl(ra_prb_t *prb, ra_pdsch_t *ra, int nof_prb);
LIBLTE_API int ra_prb_get_ul(ra_prb_slot_t *prb, ra_pusch_t *ra, int nof_prb);
LIBLTE_API int ra_nprb_dl(ra_pdsch_t *ra, int nof_prb);
LIBLTE_API int ra_nprb_ul(ra_pusch_t *ra, int nof_prb);

LIBLTE_API uint8_t ra_mcs_to_table_idx(ra_mcs_t *mcs);
LIBLTE_API int ra_mcs_from_idx_dl(uint8_t idx, ra_mcs_t *mcs);
LIBLTE_API int ra_mcs_from_idx_ul(uint8_t idx, ra_mcs_t *mcs);
LIBLTE_API int ra_tbs_from_idx_format1c(uint8_t tbs_idx);
LIBLTE_API int ra_tbs_to_table_idx_format1c(int tbs);
LIBLTE_API int ra_tbs_from_idx(uint8_t tbs_idx, int n_prb);
LIBLTE_API int ra_tbs_to_table_idx(int tbs, int n_prb);

LIBLTE_API uint8_t ra_mcs_to_table_idx(ra_mcs_t *mcs);
LIBLTE_API int ra_mcs_from_idx_dl(uint8_t idx, ra_mcs_t *mcs);
LIBLTE_API int ra_mcs_from_idx_ul(uint8_t idx, ra_mcs_t *mcs);

LIBLTE_API char *ra_mod_string(ra_mod_t mod);

LIBLTE_API int ra_type0_P(int nof_prb);

LIBLTE_API uint32_t ra_type2_to_riv(uint16_t L_crb, uint16_t RB_start, int nof_prb);
LIBLTE_API void ra_type2_from_riv(uint32_t riv, uint16_t *L_crb, uint16_t *RB_start, int nof_prb, int nof_vrb);
LIBLTE_API int ra_type2_n_vrb_dl(int nof_prb, bool ngap_is_1);
LIBLTE_API int ra_type2_n_rb_step(int nof_prb);
LIBLTE_API int ra_type2_ngap(int nof_prb, bool ngap_is_1);
LIBLTE_API int ra_type1_N_rb(int nof_prb);

LIBLTE_API void ra_pdsch_set_mcs_index(ra_pdsch_t *ra, uint8_t mcs_idx);
LIBLTE_API void ra_pdsch_set_mcs(ra_pdsch_t *ra, ra_mod_t mod, uint8_t tbs_idx);
LIBLTE_API void ra_pdsch_fprint(FILE *f, ra_pdsch_t *ra, int nof_prb);
LIBLTE_API void ra_pusch_fprint(FILE *f, ra_pusch_t *ra, int nof_prb);

#endif // RB_ALLOC_H_
