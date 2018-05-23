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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

#include "srsenb/hdr/mac/mac.h"
#include "srsenb/hdr/phy/phy.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/common/common.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/logger_file.h"
#include "srslte/common/log_filter.h"
#include "srslte/upper/rlc.h"
#include "srslte/radio/radio.h"
#include "srslte/phy/utils/debug.h"

#define START_TUNTAP
#define USE_RADIO

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/

#define LCID 3

typedef struct {
  float rx_freq; 
  float tx_freq; 
  float rx_gain;
  float tx_gain;
  bool  enable_gui; 
  int time_adv;
  std::string ip_address;
}prog_args_t;

uint32_t srsapps_verbose = 1; 

prog_args_t prog_args; 

void args_default(prog_args_t *args) {
  args->rx_freq = 2.505e9;
  args->tx_freq = 2.625e9;
  args->rx_gain = 50.0; 
  args->tx_gain = 70.0;
  args->enable_gui = false; 
  args->time_adv = -1; // calibrated for b210
  args->ip_address = "192.168.3.1";
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [gGIrfFdv] \n", prog);
  printf("\t-f RX frequency [Default %.1f MHz]\n", args->rx_freq/1e6);
  printf("\t-F TX frequency [Default %.1f MHz]\n", args->tx_freq/1e6);
  printf("\t-g RX gain [Default %.1f]\n", args->rx_gain);
  printf("\t-G TX gain [Default %.1f]\n", args->tx_gain);
  printf("\t-I IP address [Default %s]\n", args->ip_address.c_str());
  printf("\t-t time advance (in samples) [Default %d]\n", args->time_adv);
  printf("\t-d Enable gui [Default disabled]\n");
  printf("\t-v [increase verbosity, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "gGfFItdv")) != -1) {
    switch (opt) {
    case 'd':
      args->enable_gui = true;
      break;
    case 'g':
      args->rx_gain = atof(argv[optind]);
      break;
    case 'G':
      args->tx_gain = atof(argv[optind]);
      break;
    case 'f':
      args->rx_freq = atof(argv[optind]);
      break;
    case 'F':
      args->tx_freq = atof(argv[optind]);
      break;
    case 'I':
      args->ip_address = argv[optind];
      break;
    case 't':
      args->time_adv = atoi(argv[optind]);
      break;
    case 'v':
      srsapps_verbose++;
      break;
    default:
      usage(args, argv[0]);
      exit(-1);
    }
  }
  if (args->rx_freq < 0 || args->tx_freq < 0) {
    usage(args, argv[0]);
    exit(-1);
  }
}

LIBLTE_BYTE_MSG_STRUCT sib_buffer[2];
                            
int setup_if_addr(char *ip_addr);
  
