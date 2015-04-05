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

#include "srslte/ue_itf/phy.h"

void tti_callback();
void status_change();

srslte::ue::phy phy = srslte::ue::phy(tti_callback, status_change);
void tti_callback() {
  printf("called tti callback\n");
}

bool status_changed; 
void status_change() {
  printf("called status change\n");
  status_changed=true; 
}

int main(int argc, char *argv[])
{
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];

  sleep(1);
  
  /* Instruct the PHY to decode BCH */
  status_changed=false; 
  phy.dl_bch();
  while(!status_changed);
  if (!phy.status_is_bch_decoded(bch_payload)) {
    printf("Could not decode BCH\n");
    exit(-1);
  }
  srslte_vec_fprint_hex(stdout, bch_payload, SRSLTE_BCH_PAYLOAD_LEN);
  
  /* Instruct the PHY to start RX streaming and synchronize */
  status_changed=false; 
  phy.start_rxtx();
  while(!status_changed);
  if (!phy.status_is_rxtx()) {
    printf("Could not start RX\n");
    exit(-1);
  }
  /* go to idle and process each tti */
  while(1) {
    sleep(1);
  }
}