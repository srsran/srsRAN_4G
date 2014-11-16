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
#include "cell_search_utils.h"

#ifndef DISABLE_GRAPHICS
void init_plots();
void do_plots(ue_dl_t *q, uint32_t sf_idx);
#endif


cell_detect_cfg_t cell_detect_config = {
  500, // nof_frames_total 
  50,  // nof_frames_detected
  CS_FIND_THRESHOLD // threshold
};

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  int nof_subframes;
  bool disable_plots;
  int force_N_id_2;
  char *uhd_args; 
  float uhd_freq; 
  float uhd_gain;
}prog_args_t;

void args_default(prog_args_t *args) {
  args->nof_subframes = -1; 
  args->force_N_id_2 = -1; // Pick the best
  args->uhd_args = "";
  args->uhd_freq = -1.0;
  args->uhd_gain = 60.0; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [agldnv] -f rx_frequency (in Hz)\n", prog);
  printf("\t-a UHD args [Default %s]\n", args->uhd_args);
  printf("\t-g UHD RX gain [Default %.2f dB]\n", args->uhd_gain);
  printf("\t-l Force N_id_2 [Default best]\n");
#ifndef DISABLE_GRAPHICS
  printf("\t-d disable plots [Default enabled]\n");
#else
  printf("\t plots are disabled. Graphics library not available\n");
#endif
  printf("\t-n nof_subframes [Default %d]\n", args->nof_subframes);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "agldnvf")) != -1) {
    switch (opt) {
    case 'a':
      args->uhd_args = argv[optind];
      break;
    case 'g':
      args->uhd_gain = atof(argv[optind]);
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
  if (args->uhd_freq < 0) {
    usage(args, argv[0]);
    exit(-1);
  }
}
/**********************************************************************/

/* TODO: Do something with the output data */
uint8_t data[10000], data_unpacked[1000];

int cuhd_recv_wrapper(void *h, void *data, uint32_t nsamples) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv(h, data, nsamples, 1);
}

extern float mean_exec_time;

int main(int argc, char **argv) {
  int ret; 
  cf_t *sf_buffer; 
  prog_args_t prog_args; 
  lte_cell_t cell;  
  int64_t sf_cnt;
  ue_sync_t ue_sync; 
  ue_mib_t ue_mib; 
  void *uhd; 
  ue_dl_t ue_dl; 
  lte_fft_t fft; 
  chest_dl_t chest; 
  uint32_t nof_trials = 0; 
  uint32_t sfn = 0; // system frame number
  int n; 
  uint8_t bch_payload[BCH_PAYLOAD_LEN], bch_payload_unpacked[BCH_PAYLOAD_LEN];
  uint32_t sfn_offset; 
  
  parse_args(&prog_args, argc, argv);

  #ifndef DISABLE_GRAPHICS
  if (!prog_args.disable_plots) {
    init_plots();    
  }
  #endif

  printf("Opening UHD device...\n");
  if (cuhd_open(prog_args.uhd_args, &uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    exit(-1);
  }
  /* Set receiver gain */
  cuhd_set_rx_gain(uhd, prog_args.uhd_gain);

  /* set receiver frequency */
  cuhd_set_rx_freq(uhd, (double) prog_args.uhd_freq);
  cuhd_rx_wait_lo_locked(uhd);
  printf("Tunning receiver to %.3f MHz\n", (double ) prog_args.uhd_freq/1000000);

  if (detect_and_decode_cell(&cell_detect_config, uhd, prog_args.force_N_id_2, &cell)) {
    fprintf(stderr, "Cell not found\n");
    exit(-1); 
  }
  
  cuhd_start_rx_stream(uhd);
  
  if (ue_sync_init(&ue_sync, cell, cuhd_recv_wrapper, uhd)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    exit(-1); 
  }
  if (ue_dl_init(&ue_dl, cell, 1234)) { 
    fprintf(stderr, "Error initiating UE downlink processing module\n");
    exit(-1);
  }
  if (ue_mib_init_known_cell(&ue_mib, cell, false)) {
    fprintf(stderr, "Error initaiting UE MIB decoder\n");
    exit(-1);
  }
  pdsch_set_rnti(&ue_dl.pdsch, SIRNTI);

  /* Initialize subframe counter */
  sf_cnt = 0;
    
  if (lte_fft_init(&fft, cell.cp, cell.nof_prb)) {
    fprintf(stderr, "Error initiating FFT\n");
    return -1;
  }
  if (chest_dl_init(&chest, cell)) {
    fprintf(stderr, "Error initiating channel estimator\n");
    return -1;
  }
  
  /* Main loop */
  while (sf_cnt < prog_args.nof_subframes || prog_args.nof_subframes == -1) {
    
    ret = ue_sync_get_buffer(&ue_sync, &sf_buffer);
    if (ret < 0) {
      fprintf(stderr, "Error calling ue_sync_work()\n");
    }

    /* ue_sync_get_buffer returns 1 if successfully read 1 aligned subframe */
    if (ret == 1) {
        if (ue_sync_get_sfidx(&ue_sync) == 0) {
          pbch_decode_reset(&ue_mib.pbch);
          n = ue_mib_decode_aligned_frame(&ue_mib,
                                          sf_buffer, bch_payload_unpacked, 
                                          NULL, &sfn_offset);
          if (n < 0) {
            fprintf(stderr, "Error decoding UE MIB\n");
            exit(-1);
          } else if (n == MIB_FOUND) {
            bit_unpack_vector(bch_payload_unpacked, bch_payload, BCH_PAYLOAD_LEN);
            bcch_bch_unpack(bch_payload, BCH_PAYLOAD_LEN, &cell, &sfn);
            sfn = (sfn + sfn_offset)%1024; 
          }
        }
        /* We are looking for SI Blocks, search only in appropiate places */
        if ((ue_sync_get_sfidx(&ue_sync) == 5 && (sfn%2)==0)) {
          n = ue_dl_decode(&ue_dl, sf_buffer, data, ue_sync_get_sfidx(&ue_sync), sfn, SIRNTI);
          if (n < 0) {
            fprintf(stderr, "Error decoding UE DL\n");fflush(stdout);
            exit(-1);
          } else if (n == 0) {
            printf("CFO: %+8.4f KHz, SFO: %+8.4f Khz, ExecTime: %6.1f us, NOI: %.2f,"
            "PDCCH-Det: %.3f, PDSCH-BLER: %.3f\r",
                    ue_sync_get_cfo(&ue_sync)/1000, ue_sync_get_sfo(&ue_sync)/1000, 
                    mean_exec_time, pdsch_average_noi(&ue_dl.pdsch),
                    (float) ue_dl.nof_pdcch_detected/nof_trials,
                    (float) ue_dl.pkt_errors/ue_dl.pkts_total,nof_trials);                         
            
          }
          nof_trials++;             
        }
      } else if (ret == 0) {
        /*printf("Finding PSS... Peak: %8.1f, FrameCnt: %d, State: %d\r", 
          sync_get_peak_value(&ue_sync.sfind), 
          ue_sync.frame_total_cnt, ue_sync.state);      
          */
      }
      if (ue_sync_get_sfidx(&ue_sync) == 9) {
        sfn++; 
        if (sfn == 1024) {
          sfn = 0; 
        }
      }
      #ifndef DISABLE_GRAPHICS
      if (!prog_args.disable_plots && ue_sync_get_sfidx(&ue_sync) == 5) {
        do_plots(&ue_dl, 5);          
      }
      #endif

   
    sf_cnt++;                  
  } // Main loop

  ue_sync_free(&ue_sync);
  cuhd_close(uhd);
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
