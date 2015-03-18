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


#ifndef SCH_
#define SCH_

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/fec/rm_turbo.h"
#include "srslte/fec/turbocoder.h"
#include "srslte/fec/turbodecoder.h"
#include "srslte/fec/crc.h"
#include "srslte/phch/harq.h"
#include "srslte/phch/uci.h"

#define TDEC_MAX_ITERATIONS         5


#ifndef SRSLTE_RX_NULL
#define SRSLTE_RX_NULL 10000
#endif

#ifndef SRSLTE_TX_NULL
#define SRSLTE_TX_NULL 100
#endif

/* DL-SCH AND UL-SCH common functions */
typedef struct SRSLTE_API {

  uint32_t nof_iterations; 
  float average_nof_iterations; 
  
  /* buffers */
  uint8_t *cb_in; 
  void *cb_out;  
  void *pdsch_e;
  
  srslte_tcod_t encoder;
  srslte_tdec_t decoder;  
  srslte_crc_t srslte_crc_tb;
  srslte_crc_t srslte_crc_cb;
  
  uci_cqi_pusch_t uci_cqi; 
  
} sch_t;

SRSLTE_API int sch_init(sch_t *q);

SRSLTE_API void sch_free(sch_t *q);


SRSLTE_API float sch_average_noi(sch_t *q);

SRSLTE_API uint32_t sch_last_noi(sch_t *q);

SRSLTE_API int dlsch_encode(sch_t *q, 
                            harq_t *harq_process,
                            uint8_t *data, 
                            uint8_t *e_bits);

SRSLTE_API int dlsch_decode(sch_t *q, 
                            harq_t *harq_process,
                            float *e_bits, 
                            uint8_t *data);

SRSLTE_API int ulsch_encode(sch_t *q, 
                            harq_t *harq_process,
                            uint8_t *data, 
                            uint8_t *g_bits,
                            uint8_t *q_bits);

SRSLTE_API int ulsch_uci_encode(sch_t *q, 
                                harq_t *harq_process,
                                uint8_t *data, 
                                uci_data_t uci_data, 
                                uint8_t *g_bits,
                                uint8_t *q_bits);

SRSLTE_API int ulsch_decode(sch_t *q, 
                            harq_t *harq_process,
                            float *e_bits, 
                            uint8_t *data);

#endif
