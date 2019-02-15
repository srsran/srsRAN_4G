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


#include <unistd.h>
#include <signal.h>

#include "srslte/asn1/rrc_asn1.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/radio/radio_multi.h"
#include "srsue/hdr/mac/mac.h"
#include "srsue/hdr/phy/phy.h"

using namespace asn1::rrc;

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  float rf_rx_freq;
  float rf_tx_freq; 
  float rf_rx_gain;
  float rf_tx_gain;
  int   verbose; 
  bool  do_trace; 
  bool  do_pcap; 
}prog_args_t;

void args_default(prog_args_t *args) {
  args->rf_rx_freq = -1.0;
  args->rf_tx_freq = -1.0;
  args->rf_rx_gain = -1; // set to autogain
  args->rf_tx_gain = -1; 
  args->verbose     = 0; 
  args->do_trace    = false; 
  args->do_pcap     = false; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [gGtpv] -f rx_frequency (in Hz) -F tx_frequency (in Hz)\n", prog);
  printf("\t-g RF RX gain [Default AGC]\n");
  printf("\t-G RF TX gain [Default same as RX gain (AGC)]\n");
  printf("\t-t Enable trace [Default disabled]\n");
  printf("\t-p Enable PCAP capture [Default disabled]\n");
  printf("\t-v [increase verbosity, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "gGftpFv")) != -1) {
    switch (opt) {
    case 'g':
      args->rf_rx_gain = atof(argv[optind]);
      break;
    case 'G':
      args->rf_tx_gain = atof(argv[optind]);
      break;
    case 'f':
      args->rf_rx_freq = atof(argv[optind]);
      break;
    case 'F':
      args->rf_tx_freq = atof(argv[optind]);
      break;
    case 't':
      args->do_trace = true;
      break;
    case 'p':
      args->do_pcap = true;
      break;
    case 'v':
      args->verbose++;
      break;
    default:
      usage(args, argv[0]);
      exit(-1);
    }
  }
  if (args->rf_rx_freq < 0 || args->rf_tx_freq < 0) {
    usage(args, argv[0]);
    exit(-1);
  }
}

// Determine SI messages scheduling as in 36.331 5.2.3 Acquisition of an SI message
uint32_t sib_start_tti(uint32_t tti, uint32_t period, uint32_t x) {
  return (period*10*(1+tti/(period*10))+x)%10240; // the 1 means next opportunity
}

