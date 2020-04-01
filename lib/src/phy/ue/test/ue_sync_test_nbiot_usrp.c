/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include "srslte/phy/rf/rf.h"
#include "srslte/phy/sync/sync_nbiot.h"
#include "srslte/phy/ue/ue_sync_nbiot.h"
#include "srslte/srslte.h"
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#define CFO_TABLE_MAX_IDX 1024

#ifndef DISABLE_GRAPHICS
void init_plots();
void do_plots_npss(float* corr, float energy, uint32_t size);
void do_plots_cfo(float* cfo_table, uint32_t size);
#endif

bool        disable_plots = false;
char*       rf_args       = "";
float       rf_gain = 70.0, rf_freq = -1.0;
int         nof_frames  = -1;
uint32_t    fft_size    = 128;
float       threshold   = 20.0;
int         N_id_2_sync = -1;
float       cfo_ema     = 0.2;
float       do_cfo_corr = true;
srslte_cp_t cp          = SRSLTE_CP_NORM;

srslte_nbiot_cell_t cell = {
    .base      = {.nof_prb = 1, .nof_ports = 1, .cp = SRSLTE_CP_NORM, .id = 0},
    .nbiot_prb = 0,
};

void usage(char* prog)
{
  printf("Usage: %s [aedgtvndp] -f rx_frequency_hz\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-g RF Gain [Default %.2f dB]\n", rf_gain);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-c Set CFO EMA value [Default %f]\n", cfo_ema);
  printf("\t-C Disable CFO correction [Default Enabled]\n");
  printf("\t-s symbol_sz [Default %d]\n", fft_size);
  printf("\t-t threshold [Default %.2f]\n", threshold);
#ifndef DISABLE_GRAPHICS
  printf("\t-d disable plots [Default enabled]\n");
#else
  printf("\t plots are disabled. Graphics library not available\n");
#endif
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "acCdgntvsfil")) != -1) {
    switch (opt) {
      case 'a':
        rf_args = argv[optind];
        break;
      case 'g':
        rf_gain = strtof(argv[optind], NULL);
        break;
      case 'f':
        rf_freq = strtof(argv[optind], NULL);
        break;
      case 't':
        threshold = strtof(argv[optind], NULL);
        break;
      case 'c':
        cfo_ema = strtof(argv[optind], NULL);
        break;
      case 'C':
        do_cfo_corr = false;
        break;
      case 's':
        fft_size = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        nof_frames = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        disable_plots = true;
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (rf_freq < 0) {
    usage(argv[0]);
    exit(-1);
  }
}

bool go_exit = false;
void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  }
}