class tester : public srsue::pdcp_interface_rlc, 
               public srsue::rrc_interface_rlc,
               public srsue::ue_interface,
               public srsenb::rlc_interface_mac,
               public srsenb::rrc_interface_mac,
               public thread
{
public:
  
  tester() {
    rnti = 0; 
  }
  
  void init(srslte::rlc *rlc_, srsenb::mac *mac_, srsenb::phy *phy_, srslte::log *log_h_, std::string ip_address) {
    log_h = log_h_; 
    rlc   = rlc_; 
    mac   = mac_; 
    phy   = phy_;
    
    tun_fd = 0; 
    
#ifdef START_TUNTAP
    if (init_tuntap((char*) ip_address.c_str())) {
      log_h->error("Initiating IP address\n");
    }
#endif

    pool = srslte::byte_buffer_pool::get_instance();
    
    // Start reader thread
    running=true; 
    start();
  }

  void write_pdu_bcch_bch(srslte::byte_buffer_t *sdu) {}
  void write_pdu_bcch_dlsch(srslte::byte_buffer_t *sdu) {}
  void write_pdu_pcch(srslte::byte_buffer_t *sdu) {}
  void write_pdu_mch(uint32_t lcid, srslte::byte_buffer_t *pdu){}
  void max_retx_attempted(){}
  void add_user(uint16_t rnti) {} 
  void release_user(uint16_t rnti) {} 
  void upd_user(uint16_t rnti, uint16_t old_rnti) {}
  void set_activity_user(uint16_t rnti) {}
  bool is_paging_opportunity(uint32_t tti, uint32_t *payload_len) {return false;}
  void read_pdu_pcch(uint8_t* payload, uint32_t buffer_size) {}
  std::string get_rb_name(uint32_t lcid) { return std::string("lcid"); }
  
  void write_pdu(uint32_t lcid, srslte::byte_buffer_t *sdu)
  {
    int n = write(tun_fd, sdu->msg, sdu->N_bytes);
    if (n != (int) sdu->N_bytes) {
      log_h->error("TUN/TAP write failure n=%d, nof_bytes=%d\n", n, sdu->N_bytes);
      return; 
    }
    log_h->debug_hex(sdu->msg, sdu->N_bytes, 
                    "Wrote %d bytes to TUN/TAP\n", 
                    sdu->N_bytes);         
    pool->deallocate(sdu);
  }
  
  int read_pdu(uint16_t rnti, uint32_t lcid, uint8_t *payload, uint32_t nof_bytes)
  {
    return rlc->read_pdu(lcid, payload, nof_bytes);
  }
  
  void read_pdu_bcch_dlsch(uint32_t sib_index, uint8_t payload[srsenb::sched_interface::MAX_SIB_PAYLOAD_LEN])
  {
    if (sib_index < 2) {
      memcpy(payload, sib_buffer[sib_index].msg, sib_buffer[sib_index].N_bytes);
    } 
  }

  void write_pdu(uint16_t rnti, uint32_t lcid, uint8_t *payload, uint32_t nof_bytes)
  {
    srslte::byte_buffer_t *sdu = NULL;
    log_h->info("Received PDU rnti=0x%x, lcid=%d, nof_bytes=%d\n", rnti, lcid, nof_bytes);
    switch(lcid) {
      case LCID:
        rlc->write_pdu(lcid, payload, nof_bytes);
      break;
      case 0:        
        log_h->info("Received ConnectionRequest from rnti=0x%x\n", rnti);
        
        // Configure User in MAC
        srsenb::sched_interface::ue_cfg_t uecfg; 
        bzero(&uecfg, sizeof(srsenb::sched_interface::ue_cfg_t));
        uecfg.maxharq_tx = 5; 
        uecfg.continuous_pusch = false; 
        uecfg.ue_bearers[0].direction    = srsenb::sched_interface::ue_bearer_cfg_t::BOTH; 
        uecfg.ue_bearers[LCID].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH; 
        mac->ue_cfg(rnti, &uecfg);

        // configure DRB1 as UM
        LIBLTE_RRC_RLC_CONFIG_STRUCT cfg; 
        bzero(&cfg, sizeof(LIBLTE_RRC_RLC_CONFIG_STRUCT));
        cfg.rlc_mode = LIBLTE_RRC_RLC_MODE_UM_BI; 
        cfg.dl_um_bi_rlc.t_reordering = LIBLTE_RRC_T_REORDERING_MS100; 
        cfg.dl_um_bi_rlc.sn_field_len = LIBLTE_RRC_SN_FIELD_LENGTH_SIZE10;   
        cfg.ul_um_bi_rlc.sn_field_len = LIBLTE_RRC_SN_FIELD_LENGTH_SIZE10;   
        rlc->add_bearer(LCID, &cfg);
        
        // Send dummy ConnectionSetup. MAC will send contention resolution ID automatically. 
        log_h->info("Sending ConnectionSetup\n");
        sdu = pool_allocate;
        sdu->msg[0]  = 0xab; 
        sdu->N_bytes = 1; 
        rlc->write_sdu(0, sdu);
        
        // Indicate RLC status to mac 
        mac->rlc_buffer_state(rnti, 0, 1, 0);
        
        LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT dedicated; 
        bzero(&dedicated, sizeof(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT));
        dedicated.pusch_cnfg_ded.beta_offset_ack_idx = 5;
        dedicated.pusch_cnfg_ded.beta_offset_ri_idx  = 12;
        dedicated.pusch_cnfg_ded.beta_offset_cqi_idx = 15;
        dedicated.pusch_cnfg_ded_present = true; 
        dedicated.sched_request_cnfg.dsr_trans_max = LIBLTE_RRC_DSR_TRANS_MAX_N4; 
        dedicated.sched_request_cnfg.sr_pucch_resource_idx = 0;
        dedicated.sched_request_cnfg.sr_cnfg_idx = 35; 
        dedicated.sched_request_cnfg_present = true; 
        phy->set_config_dedicated(rnti, &dedicated);
        
        usleep(500);
      break;
      default:
        log_h->error("Received message for lcid=%d\n", lcid);
      break;
    }    
  }
  
  void rl_failure(uint16_t rnti) 
  {
    log_h->console("Disconnecting rnti=0x%x.\n", rnti);
    mac->ue_rem(rnti);  
    rlc->reset();
  }
  
private:
  int tun_fd;
  bool running; 
  srslte::log *log_h;
  srslte::byte_buffer_pool *pool;
  srslte::rlc *rlc; 
  srsenb::mac *mac;
  srsenb::phy *phy;
  uint16_t rnti; 
  bool read_enable;

  int init_tuntap(char *ip_address) {
    read_enable = true; 
    tun_fd = setup_if_addr(ip_address);
    if (tun_fd<0) {
      fprintf(stderr, "Error setting up IP %s\n", ip_address);
      return -1;
    }
    printf("Created tun/tap interface at IP %s\n", ip_address);
    return 0; 
  }
  
  void run_thread() {
    struct iphdr   *ip_pkt;
    uint32_t        idx = 0;
    int32_t         N_bytes = 0;
    srslte::byte_buffer_t  *pdu = pool_allocate;

    log_h->info("TUN/TAP reader thread running\n");

    int first=1;
    while(running) {
      if (tun_fd > 0) {
        pdu->msg[0] = 0x0; 
        N_bytes = read(tun_fd, &pdu->msg[idx], SRSLTE_MAX_BUFFER_SIZE_BYTES-SRSLTE_BUFFER_HEADER_OFFSET - idx);
      } 
      if(N_bytes > 0)
      {
        if (read_enable && pdu->msg[0] != 0x60) {

          pdu->N_bytes = idx + N_bytes;
          ip_pkt       = (struct iphdr*)pdu->msg;

          log_h->debug_hex(pdu->msg, pdu->N_bytes, 
                          "Read %d bytes from TUN/TAP\n", 
                          N_bytes);
          
          // Check if entire packet was received
          if(ntohs(ip_pkt->tot_len) == pdu->N_bytes)
          {
            // Send PDU directly to RLC
            pdu->set_timestamp();
            rlc->write_sdu(LCID, pdu);
            
            // Indicate RLC status to mac 
            mac->rlc_buffer_state(rnti, LCID, rlc->get_buffer_state(LCID), 0);
            
            pdu = pool_allocate;
            idx = 0;
          } else{
            idx += N_bytes;
          }
        }
      }else{
        log_h->error("Failed to read from TUN interface - gw receive thread exiting.\n");
        break;
      }
    }
  }  
};