void setup_mac_phy_sib2(sib_type2_s* sib2, srsue::mac* mac, srsue::phy* phy)
{

  // Apply RACH configuration 
  srsue::mac_interface_rrc::mac_cfg_t mac_cfg; 
  mac->get_config(&mac_cfg);
  mac_cfg.rach = sib2->rr_cfg_common.rach_cfg_common;
  mac->set_config(&mac_cfg);

  printf("Set RACH ConfigCommon: NofPreambles=%d, ResponseWindow=%d, ContentionResolutionTimer=%d ms, MaxTrials=%d\n",
         sib2->rr_cfg_common.rach_cfg_common.preamb_info.nof_ra_preambs.to_number(),
         sib2->rr_cfg_common.rach_cfg_common.ra_supervision_info.ra_resp_win_size.to_number(),
         sib2->rr_cfg_common.rach_cfg_common.ra_supervision_info.mac_contention_resolution_timer.to_number(),
         sib2->rr_cfg_common.rach_cfg_common.ra_supervision_info.preamb_trans_max.to_number());

  // Apply PHY RR Config Common
  srsue::phy_interface_rrc::phy_cfg_common_t common;
  common.pdsch_cnfg  = sib2->rr_cfg_common.pdsch_cfg_common;
  common.pusch_cnfg  = sib2->rr_cfg_common.pusch_cfg_common;
  common.pucch_cnfg  = sib2->rr_cfg_common.pucch_cfg_common;
  common.ul_pwr_ctrl = sib2->rr_cfg_common.ul_pwr_ctrl_common;
  common.prach_cnfg  = sib2->rr_cfg_common.prach_cfg;
  common.srs_ul_cnfg = sib2->rr_cfg_common.srs_ul_cfg_common;

  phy->set_config_common(&common);
  phy->configure_ul_params();

  printf("Set PUSCH ConfigCommon: HopOffset=%d, RSGroup=%d, RSNcs=%d, N_sb=%d\n",
         sib2->rr_cfg_common.pusch_cfg_common.pusch_cfg_basic.pusch_hop_offset,
         sib2->rr_cfg_common.pusch_cfg_common.ul_ref_sigs_pusch.group_assign_pusch,
         sib2->rr_cfg_common.pusch_cfg_common.ul_ref_sigs_pusch.cyclic_shift,
         sib2->rr_cfg_common.pusch_cfg_common.pusch_cfg_basic.n_sb);

  printf("Set PUCCH ConfigCommon: DeltaShift=%d, CyclicShift=%d, N1=%d, NRB=%d\n",
         sib2->rr_cfg_common.pucch_cfg_common.delta_pucch_shift.to_number(),
         sib2->rr_cfg_common.pucch_cfg_common.n_cs_an, sib2->rr_cfg_common.pucch_cfg_common.n1_pucch_an,
         sib2->rr_cfg_common.pucch_cfg_common.n_rb_cqi);

  printf("Set PRACH ConfigCommon: SeqIdx=%d, HS=%d, FreqOffset=%d, ZC=%d, ConfigIndex=%d\n",
         sib2->rr_cfg_common.prach_cfg.root_seq_idx,
         sib2->rr_cfg_common.prach_cfg.prach_cfg_info.high_speed_flag ? 1 : 0,
         sib2->rr_cfg_common.prach_cfg.prach_cfg_info.prach_freq_offset,
         sib2->rr_cfg_common.prach_cfg.prach_cfg_info.zero_correlation_zone_cfg,
         sib2->rr_cfg_common.prach_cfg.prach_cfg_info.prach_cfg_idx);

  if (sib2->rr_cfg_common.srs_ul_cfg_common.type() == srs_ul_cfg_common_c::types::setup) {
    srs_ul_cfg_common_c::setup_s_* setup = &sib2->rr_cfg_common.srs_ul_cfg_common.setup();
    printf("Set SRS ConfigCommon: BW-Configuration=%d, SF-Configuration=%d, ACKNACK=%d\n",
           setup->srs_bw_cfg.to_number(), setup->srs_sf_cfg.to_number(), setup->ack_nack_srs_simul_tx ? 1 : 0);
  }
}

