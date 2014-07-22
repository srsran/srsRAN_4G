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

#include "liblte/phy/phy.h"
#include "iodev.h"

#ifndef DISABLE_GRAPHICS
void init_plots();
void do_plots(ue_dl_t *q, uint32_t sf_idx);
#endif

int go_exit = 0;

/* Local function definitions */
void init_plots();

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  uint32_t cell_id_file;
  uint32_t nof_prb_file; 
  uint16_t rnti; 
  int nof_subframes;
  bool disable_plots;
  iodev_cfg_t io_config; 
}prog_args_t;

void args_default(prog_args_t *args) {
  args->cell_id_file = 1; 
  args->nof_prb_file = 6;
  args->rnti = SIRNTI;
  args->nof_subframes = -1; 
  args->disable_plots = false; 
  args->io_config.find_threshold = -1.0; 
  args->io_config.input_file_name = NULL; 
  args->io_config.uhd_args = "";
  args->io_config.uhd_freq = -1.0;
  args->io_config.uhd_gain = 20.0; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [cargfndvtb] [-i input_file | -f rx_frequency (in Hz)]\n", prog);
  printf("\t-c cell_id if reading from file [Default %d]\n", args->cell_id_file);
  printf("\t-p nof_prb if reading from file [Default %d]\n", args->nof_prb_file);
  printf("\t-r RNTI to look for [Default 0x%x]\n", args->rnti);
#ifndef DISABLE_UHD
  printf("\t-a UHD args [Default %s]\n", args->io_config.uhd_args);
  printf("\t-g UHD RX gain [Default %.2f dB]\n", args->io_config.uhd_gain);
#else
  printf("\t   UHD is disabled. CUHD library not available\n");
#endif
  printf("\t-b Decode PBCH only [Default All channels]\n");
  printf("\t-n nof_subframes [Default %d]\n", args->nof_subframes);
  printf("\t-t PSS threshold [Default %f]\n", args->io_config.find_threshold);
#ifndef DISABLE_GRAPHICS
  printf("\t-d disable plots [Default enabled]\n");
#else
  printf("\t plots are disabled. Graphics library not available\n");
#endif
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "icagfndvtbpr")) != -1) {
    switch (opt) {
    case 'i':
      args->io_config.input_file_name = argv[optind];
      break;
    case 'c':
      args->cell_id_file = atoi(argv[optind]);
      break;
    case 'p':
      args->nof_prb_file = atoi(argv[optind]);
      break;
    case 'a':
      args->io_config.uhd_args = argv[optind];
      break;
    case 'g':
      args->io_config.uhd_gain = atof(argv[optind]);
      break;
    case 'f':
      args->io_config.uhd_freq = atof(argv[optind]);
      break;
   case 't':
      args->io_config.find_threshold = atof(argv[optind]);
      break;
    case 'n':
      args->nof_subframes = atoi(argv[optind]);
      break;
    case 'r':
      args->rnti= atoi(argv[optind]);
      break;
    case 'd':
      args->disable_plots = true;
      break;
    case 'v':
      verbose++;
      break;
    default:
      usage(args, argv[0]);
      exit(-1);
    }
  }
  if (args->io_config.uhd_freq < 0 && args->io_config.input_file_name == NULL) {
    usage(args, argv[0]);
  }
}
/**********************************************************************/

void sigintHandler(int x) {
  go_exit = 1; 
}

/* TODO: Do something with the output data */
char data[10000];

