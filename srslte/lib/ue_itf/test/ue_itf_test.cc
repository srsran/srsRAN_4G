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

srslte::ue::phy phy;

uint8_t payload[1024]; 

void tti_callback(uint32_t tti) {
  printf("called tti: %d\n", tti);
  
  // This is the MAC implementation
  if ((phy.tti_to_SFN(tti)%2) == 0 && phy.tti_to_subf(tti) == 5) {
    srslte::ue::sched_grant grant; 
    srslte_verbose = SRSLTE_VERBOSE_DEBUG; 
    if (phy.get_dl_buffer(tti)->get_dl_grant(srslte::ue::dl_buffer::PDCCH_DL_SEARCH_SIRNTI, SRSLTE_SIRNTI, &grant)) {
      if (phy.get_dl_buffer(tti)->decode_pdsch(grant, payload)) {
        printf("Decoded SIB1 ok TBS: %d\n", grant.get_tbs());
        srslte_vec_fprint_hex(stdout, payload, grant.get_tbs());
      } else {
        fprintf(stderr, "Could not decode SIB\n");
      }    
    } else {
      fprintf(stderr, "Error getting DL grant\n");
    }    
    exit(0);
  }
}

void status_change() {
  printf("called status change\n");
}

int main(int argc, char *argv[])
{
  srslte_cell_t cell; 
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];
  phy.init(tti_callback, status_change);
  sleep(1);
  
  phy.set_rx_freq(1825000000);
  phy.set_rx_gain(60.0);
  
  /* Instruct the PHY to decode BCH */
  if (!phy.decode_mib_best(&cell, bch_payload)) {
    exit(-1);
  }
  srslte_pbch_mib_fprint(stdout, &cell, phy.get_current_tti()/10);
  srslte_vec_fprint_hex(stdout, bch_payload, SRSLTE_BCH_PAYLOAD_LEN);
  
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