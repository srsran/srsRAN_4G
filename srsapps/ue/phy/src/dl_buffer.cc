/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <string.h>
#include <strings.h>
#include <pthread.h>
#include "srslte/srslte.h"

#include "srsapps/common/log.h"
#include "srsapps/ue/phy/sched_grant.h"
#include "srsapps/ue/phy/dl_buffer.h"
#include "srsapps/ue/phy/phy.h"
#include "srsapps/ue/phy/phy_params.h"

    
namespace srslte {
namespace ue {
 
bool dl_buffer::init_cell(srslte_cell_t cell_, phy_params *params_db_, log *log_h_)
{
  log_h = log_h_; 
  params_db = params_db_; 
  cell = cell_; 
  sf_symbols_and_ce_done = false; 
  pdcch_llr_extracted = false; 
  pending_rar_grant = false; 
  tti = 0; 
  if (!srslte_ue_dl_init(&ue_dl, cell)) {
    signal_buffer = (cf_t*) srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
    return signal_buffer?true:false;       
  } else {
    return false; 
  }
}

void dl_buffer::free_cell()
{
  if (signal_buffer) {
    free(signal_buffer);
  }
  srslte_ue_dl_free(&ue_dl);
}

void dl_buffer::set_crnti(uint16_t rnti)
{
  srslte_ue_dl_set_rnti(&ue_dl, rnti);
}

// FIXME: Avoid this memcpy modifying ue_sync to directly write into provided pointer
bool dl_buffer::recv_ue_sync(srslte_ue_sync_t *ue_sync, srslte_timestamp_t *rx_time)
{
  bool ret = false; 
  cf_t *sf_buffer = NULL;
  sf_symbols_and_ce_done = false; 
  pdcch_llr_extracted = false; 
  if (signal_buffer) {
    bzero(signal_buffer, sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
    if (srslte_ue_sync_get_buffer(ue_sync, &sf_buffer) == 1) {
      memcpy(signal_buffer, sf_buffer, sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
      ready();
      ret = true; 
    } 
    srslte_ue_sync_get_last_timestamp(ue_sync, rx_time);        
  }
  return ret; 
}

void dl_buffer::discard_pending_rar_grant() {
  pending_rar_grant = false; 
}

bool dl_buffer::get_ul_grant(ul_sched_grant *grant)
{
  if (signal_buffer) {
    if (pending_rar_grant && grant->is_temp_rnti()) {
      return grant->create_from_rar(&rar_grant, cell, params_db->get_param(phy_params::PUSCH_HOPPING_OFFSET)); 
    } else {
      if (!sf_symbols_and_ce_done) {
        if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
          return false; 
        }
        sf_symbols_and_ce_done = true; 
      }
      if (!pdcch_llr_extracted) {
        if (srslte_pdcch_extract_llr(&ue_dl.pdcch, ue_dl.sf_symbols, ue_dl.ce, 0, tti%10, cfi)) {
          return false; 
        }
        pdcch_llr_extracted = true; 
      }
      
      srslte_dci_msg_t dci_msg; 
      if (srslte_ue_dl_find_ul_dci(&ue_dl, &dci_msg, cfi, tti%10, grant->get_rnti()) != 1) {
        return false; 
      }
      
      grant->set_tti(tti);
    
      Info("PDCCH: UL DCI Format0 cce_index=%d, n_data_bits=%d\n", ue_dl.last_n_cce, dci_msg.nof_bits);

      return grant->create_from_dci(&dci_msg, cell, params_db->get_param(phy_params::PUSCH_HOPPING_OFFSET));     
    }      
  }
}

// Unpack RAR grant as defined in Section 6.2 of 36.213 
void dl_buffer::set_rar_grant(uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN])
{
  pending_rar_grant = true; 
  srslte_dci_rar_grant_unpack(&rar_grant, grant_payload);
}

void dl_buffer::set_rar_grant(srslte_dci_rar_grant_t* rar_grant_)
{
  pending_rar_grant = true; 
  memcpy(&rar_grant, rar_grant_, sizeof(srslte_dci_rar_grant_t));
}

bool dl_buffer::get_dl_grant(dl_sched_grant *grant)
{
  if (signal_buffer && is_ready()) {
    Debug("DL Buffer TTI %d: Getting DL grant\n", tti);
    if (!sf_symbols_and_ce_done) {
    Debug("DL Buffer TTI %d: Getting DL grant. Calling fft estimate\n", tti);
      if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
        return false; 
      }
      sf_symbols_and_ce_done = true; 
    }
    if (!pdcch_llr_extracted) {
    Debug("DL Buffer TTI %d: Getting DL grant. extracting LLR\n", tti);
      if (srslte_pdcch_extract_llr(&ue_dl.pdcch, ue_dl.sf_symbols, ue_dl.ce, 0, tti%10, cfi)) {
        return false; 
      }
      pdcch_llr_extracted = true; 
    }
    
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      srslte_vec_save_file((char*) "ce1", ue_dl.ce[0], SRSLTE_SF_LEN_RE(ue_dl.cell.nof_prb, ue_dl.cell.cp)*sizeof(cf_t));      
      srslte_vec_save_file((char*) "ce2", ue_dl.ce[1], SRSLTE_SF_LEN_RE(ue_dl.cell.nof_prb, ue_dl.cell.cp)*sizeof(cf_t));      
      srslte_vec_save_file((char*) "pdcch_d", ue_dl.pdcch.d, 36*ue_dl.pdcch.nof_cce*sizeof(cf_t));      
      srslte_vec_save_file((char*) "pdcch_llr", ue_dl.pdcch.llr, 72*ue_dl.pdcch.nof_cce*sizeof(cf_t));      
    }
    
