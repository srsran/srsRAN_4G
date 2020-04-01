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

#include "srslte/phy/ch_estimation/chest_sl.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/phch/pscch.h"
#include "srslte/phy/phch/sci.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/ue/ue_sync.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

uint32_t         nof_ports    = 1;
static bool      keep_running = true;
char*            output_file_name;
static char      rf_devname[64] = "";
static char      rf_args[64]    = "auto";
float            rf_gain = 40.0, rf_freq = -1.0;
int              nof_rx_antennas  = 1;
srslte_cell_sl_t sl_cell          = {.nof_prb = 50, .tm = SRSLTE_SIDELINK_TM4, .cp = SRSLTE_CP_NORM, .N_sl_id = 168};
uint32_t         size_sub_channel = 10;
uint32_t         num_sub_channel  = 5;
uint32_t         prb_idx          = 20;
bool             use_standard_lte_rates = false;
bool             disable_plots          = false;

srslte_pscch_t pscch; ///< Defined global for plotting thread

#ifdef ENABLE_GUI
#include "srsgui/srsgui.h"
void      init_plots();
pthread_t plot_thread;
sem_t     plot_sem;
#endif // ENABLE_GUI

void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    keep_running = false;
  } else if (signo == SIGSEGV) {
    exit(1);
  }
}

