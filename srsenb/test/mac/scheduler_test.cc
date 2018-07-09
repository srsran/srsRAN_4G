/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#include <unistd.h>

#include "srsenb/hdr/mac/mac.h"
#include "srsenb/hdr/phy/phy.h"

#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/sched_interface.h"
#include "srslte/common/log_filter.h"
#include "srslte/radio/radio.h"
#include "srslte/phy/utils/debug.h"



  
uint8_t sib1_payload[18] = {0x60,0x40,0x04,0x03,0x00,0x01,0x1a,0x2d,0x00,0x18,0x02,0x81,0x80,0x42,0x0c,0x80,0x00,0x00};
uint8_t sib2_payload[41] = {0x00,0x80,0x1c,0x31,0x18,0x6f,0xe1,0x20,0x00,0x35,0x84,0x8c,
                            0xe2,0xd0,0x00,0x02,0x00,0x78,0xee,0x31,0x6a,0xa5,0x37,0x30,
                            0xa0,0x70,0xc9,0x49,0xfa,0x8d,0xd2,0x78,0x1a,0x02,0x77,0x4a,
                            0x92,0x40,0x00,0x00,0x00};

// Define dummy RLC always transmitts
class rlc : public srsenb::rlc_interface_mac
{
public:
  uint32_t get_buffer_state(uint16_t rnti, uint32_t lcid)
  {
    return 1; 
  }

  int read_pdu(uint16_t rnti, uint32_t lcid, uint8_t *payload, uint32_t nof_bytes)
  {
    for (uint32_t i=0;i<nof_bytes;i++) {
      payload[i] = i; 
    }
    return nof_bytes;
  }
  
  void read_pdu_pcch(uint8_t* payload, uint32_t buffer_size) {
    
  }
      
  void read_pdu_bcch_dlsch(uint32_t sib_index, uint8_t payload[srsenb::sched_interface::MAX_SIB_PAYLOAD_LEN])
  {
    switch(sib_index) {
      case 0:
        memcpy(payload, sib1_payload, 18);
        break;
      case 1:
        memcpy(payload, sib2_payload, 41);
        break;
    }
  }

  void write_pdu(uint16_t rnti, uint32_t lcid, uint8_t *payload, uint32_t nof_bytes)
  {
    
  }
  
private:
};


// Create classes
srslte::log_filter   log_out("ALL");
srsenb::sched        my_sched;
srsenb::dl_metric_rr dl_metric;
srsenb::ul_metric_rr ul_metric;
rlc my_rlc; 

int main(int argc, char *argv[])
{
  
  log_out.set_level(srslte::LOG_LEVEL_INFO);
  
  /* Set PHY cell configuration */
  srslte_cell_t cell_cfg_phy; 
  cell_cfg_phy.id = 1; 
  cell_cfg_phy.cp = SRSLTE_CP_NORM; 
  cell_cfg_phy.nof_ports = 1; 
  cell_cfg_phy.nof_prb = 25; 
  cell_cfg_phy.phich_length = SRSLTE_PHICH_NORM;
  cell_cfg_phy.phich_resources = SRSLTE_PHICH_R_1;

  srsenb::sched_interface::cell_cfg_t cell_cfg; 
  
  /* Set MAC cell configuration */
  bzero(&cell_cfg, sizeof(srsenb::sched_interface::cell_cfg_t));
  memcpy(&cell_cfg.cell, &cell_cfg_phy, sizeof(srslte_cell_t));
  cell_cfg.sibs[0].len = 18;
  cell_cfg.sibs[0].period_rf = 8;
  cell_cfg.sibs[1].len = 41;
  cell_cfg.sibs[1].period_rf = 16;
  cell_cfg.si_window_ms = 40;
  
  my_sched.init(NULL, &log_out);
  my_sched.set_metric(&dl_metric, &ul_metric);
  my_sched.cell_cfg(&cell_cfg);  

  srsenb::sched_interface::dl_sched_res_t sched_result_dl;
  srsenb::sched_interface::ul_sched_res_t sched_result_ul;
  uint32_t dl_info_len = 0;
  
  srsenb::sched_interface::ue_cfg_t ue_cfg;
  bzero(&ue_cfg, sizeof(srsenb::sched_interface::ue_cfg_t));
  uint16_t rnti = 30; 
  
  ue_cfg.aperiodic_cqi_period = 40; 
  ue_cfg.maxharq_tx = 5; 
  
  srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg;
  bzero(&bearer_cfg, sizeof(srsenb::sched_interface::ue_bearer_cfg_t));
  bearer_cfg.direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH; 

  my_sched.ue_cfg(rnti, &ue_cfg);
  my_sched.bearer_ue_cfg(rnti, 0, &bearer_cfg);
  //my_sched.dl_rlc_buffer_state(rnti, 0, 1e6, 0);
  my_sched.ul_bsr(rnti, 0, 1e6);

  bool running = true; 
  uint32_t tti = 0; 
  while(running) {
    log_out.step(tti);
    if (tti > 50) {
      running = false; 
    }
    my_sched.dl_sched(tti, &sched_result_dl);
    my_sched.ul_sched(tti, &sched_result_ul);
    tti = (tti+1)%10240;
    if (tti >= 4) {
      my_sched.ul_crc_info(tti, rnti,  tti%2);
    }
  }
  
  
}