int srslte_rf_recv_wrapper_cs(void* h, void* data, uint32_t nsamples, srslte_timestamp_t* t)
{
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return srslte_rf_recv(h, data, nsamples, 1);
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

#ifndef DISABLE_GRAPHICS
  float    cfo_table[CFO_TABLE_MAX_IDX];
  uint32_t cfo_table_index = 0;
  uint32_t cfo_num_plot    = CFO_TABLE_MAX_IDX;

  if (!disable_plots) {
    init_plots();
  }
#endif

  signal(SIGINT, sig_int_handler);

  float   srate = 15000.0 * fft_size;
  int32_t flen  = srate * 10 / 1000;

  printf("Frame length %d samples\n", flen);

  printf("Opening RF device...\n");
  srslte_rf_t rf;
  if (srslte_rf_open(&rf, rf_args)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }

  printf("Set RX rate: %.2f MHz\n", srslte_rf_set_rx_srate(&rf, srate) / 1000000);
  printf("Set RX gain: %.1f dB\n", srslte_rf_set_rx_gain(&rf, rf_gain));
  printf("Set RX freq: %.2f MHz\n", srslte_rf_set_rx_freq(&rf, 0, rf_freq) / 1000000);

  // Allocate memory for rx'ing samples (1 full frame)
  cf_t* rx_buffer[SRSLTE_MAX_PORTS] = {NULL, NULL, NULL, NULL};
  for (uint32_t i = 0; i < SRSLTE_NBIOT_NUM_RX_ANTENNAS; i++) {
    rx_buffer[i] = srslte_vec_cf_malloc(SRSLTE_NOF_SF_X_FRAME * SRSLTE_SF_LEN_PRB_NBIOT);
    if (!rx_buffer[i]) {
      perror("malloc");
      goto clean_exit;
    }
  }

  srslte_nbiot_ue_sync_t ue_sync = {};
  if (srslte_ue_sync_nbiot_init(&ue_sync, cell, srslte_rf_recv_wrapper_cs, (void*)&rf)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    exit(-1);
  }

  srslte_ue_sync_nbiot_set_cfo_enable(&ue_sync, do_cfo_corr);
  srslte_ue_sync_nbiot_set_cfo_ema(&ue_sync, cfo_ema);

  srslte_rf_start_rx_stream(&rf, false);

  int frame_cnt = 0;
  printf("Trying to keep syncronized to cell for %d frames\n", nof_frames);

  while ((frame_cnt < nof_frames || nof_frames == -1) && !go_exit) {
    if (srslte_ue_sync_nbiot_zerocopy_multi(&ue_sync, rx_buffer) < 0) {
      fprintf(stderr, "Error calling srslte_nbiot_ue_sync_work()\n");
      break;
    }

    if (srslte_ue_sync_nbiot_get_sfidx(&ue_sync) == 0) {
      printf("CFO: %+6.2f kHz\r", srslte_ue_sync_nbiot_get_cfo(&ue_sync) / 1000);
      frame_cnt++;
    }

#ifndef DISABLE_GRAPHICS
    if (!disable_plots) {
      // get current CFO estimate
      cfo_table[cfo_table_index++] = srslte_ue_sync_nbiot_get_cfo(&ue_sync) / 1000;
      if (cfo_table_index == cfo_num_plot) {
        do_plots_cfo(cfo_table, cfo_num_plot);
        cfo_table_index = 0;
      }
    }

    if (!disable_plots) {
      srslte_npss_synch_t* pss_obj = ue_sync.state == SF_FIND ? &ue_sync.sfind.npss : &ue_sync.strack.npss;
      int                  len     = SRSLTE_NPSS_CORR_FILTER_LEN + pss_obj->frame_size - 1;
      int max = srslte_vec_max_fi(pss_obj->conv_output_avg, pss_obj->frame_size + pss_obj->fft_size - 1);
      do_plots_npss(pss_obj->conv_output_avg, pss_obj->conv_output_avg[max], len);
    }
#endif
  }

clean_exit:
  srslte_ue_sync_nbiot_free(&ue_sync);
  srslte_rf_close(&rf);

  for (uint32_t i = 0; i < SRSLTE_MAX_PORTS; i++) {
    if (rx_buffer[i] != NULL) {
      free(rx_buffer[i]);
    }
  }

  exit(0);
}

/**********************************************************************
 *  Plotting Functions
 ***********************************************************************/
#ifndef DISABLE_GRAPHICS

#include "srsgui/srsgui.h"
plot_real_t npss_plot;
plot_real_t cfo_plot;

float tmp[1000000];

void init_plots()
{
  sdrgui_init();
  plot_real_init(&npss_plot);
  plot_real_setTitle(&npss_plot, "NPSS xCorr");
  plot_real_setLabels(&npss_plot, "Index", "Absolute value");
  plot_real_setYAxisScale(&npss_plot, 0, 1);

  plot_real_init(&cfo_plot);
  plot_real_setTitle(&cfo_plot, "Carrier Frequency Offset");
  plot_real_setLabels(&cfo_plot, "subframe index", "kHz");

  plot_real_setYAxisScale(&cfo_plot, -3.5, 3.5);

  plot_real_addToWindowGrid(&npss_plot, (char*)"nbiot_ue_sync", 0, 0);
  plot_real_addToWindowGrid(&cfo_plot, (char*)"nbiot_ue_sync", 0, 1);
}

void do_plots_npss(float* corr, float peak, uint32_t size)
{
  srslte_vec_sc_prod_fff(corr, 1. / peak, tmp, size);
  plot_real_setNewData(&npss_plot, tmp, size);
}

void do_plots_cfo(float* cfo_table, uint32_t size)
{
  plot_real_setNewData(&cfo_plot, cfo_table, size);
}

#endif