int main(int argc, char **argv) {
  int ret; 
  cf_t *sf_buffer; 
  iodev_t iodev; 
  prog_args_t prog_args; 
  lte_cell_t cell; 
  ue_dl_t ue_dl; 
  bool ue_dl_initiated = false; 
  int64_t sf_cnt;
  uint32_t sf_idx;
  pbch_mib_t mib; 
  bool printed_sib = false; 
  int rlen; 
  int symbol_sz; 
  
  parse_args(&prog_args, argc, argv);
  
  symbol_sz = lte_symbol_sz(prog_args.nof_prb_file);
  if (symbol_sz > 0) {
    if (iodev_init(&iodev, &prog_args.io_config, SF_LEN(symbol_sz, CPNORM))) {
      fprintf(stderr, "Error initiating input device\n");
      exit(-1);
    }    
  } else {
    fprintf(stderr, "Invalid number of PRB %d\n", prog_args.nof_prb_file);
    exit(-1);
  }
  
#ifndef DISABLE_GRAPHICS
  if (!prog_args.disable_plots) {
    init_plots();    
  }
#endif
  
  /* Setup SIGINT handler */
  printf("\n --- Press Ctrl+C to exit --- \n");
  signal(SIGINT, sigintHandler);

  /* Initialize frame and subframe counters */
  sf_cnt = 0;
  sf_idx = 0; 

  /* Decodes the SSS signal during the tracking phase. Extra overhead, but makes sure we are in the correct subframe */  
  ue_sync_decode_sss_on_track(&iodev.sframe, true);
  
  /* Decodes the PBCH on each frame. Around 10% more overhead, but makes sure we are in the current System Frame Number (SFN) */
  ue_sync_pbch_always(&iodev.sframe, false);
  
  /* Main loop */
  while (!go_exit && (sf_cnt < prog_args.nof_subframes || prog_args.nof_subframes == -1)) {

    ret = iodev_receive(&iodev, &sf_buffer);
    if (ret < 0) {
      fprintf(stderr, "Error reading from input device (%d)\n", ret);
      break;
    }
    
    /* iodev_receive returns 1 if successfully read 1 aligned subframe */
    if (ret == 0) {
      printf("Finding PSS... Peak: %8.1f, Output level: %+.2f dB FrameCnt: %d, State: %d\r", 
             sync_get_peak_value(&iodev.sframe.s), 20*log10f(agc_get_output_level(&iodev.sframe.agc)), 
             iodev.sframe.frame_total_cnt, iodev.sframe.state);      
    } else if (ret == 1) {
      if (!ue_dl_initiated) {
        if (iodev_isUSRP(&iodev)) {
          cell = ue_sync_get_cell(&iodev.sframe);
          mib = ue_sync_get_mib(&iodev.sframe);
        } else {
          cell.id = prog_args.cell_id_file;
          cell.cp = CPNORM; 
          cell.nof_ports = 1; // TODO: Use prog_args 
          cell.nof_prb = prog_args.nof_prb_file; 
          mib.phich_resources = R_1; 
          mib.phich_length = PHICH_NORM;
        }        
        if (ue_dl_init(&ue_dl, cell, mib.phich_resources, mib.phich_length, 1234)) { 
          fprintf(stderr, "Error initiating UE downlink processing module\n");
          exit(-1);
        }
        pdsch_set_rnti(&ue_dl.pdsch, prog_args.rnti);
        ue_dl_initiated = true; 
      } else {
        if (iodev_isUSRP(&iodev)) {
          sf_idx = ue_sync_get_sfidx(&iodev.sframe);
        } 
        rlen = ue_dl_receive(&ue_dl, sf_buffer, data, sf_idx, ue_sync_get_mib(&iodev.sframe).sfn, prog_args.rnti);
        if (rlen < 0) {
          fprintf(stderr, "\nError running receiver\n");fflush(stdout);
          exit(-1);
        }
        if (prog_args.rnti == SIRNTI && !printed_sib && rlen > 0) {
          printf("\n\nDecoded SIB1 Message: ");
          vec_fprint_hex(stdout, data, rlen);
          printf("\n");fflush(stdout);
          printed_sib = true; 
        }
        if (!(sf_cnt % 10)) {       
          printf("RSSI: %+.2f dBm, CFO: %+.4f KHz, SFO: %+.4f Khz, NOI: %.2f Errors: %4d/%4d, BLER: %.1e\r",
                 20*log10f(agc_get_rssi(&iodev.sframe.agc))+30, 
                 ue_sync_get_cfo(&iodev.sframe)/1000, ue_sync_get_sfo(&iodev.sframe)/1000, 
                 pdsch_average_noi(&ue_dl.pdsch),
                 (int) ue_dl.pkt_errors, (int) ue_dl.pkts_total, (float) ue_dl.pkt_errors / ue_dl.pkts_total);
          
          fflush(stdout);       
          if (VERBOSE_ISINFO()) {
            printf("\n");
          }
        }  
        #ifndef DISABLE_GRAPHICS
        if (!prog_args.disable_plots && sf_idx == 5) {
          do_plots(&ue_dl, sf_idx);          
        }
        #endif
      }
      if (iodev_isfile(&iodev)) {
        sf_idx++;       
        if (sf_idx == NSUBFRAMES_X_FRAME) {
          sf_idx = 0;
        }        
      }
    }

    if (prog_args.nof_subframes > 0) {
      sf_cnt++;      
    }    
    if (iodev_isfile(&iodev)) {
      usleep(5000);
    }
  }

  if (ue_dl_initiated) {
    ue_dl_free(&ue_dl);    
  }
  iodev_free(&iodev);

  printf("\nBye\n");
  exit(0);
}




