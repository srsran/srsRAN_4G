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


#ifndef PUCCH_
#define PUCCH_

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/mimo/precoding.h"
#include "liblte/phy/mimo/layermap.h"
#include "liblte/phy/modem/mod.h"
#include "liblte/phy/modem/demod_soft.h"
#include "liblte/phy/scrambling/scrambling.h"
#include "liblte/phy/phch/regs.h"
#include "liblte/phy/phch/sch.h"
#include "liblte/phy/phch/harq.h"
#include "liblte/phy/filter/dft_precoding.h"

#define TDEC_MAX_ITERATIONS         5

typedef _Complex float cf_t;

#define PUCCH_N_SEQ     12 // Only Format 1, 1a and 1b supported
#define PUCCH_MAX_BITS  2 
#define PUCCH_N_SF_MAX  4

typedef enum LIBLTE_API {
  PUCCH_FORMAT_1 = 0, 
  PUCCH_FORMAT_1A, 
  PUCCH_FORMAT_1B, 
  PUCCH_FORMAT_2, 
  PUCCH_FORMAT_2A, 
  PUCCH_FORMAT_2B, 
} pucch_format_t; 

typedef struct LIBLTE_API {
  pucch_format_t format; 
  float beta_pucch;
  uint32_t delta_pucch_shift; 
  uint32_t n_pucch; 
  uint32_t N_cs; 
} pucch_cfg_t;

/* PUSCH object */
typedef struct LIBLTE_API {
  lte_cell_t cell;
  pucch_cfg_t pucch_cfg;
 
  uint32_t n_cs_cell[NSLOTS_X_FRAME][CPNORM_NSYMB]; 
  float tmp_arg[PUCCH_N_SF_MAX*PUCCH_N_SEQ];
  float y[PUCCH_N_SEQ];
}pucch_t;


LIBLTE_API int pucch_init(pucch_t *q, 
                          lte_cell_t cell);

LIBLTE_API void pucch_free(pucch_t *q);

LIBLTE_API void pucch_set_cfg(pucch_t *q, 
                              pucch_cfg_t *cfg); 

LIBLTE_API int pucch_set_rnti(pucch_t *q, 
                              uint16_t rnti);

LIBLTE_API int pucch_encode(pucch_t *q, 
                            pucch_cfg_t *cfg, 
                            uint8_t bits[PUCCH_MAX_BITS], 
                            cf_t *sf_symbols); 

LIBLTE_API float pucch_get_alpha(uint32_t n_cs_cell[NSLOTS_X_FRAME][CPNORM_NSYMB], 
                                 pucch_cfg_t *cfg, 
                                 lte_cp_t cp, 
                                 bool is_drms,
                                 uint32_t ns, 
                                 uint32_t l,
                                 uint32_t *n_oc); 

LIBLTE_API int generate_n_cs_cell(lte_cell_t cell, 
                                  uint32_t n_cs_cell[NSLOTS_X_FRAME][CPNORM_NSYMB]);

LIBLTE_API bool pucch_cfg_isvalid(pucch_cfg_t *cfg); 

#endif