void process_connsetup(rrc_conn_setup_s* msg, srsue::mac* mac, srsue::phy* phy)
{

  // FIXME: There's an error parsing the connectionSetup message. This value is hard-coded:

  rr_cfg_ded_s* rr_ded = &msg->crit_exts.c1().rrc_conn_setup_r8().rr_cfg_ded;
  if (rr_ded->phys_cfg_ded_present) {
    phy->set_config_dedicated(&rr_ded->phys_cfg_ded);
    printf("Set PHY configuration: SR-n_pucch=%d, SR-ConfigIndex=%d, SRS-ConfigIndex=%d, SRS-bw=%d, SRS-Nrcc=%d, "
           "SRS-hop=%d, SRS-Ncs=%d\n",
           rr_ded->phys_cfg_ded.sched_request_cfg.setup().sr_pucch_res_idx,
           rr_ded->phys_cfg_ded.sched_request_cfg.setup().sr_cfg_idx,
           rr_ded->phys_cfg_ded.srs_ul_cfg_ded.setup().srs_cfg_idx,
           rr_ded->phys_cfg_ded.srs_ul_cfg_ded.setup().srs_bw.to_number(),
           rr_ded->phys_cfg_ded.srs_ul_cfg_ded.setup().freq_domain_position,
           rr_ded->phys_cfg_ded.srs_ul_cfg_ded.setup().srs_hop_bw.to_number(),
           rr_ded->phys_cfg_ded.srs_ul_cfg_ded.setup().cyclic_shift.to_number());
  }

  srsue::mac_interface_rrc::mac_cfg_t mac_set;
  mac->get_config(&mac_set);
  mac_set.main = rr_ded->mac_main_cfg.explicit_value();
  // SR is a PHY config but is needed by SR procedure in 36.321 5.4.4
  mac_set.sr = rr_ded->phys_cfg_ded.sched_request_cfg;
  mac->set_config(&mac_set);

  printf("Set MAC configuration: dsr-TransMAX: %d, harq-MaxReTX=%d, bsr-TimerReTX=%d, bsr-TimerPeriodic=%d\n",
         rr_ded->phys_cfg_ded.sched_request_cfg.setup().dsr_trans_max.to_number(),
         rr_ded->mac_main_cfg.explicit_value().ul_sch_cfg.max_harq_tx.to_number(),
         rr_ded->mac_main_cfg.explicit_value().ul_sch_cfg.retx_bsr_timer.to_number(),
         rr_ded->mac_main_cfg.explicit_value().ul_sch_cfg.periodic_bsr_timer.to_number());

  phy->configure_ul_params();

  // Setup radio bearers
  for (uint32_t i = 0; i < rr_ded->srb_to_add_mod_list.size(); i++) {
    if (rr_ded->srb_to_add_mod_list[i].lc_ch_cfg_present) {
      printf("Setting up Default Configuration for SRB%d \n", rr_ded->srb_to_add_mod_list[i].srb_id);
      switch (rr_ded->srb_to_add_mod_list[i].srb_id) {
        case 1:
          mac->setup_lcid(1, 0, 1, -1, -1);
          break;
        case 2: 
          mac->setup_lcid(2, 0, 3, -1, -1);
          break;
      }
    }
  }
//  for (int i=0;i<msg->rr_cnfg.drb_to_add_mod_list_size;i++) {
//    printf("Setting up DRB%d\n", msg->rr_cnfg.drb_to_add_mod_list[i].drb_id);
//    // todo
//  }
}


// Hex bytes for the connection setup complete packet
// Got hex bytes from http://www.sharetechnote.com/html/RACH_LTE.html 
uint8_t setupComplete_segm[2][41] ={ {
  0x88, 0x00, 0x00, 0x20, 0x21, 0x90, 0xa0, 0x12, 0x00, 0x00, 0x80, 0xf0, 0x5e, 0x3b, 0xf1, 0x04, 
  0x64, 0x04, 0x1d, 0x20, 0x44, 0x2f, 0xd8, 0x4b, 0xd1, 0x02, 0x00, 0x00, 0x83, 0x03, 0x41, 0xb0,
  0xe5, 0x60, 0x13, 0x81, 0x83},
  
 {0xb0, 0x01, 0x01, 0x01, 0x48, 0x4b, 0xd1, 0x00, 0x7d, 0x21, 0x70, 0x28, 0x01, 0x5c, 0x08, 0x80,
  0x00, 0xc4, 0x0f, 0x97, 0x80, 0xd0, 0x4c, 0x4b, 0xd1, 0x00, 0xc0, 0x58, 0x44, 0x0d, 0x5d, 0x62,
  0x99, 0x74, 0x04, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00}
};
uint8_t setupComplete[80] = {
  0x88, 0x00, 0x00, 0x20, 0x21, 0x90, 0xa0, 0x12, 0x00, 0x00, 0x80, 0xf0, 0x5e, 0x3b, 0xf1, 0x04, 
  0x64, 0x04, 0x1d, 0x20, 0x44, 0x2f, 0xd8, 0x4b, 0xd1, 0x02, 0x00, 0x00, 0x83, 0x03, 0x41, 0xb0,
  0xe5, 0x60, 0x13, 0x81, 0x83, 0x48, 0x4b, 0xd1, 0x00, 0x7d, 0x21, 0x70, 0x28, 0x01, 0x5c, 0x08, 0x80,
  0x00, 0xc4, 0x0f, 0x97, 0x80, 0xd0, 0x4c, 0x4b, 0xd1, 0x00, 0xc0, 0x58, 0x44, 0x0d, 0x5d, 0x62,
  0x99, 0x74, 0x04, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00};
  