    srslte_dci_msg_t dci_msg; 
    if (srslte_ue_dl_find_dl_dci(&ue_dl, &dci_msg, cfi, tti%10, grant->get_rnti()) != 1) {
      return false; 
    }
    
    grant->set_tti(tti);
    
    Info("PDCCH: DL DCI %s cce_index=%d, n_data_bits=%d\n", grant->get_dciformat_string(), ue_dl.last_n_cce, dci_msg.nof_bits);
    
    return grant->create_from_dci(&dci_msg, cell.nof_prb, srslte_ue_dl_get_ncce(&ue_dl));     
  }
}

bool dl_buffer::decode_ack(ul_sched_grant *grant)
{
  if (signal_buffer && is_ready()) {
    if (!sf_symbols_and_ce_done) {
      if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
        return false; 
      }
      sf_symbols_and_ce_done = true; 
    }

    return srslte_ue_dl_decode_phich(&ue_dl, tti%10, grant->get_I_lowest(), grant->get_n_dmrs());     
  }
}

void dl_buffer::reset_softbuffer() 
{
  srslte_softbuffer_rx_reset(&ue_dl.softbuffer);
}

bool dl_buffer::decode_data(dl_sched_grant *grant, uint8_t *payload)
{
  return decode_data(grant, &ue_dl.softbuffer, payload);
}

bool dl_buffer::decode_data(dl_sched_grant *grant, srslte_softbuffer_rx_t *softbuffer, uint8_t *payload)
{
  if (signal_buffer && is_ready()) {
    Debug("DL Buffer TTI %d: Decoding PDSCH\n", tti);
    if (!sf_symbols_and_ce_done) {
      Debug("DL Buffer TTI %d: Decoding PDSCH. Calling fft estimate\n", tti);
      if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
        return false; 
      }
      sf_symbols_and_ce_done = true; 
    }
    
    grant->get_pdsch_cfg(tti%10, cfi, &ue_dl);
    if (ue_dl.pdsch_cfg.grant.mcs.mod > 0 && ue_dl.pdsch_cfg.grant.mcs.tbs >= 0) {
      
      int ret = srslte_pdsch_decode_rnti(&ue_dl.pdsch, &ue_dl.pdsch_cfg, softbuffer, ue_dl.sf_symbols, 
                                         ue_dl.ce, 0, grant->get_rnti(), payload);

      if (SRSLTE_VERBOSE_ISDEBUG()) {
        srslte_vec_save_file((char*) "pdsch_d", ue_dl.pdsch.d, ue_dl.pdsch_cfg.nbits.nof_re*sizeof(cf_t));      
      }
      if (ret == SRSLTE_SUCCESS) {
        return true; 
      } 
    }
    return false;    
  }
}
  
}
}