// Create classes
srslte::logger_file logger;
srslte::log_filter  log_phy;
srslte::log_filter  log_mac;
srslte::log_filter  log_rlc;
srslte::log_filter  log_tester;
srsenb::phy my_phy;
srsenb::mac my_mac;
srslte::rlc  my_rlc;
srslte::radio my_radio; 

// Local classes for testing
tester my_tester; 


void generate_cell_configuration(srsenb::sched_interface::cell_cfg_t *mac_cfg, srsenb::phy_cfg_t *phy_cfg)
{
  // Main cell configuration 
  srslte_cell_t cell;
  cell.id = 0; 
  cell.cp = SRSLTE_CP_NORM; 
  cell.nof_ports = 1; 
  cell.nof_prb = 25; 
  cell.phich_length = SRSLTE_PHICH_NORM;
  cell.phich_resources = SRSLTE_PHICH_R_1;
  
  // Generate SIB1
  LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT msg[2];
  bzero(&msg[0], sizeof(LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT));
  bzero(&msg[1], sizeof(LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT));    
  
  msg[0].N_sibs = 1; 
  msg[0].sibs[0].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *sib1 = &msg[0].sibs[0].sib.sib1;
  
  sib1->cell_id = 0x1234;
  sib1->tracking_area_code = 0x1234;
  sib1->freq_band_indicator = 2; 
  sib1->N_plmn_ids = 1; 
  sib1->plmn_id[0].id.mcc = 1;  
  sib1->plmn_id[0].id.mnc = 1;
  sib1->plmn_id[0].resv_for_oper = LIBLTE_RRC_NOT_RESV_FOR_OPER;
  sib1->cell_barred = LIBLTE_RRC_CELL_NOT_BARRED;
  sib1->intra_freq_reselection = LIBLTE_RRC_INTRA_FREQ_RESELECTION_ALLOWED;
  sib1->q_rx_lev_min = -140;
  sib1->q_rx_lev_min_offset = 1;
  sib1->p_max = 10; 
  sib1->p_max_present = true; 
  sib1->si_window_length = LIBLTE_RRC_SI_WINDOW_LENGTH_MS40;
  sib1->N_sched_info = 1; 
  sib1->sched_info[0].si_periodicity = LIBLTE_RRC_SI_PERIODICITY_RF16; 
  sib1->sched_info[0].N_sib_mapping_info = 0;
  sib1->system_info_value_tag = 8; 
    
  // Generate SIB2
  msg[1].N_sibs = 2; 
  msg[1].sibs[0].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2;
  msg[1].sibs[1].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *sib2 = &msg[1].sibs[0].sib.sib2;
  
  // RACH configuration 
  sib2->rr_config_common_sib.rach_cnfg.num_ra_preambles = LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N64;
  sib2->rr_config_common_sib.rach_cnfg.preambles_group_a_cnfg.present = false; 
  sib2->rr_config_common_sib.rach_cnfg.preamble_init_rx_target_pwr = LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N90;
  sib2->rr_config_common_sib.rach_cnfg.pwr_ramping_step = LIBLTE_RRC_POWER_RAMPING_STEP_DB6;
  sib2->rr_config_common_sib.rach_cnfg.preamble_trans_max = LIBLTE_RRC_PREAMBLE_TRANS_MAX_N10;
  sib2->rr_config_common_sib.rach_cnfg.ra_resp_win_size = LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_SF10;
  sib2->rr_config_common_sib.rach_cnfg.mac_con_res_timer = LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_SF40;
  sib2->rr_config_common_sib.rach_cnfg.max_harq_msg3_tx = 4;
  
  // BCCH
  sib2->rr_config_common_sib.bcch_cnfg.modification_period_coeff = LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_N16;
    
  // PCCH 
  sib2->rr_config_common_sib.pcch_cnfg.default_paging_cycle = LIBLTE_RRC_DEFAULT_PAGING_CYCLE_RF128;
  sib2->rr_config_common_sib.pcch_cnfg.nB = LIBLTE_RRC_NB_ONE_THIRTY_SECOND_T;

  // PRACH Configuration
  sib2->rr_config_common_sib.prach_cnfg.root_sequence_index = 41; 
  sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.high_speed_flag = false; 
  sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.prach_config_index = 4;
  sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.prach_freq_offset = 2;
  sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.zero_correlation_zone_config = 11;

  // PDSCH configuration
  sib2->rr_config_common_sib.pdsch_cnfg.p_b = 0; 
  sib2->rr_config_common_sib.pdsch_cnfg.rs_power = -5; 

  // PUSCH configuration
  sib2->rr_config_common_sib.pusch_cnfg.n_sb = 1;
  sib2->rr_config_common_sib.pusch_cnfg.hopping_mode = LIBLTE_RRC_HOPPING_MODE_INTER_SUBFRAME;
  sib2->rr_config_common_sib.pusch_cnfg.pusch_hopping_offset = 4;
  sib2->rr_config_common_sib.pusch_cnfg.enable_64_qam = false; 
  sib2->rr_config_common_sib.pusch_cnfg.ul_rs.cyclic_shift = 0; 
  sib2->rr_config_common_sib.pusch_cnfg.ul_rs.group_assignment_pusch = 0;
  sib2->rr_config_common_sib.pusch_cnfg.ul_rs.group_hopping_enabled = false; 
  sib2->rr_config_common_sib.pusch_cnfg.ul_rs.sequence_hopping_enabled = false; 
  
  // PUCCH configuration
  sib2->rr_config_common_sib.pucch_cnfg.delta_pucch_shift = LIBLTE_RRC_DELTA_PUCCH_SHIFT_DS2;
  sib2->rr_config_common_sib.pucch_cnfg.n_rb_cqi = 2;
  sib2->rr_config_common_sib.pucch_cnfg.n_cs_an = 0;
  sib2->rr_config_common_sib.pucch_cnfg.n1_pucch_an = 12;

  // SRS configuration 
  sib2->rr_config_common_sib.srs_ul_cnfg.present = false; 
  
  // UL power control 
  sib2->rr_config_common_sib.ul_pwr_ctrl.p0_nominal_pusch = -80; 
  sib2->rr_config_common_sib.ul_pwr_ctrl.alpha = LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_1; 
  sib2->rr_config_common_sib.ul_pwr_ctrl.p0_nominal_pucch = -80; 
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_1  = LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_0; 
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_1b = LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_5; 
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_2  = LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_2; 
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_2a = LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_2; 
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_2b = LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_2; 
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_preamble_msg3 = 4;
  
  sib2->rr_config_common_sib.ul_cp_length = LIBLTE_RRC_UL_CP_LENGTH_1;
  
  sib2->ue_timers_and_constants.t300 = LIBLTE_RRC_T300_MS1000;
  sib2->ue_timers_and_constants.t301 = LIBLTE_RRC_T301_MS1000;
  sib2->ue_timers_and_constants.n310 = LIBLTE_RRC_N310_N10;
  sib2->ue_timers_and_constants.t311 = LIBLTE_RRC_T311_MS1000;
  sib2->ue_timers_and_constants.n311 = LIBLTE_RRC_N311_N1;

  sib2->time_alignment_timer = LIBLTE_RRC_TIME_ALIGNMENT_TIMER_INFINITY;
  sib2->additional_spectrum_emission = 1; 
  sib2->arfcn_value_eutra.present = false; 
  sib2->ul_bw.present = false; 
  
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT *sib3 = &msg[1].sibs[1].sib.sib3;
  
  bzero(sib3, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT));
  sib3->q_hyst = LIBLTE_RRC_Q_HYST_DB_2;
  sib3->s_non_intra_search = 6;
  sib3->s_non_intra_search_present = true; 
  sib3->thresh_serving_low = 4;
  sib3->cell_resel_prio = 6; 
  sib3->q_rx_lev_min = -122; 
  sib3->p_max = 23;
  sib3->p_max_present = true; 
  sib3->s_intra_search = 10; 
  sib3->s_intra_search_present = true; 
  sib3->presence_ant_port_1 = true; 
  sib3->neigh_cell_cnfg = 1;
  sib3->t_resel_eutra = 1; 
  
  // Genreate payload   
  LIBLTE_BIT_MSG_STRUCT bitbuffer[2];
  for (int i=0;i<2;i++) {
    liblte_rrc_pack_bcch_dlsch_msg(&msg[i], &bitbuffer[i]);
    srslte_bit_pack_vector(bitbuffer[i].msg, sib_buffer[i].msg, bitbuffer[i].N_bits);
    sib_buffer[i].N_bytes = (bitbuffer[i].N_bits-1)/8+1;
  }

  // Fill MAC scheduler configuration 
  bzero(mac_cfg, sizeof(srsenb::sched_interface::cell_cfg_t));
  memcpy(&mac_cfg->cell, &cell, sizeof(srslte_cell_t));
  mac_cfg->sibs[0].len = sib_buffer[0].N_bytes;
  mac_cfg->sibs[0].period_rf = 8; // Fixed to 8 rf
  mac_cfg->sibs[1].len = sib_buffer[1].N_bytes;
  mac_cfg->sibs[1].period_rf = liblte_rrc_si_periodicity_num[sib1->sched_info[0].si_periodicity];
  mac_cfg->si_window_ms = liblte_rrc_si_window_length_num[sib1->si_window_length];
  
 
  mac_cfg->prach_rar_window = liblte_rrc_ra_response_window_size_num[sib2->rr_config_common_sib.rach_cnfg.ra_resp_win_size];

  // Copy PHY common configuration 
  bzero(phy_cfg, sizeof(srsenb::phy_cfg_t));  
  memcpy(&phy_cfg->cell, &cell, sizeof(srslte_cell_t));
  memcpy(&phy_cfg->prach_cnfg,  &sib2->rr_config_common_sib.prach_cnfg, sizeof(LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT));
  memcpy(&phy_cfg->pdsch_cnfg,  &sib2->rr_config_common_sib.pdsch_cnfg, sizeof(LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT));
  memcpy(&phy_cfg->pusch_cnfg,  &sib2->rr_config_common_sib.pusch_cnfg, sizeof(LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT));
  memcpy(&phy_cfg->pucch_cnfg,  &sib2->rr_config_common_sib.pucch_cnfg, sizeof(LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT));
  memcpy(&phy_cfg->srs_ul_cnfg, &sib2->rr_config_common_sib.srs_ul_cnfg, sizeof(LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT));
}

