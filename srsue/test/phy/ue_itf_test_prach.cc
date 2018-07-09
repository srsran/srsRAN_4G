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

#include "srslte/phy/utils/debug.h"
#include "srsue/hdr/phy/phy.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/log_filter.h"
#include "srslte/radio/radio_multi.h"

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  float rf_rx_freq;
  float rf_tx_freq; 
  float rf_rx_gain;
  float rf_tx_gain;
  bool  continous; 
}prog_args_t;

prog_args_t prog_args; 
uint32_t srsapps_verbose = 0; 

void args_default(prog_args_t *args) {
  args->rf_rx_freq = -1.0;
  args->rf_tx_freq = -1.0;
  args->rf_rx_gain = -1; // set to autogain
  args->rf_tx_gain = -1; 
  args->continous = false; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [gGcv] -f rx_frequency -F tx_frequency (in Hz)\n", prog);
  printf("\t-g RF RX gain [Default AGC]\n");
  printf("\t-G RF TX gain [Default same as RX gain (AGC)]\n");
  printf("\t-c Run continuously [Default only once]\n");
  printf("\t-v [increase verbosity, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "gGfFcv")) != -1) {
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
    case 'c':
      args->continous = true; 
      break;
    case 'v':
      srsapps_verbose++;
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



typedef enum{
    rar_header_type_bi = 0,
    rar_header_type_rapid,
    rar_header_type_n_items,
}rar_header_t;
static const char rar_header_text[rar_header_type_n_items][8] = {"BI", "RAPID"};

typedef struct {
  rar_header_t      hdr_type;
  bool              hopping_flag;
  uint32_t          tpc_command;
  bool              ul_delay;
  bool              csi_req;
  uint16_t          rba; 
  uint16_t          timing_adv_cmd;
  uint16_t          temp_c_rnti;
  uint8_t           mcs; 
  uint8_t           RAPID;
  uint8_t           BI;
}rar_msg_t; 


int rar_unpack(uint8_t *buffer, rar_msg_t *msg)
{
    int ret = SRSLTE_ERROR;
    uint8_t *ptr = buffer; 
    
    if(buffer != NULL &&
          msg != NULL)
    {
      ptr++;
      msg->hdr_type = (rar_header_t) *ptr++;
      if(msg->hdr_type == rar_header_type_bi) {
        ptr += 2; 
        msg->BI = srslte_bit_pack(&ptr, 4);
        ret = SRSLTE_SUCCESS; 
      } else if (msg->hdr_type == rar_header_type_rapid) {
        msg->RAPID = srslte_bit_pack(&ptr, 6);
        ptr++;
        
        msg->timing_adv_cmd = srslte_bit_pack(&ptr, 11);
        msg->hopping_flag   = *ptr++;
        msg->rba            = srslte_bit_pack(&ptr, 10); 
        msg->mcs            = srslte_bit_pack(&ptr, 4);
        msg->tpc_command    = srslte_bit_pack(&ptr, 3);
        msg->ul_delay       = *ptr++;
        msg->csi_req        = *ptr++;
        msg->temp_c_rnti    = srslte_bit_pack(&ptr, 16);
        ret = SRSLTE_SUCCESS;
      } 
    }

    return(ret);
}



srsue::phy my_phy;
bool bch_decoded = false; 

uint8_t payload[SRSLTE_MAX_TB][10240];
uint8_t payload_bits[SRSLTE_MAX_TB][10240];
const uint8_t conn_request_msg[] = {0x20, 0x06, 0x1F, 0x5C, 0x2C, 0x04, 0xB2, 0xAC, 0xF6, 0x00, 0x00, 0x00};

enum mac_state {RA, RAR, CONNREQUEST, CONNSETUP} state = RA; 

uint32_t preamble_idx = 0; 
rar_msg_t rar_msg;

uint32_t nof_rtx_connsetup = 0; 
uint32_t rv_value[4] = {0, 2, 3, 1};

void config_phy() {
  srsue::phy_interface_rrc::phy_cfg_t config; 
  
  config.common.prach_cnfg.prach_cnfg_info.prach_config_index = 0; 
  config.common.prach_cnfg.prach_cnfg_info.prach_freq_offset  = 0; 
  config.common.prach_cnfg.prach_cnfg_info.high_speed_flag    = false;
  config.common.prach_cnfg.root_sequence_index                = 0; 
  config.common.prach_cnfg.prach_cnfg_info.zero_correlation_zone_config = 11;
  
  config.common.pusch_cnfg.ul_rs.group_hopping_enabled    = false; 
  config.common.pusch_cnfg.ul_rs.sequence_hopping_enabled = false; 
  config.common.pusch_cnfg.n_sb = 2;
  config.common.pusch_cnfg.ul_rs.cyclic_shift = 0;
  config.common.pusch_cnfg.ul_rs.group_assignment_pusch = 0; 
  config.common.pusch_cnfg.pusch_hopping_offset = 0; 
  
  config.common.pucch_cnfg.delta_pucch_shift = LIBLTE_RRC_DELTA_PUCCH_SHIFT_DS2; 
  config.common.pucch_cnfg.n_cs_an = 0;
  config.common.pucch_cnfg.n1_pucch_an = 1;
  
  my_phy.configure_ul_params();
  my_phy.configure_prach_params();
}

srslte_softbuffer_rx_t softbuffers_rx[SRSLTE_MAX_TB];
srslte_softbuffer_tx_t softbuffers_tx[SRSLTE_MAX_TB];

uint16_t temp_c_rnti; 


class rrc_dummy : public srsue::rrc_interface_phy
{
public:
  void in_sync() {};
  void out_of_sync() {};
};

/******** MAC Interface implementation */
class testmac : public srsue::mac_interface_phy
{
public:
  
  testmac() { 
    rar_rnti_set = false; 
  }
  
  bool rar_rnti_set;

  void pch_decoded_ok(uint32_t len) {}

  
  void tti_clock(uint32_t tti) {
    if (!rar_rnti_set) {
      int prach_tti = my_phy.prach_tx_tti();
      if (prach_tti > 0) {
        my_phy.pdcch_dl_search(SRSLTE_RNTI_RAR, 1+prach_tti%10, prach_tti+3, prach_tti+13);
        rar_rnti_set = true; 
      }
    }
  }
  
  void new_grant_ul(mac_grant_t grant, tb_action_ul_t *action) {
    printf("New grant UL\n");
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb ++) {
      memcpy(payload[tb], conn_request_msg, grant.n_bytes[tb]*sizeof(uint8_t));
      action->rv[tb] = rv_value[nof_rtx_connsetup%4];
      action->payload_ptr[tb] = payload[tb];
      if (action->rv[tb] == 0) {
        srslte_softbuffer_tx_reset(&softbuffers_tx[tb]);
      }
    }
    action->current_tx_nb = nof_rtx_connsetup;
    action->softbuffers = softbuffers_tx;
    action->rnti = temp_c_rnti; 
    action->expect_ack = (nof_rtx_connsetup < 5)?true:false;
    memcpy(&action->phy_grant, &grant.phy_grant, sizeof(srslte_phy_grant_t));
    memcpy(&last_grant, &grant, sizeof(mac_grant_t));
    action->tx_enabled = true; 
    my_phy.pdcch_dl_search(SRSLTE_RNTI_USER, temp_c_rnti);
  }
  
  void new_grant_ul_ack(mac_grant_t grant, bool ack, tb_action_ul_t *action) {
    printf("New grant UL ACK\n");    
  }

  void harq_recv(uint32_t tti, bool ack, tb_action_ul_t *action) {
    printf("harq recv hi=%d\n", ack?1:0);    
    if (!ack) {
      nof_rtx_connsetup++;
      action->current_tx_nb = nof_rtx_connsetup;
      action->softbuffers = softbuffers_tx;
      action->rnti = temp_c_rnti; 
      action->expect_ack = true; 
      memcpy(&action->phy_grant, &last_grant.phy_grant, sizeof(srslte_phy_grant_t));
      action->tx_enabled = true;
      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb ++) {
        action->rv[tb] = rv_value[nof_rtx_connsetup%4];
        if (action->rv[tb] == 0) {
          srslte_softbuffer_tx_reset(&softbuffers_tx[tb]);
        }
        printf("Retransmission %d (TB %d), rv=%d\n", nof_rtx_connsetup, tb, action->rv[tb]);

      }
    }
  }

  void new_grant_dl(mac_grant_t grant, tb_action_dl_t *action) {
    action->decode_enabled[0] = true;
    action->default_ack[0] = false;
    if (grant.rnti == 2) {
      action->generate_ack = false; 
    } else {
      action->generate_ack = true; 
    }
    action->rnti = grant.rnti;
    memcpy(&action->phy_grant, &grant.phy_grant, sizeof(srslte_phy_grant_t));
    memcpy(&last_grant, &grant, sizeof(mac_grant_t));
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb ++) {
      action->softbuffers[tb] = &softbuffers_rx[tb];
      action->rv[tb] = grant.rv[tb];
      action->payload_ptr[tb] = payload[tb];
      if (action->rv[tb] == 0) {
        srslte_softbuffer_rx_reset(&softbuffers_rx[tb]);
      }
    }
  }
  
  void tb_decoded(bool ack, uint32_t tb_idx, srslte_rnti_type_t rnti_type, uint32_t harq_pid) {
    if (ack) {
      if (rnti_type == SRSLTE_RNTI_RAR) {
        my_phy.pdcch_dl_search_reset();
        srslte_bit_unpack_vector(payload[tb_idx], payload_bits[tb_idx], last_grant.n_bytes[tb_idx]*8);
        rar_unpack(payload_bits[tb_idx], &rar_msg);
        if (rar_msg.RAPID == preamble_idx) {

          printf("Received RAR at TTI: %d\n", last_grant.tti);
          my_phy.set_timeadv_rar(rar_msg.timing_adv_cmd);
          
          temp_c_rnti = rar_msg.temp_c_rnti; 
          
          if (last_grant.n_bytes[0]*8 > 20 + SRSLTE_RAR_GRANT_LEN) {
            uint8_t rar_grant[SRSLTE_RAR_GRANT_LEN];
            memcpy(rar_grant, &payload_bits[20], sizeof(uint8_t)*SRSLTE_RAR_GRANT_LEN);
            my_phy.set_rar_grant(last_grant.tti, rar_grant);          
          }
        } else {
          printf("Received RAR RAPID=%d\n", rar_msg.RAPID);        
        }
      } else {
        printf("Received Connection Setup\n");
        my_phy.pdcch_dl_search_reset();
      }
    }
  }

  void bch_decoded_ok(uint8_t *payload, uint32_t len) {
    printf("BCH decoded\n");
    bch_decoded = true; 
    srslte_cell_t cell; 
    my_phy.get_current_cell(&cell);
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
      srslte_softbuffer_rx_init(&softbuffers_rx[tb], cell.nof_prb);
      srslte_softbuffer_tx_init(&softbuffers_tx[tb], cell.nof_prb);
    }
  }
   
