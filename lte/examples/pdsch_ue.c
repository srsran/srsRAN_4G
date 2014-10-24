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
  uint16_t rnti; 
  int nof_subframes;
  bool disable_plots;
  iodev_cfg_t io_config; 
}prog_args_t;

void args_default(prog_args_t *args) {
  args->io_config.cell_id_file = 195; 
  args->io_config.nof_prb_file = 50;
  args->io_config.nof_ports_file = 2; 
  args->rnti = SIRNTI;
  args->nof_subframes = -1; 
  args->disable_plots = false; 
  args->io_config.find_threshold = -1.0; 
  args->io_config.input_file_name = NULL; 
  args->io_config.force_N_id_2 = -1; // Pick the best
  args->io_config.uhd_args = "";
  args->io_config.uhd_freq = -1.0;
  args->io_config.uhd_gain = 60.0; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [cargndvtbl] [-i input_file | -f rx_frequency (in Hz)]\n", prog);
  printf("\t-c cell_id if reading from file [Default %d]\n", args->io_config.cell_id_file);
  printf("\t-p nof_prb if reading from file [Default %d]\n", args->io_config.nof_prb_file);
  printf("\t-o nof_ports if reading from file [Default %d]\n", args->io_config.nof_ports_file);
  printf("\t-r RNTI to look for [Default 0x%x]\n", args->rnti);
#ifndef DISABLE_UHD
  printf("\t-a UHD args [Default %s]\n", args->io_config.uhd_args);
  printf("\t-g UHD RX gain [Default %.2f dB]\n", args->io_config.uhd_gain);
#else
  printf("\t   UHD is disabled. CUHD library not available\n");
#endif
  printf("\t-l Force N_id_2 [Default best]\n");
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
  while ((opt = getopt(argc, argv, "icagfndvtbprol")) != -1) {
    switch (opt) {
    case 'i':
      args->io_config.input_file_name = argv[optind];
      break;
    case 'c':
      args->io_config.cell_id_file = atoi(argv[optind]);
      break;
    case 'p':
      args->io_config.nof_prb_file = atoi(argv[optind]);
      break;
    case 'o':
      args->io_config.nof_ports_file = atoi(argv[optind]);
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
    case 'l':
      args->io_config.force_N_id_2 = atoi(argv[optind]);
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
    exit(-1);
  }
}
/**********************************************************************/

void sigintHandler(int x) {
  go_exit = 1; 
}

/* TODO: Do something with the output data */
uint8_t data[10000];

extern float mean_exec_time; 

int main(int argc, char **argv) {
  int ret; 
  cf_t *sf_buffer; 
  iodev_t iodev; 
  prog_args_t prog_args; 
  lte_cell_t cell; 
  ue_dl_t ue_dl; 
  int64_t sf_cnt;
  bool printed_sib = false; 
  int rlen; 
  
  parse_args(&prog_args, argc, argv);
  
#ifndef DISABLE_GRAPHICS
  if (!prog_args.disable_plots) {
    init_plots();    
  }
#endif
  
  /* Initialize subframe counter */
  sf_cnt = 0;

  if (iodev_init(&iodev, &prog_args.io_config, &cell)) {
    exit(-1);
  }

  if (ue_dl_init(&ue_dl, cell, 1234)) { 
    fprintf(stderr, "Error initiating UE downlink processing module\n");
    exit(-1);
  }
  pdsch_set_rnti(&ue_dl.pdsch, prog_args.rnti);
  
  /* Main loop */
  while (!go_exit && (sf_cnt < prog_args.nof_subframes || prog_args.nof_subframes == -1)) {
    
    ret = iodev_receive(&iodev, &sf_buffer);
    if (ret < 0) {
      fprintf(stderr, "Error reading from input device (%d)\n", ret);
      break;
    }
    
    fprintf(stderr, "Change in ue_dl API\n");
    exit(-1);
    
    /* iodev_receive returns 1 if successfully read 1 aligned subframe */
    if (ret == 1) {
      rlen = ue_dl_decode(&ue_dl, sf_buffer, data, iodev_get_sfidx(&iodev), 0, prog_args.rnti);
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

      // Plot and Printf
      if (!(sf_cnt % 10)) {       
        printf("CFO: %+.4f KHz, SFO: %+.4f Khz, NOI: %.2f Errors: %4d/%4d, BLER: %.1e, Texec: %.2f\r",
                ue_sync_get_cfo(&iodev.sframe)/1000, ue_sync_get_sfo(&iodev.sframe)/1000, 
                pdsch_average_noi(&ue_dl.pdsch),
                (int) ue_dl.pkt_errors, (int) ue_dl.pkts_total, (float) ue_dl.pkt_errors / ue_dl.pkts_total, 
               mean_exec_time);                
        
      }      
      #ifndef DISABLE_GRAPHICS
      if (!prog_args.disable_plots && iodev_get_sfidx(&iodev) == 5) {
        do_plots(&ue_dl, 5);          
      }
      #endif
    } else if (ret == 0) {
      printf("Finding PSS... Peak: %8.1f, FrameCnt: %d, State: %d\r", 
        sync_get_peak_value(&iodev.sframe.sfind), 
        iodev.sframe.frame_total_cnt, iodev.sframe.state);      
    }
    sf_cnt++;                  
  } // Main loop

  ue_dl_free(&ue_dl);    
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
