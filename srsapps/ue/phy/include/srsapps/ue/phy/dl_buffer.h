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
#include "srsapps/common/queue.h"
#include "srsapps/ue/phy/ul_sched_grant.h"
#include "srsapps/ue/phy/dl_sched_grant.h"
#include "srsapps/ue/phy/phy_params.h"

#ifndef UEDLBUFFER_H
#define UEDLBUFFER_H

namespace srslte {
namespace ue {

  /* Class for the processing of Downlink buffers. The MAC obtains a buffer for a given TTI and then 
    * gets ul/dl scheduling grants and/or processes phich/pdsch channels 
    */
  class SRSLTE_API dl_buffer : public queue::element {
  public:
    
    int buffer_id; 
    
    bool           init_cell(srslte_cell_t cell, phy_params *params_db);
    void           free_cell();
    bool           recv_ue_sync(srslte_ue_sync_t *ue_sync, srslte_timestamp_t *rx_time);
    bool           get_ul_grant(ul_sched_grant *grant);
    bool           get_dl_grant(dl_sched_grant *grant);
    void           discard_pending_rar_grant(); 
    void           set_rar_grant(srslte_dci_rar_grant_t *rar_grant);
    void           set_rar_grant(uint8_t grant[SRSLTE_RAR_GRANT_LEN]);
    bool           decode_ack(ul_sched_grant *pusch_grant);
    bool           decode_data(dl_sched_grant *pdsch_grant, uint8_t *payload); // returns true or false for CRC OK/NOK
    bool           decode_data(dl_sched_grant *grant, srslte_softbuffer_rx_t *softbuffer, uint8_t *payload);
    
  private: 
    phy_params    *params_db;
    srslte_cell_t  cell; 
    srslte_ue_dl_t ue_dl; 
    srslte_phich_t phich; 
    cf_t          *signal_buffer; 
    uint32_t       cfi; 
    bool           sf_symbols_and_ce_done; 
    bool           pdcch_llr_extracted;    
    bool           pending_rar_grant; 
    srslte_dci_rar_grant_t rar_grant; 
  };    
}
}
#endif
