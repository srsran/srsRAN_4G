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
#include "srslte/common/log_filter.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/radio/radio_multi.h"


/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/

typedef struct {
  float rf_freq; 
  float rf_gain;
}prog_args_t;

uint32_t srsapps_verbose = 0; 

prog_args_t prog_args; 

void args_default(prog_args_t *args) {
  args->rf_freq = -1.0;
  args->rf_gain = -1.0; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [gv] -f rx_frequency (in Hz)\n", prog);
  printf("\t-g RF RX gain [Default AGC]\n");
  printf("\t-v [increase verbosity, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "gfv")) != -1) {
    switch (opt) {
    case 'g':
      args->rf_gain = atof(argv[optind]);
      break;
    case 'f':
      args->rf_freq = atof(argv[optind]);
      break;
    case 'v':
      srsapps_verbose++;
      break;
    default:
      usage(args, argv[0]);
      exit(-1);
    }
  }
  if (args->rf_freq < 0) {
    usage(args, argv[0]);
    exit(-1);
  }
}

srsue::phy my_phy;
bool bch_decoded = false; 
uint32_t total_pkts=0;
uint32_t total_dci=0;
uint32_t total_oks=0;
uint8_t payload[SRSLTE_MAX_TB][1024];
srslte_softbuffer_rx_t softbuffers[SRSLTE_MAX_TB];

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
  void new_grant_ul(mac_grant_t grant, tb_action_ul_t *action) {
    printf("New grant UL\n");
  }
  void new_grant_ul_ack(mac_grant_t grant, bool ack, tb_action_ul_t *action) {
    printf("New grant UL ACK\n");    
  }

  void harq_recv(uint32_t tti, bool ack, tb_action_ul_t *action) {
    printf("harq recv\n");    
  }

  void new_grant_dl(mac_grant_t grant, tb_action_dl_t *action) {
    total_dci++; 
    

    action->decode_enabled[0] = true;
    action->default_ack[0] = false;
    action->generate_ack = false;
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
      action->payload_ptr[tb] = payload[tb];
      action->rv[tb] = ((uint32_t) ceilf((float) 3 * ((my_phy.tti_to_SFN(grant.tti) / 2) % 4) / 2)) % 4;
      if (action->rv == 0) {
        srslte_softbuffer_rx_reset(&softbuffers[tb]);
      }
      action->softbuffers[0] = &softbuffers[tb];
    }
    memcpy(&action->phy_grant, &grant.phy_grant, sizeof(srslte_phy_grant_t));

    action->rnti = grant.rnti;
  }
  
  
  
  void tb_decoded(bool ack, uint32_t tb_idx, srslte_rnti_type_t rnti_type, uint32_t harq_pid) {
    if (ack) {
      total_oks++;     
    }
  }
  
  void pch_decoded_ok(uint32_t len) {}

  void bch_decoded_ok(uint8_t *payload, uint32_t len) {
    printf("BCH decoded\n");
    bch_decoded = true; 
    srslte_cell_t cell; 
    my_phy.get_current_cell(&cell); 
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
      srslte_softbuffer_rx_init(&softbuffers[tb], cell.nof_prb);
    }
  }
  void tti_clock(uint32_t tti) {
    
  }
};


testmac             my_mac;
srslte::radio_multi radio; 
rrc_dummy           rrc_dummy;



int main(int argc, char *argv[])
{
  srslte::log_filter log("PHY");
  
  parse_args(&prog_args, argc, argv);

  // Init Radio and PHY
  radio.init();
  my_phy.init(&radio, &my_mac, &rrc_dummy, &log);
  if (prog_args.rf_gain > 0) {
    radio.set_rx_gain(prog_args.rf_gain);     
  } else {
    radio.start_agc(false);
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
    
  // Set RX freq and gain
  radio.set_rx_freq(prog_args.rf_freq);
  
  bool running = true;
  while(running) {
    if (bch_decoded && my_phy.cell_is_camping()) {
      uint32_t tti = my_phy.get_current_tti();
      
      // SIB1 is scheduled in subframe #5 of even frames, try to decode next frame SIB1
      tti = (((tti/20)*20) + 25)%10240;
      my_phy.pdcch_dl_search(SRSLTE_RNTI_SI, SRSLTE_SIRNTI, tti, tti+1);
      
      total_pkts++;       
    }
    usleep(30000);    
    if (bch_decoded && my_phy.cell_is_camping() && total_pkts > 0) {
      if (srslte_verbose == SRSLTE_VERBOSE_NONE && srsapps_verbose == 0) {
        float gain = prog_args.rf_gain; 
        if (gain < 0) {
          gain = radio.get_rx_gain();
        }
        printf("PDCCH BLER %.1f \%% PDSCH BLER %.1f \%% (total pkts: %5u) Gain: %.1f dB\r", 
            100-(float) 100*total_dci/total_pkts, 
            (float) 100*(1 - total_oks/total_pkts), 
            total_pkts, gain);   
      }
    }
  }
  my_phy.stop();
  radio.stop_rx();
}
