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

#include <unistd.h>

#include "srslte/utils/debug.h"
#include "srsapps/ue/phy/phy.h"
#include "srsapps/common/log_stdout.h"
#include "srsapps/common/mac_interface.h"
#include "srsapps/radio/radio_uhd.h"


/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/

typedef struct {
  float uhd_freq; 
  float uhd_gain;
}prog_args_t;

uint32_t srsapps_verbose = 0; 

prog_args_t prog_args; 

void args_default(prog_args_t *args) {
  args->uhd_freq = -1.0;
  args->uhd_gain = -1.0; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [gv] -f rx_frequency (in Hz)\n", prog);
  printf("\t-g UHD RX gain [Default AGC]\n");
  printf("\t-v [increase verbosity, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "gfv")) != -1) {
    switch (opt) {
    case 'g':
      args->uhd_gain = atof(argv[optind]);
      break;
    case 'f':
      args->uhd_freq = atof(argv[optind]);
      break;
    case 'v':
      srsapps_verbose++;
      break;
    default:
      usage(args, argv[0]);
      exit(-1);
    }
  }
  if (args->uhd_freq < 0) {
    usage(args, argv[0]);
    exit(-1);
  }
}

srslte::ue::phy my_phy;
bool bch_decoded = false; 
uint32_t total_pkts=0;
uint32_t total_dci=0;
uint32_t total_oks=0;
uint8_t payload[1024]; 
srslte_softbuffer_rx_t softbuffer; 

/******** MAC Interface implementation */
class testmac : public srslte::ue::mac_interface_phy
{
public:
  void new_grant_ul(mac_grant_t grant, uint8_t *payload_ptr, tb_action_ul_t *action) {
    printf("New grant UL\n");
  }
  void new_grant_ul_ack(mac_grant_t grant, uint8_t *payload_ptr, bool ack, tb_action_ul_t *action) {
    printf("New grant UL ACK\n");    
  }

  void harq_recv(uint32_t tti, bool ack, tb_action_ul_t *action) {
    printf("harq recv\n");    
  }

  void new_grant_dl(mac_grant_t grant, tb_action_dl_t *action) {
    total_dci++; 
    

    action->decode_enabled = true; 
    action->default_ack = false; 
    action->generate_ack = false; 
    action->payload_ptr = payload; 
    memcpy(&action->phy_grant, &grant.phy_grant, sizeof(srslte_phy_grant_t));
    action->rv = ((uint32_t) ceilf((float)3*((my_phy.tti_to_SFN(grant.tti)/2)%4)/2))%4;
    action->softbuffer = &softbuffer;
    action->rnti = grant.rnti; 
    if (action->rv == 0) {
      srslte_softbuffer_rx_reset(&softbuffer);
    }
  }
  
  void tb_decoded(bool ack, srslte_rnti_type_t rnti, uint32_t harq_pid) {
    if (ack) {
      total_oks++;     
    }
  }

  void bch_decoded_ok(uint8_t *payload, uint32_t len) {
    printf("BCH decoded\n");
    bch_decoded = true; 
    srslte_cell_t cell; 
    my_phy.get_current_cell(&cell); 
    srslte_softbuffer_rx_init(&softbuffer, cell.nof_prb);
  }
  void tti_clock(uint32_t tti) {
    
  }
};


testmac         my_mac;
srslte::radio_uhd radio_uhd; 




int main(int argc, char *argv[])
{
  srslte::log_stdout log("PHY");
  
  parse_args(&prog_args, argc, argv);

  // Init Radio and PHY
  if (prog_args.uhd_gain > 0) {
    radio_uhd.init();
    radio_uhd.set_rx_gain(prog_args.uhd_gain);    
    my_phy.init(&radio_uhd, &my_mac, &log);
  } else {
    radio_uhd.init_agc();
    my_phy.init_agc(&radio_uhd, &my_mac, &log);
  }
  
  if (srsapps_verbose == 1) {
    log.set_level_info();
    printf("Log level info\n");
  }
  if (srsapps_verbose == 2) {
    log.set_level_debug();
    printf("Log level debug\n");
  }
    
  // Give it time to create thread 
  sleep(1);
    
  // Set RX freq and gain
  radio_uhd.set_rx_freq(prog_args.uhd_freq);
  
  my_phy.sync_start();
  
  bool running = true; 
  while(running) {
    if (bch_decoded && my_phy.status_is_sync()) {
      uint32_t tti = my_phy.get_current_tti();
      
      // SIB1 is scheduled in subframe #5 of even frames, try to decode next frame SIB1
      tti = (((tti/20)*20) + 25)%10240;
      my_phy.pdcch_dl_search(SRSLTE_RNTI_SI, SRSLTE_SIRNTI, tti, tti+1);
      
      total_pkts++;       
    }
    usleep(30000);    
    if (bch_decoded && my_phy.status_is_sync() && total_pkts > 0) {
      if (srslte_verbose == SRSLTE_VERBOSE_NONE && srsapps_verbose == 0) {
        float gain = prog_args.uhd_gain; 
        if (gain < 0) {
          gain = radio_uhd.get_rx_gain();
        }
        printf("PDCCH BLER %.1f \%% PDSCH BLER %.1f \%% (total pkts: %5u) Gain: %.1f dB\r", 
            100-(float) 100*total_dci/total_pkts, 
            (float) 100*(1 - total_oks/total_pkts), 
            total_pkts, gain);   
      }
    }
  }
  my_phy.stop();
  radio_uhd.stop_rx();
}