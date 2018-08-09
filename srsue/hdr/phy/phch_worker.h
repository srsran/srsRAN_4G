/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSUE_PHCH_WORKER_H
#define SRSUE_PHCH_WORKER_H

#include <string.h>
#include "srslte/srslte.h"
#include "srslte/common/thread_pool.h"
#include "srslte/common/trace.h"
#include "phch_common.h"

#define LOG_EXECTIME

namespace srsue {

class phch_worker : public srslte::thread_pool::worker
{
public:
  
  phch_worker();
  ~phch_worker();
  void  reset(); 
  void  set_common(phch_common *phy);
  void  enable_pdsch_coworker();
  bool  init(uint32_t max_prb, srslte::log *log, srslte::log *log_phy_lib_h, chest_feedback_itf *chest_loop);

  bool  set_cell(srslte_cell_t cell);

  /* Functions used by main PHY thread */
  cf_t* get_buffer(uint32_t antenna_idx);
  void  set_tti(uint32_t tti, uint32_t tx_tti); 
  void  set_tx_time(srslte_timestamp_t tx_time, uint32_t next_offset);
  void  set_prach(cf_t *prach_ptr, float prach_power);
  void  set_cfo(float cfo);

  void  set_ul_params(bool pregen_disabled = false);
  void  set_crnti(uint16_t rnti);
  void  enable_pregen_signals(bool enabled);
  
  void start_trace();
  void write_trace(std::string filename);
  
  int read_ce_abs(float *ce_abs, uint32_t tx_antenna, uint32_t rx_antenna);
  uint32_t get_cell_nof_ports() {
    if (cell_initiated) {
      return cell.nof_ports;
    } else {
      return 1;
    }
  };
  uint32_t get_rx_nof_antennas() {
    return ue_dl.nof_rx_antennas;
  };
  int read_pdsch_d(cf_t *pdsch_d);
  void start_plot();

  float get_ref_cfo();
  float get_snr();
  float get_rsrp();
  float get_noise();
  float get_cfo();

private:
  /* Inherited from thread_pool::worker. Function called every subframe to run the DL/UL processing */
  void work_imp();

  
  /* Internal methods */

  void compute_ri(uint8_t *ri, uint8_t *pmi, float *sinr);
  bool extract_fft_and_pdcch_llr(subframe_cfg_t sf_cfg);
  
  /* ... for DL */
  bool decode_pdcch_ul(mac_interface_phy::mac_grant_t *grant);
  bool decode_pdcch_dl(mac_interface_phy::mac_grant_t *grant);
  bool decode_phich(bool *ack);

  int decode_pdsch(srslte_ra_dl_grant_t *grant,
                   uint8_t *payload[SRSLTE_MAX_CODEWORDS],
                   srslte_softbuffer_rx_t *softbuffers[SRSLTE_MAX_CODEWORDS],
                   int rv[SRSLTE_MAX_CODEWORDS],
                   uint16_t rnti,
                   uint32_t pid,
                   bool acks[SRSLTE_MAX_CODEWORDS]);

  bool decode_pmch(srslte_ra_dl_grant_t *grant,
                   uint8_t *payload,
                   srslte_softbuffer_rx_t* softbuffer,
                   uint16_t mbsfn_area_id);

  /* ... for UL */
  void encode_pusch(srslte_ra_ul_grant_t *grant, uint8_t *payload, uint32_t current_tx_nb, srslte_softbuffer_tx_t *softbuffer, 
                    uint32_t rv, uint16_t rnti, bool is_from_rar);
  void encode_pucch();
  void encode_srs();
  void reset_uci();
  void set_uci_sr();
  void set_uci_periodic_cqi();
  void set_uci_aperiodic_cqi();
  void set_uci_ack(bool ack[SRSLTE_MAX_CODEWORDS], bool tb_en[SRSLTE_MAX_CODEWORDS]);
  bool srs_is_ready_to_send();
  float set_power(float tx_power);
  void setup_tx_gain();
  
  void update_measurements();
  
  void tr_log_start();
  void tr_log_end();
  struct timeval tr_time[3];
  srslte::trace<uint32_t> tr_exec;
  bool trace_enabled; 

  pthread_mutex_t mutex;
  
  /* Common objects */  
  phch_common    *phy;
  srslte::log    *log_h;
  srslte::log    *log_phy_lib_h;
  chest_feedback_itf *chest_loop;
  srslte_cell_t  cell;
  bool           mem_initiated;
  bool           cell_initiated;
  cf_t          *signal_buffer[SRSLTE_MAX_PORTS]; 
  uint32_t       tti; 
  uint32_t       tx_tti;
  bool           pregen_enabled;
  uint32_t       last_dl_pdcch_ncce;
  bool           rnti_is_set;

  uint32_t next_offset;

  /* Objects for DL */
  srslte_ue_dl_t ue_dl; 
  uint32_t       cfi; 
  uint16_t       dl_rnti;
  
  /* Objects for UL */
  srslte_ue_ul_t     ue_ul; 
  srslte_timestamp_t tx_time; 
  srslte_uci_data_t  uci_data; 
  srslte_cqi_value_t cqi_report;
  uint16_t           ul_rnti;
  
  // UL configuration parameters 
  srslte_refsignal_srs_cfg_t        srs_cfg;           
  srslte_pucch_cfg_t                pucch_cfg; 
  srslte_refsignal_dmrs_pusch_cfg_t dmrs_cfg; 
  srslte_pusch_hopping_cfg_t        pusch_hopping; 
  srslte_pucch_sched_t              pucch_sched; 
  srslte_uci_cfg_t                  uci_cfg; 
  srslte_cqi_periodic_cfg_t         period_cqi; 
  srslte_ue_ul_powerctrl_t          power_ctrl;           
  uint32_t                          I_sr;
  bool                              sr_configured;
  float                             cfo;
  bool                              rar_cqi_request;
  cf_t                             *prach_ptr;
  float                             prach_power;

  uint32_t rssi_read_cnt;

  // Metrics
  dl_metrics_t dl_metrics;
  ul_metrics_t ul_metrics;
  
#ifdef LOG_EXECTIME
  struct timeval logtime_start[3]; 
  bool chest_done;
#endif
  
};

} // namespace srsue

#endif // SRSUE_PHCH_WORKER_H

