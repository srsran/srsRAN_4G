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


#ifndef SCH_
#define SCH_

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/fec/rm_turbo.h"
#include "srslte/phy/fec/turbocoder.h"
#include "srslte/phy/fec/turbodecoder.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/phch/harq.h"
#include "srslte/phy/phch/uci.h"

#define TDEC_MAX_ITERATIONS         5


#ifndef RX_NULL
#define RX_NULL 10000
#endif

#ifndef TX_NULL
#define TX_NULL 100
#endif

/* DL-SCH AND UL-SCH common functions */
typedef struct LIBLTE_API {

  uint32_t nof_iterations; 
  float average_nof_iterations; 
  
  /* buffers */
  uint8_t *cb_in; 
  void *cb_out;  
  void *pdsch_e;
  
  tcod_t encoder;
  tdec_t decoder;  
  crc_t crc_tb;
  crc_t crc_cb;
  
  uci_cqi_pusch_t uci_cqi; 
  
} sch_t;

LIBLTE_API int sch_init(sch_t *q);

LIBLTE_API void sch_free(sch_t *q);


LIBLTE_API float sch_average_noi(sch_t *q);

LIBLTE_API uint32_t sch_last_noi(sch_t *q);

LIBLTE_API int dlsch_encode(sch_t *q, 
                            harq_t *harq_process,
                            uint8_t *data, 
                            uint8_t *e_bits);

LIBLTE_API int dlsch_decode(sch_t *q, 
                            harq_t *harq_process,
                            float *e_bits, 
                            uint8_t *data);

LIBLTE_API int ulsch_encode(sch_t *q, 
                            harq_t *harq_process,
                            uint8_t *data, 
                            uint8_t *g_bits,
                            uint8_t *q_bits);

LIBLTE_API int ulsch_uci_encode(sch_t *q, 
                                harq_t *harq_process,
                                uint8_t *data, 
                                uci_data_t uci_data, 
                                uint8_t *g_bits,
                                uint8_t *q_bits);

LIBLTE_API int ulsch_decode(sch_t *q, 
                            harq_t *harq_process,
                            float *e_bits, 
                            uint8_t *data);

#endif