uint32_t lengths[2] = {37, 41}; 
uint8_t reply[2] = {0x00, 0x04};


srslte::radio_multi radio; 
srsue::phy phy; 
srsue::mac mac; 
srslte::mac_pcap mac_pcap; 

prog_args_t prog_args; 
  
void sig_int_handler(int signo)
{
  if (prog_args.do_trace) {
    //radio.write_trace("radio");
    phy.write_trace("phy");
  }
  if (prog_args.do_pcap) {
    mac_pcap.close();
  }
  mac.stop();
  exit(0);
}

class rlctest : public srsue::rlc_interface_mac {
public:
  bool mib_decoded; 
  bool sib1_decoded; 
  bool sib2_decoded; 
  bool connsetup_decoded; 
  int nsegm_dcch; 
  int send_ack; 
  uint8_t si_window_len, sib2_period; 
  
  rlctest() {
    mib_decoded  = false; 
    sib1_decoded = false;
    sib2_decoded = false; 
    connsetup_decoded = false; 
    nsegm_dcch = 0; 
    si_window_len = 0; 
    sib2_period = 0; 
    send_ack = 0;
    bzero(&bit_msg, sizeof(bit_msg));
    bzero(&byte_msg, sizeof(byte_msg));
  }
  bool has_data(uint32_t lcid) {
    return get_buffer_state(lcid); 
  }
  uint32_t get_buffer_state(uint32_t lcid) {
    if (lcid == 0) {
      if (sib2_decoded && !connsetup_decoded) {
        return 6;
      }
    } else if (lcid == 1) {
      if (connsetup_decoded && nsegm_dcch < 2) {    
        return lengths[nsegm_dcch];
      } else if (send_ack == 1) {
        return 2; 
      }
    }
    return 0; 
  }
  