int main(int argc, char *argv[])
{
  
  parse_args(&prog_args, argc, argv);

  logger.init("/tmp/ip_test.log");
  log_phy.init("PHY ", &logger, true);
  log_mac.init("MAC ", &logger, true);
  log_rlc.init("RLC ", &logger);
  log_tester.init("TEST", &logger);
  logger.log("\n\n");
 
  if (srsapps_verbose == 1) {
    log_phy.set_level(srslte::LOG_LEVEL_INFO);
    log_phy.set_hex_limit(100);
    log_mac.set_level(srslte::LOG_LEVEL_DEBUG);
    log_mac.set_hex_limit(100);
    log_rlc.set_level(srslte::LOG_LEVEL_DEBUG);
    log_rlc.set_hex_limit(1000);
    log_tester.set_level(srslte::LOG_LEVEL_DEBUG);
    log_tester.set_hex_limit(100);
    printf("Log level info\n");
  }
  if (srsapps_verbose == 2) {
    log_phy.set_level(srslte::LOG_LEVEL_DEBUG);
    log_phy.set_hex_limit(100);
    log_mac.set_level(srslte::LOG_LEVEL_DEBUG);
    log_mac.set_hex_limit(100);
    log_rlc.set_level(srslte::LOG_LEVEL_DEBUG);
    log_rlc.set_hex_limit(100);
    log_tester.set_level(srslte::LOG_LEVEL_DEBUG);
    log_tester.set_hex_limit(100);
    srslte_verbose = SRSLTE_VERBOSE_DEBUG;
    printf("Log level debug\n");
  }

  // Init Radio and PHY
#ifdef USE_RADIO
  my_radio.init();
#else
  my_radio.init(NULL, (char*) "dummy");
#endif
  my_radio.set_tx_freq(prog_args.tx_freq);
  my_radio.set_tx_gain(prog_args.tx_gain);
  my_radio.set_rx_freq(prog_args.rx_freq);
  my_radio.set_rx_gain(prog_args.rx_gain);
  //my_radio.set_tx_adv_neg(true);
  if (prog_args.time_adv >= 0) {
    printf("Setting TA=%d samples\n", prog_args.time_adv);
    my_radio.set_tx_adv(prog_args.time_adv);
  }
  
  // Configuure cell 
  srsenb::phy_cfg_t phy_cfg;   
  srsenb::sched_interface::cell_cfg_t mac_cfg; 
  srsenb::mac_args_t mac_args; 
  srsenb::phy_args_t phy_args; 
  
  mac_args.link_failure_nof_err = 10; 
  phy_args.equalizer_mode  = "mmse"; 
  phy_args.estimator_fil_w = 0.2;
  phy_args.max_prach_offset_us = 50; 
  phy_args.nof_phy_threads = 1; 
  phy_args.pusch_max_its   = 5; 
  
  generate_cell_configuration(&mac_cfg, &phy_cfg);
  
  my_phy.init(&phy_args, &phy_cfg, &my_radio, &my_mac, &log_phy);
  my_mac.init(&mac_args, &mac_cfg.cell, &my_phy, &my_tester, &my_tester, &log_mac);
  my_rlc.init(&my_tester, &my_tester, &my_tester, &log_rlc, &my_mac, 0 /* SRB0 */);
  my_tester.init(&my_rlc, &my_mac, &my_phy, &log_tester, prog_args.ip_address);
    
  if (prog_args.enable_gui) {
    sleep(1);
    my_phy.start_plot();
  }
      
  bool running = true; 
  while(running) {
    printf("Main running\n");
    sleep(1);
  }
  my_phy.stop();
  my_mac.stop();
}