/**********************************************************************
 *  Plotting Functions
 ***********************************************************************/
#ifndef DISABLE_GRAPHICS


#include "liblte/graphics/plot.h"
plot_real_t poutfft;
plot_complex_t pce;
plot_scatter_t pscatrecv, pscatequal;

float tmp_plot[SLOT_LEN_RE(MAX_PRB, CPNORM)];

void init_plots() {
  plot_init();
  plot_real_init(&poutfft);
  plot_real_setTitle(&poutfft, "Output FFT - Magnitude");
  plot_real_setLabels(&poutfft, "Index", "dB");
  plot_real_setYAxisScale(&poutfft, -30, 20);

  plot_complex_init(&pce);
  plot_complex_setTitle(&pce, "Channel Estimates");
  plot_complex_setYAxisScale(&pce, Ip, -3, 3);
  plot_complex_setYAxisScale(&pce, Q, -3, 3);
  plot_complex_setYAxisScale(&pce, Magnitude, 0, 4);
  plot_complex_setYAxisScale(&pce, Phase, -M_PI, M_PI);

  plot_scatter_init(&pscatrecv);
  plot_scatter_setTitle(&pscatrecv, "Received Symbols");
  plot_scatter_setXAxisScale(&pscatrecv, -4, 4);
  plot_scatter_setYAxisScale(&pscatrecv, -4, 4);

  plot_scatter_init(&pscatequal);
  plot_scatter_setTitle(&pscatequal, "Equalized Symbols");
  plot_scatter_setXAxisScale(&pscatequal, -2, 2);
  plot_scatter_setYAxisScale(&pscatequal, -2, 2);
}

void do_plots(ue_dl_t *q, uint32_t sf_idx) {
  int i;
  uint32_t nof_re = SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp);
  uint32_t nof_symbols = q->harq_process[0].prb_alloc.re_sf[sf_idx];
  for (i = 0; i < nof_re; i++) {
    tmp_plot[i] = 10 * log10f(cabsf(q->sf_symbols[i]));
    if (isinf(tmp_plot[i])) {
      tmp_plot[i] = -80;
    }
  }
  plot_real_setNewData(&poutfft, tmp_plot, nof_re);        
  plot_complex_setNewData(&pce, q->ce[0], nof_re);
  plot_scatter_setNewData(&pscatrecv, q->pdsch.pdsch_symbols[0], nof_symbols);
  plot_scatter_setNewData(&pscatequal, q->pdsch.pdsch_d, nof_symbols);
}

#endif