  int read_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes) 
  {
    if (lcid == 0) {
      ul_ccch_msg_s ul_ccch_msg;
      // Prepare ConnectionRequest packet
      ul_ccch_msg.msg.set(ul_ccch_msg_type_c::types::c1);
      ul_ccch_msg.msg.c1().set(ul_ccch_msg_type_c::c1_c_::types::rrc_conn_request);
      ul_ccch_msg.msg.c1().rrc_conn_request().crit_exts.set(
          rrc_conn_request_s::crit_exts_c_::types::rrc_conn_request_r8);
      ul_ccch_msg.msg.c1().rrc_conn_request().crit_exts.rrc_conn_request_r8().ue_id.set(
          init_ue_id_c::types::random_value);
      ul_ccch_msg.msg.c1().rrc_conn_request().crit_exts.rrc_conn_request_r8().ue_id.random_value().from_number(1000);
      ul_ccch_msg.msg.c1().rrc_conn_request().crit_exts.rrc_conn_request_r8().establishment_cause =
          establishment_cause_e::mo_sig;

      asn1::bit_ref bref(payload, nof_bytes);

      ul_ccch_msg.pack(bref);
      uint32_t nbytes = (uint32_t)bref.distance_bytes(payload);
      //      assert(nbytes<1024);

      uint64_t uecri      = 0;
      uint8_t* ue_cri_ptr = (uint8_t*)&uecri;
      uint8_t* ptr        = bit_msg.msg;
      for (uint32_t i=0;i<nbytes;i++) {
        ue_cri_ptr[nbytes - i - 1] = payload[i]; //(uint8_t) srslte_bit_pack(&ptr, 8);
      }
      printf("Setting UE contention resolution ID: %lu\n", uecri);
      mac.set_contention_id(uecri);

      // Send ConnectionRequest Packet
      printf("Send ConnectionRequest %d/%d bytes\n", nbytes, nof_bytes);
      bzero(&payload[nbytes], (nof_bytes-nbytes)*sizeof(uint8_t));
      return nof_bytes;
    } else if (lcid == 1) {
      if (nsegm_dcch < 2) {
        if (nof_bytes >= 80) {
          printf("Sending Connection Setup Complete length 80\n");
          memcpy(payload, setupComplete, 80);
          return 80; 
        } else {
          uint32_t r = 0; 
          if (nof_bytes >= lengths[nsegm_dcch]) {
            printf("Sending Connection Setup Complete %d/2 length %d\n", nsegm_dcch, lengths[nsegm_dcch]);
            memcpy(payload, setupComplete_segm[nsegm_dcch], lengths[nsegm_dcch]);            
            r = lengths[nsegm_dcch];
            nsegm_dcch++;
          } else {
            r = 0;
          }
          return r; 
        }
      } else if (send_ack == 1) {
        printf("Send RLC ACK\n");
        memcpy(payload, reply, 2*sizeof(uint8_t));
        send_ack = 2; 
        return 2; 
      }
    }
    return 0; 
  }

  void     write_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes) {
    if (lcid == 0) {
      dl_ccch_msg_s dl_ccch_msg;
      printf("ConnSetup received %d bytes\n", nof_bytes);
      srslte_vec_fprint_byte(stdout, payload, nof_bytes);

      asn1::bit_ref bref(payload, nof_bytes);
      dl_ccch_msg.unpack(bref);

      printf("Response: %s\n", dl_ccch_msg.msg.c1().type().to_string().c_str());
      switch (dl_ccch_msg.msg.c1().type().value) {
        case dl_ccch_msg_type_c::c1_c_::types::rrc_conn_setup:
          // Process ConnectionSetup
          process_connsetup(&dl_ccch_msg.msg.c1().rrc_conn_setup(), &mac, &phy);
          connsetup_decoded = true; 
          break;
        case dl_ccch_msg_type_c::c1_c_::types::rrc_conn_reject:
        case dl_ccch_msg_type_c::c1_c_::types::rrc_conn_reest:
        case dl_ccch_msg_type_c::c1_c_::types::rrc_conn_reest_reject:
          break;
      } 
    } else if (lcid == 1) {
      printf("Received on DCCH0 %d bytes\n", nof_bytes);
      if (send_ack == 0) {
        send_ack = 1;         
      }
    }
  }
  
  void     write_pdu_bcch_bch(uint8_t *payload, uint32_t nof_bytes) 
  {
    mib_s mib;
    srslte_vec_fprint_byte(stdout, payload, nof_bytes);

    asn1::bit_ref bref(payload, nof_bytes);
    mib.unpack(bref);

    printf("MIB received %d bytes, BW=%s MHz\n", nof_bytes, mib.dl_bw.to_string().c_str());
    mib_decoded = true; 
  }

  void write_pdu_bcch_dlsch(uint8_t* payload, uint32_t nof_bytes)
  {
    bcch_dl_sch_msg_s dlsch_msg;

    asn1::bit_ref bref(payload, nof_bytes);
    dlsch_msg.unpack(bref);

    if (dlsch_msg.msg.c1().type().value == bcch_dl_sch_msg_type_c::c1_c_::types::sib_type1) {
      si_window_len = dlsch_msg.msg.c1().sib_type1().si_win_len.to_number();
      sib2_period   = dlsch_msg.msg.c1().sib_type1().sched_info_list[0].si_periodicity.to_number();
      printf("SIB1 received %d bytes, CellID=%d, si_window=%d, sib2_period=%d\n", nof_bytes,
             (uint32_t)dlsch_msg.msg.c1().sib_type1().cell_access_related_info.cell_id.to_number() & 0xfff,
             si_window_len, sib2_period);
      mac.clear_rntis();
    } else {
      sys_info_r8_ies_s::sib_type_and_info_l_& sib_list =
          dlsch_msg.msg.c1().sys_info().crit_exts.sys_info_r8().sib_type_and_info;

      if (sib_list.size() > 0 and sib_list[0].type() == sib_info_item_c::types::sib2) {
        printf("SIB2 received %d bytes\n", nof_bytes);
        setup_mac_phy_sib2(&sib_list[0].sib2(), &mac, &phy);
        sib2_decoded = true;
        mac.clear_rntis();
      }
    }
  }

  void write_pdu_pcch(uint8_t *payload, uint32_t nof_bytes) {}
  void write_pdu_mch(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes) {}
  
