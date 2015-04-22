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

#include <string.h>
#include <strings.h>
#include <pthread.h>
#include "srslte/srslte.h"

#include "srslte/ue_itf/sched_grant.h"
#include "srslte/ue_itf/dl_buffer.h"
#include "srslte/ue_itf/phy.h"
#include "srslte/ue_itf/phy_params.h"

    
namespace srslte {
namespace ue {
 
bool dl_buffer::init_cell(srslte_cell_t cell_, phy_params *params_db_)
{
  params_db = params_db_; 
  cell = cell_; 
  sf_symbols_and_ce_done = false; 
  pdcch_llr_extracted = false; 
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

// FIXME: Avoid this memcpy modifying ue_sync to directly write into provided pointer
bool dl_buffer::recv_ue_sync(srslte_ue_sync_t *ue_sync, srslte_timestamp_t *rx_time)
{
  bool ret = false; 
  if (signal_buffer) {
    INFO("DL Buffer TTI %d: Receiving packet\n", tti);
    cf_t *sf_buffer = NULL;
    sf_symbols_and_ce_done = false; 
    pdcch_llr_extracted = false; 
    if (srslte_ue_sync_get_buffer(ue_sync, &sf_buffer) == 1) {
      memcpy(signal_buffer, sf_buffer, sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
      ready();
      ret = true; 
    } 
    srslte_ue_sync_get_last_timestamp(ue_sync, rx_time);        
  }
  return ret; 
}

bool dl_buffer::get_ul_grant(pdcch_ul_search_t mode, sched_grant *grant)
{
  if (signal_buffer) {
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
    if (srslte_ue_dl_find_ul_dci(&ue_dl, &dci_msg, cfi, tti%10, grant->get_rnti())) {
      return false; 
    }
    
    if (srslte_dci_msg_to_ra_ul(&dci_msg, cell.nof_prb, 
                            params_db->get_param(phy_params::PUSCH_HOPPING_OFFSET),  
                            (srslte_ra_pusch_t*) grant->get_grant_ptr())) 
    {
      return false; 
    }

    return true; 
  }

}

bool dl_buffer::get_dl_grant(pdcch_dl_search_t mode, sched_grant *grant)
{
  if (signal_buffer && is_ready()) {
    INFO("DL Buffer TTI %d: Getting DL grant\n", tti);
    if (!sf_symbols_and_ce_done) {
    INFO("DL Buffer TTI %d: Getting DL grant. Calling fft estimate\n", tti);
      if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
        return false; 
      }
      sf_symbols_and_ce_done = true; 
    }
    if (!pdcch_llr_extracted) {
    INFO("DL Buffer TTI %d: Getting DL grant. extracting LLR\n", tti);
      if (srslte_pdcch_extract_llr(&ue_dl.pdcch, ue_dl.sf_symbols, ue_dl.ce, 0, tti%10, cfi)) {
        return false; 
      }
      pdcch_llr_extracted = true; 
    }
    
    if (SRSLTE_VERBOSE_ISINFO()) {
      srslte_vec_save_file((char*) "ce1", ue_dl.ce[0], SRSLTE_SF_LEN_RE(ue_dl.cell.nof_prb, ue_dl.cell.cp)*sizeof(cf_t));      
      srslte_vec_save_file((char*) "ce2", ue_dl.ce[1], SRSLTE_SF_LEN_RE(ue_dl.cell.nof_prb, ue_dl.cell.cp)*sizeof(cf_t));      
      srslte_vec_save_file((char*) "pdcch_d", ue_dl.pdcch.d, 36*ue_dl.pdcch.nof_cce*sizeof(cf_t));      
      srslte_vec_save_file((char*) "pdcch_llr", ue_dl.pdcch.llr, 72*ue_dl.pdcch.nof_cce*sizeof(cf_t));      
    }
    
    srslte_dci_msg_t dci_msg; 
    if (srslte_ue_dl_find_dl_dci(&ue_dl, &dci_msg, cfi, tti%10, grant->get_rnti()) != 1) {
      return false; 
    }
    
    grant->set_ncce(srslte_ue_dl_get_ncce(&ue_dl));
    
    if (srslte_dci_msg_to_ra_dl(&dci_msg, grant->get_rnti(), cell, cfi, 
                                (srslte_ra_pdsch_t*) grant->get_grant_ptr())) {
      return false; 
    }
    
    return true;     
  }
}

bool dl_buffer::decode_ack(sched_grant pusch_grant)
{
  if (signal_buffer && is_ready()) {
    if (!sf_symbols_and_ce_done) {
      if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
        return false; 
      }
      sf_symbols_and_ce_done = true; 
    }

    srslte_ra_pusch_t *ra_ul = (srslte_ra_pusch_t*) pusch_grant.get_grant_ptr();
    
    return srslte_ue_dl_decode_phich(&ue_dl, tti%10, ra_ul->prb_alloc.n_prb[0], ra_ul->n_dmrs);     
  }
}

bool dl_buffer::decode_data(sched_grant pdsch_grant, uint8_t *payload)
{
  if (signal_buffer && is_ready()) {
    INFO("DL Buffer TTI %d: Decoding PDSCH\n", tti);
    if (!sf_symbols_and_ce_done) {
      INFO("DL Buffer TTI %d: Decoding PDSCH. Calling fft estimate\n", tti);
      if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
        return false; 
      }
      sf_symbols_and_ce_done = true; 
    }
    
    srslte_ra_pdsch_t *ra_dl = (srslte_ra_pdsch_t*) pdsch_grant.get_grant_ptr();
    if (srslte_harq_setup_dl(&ue_dl.harq_process[0], ra_dl->mcs, 
                            ra_dl->rv_idx, tti%10, &ra_dl->prb_alloc)) {
      fprintf(stderr, "Error configuring HARQ process\n");
      return SRSLTE_ERROR;
    }
    if (ue_dl.harq_process[0].mcs.mod > 0 && ue_dl.harq_process[0].mcs.tbs >= 0) {
      int ret = srslte_pdsch_decode_rnti(&ue_dl.pdsch, &ue_dl.harq_process[0], ue_dl.sf_symbols, 
                              ue_dl.ce, 0, pdsch_grant.get_rnti(), payload);

      if (SRSLTE_VERBOSE_ISINFO()) {
        srslte_vec_save_file((char*) "pdsch_d", ue_dl.pdsch.d, ue_dl.harq_process[0].nof_re*sizeof(cf_t));      
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