/******************* This is copied from srsue gw **********************/
int setup_if_addr(char *ip_addr)
{
  char *dev = (char*) "tun_srsenb";
  int sock = -1;

  // Construct the TUN device
  int tun_fd = open("/dev/net/tun", O_RDWR);
  if(0 > tun_fd)
  {
    perror("open");
    return SRSLTE_ERROR;
  }

  struct ifreq ifr;
  
  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  strncpy(ifr.ifr_ifrn.ifrn_name, dev, IFNAMSIZ);
  if(0 > ioctl(tun_fd, TUNSETIFF, &ifr))
  {
    perror("ioctl1");
    goto clean_exit;
  }

  // Bring up the interface
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(0 > ioctl(sock, SIOCGIFFLAGS, &ifr))
  {
    perror("socket");
    goto clean_exit;
  }
  ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
  if(0 > ioctl(sock, SIOCSIFFLAGS, &ifr))
  {
    perror("ioctl2");
    goto clean_exit;
  }

  // Setup the IP address    
  sock                                                   = socket(AF_INET, SOCK_DGRAM, 0);
  ifr.ifr_addr.sa_family                                 = AF_INET;
  ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = inet_addr(ip_addr);
  if(0 > ioctl(sock, SIOCSIFADDR, &ifr))
  {
    perror("ioctl");
    goto clean_exit;
  }
  ifr.ifr_netmask.sa_family                                 = AF_INET;
  ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr = inet_addr("255.255.255.0");
  if(0 > ioctl(sock, SIOCSIFNETMASK, &ifr))
  {
    perror("ioctl");
    goto clean_exit;
  }
  shutdown(sock, SHUT_RDWR);

  return(tun_fd);

clean_exit:
  if (sock != -1) {
    shutdown(sock, SHUT_RDWR);
  }
  if (tun_fd != -1) {
    close(tun_fd);
  }
  return SRSLTE_ERROR;
}