void usage(char* prog)
{
  printf("Usage: %s [agrnv] -f rx_frequency_hz\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-d RF devicename [Default %s]\n", rf_devname);
  printf("\t-g RF Gain [Default %.2f dB]\n", rf_gain);
  printf("\t-A nof_rx_antennas [Default %d]\n", nof_rx_antennas);
  printf("\t-c N_sl_id [Default %d]\n", sl_cell.N_sl_id);
  printf("\t-p nof_prb [Default %d]\n", sl_cell.nof_prb);
  printf("\t-x prb_idx [Default %i]\n", prb_idx);
  printf("\t-r use_standard_lte_rates [Default %i]\n", use_standard_lte_rates);
#ifdef ENABLE_GUI
  printf("\t-w disable plots [Default enabled]\n");
#endif
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "acdgpvwrxfA")) != -1) {
    switch (opt) {
      case 'a':
        strncpy(rf_args, argv[optind], 63);
        rf_args[63] = '\0';
        break;
      case 'c':
        sl_cell.N_sl_id = atoi(argv[optind]);
        break;
      case 'd':
        strncpy(rf_devname, argv[optind], 63);
        rf_devname[63] = '\0';
        break;
      case 'g':
        rf_gain = atof(argv[optind]);
        break;
      case 'p':
        sl_cell.nof_prb = atoi(argv[optind]);
        break;
      case 'f':
        rf_freq = atof(argv[optind]);
        break;
      case 'A':
        nof_rx_antennas = atoi(argv[optind]);
        break;
      case 'v':
        srslte_verbose++;
        break;
      case 'w':
        disable_plots = true;
        break;
      case 'r':
        use_standard_lte_rates = true;
        break;
      case 'x':
        prb_idx = atoi(argv[optind]);
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

int srslte_rf_recv_wrapper(void* h, cf_t* data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t* t)
{
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  void* ptr[SRSLTE_MAX_PORTS];
  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    ptr[i] = data[i];
  }
  return srslte_rf_recv_with_time_multi(h, ptr, nsamples, true, &t->full_secs, &t->frac_secs);
}

int main(int argc, char** argv)
{
  signal(SIGINT, sig_int_handler);
  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);

  parse_args(argc, argv);

  srslte_use_standard_symbol_size(use_standard_lte_rates);

  printf("Opening RF device...\n");
  srslte_rf_t rf;
  if (srslte_rf_open_multi(&rf, rf_args, nof_rx_antennas)) {
    ERROR("Error opening rf\n");
    exit(-1);
  }

  printf("Set RX freq: %.6f MHz\n", srslte_rf_set_rx_freq(&rf, nof_rx_antennas, rf_freq) / 1000000);
  printf("Set RX gain: %.1f dB\n", srslte_rf_set_rx_gain(&rf, rf_gain));
  int srate = srslte_sampling_freq_hz(sl_cell.nof_prb);

  if (srate != -1) {
    printf("Setting sampling rate %.2f MHz\n", (float)srate / 1000000);
    float srate_rf = srslte_rf_set_rx_srate(&rf, (double)srate);
    if (srate_rf != srate) {
      ERROR("Could not set sampling rate\n");
      exit(-1);
    }
  } else {
    ERROR("Invalid number of PRB %d\n", sl_cell.nof_prb);
    exit(-1);
  }

  // allocate Rx buffers for 1ms worth of samples
  uint32_t sf_len = SRSLTE_SF_LEN_PRB(sl_cell.nof_prb);
  printf("Using a SF len of %d samples\n", sf_len);

  cf_t* rx_buffer[SRSLTE_MAX_PORTS] = {NULL}; //< For radio to receive samples
  cf_t* sf_buffer[SRSLTE_MAX_PORTS] = {NULL}; ///< For OFDM object to store subframe after FFT

  for (int i = 0; i < nof_rx_antennas; i++) {
    rx_buffer[i] = srslte_vec_cf_malloc(sf_len);
    if (!rx_buffer[i]) {
      perror("malloc");
      exit(-1);
    }
    sf_buffer[i] = srslte_vec_cf_malloc(sf_len);
    if (!sf_buffer[i]) {
      perror("malloc");
      exit(-1);
    }
  }

  uint32_t sf_n_re             = SRSLTE_CP_NSYMB(SRSLTE_CP_NORM) * SRSLTE_NRE * 2 * sl_cell.nof_prb;
  cf_t*    equalized_sf_buffer = srslte_vec_cf_malloc(sf_n_re);

  // RX
  srslte_ofdm_t fft;
  if (srslte_ofdm_rx_init(&fft, sl_cell.cp, rx_buffer[0], sf_buffer[0], sl_cell.nof_prb)) {
    fprintf(stderr, "Error creating FFT object\n");
    return SRSLTE_ERROR;
  }
  srslte_ofdm_set_normalize(&fft, true);
  srslte_ofdm_set_freq_shift(&fft, -0.5);

  // PSCCH Channel estimation
  srslte_chest_sl_t pscch_chest;
  srslte_chest_sl_init_pscch_dmrs(&pscch_chest);

  // init PSCCH object
  if (srslte_pscch_init(&pscch, SRSLTE_MAX_PRB) != SRSLTE_SUCCESS) {
    ERROR("Error in PSCCH init\n");
    return SRSLTE_ERROR;
  }

  if (srslte_pscch_set_cell(&pscch, sl_cell) != SRSLTE_SUCCESS) {
    ERROR("Error in PSCCH set cell\n");
    return SRSLTE_ERROR;
  }

  srslte_sci_t sci;
  srslte_sci_init(&sci, sl_cell.nof_prb, sl_cell.tm, size_sub_channel, num_sub_channel);

  srslte_ue_sync_t sync;
  if (srslte_ue_sync_init_multi_decim_mode(
          &sync, SRSLTE_MAX_PRB, false, srslte_rf_recv_wrapper, nof_rx_antennas, (void*)&rf, 1.0, SYNC_MODE_GNSS)) {
    fprintf(stderr, "Error initiating sync_gnss\n");
    exit(-1);
  }

  srslte_cell_t cell = {};
  cell.nof_prb       = sl_cell.nof_prb;
  if (srslte_ue_sync_set_cell(&sync, cell)) {
    ERROR("Error initiating ue_sync\n");
    exit(-1);
  }

#ifdef ENABLE_GUI
  if (!disable_plots) {
    init_plots(&pscch);
    sleep(1);
  }
#endif

  // after configuring RF params and before starting streamer, set device to GPS time
  srslte_rf_sync(&rf);

  // start streaming
  srslte_rf_start_rx_stream(&rf, false);

  uint32_t num_decoded_sci = 0;
  uint32_t subframe_count  = 0;
  while (keep_running) {
    // receive subframe
    int ret = srslte_ue_sync_zerocopy(&sync, rx_buffer, sf_len);
    if (ret < 0) {
      ERROR("Error calling srslte_ue_sync_work()\n");
    }

    if (subframe_count == 0) {
      // print timestamp of the first samples
      srslte_timestamp_t ts_rx;
      srslte_ue_sync_get_last_timestamp(&sync, &ts_rx);
      printf("Received samples start at %ld + %.10f. TTI=%d.%d\n",
             ts_rx.full_secs,
             ts_rx.frac_secs,
             srslte_ue_sync_get_sfn(&sync),
             srslte_ue_sync_get_sfidx(&sync));
    }

    // do FFT
    srslte_ofdm_rx_sf(&fft);

    for (int i = 0; i < num_sub_channel; i++) {
      uint32_t pscch_prb_idx = size_sub_channel * i;

      for (uint32_t cyclic_shift = 0; cyclic_shift <= 9; cyclic_shift += 3) {

        uint8_t sci_rx[SRSLTE_SCI_MAX_LEN]      = {};
        char    sci_msg[SRSLTE_SCI_MSG_MAX_LEN] = {};

        // PSCCH Channel estimation
        srslte_chest_sl_gen_pscch_dmrs(&pscch_chest, cyclic_shift, sl_cell.tm);
        srslte_chest_sl_pscch_ls_estimate_equalize(
            &pscch_chest, sf_buffer[0], pscch_prb_idx, equalized_sf_buffer, sl_cell.nof_prb, sl_cell.tm, sl_cell.cp);

        if (srslte_pscch_decode(&pscch, equalized_sf_buffer, sci_rx, pscch_prb_idx) == SRSLTE_SUCCESS) {
          if (srslte_sci_format1_unpack(&sci, sci_rx) != SRSLTE_SUCCESS) {
            printf("Error unpacking sci format 1\n");
            return SRSLTE_ERROR;
          }

          srslte_sci_info(sci_msg, &sci);
          fprintf(stdout, "%s", sci_msg);

          num_decoded_sci++;

          // plot PSCCH
#ifdef ENABLE_GUI
          if (!disable_plots) {
            sem_post(&plot_sem);
          }
#endif
        }
        if (SRSLTE_VERBOSE_ISDEBUG()) {
          char filename[64];
          snprintf(
              filename, 64, "pscch_rx_syms_sf%d_shift%d_prbidx%d.bin", subframe_count, cyclic_shift, pscch_prb_idx);
          printf("Saving PSCCH symbols (%d) to %s\n", pscch.E / SRSLTE_PSCCH_QM, filename);
          srslte_vec_save_file(filename, pscch.mod_symbols, pscch.E / SRSLTE_PSCCH_QM * sizeof(cf_t));
        }
      }
    }

    subframe_count++;
  }

  printf("Processed %d subframes.\n", subframe_count);

#ifdef ENABLE_GUI
  if (!disable_plots) {
    sem_post(&plot_sem);
    usleep(1000);
    if (!pthread_kill(plot_thread, 0)) {
      pthread_kill(plot_thread, SIGHUP);
      pthread_join(plot_thread, NULL);
    }
  }
  sdrgui_exit();
#endif

  srslte_rf_stop_rx_stream(&rf);
  srslte_rf_close(&rf);
  srslte_ue_sync_free(&sync);
  srslte_sci_free(&sci);
  srslte_pscch_free(&pscch);
  srslte_chest_sl_free(&pscch_chest);

  for (int i = 0; i < nof_rx_antennas; i++) {
    if (rx_buffer[i]) {
      free(rx_buffer[i]);
    }
    if (sf_buffer[i]) {
      free(sf_buffer[i]);
    }
  }

  if (equalized_sf_buffer) {
    free(equalized_sf_buffer);
  }

  return SRSLTE_SUCCESS;
}