private: 
  mac_grant_t last_grant; 
};


testmac               my_mac;
srslte::radio_multi   radio;
rrc_dummy             rrc_dummy;
  
int main(int argc, char *argv[])
{
  srslte::log_filter log("PHY");
  
  parse_args(&prog_args, argc, argv);

  // Init Radio and PHY
  radio.init();
  my_phy.init(&radio, &my_mac, &rrc_dummy, &log);
  if (prog_args.rf_rx_gain > 0 && prog_args.rf_tx_gain > 0) {
    radio.set_rx_gain(prog_args.rf_rx_gain);
    radio.set_tx_gain(prog_args.rf_tx_gain);
  } else {
    radio.start_agc(false);
    radio.set_tx_rx_gain_offset(10);
    my_phy.set_agc_enable(true);
  }
  
  if (srsapps_verbose == 1) {
    log.set_level(srslte::LOG_LEVEL_INFO);
    printf("Log level info\n");
  }
  if (srsapps_verbose == 2) {
    log.set_level(srslte::LOG_LEVEL_DEBUG);
    printf("Log level debug\n");
  }

  // Give it time to create thread 
  sleep(1);
  
  // Set RX freq
  radio.set_rx_freq(prog_args.rf_rx_freq);
  radio.set_tx_freq(prog_args.rf_tx_freq);
  
  // Instruct the PHY to configure PRACH parameters and sync to current cell 
  while(!my_phy.cell_is_camping()) {
    usleep(20000);
  }

  // Setup PHY parameters
  config_phy();
    
  /* Instruct PHY to send PRACH and prepare it for receiving RAR */
  my_phy.prach_send(preamble_idx);
  
  /* go to idle and process each tti */
  bool running = true; 
  while(running) {
    sleep(1);
  }
  my_phy.stop();
  radio.stop_rx();
}



