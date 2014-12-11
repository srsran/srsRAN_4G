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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "liblte/config.h"

#define NSUBFRAMES_X_FRAME  10
#define NSLOTS_X_FRAME      (2*NSUBFRAMES_X_FRAME)

#define LTE_NSOFT_BITS  250368 // Soft buffer size for Category 1 UE

#define LTE_NULL_BIT    0
#define LTE_NULL_SYMBOL 2
#define LTE_NIL_SYMBOL  2

#define MAX_PORTS     4
#define MAX_LAYERS    8
#define MAX_CODEWORDS 2

#define LTE_CRC24A  0x1864CFB
#define LTE_CRC24B  0X1800063
#define LTE_CRC16   0x11021
#define LTE_CRC8    0x19B

typedef enum {CPNORM, CPEXT} lte_cp_t;

#define SIRNTI  0xFFFF
#define PRNTI   0xFFFE
#define MRNTI   0xFFFD

#define CELL_ID_UNKNOWN         1000

#define MAX_NSYMB     7

#define MAX_PRB   110
#define RE_X_RB   12

#define SYMBOL_SZ_MAX   2048

#define CPNORM_NSYMB    7
#define CPNORM_SF_NSYMB (2*CPNORM_NSYMB)
#define CPNORM_0_LEN    160
#define CPNORM_LEN      144

#define CPEXT_NSYMB     6
#define CPEXT_SF_NSYMB  (2*CPEXT_NSYMB)
#define CPEXT_LEN       512
#define CPEXT_7_5_LEN   1024

#define CP_ISNORM(cp) (cp==CPNORM)
#define CP_ISEXT(cp) (cp==CPEXT)
#define CP_NSYMB(cp) (CP_ISNORM(cp)?CPNORM_NSYMB:CPEXT_NSYMB)

#define CP(symbol_sz, c) ((int) ceil((((float) (c)*(symbol_sz))/2048)))
#define CP_NORM(symbol, symbol_sz) ((symbol==0)?CP((symbol_sz),CPNORM_0_LEN):CP((symbol_sz),CPNORM_LEN))
#define CP_EXT(symbol_sz) (CP((symbol_sz),CPEXT_LEN))

#define SLOT_LEN(symbol_sz)     (480*((symbol_sz)/64))
#define SF_LEN(symbol_sz)       (2*SLOT_LEN(symbol_sz))
#define SF_LEN_MAX              (SF_LEN(SYMBOL_SZ_MAX))

#define SLOT_LEN_PRB(nof_prb)   (SLOT_LEN(lte_symbol_sz(nof_prb)))
#define SF_LEN_PRB(nof_prb)     (SF_LEN(lte_symbol_sz(nof_prb)))

#define SLOT_LEN_RE(nof_prb, cp)        (nof_prb*RE_X_RB*CP_NSYMB(cp))
#define SF_LEN_RE(nof_prb, cp)          (2*SLOT_LEN_RE(nof_prb, cp))

#define SLOT_IDX_CPNORM(symbol_idx, symbol_sz) (symbol_idx==0?0:(symbol_sz + CP(symbol_sz, CPNORM_0_LEN) + \
                                                (symbol_idx-1)*(symbol_sz+CP(symbol_sz, CPNORM_LEN))))
#define SLOT_IDX_CPEXT(idx, symbol_sz) (idx*(symbol_sz+CP(symbol_sz, CPEXT_LEN)))

#define RE_IDX(nof_prb, symbol_idx, sample_idx) ((symbol_idx)*(nof_prb)*(RE_X_RB) + sample_idx)

#define RS_VSHIFT(cell_id) (cell_id%6)

#define GUARD_RE(nof_prb) ((lte_symbol_sz(nof_prb)-nof_prb*RE_X_RB)/2)

#define SYMBOL_HAS_REF(l, cp, nof_ports) ((l == 1 && nof_ports == 4) \
        || l == 0 \
        || l == CP_NSYMB(cp) - 3)


#define NOF_LTE_BANDS 29

#define NOF_TC_CB_SIZES 188

typedef enum LIBLTE_API { PHICH_NORM, PHICH_EXT} phich_length_t;
typedef enum LIBLTE_API { R_1_6, R_1_2, R_1, R_2} phich_resources_t;

typedef struct LIBLTE_API {
  uint32_t nof_prb;
  uint32_t nof_ports; 
  uint32_t id;
  lte_cp_t cp;
  phich_length_t phich_length;
  phich_resources_t phich_resources;
}lte_cell_t;

typedef enum LIBLTE_API {
  SINGLE_ANTENNA,TX_DIVERSITY, SPATIAL_MULTIPLEX
} lte_mimo_type_t;

typedef enum LIBLTE_API {
  LTE_BPSK = 1, LTE_QPSK = 2, LTE_QAM16 = 4, LTE_QAM64 = 6
} lte_mod_t;


typedef struct LIBLTE_API {
  int id;
  float fd;
}lte_earfcn_t;

LIBLTE_API enum band_geographical_area {
  ALL, NAR, APAC, EMEA, JAPAN, CALA, NA
};

LIBLTE_API bool lte_cell_isvalid(lte_cell_t *cell);

LIBLTE_API void lte_cell_fprint(FILE *stream, 
                                lte_cell_t *cell); 

LIBLTE_API bool lte_cellid_isvalid(uint32_t cell_id);

LIBLTE_API bool lte_nofprb_isvalid(uint32_t nof_prb);

LIBLTE_API bool lte_sfidx_isvalid(uint32_t sf_idx);

LIBLTE_API bool lte_portid_isvalid(uint32_t port_id);

LIBLTE_API bool lte_N_id_2_isvalid(uint32_t N_id_2);

LIBLTE_API bool lte_N_id_1_isvalid(uint32_t N_id_1);

LIBLTE_API bool lte_symbol_sz_isvalid(uint32_t symbol_sz); 

LIBLTE_API int lte_symbol_sz(uint32_t nof_prb);


LIBLTE_API int lte_sampling_freq_hz(uint32_t nof_prb);

LIBLTE_API uint32_t lte_re_x_prb(uint32_t ns, 
                            uint32_t symbol, 
                            uint32_t nof_ports, 
                            uint32_t nof_symbols);

LIBLTE_API uint32_t lte_voffset(uint32_t symbol_id, 
                           uint32_t cell_id, 
                           uint32_t nof_ports);

LIBLTE_API int lte_cb_size(uint32_t index);

LIBLTE_API char *lte_cp_string(lte_cp_t cp); 

LIBLTE_API char *lte_mod_string(lte_mod_t mod);

LIBLTE_API uint32_t lte_mod_bits_x_symbol(lte_mod_t mod);

LIBLTE_API int lte_find_cb_index(uint32_t long_cb);

LIBLTE_API float lte_band_fd(uint32_t earfcn);

LIBLTE_API int lte_band_get_fd_band(uint32_t band, 
                                    lte_earfcn_t *earfcn, 
                                    int earfcn_start, 
                                    int earfcn_end, 
                                    uint32_t max_elems);

LIBLTE_API int lte_band_get_fd_band_all(uint32_t band, 
                                        lte_earfcn_t *earfcn, 
                                        uint32_t max_nelems);

LIBLTE_API int lte_band_get_fd_region(enum band_geographical_area region, 
                                      lte_earfcn_t *earfcn, 
                                      uint32_t max_elems);

LIBLTE_API int lte_str2mimotype(char *mimo_type_str, 
                                lte_mimo_type_t *type);

LIBLTE_API char *lte_mimotype2str(lte_mimo_type_t type);



#endif