///< Plotting Functions
#ifdef ENABLE_GUI

plot_scatter_t pscatequal_pscch;

void* plot_thread_run(void* arg)
{
  sdrgui_init();

  plot_scatter_init(&pscatequal_pscch);
  plot_scatter_setTitle(&pscatequal_pscch, "PSCCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&pscatequal_pscch, -4, 4);
  plot_scatter_setYAxisScale(&pscatequal_pscch, -4, 4);

  plot_scatter_addToWindowGrid(&pscatequal_pscch, (char*)"pssch_ue", 0, 0);

  while (keep_running) {
    sem_wait(&plot_sem);
    plot_scatter_setNewData(&pscatequal_pscch, pscch.mod_symbols, pscch.nof_tx_re);
  }

  return NULL;
}

void init_plots()
{
  if (sem_init(&plot_sem, 0, 0)) {
    perror("sem_init");
    exit(-1);
  }

  pthread_attr_t     attr;
  struct sched_param param;
  param.sched_priority = 0;
  pthread_attr_init(&attr);
  pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
  pthread_attr_setschedparam(&attr, &param);
  if (pthread_create(&plot_thread, NULL, plot_thread_run, NULL)) {
    perror("pthread_create");
    exit(-1);
  }
}

#endif // ENABLE_GUI
