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

#include "srslte/srslte.h"
#include "srslte/ue_itf/queue.h"
#include "srslte/ue_itf/sched_grant.h"
#include "srslte/ue_itf/params.h"

#ifndef UEDLBUFFER_H
#define UEDLBUFFER_H

namespace srslte {
namespace ue {

  /* Class for the processing of Downlink buffers. The MAC obtains a buffer for a given TTI and then 
    * gets ul/dl scheduling grants and/or processes phich/pdsch channels 
    */
  class SRSLTE_API dl_buffer : public queue::element {
  public:
    typedef enum {
      PDCCH_UL_SEARCH_CRNTI = 0,
      PDCCH_UL_SEARCH_RA_PROC,
      PDCCH_UL_SEARCH_SPS,
      PDCCH_UL_SEARCH_TEMPORAL,
      PDCCH_UL_SEARCH_TPC_PUSCH,
      PDCCH_UL_SEARCH_TPC_PUCCH
    } pdcch_ul_search_t; 

    typedef enum {
      PDCCH_DL_SEARCH_CRNTI = 0,
      PDCCH_DL_SEARCH_SIRNTI,
      PDCCH_DL_SEARCH_PRNTI,
      PDCCH_DL_SEARCH_RARNTI,
      PDCCH_DL_SEARCH_TEMPORAL,    
      PDCCH_DL_SEARCH_SPS
    } pdcch_dl_search_t; 

    int buffer_id; 
    
    bool           init_cell(srslte_cell_t cell, params *params_db);
    void           free_cell();
    bool           recv_ue_sync(srslte_ue_sync_t *ue_sync, srslte_timestamp_t *rx_time);
    bool           get_ul_grant(pdcch_ul_search_t mode, sched_grant *grant);
    bool           get_dl_grant(pdcch_dl_search_t mode, sched_grant *grant);
    bool           decode_ack(srslte::ue::sched_grant pusch_grant);
    bool           decode_data(sched_grant pdsch_grant, uint8_t *payload); // returns true or false for CRC OK/KO
  private: 
    params       *params_db;
    srslte_cell_t  cell; 
    srslte_ue_dl_t ue_dl; 
    srslte_phich_t phich; 
    cf_t          *signal_buffer; 
    uint32_t       cfi; 
    bool           sf_symbols_and_ce_done; 
    bool           pdcch_llr_extracted;    
  };    
}
}
#endif
