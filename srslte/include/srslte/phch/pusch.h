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


#ifndef PUSCH_
#define PUSCH_

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/mimo/precoding.h"
#include "srslte/mimo/layermap.h"
#include "srslte/modem/mod.h"
#include "srslte/modem/demod_soft.h"
#include "srslte/scrambling/scrambling.h"
#include "srslte/phch/regs.h"
#include "srslte/phch/sch.h"
#include "srslte/phch/harq.h"
#include "srslte/filter/dft_precoding.h"

#define TDEC_MAX_ITERATIONS         5

typedef _Complex float cf_t;


typedef struct {
  enum {
    hop_mode_inter_sf = 1,
    hop_mode_intra_sf = 0
  } hop_mode; 
  uint32_t current_tx_nb;
  uint32_t hopping_offset;
  uint32_t n_sb;
} pusch_hopping_cfg_t;


/* PUSCH object */
typedef struct SRSLTE_API {
  lte_cell_t cell;

  pusch_hopping_cfg_t hopping_cfg;
  
  uint32_t max_re;
  bool rnti_is_set; 
  uint16_t rnti; 

  dft_precoding_t dft_precoding;  
  
  precoding_t equalizer; 
  
  /* buffers */
  // void buffers are shared for tx and rx
  cf_t *ce;
  cf_t *pusch_z;
  cf_t *pusch_d;

  void *pusch_q;
  void *pusch_g;

  /* tx & rx objects */
  modem_table_t mod[4];
  demod_soft_t demod;
  sequence_t seq_pusch[NSUBFRAMES_X_FRAME];
  sequence_t seq_type2_fo; 
  
  sch_t dl_sch;
  
}pusch_t;


SRSLTE_API int pusch_init(pusch_t *q, 
                          lte_cell_t cell);

SRSLTE_API void pusch_free(pusch_t *q);

SRSLTE_API void pusch_set_hopping_cfg(pusch_t *q, 
                                      pusch_hopping_cfg_t *cfg); 

SRSLTE_API int pusch_set_rnti(pusch_t *q, 
                              uint16_t rnti);

SRSLTE_API int pusch_encode(pusch_t *q, 
                            harq_t *harq_process,
                            uint8_t *data, 
                            cf_t *sf_symbols);

SRSLTE_API int pusch_encode_rnti(pusch_t *q, 
                                 harq_t *harq_process, 
                                 uint8_t *data, 
                                 uint16_t rnti, 
                                 cf_t *sf_symbols); 

SRSLTE_API int pusch_uci_encode(pusch_t *q, 
                                harq_t *harq_process,
                                uint8_t *data, 
                                uci_data_t uci_data, 
                                cf_t *sf_symbols);

SRSLTE_API int pusch_uci_encode_rnti(pusch_t *q, 
                                     harq_t *harq, 
                                     uint8_t *data, 
                                     uci_data_t uci_data, 
                                     uint16_t rnti, 
                                     cf_t *sf_symbols); 

SRSLTE_API int pusch_decode(pusch_t *q, 
                            harq_t *harq_process,
                            cf_t *sf_symbols, 
                            cf_t *ce,
                            float noise_estimate, 
                            uint8_t *data);

SRSLTE_API float pusch_average_noi(pusch_t *q); 

SRSLTE_API uint32_t pusch_last_noi(pusch_t *q); 

#endif
