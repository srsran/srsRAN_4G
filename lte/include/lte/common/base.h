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


#ifndef _LTEBASE_
#define _LTEBASE_

#define NSUBFRAMES_X_FRAME	10
#define NSLOTS_X_FRAME (2*NSUBFRAMES_X_FRAME)

#define LTE_NIL_SYMBOL	2

#define MAX_PORTS		4
#define MAX_PORTS_CTRL	4
#define MAX_LAYERS		8
#define MAX_CODEWORDS	2

typedef enum {CPNORM, CPEXT} lte_cp_t;

#define SIRNTI		0xFFFF
#define PRNTI		0xFFFE

#define MAX_NSYMB		7

#define CPNORM_NSYMB	7
#define CPNORM_SF_NSYMB	2*CPNORM_NSYMB
#define CPNORM_0_LEN	160
#define CPNORM_LEN		144

#define CPEXT_NSYMB		6
#define CPEXT_SF_NSYMB	2*CPEXT_NSYMB
#define CPEXT_LEN		512
#define CPEXT_7_5_LEN	1024

#define CP_ISNORM(cp) (cp==CPNORM)
#define CP_ISEXT(cp) (cp==CPEXT)
#define CP_NSYMB(cp) (CP_ISNORM(cp)?CPNORM_NSYMB:CPEXT_NSYMB)

#define CP(symbol_sz, c) (c*symbol_sz/2048)
#define CP_NORM(symbol, symbol_sz) (symbol==0)?CP(symbol_sz,CPNORM_0_LEN):CP(symbol_sz,CPNORM_LEN)
#define CP_EXT(symbol_sz) CP(symbol_sz,CPEXT_LEN)

#define SLOT_LEN_CPNORM(symbol_sz) (symbol_sz+CP(symbol_sz,CPNORM_0_LEN)+(CPNORM_NSYMB-1)*(symbol_sz+CP(symbol_sz,CPNORM_LEN)))
#define SLOT_LEN_CPEXT(symbol_sz) (CPEXT_NSYMB*(symbol_sz+CP(symbol_sz, CPEXT_LEN)))
#define SLOT_LEN(symbol_sz, cp) CP_ISNORM(cp)?SLOT_LEN_CPNORM(symbol_sz):SLOT_LEN_CPEXT(symbol_sz)

#define SF_LEN_CPNORM(symbol_sz) 2*SLOT_LEN_CPNORM(symbol_sz)
#define SF_LEN_CPEXT(symbol_sz) 2*SLOT_LEN_CPEXT(symbol_sz)
#define SF_LEN(symbol_sz, cp) 2*SLOT_LEN(cp, symbol_sz)

#define SLOT_IDX_CPNORM(idx, symbol_sz) (idx==0?(CP(symbol_sz, CPNORM_0_LEN)):(CP(symbol_sz, CPNORM_0_LEN)+idx*(symbol_sz+CP(symbol_sz, CPNORM_LEN))))
#define SLOT_IDX_CPEXT(idx, symbol_sz) (idx*(symbol_sz+CP(symbol_sz, CPEXT_LEN)))

#define MAX_PRB		110
#define RE_X_RB		12

#define RS_VSHIFT(cell_id) (cell_id%6)

#define GUARD_RE(nof_prb)	((lte_symbol_sz(nof_prb)-nof_prb*RE_X_RB)/2)

#define SAMPLE_IDX(nof_prb, symbol_idx, sample_idx) (symbol_idx*nof_prb*RE_X_RB + sample_idx)

const int lte_symbol_sz(int nof_prb);
int lte_re_x_prb(int ns, int symbol, int nof_ports, int nof_symbols);
int lte_voffset(int symbol_id, int cell_id, int nof_ports);

#define NOF_LTE_BANDS	29


typedef enum {
	SINGLE_ANTENNA,TX_DIVERSITY, SPATIAL_MULTIPLEX
} mimo_type_t;

typedef enum { PHICH_NORM, PHICH_EXT} phich_length_t;
typedef enum { R_1_6, R_1_2, R_1, R_2} phich_resources_t;


typedef struct {
	int id;
	float fd;
}lte_earfcn_t;

enum band_geographical_area {
	ALL, NAR, APAC, EMEA, JAPAN, CALA, NA
};

float lte_band_fd(int earfcn);
int lte_band_get_fd_band(int band, lte_earfcn_t *earfcn, int earfcn_start, int earfcn_end, int max_elems);
int lte_band_get_fd_band_all(int band, lte_earfcn_t *earfcn, int max_nelems);
int lte_band_get_fd_region(enum band_geographical_area region, lte_earfcn_t *earfcn, int max_elems);

int lte_str2mimotype(char *mimo_type_str, mimo_type_t *type);
char *lte_mimotype2str(mimo_type_t type);


#endif
