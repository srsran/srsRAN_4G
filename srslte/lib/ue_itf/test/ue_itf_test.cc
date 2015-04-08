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

#include <unistd.h>

#include "srslte/utils/debug.h"
#include "srslte/ue_itf/phy.h"

void tti_callback();
void status_change();





/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  float uhd_freq; 
  float uhd_gain;
}prog_args_t;

void args_default(prog_args_t *args) {
  args->uhd_freq = -1.0;
  args->uhd_gain = 60.0; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [gv] -f rx_frequency (in Hz)\n", prog);
  printf("\t-g UHD RX gain [Default %.2f dB]\n", args->uhd_gain);
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
      srslte_verbose++;
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







srslte::ue::phy phy;

uint32_t total_pkts=0;
uint32_t total_dci=0;
uint32_t total_errors=0;
uint8_t payload[1024]; 

// This is the MAC implementation
void tti_callback(uint32_t tti) {
  srslte::ue::sched_grant grant; 
  INFO("called tti: %d\n", tti);
  
  // SIB1 is scheduled in subframe #5 of even frames
  if ((phy.tti_to_SFN(tti)%2) == 0 && phy.tti_to_subf(tti) == 5) {
    
    // Get buffer 
    srslte::ue::dl_buffer *buffer = phy.get_dl_buffer(tti); 
    
    // Get DL grant
    if (buffer->get_dl_grant(srslte::ue::dl_buffer::PDCCH_DL_SEARCH_SIRNTI, SRSLTE_SIRNTI, &grant)) 
    {
      // MAC sets RV 
      grant.set_rv(((uint32_t) ceilf((float)3*((phy.tti_to_SFN(tti)/2)%4)/2))%4);
      
      // Decode packet
      if (buffer->decode_pdsch(grant, payload)) {
        printf("Decoded SIB1 ok TBS: %d\n", grant.get_tbs());
        srslte_vec_fprint_hex(stdout, payload, grant.get_tbs());
        exit(0);
      } else {
        total_errors++; 
      }
      exit(0);
      total_dci++; 
    }       
    total_pkts++; 
    if (total_pkts==4) {
      exit(-1);
    }
  }
  printf("PDCCH BLER %.3f \%% PDSCH BLER %.3f \%% (total pkts: %d) \r", 
         1-(float) total_dci/total_pkts, 
         (float) total_errors/total_pkts, 
         total_pkts);
}

void status_change() {
  printf("called status change\n");
}

int main(int argc, char *argv[])
{
  srslte_cell_t cell; 
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];
  prog_args_t prog_args; 
  
  parse_args(&prog_args, argc, argv);

  // Init PHY 
  phy.init(tti_callback, status_change);
  
  // Give it time to create thread 
  sleep(1);
  
  // Set RX freq and gain
  phy.set_rx_freq(prog_args.uhd_freq);
  phy.set_rx_gain(prog_args.uhd_gain);
  
  /* Instruct the PHY to decode BCH */
  if (!phy.decode_mib_best(&cell, bch_payload)) {
    exit(-1);
  }
  // Print MIB 
  srslte_pbch_mib_fprint(stdout, &cell, phy.get_current_tti()/10);
  
  // Set the current PHY cell to the detected cell
  if (!phy.set_cell(cell)) {
    printf("Error setting cell\n");
    exit(-1);
  }
  
  /* Instruct the PHY to start RX streaming and synchronize */
  if (!phy.start_rxtx()) {
    printf("Could not start RX\n");
    exit(-1);
  }
  /* go to idle and process each tti */
  while(1) {
    sleep(1);
  }
}