private:
  LIBLTE_BIT_MSG_STRUCT  bit_msg; 
  LIBLTE_BYTE_MSG_STRUCT byte_msg;
};


int main(int argc, char *argv[])
{
  srslte::log_filter mac_log("MAC");
  rlctest my_rlc;  
  parse_args(&prog_args, argc, argv);
  
  // Capture SIGINT to write traces
  if (prog_args.do_trace) {
    signal(SIGINT, sig_int_handler);
    //radio.start_trace();
    phy.start_trace();
  }
  
  if (prog_args.do_pcap) {
    if (!prog_args.do_trace) {
      signal(SIGINT, sig_int_handler);
    }
    mac_pcap.open("/tmp/ue_mac.pcap");
    mac.start_pcap(&mac_pcap);
  }
  
  // Init Radio and PHY
  if (!radio.init()) {
    exit(1);
  }

  std::vector<srslte::log_filter*> phy_log;

  srslte::log_filter *mylog = new srslte::log_filter("PHY");
  char tmp[16];
  sprintf(tmp, "PHY%d",0);
  phy_log.push_back(mylog);

  switch (prog_args.verbose) {
    case 1:
      mac_log.set_level(srslte::LOG_LEVEL_INFO);
      mylog->set_level(srslte::LOG_LEVEL_INFO);
      break;
    case 2:
      mac_log.set_level(srslte::LOG_LEVEL_DEBUG);
      mylog->set_level(srslte::LOG_LEVEL_DEBUG);
      break;
  }

  phy.init(&radio, &mac, NULL, phy_log);
  if (prog_args.rf_rx_gain > 0 && prog_args.rf_tx_gain > 0) {
    radio.set_rx_gain(prog_args.rf_rx_gain);
    radio.set_tx_gain(prog_args.rf_tx_gain);
  } else {
    radio.start_agc(false);
    radio.set_tx_rx_gain_offset(10);
    phy.set_agc_enable(true);
  }  
  // Init MAC 
  mac.init(&phy, &my_rlc, NULL, &mac_log);
    
  // Set RX freq
  radio.set_rx_freq(prog_args.rf_rx_freq);
  radio.set_tx_freq(prog_args.rf_tx_freq);
  
  
  while(1) {
    uint32_t tti; 
    if (my_rlc.mib_decoded && mac.get_current_tti()) {
      if (!my_rlc.sib1_decoded) {
        usleep(10000);
        tti = mac.get_current_tti();           
        mac.bcch_start_rx(sib_start_tti(tti, 2, 5), 1);        
      } else if (!my_rlc.sib2_decoded) {        
        usleep(10000);
        tti = mac.get_current_tti(); 
        mac.bcch_start_rx(sib_start_tti(tti, my_rlc.sib2_period, 0), my_rlc.si_window_len);                     
      }
    }
    usleep(50000);
  }
}

  

