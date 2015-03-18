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


#ifndef SCRAMBLING_
#define SCRAMBLING_

#include "srslte/config.h"
#include "srslte/common/sequence.h"
#include "srslte/common/phy_common.h"

typedef _Complex float cf_t;

/* Scrambling has no state */
SRSLTE_API void scrambling_b(srslte_sequence_t *s, 
                             uint8_t *data);

SRSLTE_API void scrambling_b_offset(srslte_sequence_t *s, 
                                    uint8_t *data, 
                                    int offset, 
                                    int len);

SRSLTE_API void scrambling_b_offset_pusch(srslte_sequence_t *s, 
                                          uint8_t *data, 
                                          int offset, 
                                          int len); 

SRSLTE_API void scrambling_f(srslte_sequence_t *s, 
                             float *data);

SRSLTE_API void scrambling_f_offset(srslte_sequence_t *s, 
                                    float *data, 
                                    int offset, 
                                    int len);

SRSLTE_API void scrambling_c(srslte_sequence_t *s, 
                             cf_t *data);

SRSLTE_API void scrambling_c_offset(srslte_sequence_t *s, 
                                    cf_t *data, 
                                    int offset, 
                                    int len);


/* High-level API */

/* channel integer values */
#define SCRAMBLING_PDSCH      0  /* also PUSCH */
#define SCRAMBLING_PCFICH      1
#define SCRAMBLING_PDCCH      2
#define SCRAMBLING_PBCH      3
#define SCRAMBLING_PMCH      4
#define SCRAMBLING_PUCCH      5

typedef struct SRSLTE_API {
  srslte_sequence_t seq[SRSLTE_NSUBFRAMES_X_FRAME];
}scrambling_t;

typedef struct SRSLTE_API {
  scrambling_t obj;
  struct scrambling_init {
    int hard;
    int q;
    int cell_id;
    int nrnti;
    int nMBSFN;
    int channel;
    int nof_symbols;  // 7 normal 6 extended
  } init;
  void *input;      // input type may be uint8_t or float depending on hard
  int in_len;
  struct scrambling_ctrl_in {
    int subframe;
  } ctrl_in;
  void *output;
  int out_len;
}scrambling_hl;

#endif // SCRAMBLING_
