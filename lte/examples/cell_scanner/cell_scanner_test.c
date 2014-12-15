/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#include "liblte/rrc/rrc.h"
#include "liblte/phy/phy.h"
#include "liblte/cuhd/cuhd.h"
#include "cell_scanner.h"


/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  int nof_subframes;
  int force_N_id_2;
  char *uhd_args; 
  float uhd_freq; 
}prog_args_t;

void args_default(prog_args_t *args) {
  args->nof_subframes = -1; 
  args->force_N_id_2 = -1; // Pick the best
  args->uhd_args = "";
  args->uhd_freq = -1.0;
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [alnv] -f rx_frequency (in Hz)\n", prog);
  printf("\t-a UHD args [Default %s]\n", args->uhd_args);
  printf("\t-l Force N_id_2 [Default best]\n");
  printf("\t-n nof_subframes [Default %d]\n", args->nof_subframes);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "alnvf")) != -1) {
    switch (opt) {
    case 'a':
      args->uhd_args = argv[optind];
      break;
    case 'f':
      args->uhd_freq = atof(argv[optind]);
      break;
    case 'n':
      args->nof_subframes = atoi(argv[optind]);
      break;
    case 'l':
      args->force_N_id_2 = atoi(argv[optind]);
      break;
    case 'v':
      verbose++;
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
/**********************************************************************/

int main(int argc, char **argv) {
  prog_args_t prog_args; 
  cell_scanner_result_t result; 
  cell_scanner_config_t cfg; 
  cell_scanner_t cs; 
    
  parse_args(&prog_args, argc, argv);

  // Peak-to-sidelobe ratio (PSR) threshold for cell detection early stopping. 
  cfg.cell_detect_early_stop_threshold = 20.0; // This is a normal value. 
  // maximum 5 ms frames that will be scanned maximum in the case the threshold is not exceed
  // ie for bad cells 
  cfg.pss_max_frames = 50; // this is 250 ms

  // maximum 5 ms frames that will be received to decode the PBCH 
  // ie for bad cells 
  cfg.pbch_max_frames = 500; // this is 2500 ms
  
  // Number of 1 ms subframes that will be used to compute rsrp, rsrq, snr, etc average
  cfg.measure_avg_nof_frames = 1000; // 1 sec 
  
  cfg.uhd_args = prog_args.uhd_args;
  
  // Init USRP 
  if (cell_scanner_init(&cs, &cfg)) {
    fprintf(stderr, "Error initiating cell scanner\n");
    exit(-1);
  }
  
  // Scan for a frequency 
  if (prog_args.force_N_id_2 < 0) {
    // We have 2 options, either we scan for the three possible N_id_2 root sequences...
    cell_scanner_all_cells(&cs, prog_args.uhd_freq, &result);    
  } else {
    // or we scan for a single one. 
    cell_scanner_cell(&cs, prog_args.uhd_freq, prog_args.force_N_id_2, &result);        
  }
  
  printf("\nResult: CellID: %d, PHYID: %d, RSRP: %.1f dBm, RSRQ: %.1f dB, SNR: %.1f dB\n", 
    result.cell_id, result.phy_cell.id, 
    10*log10(result.rsrp*1000), 10*log10(result.rsrq), 10*log10(result.snr)); 
}
