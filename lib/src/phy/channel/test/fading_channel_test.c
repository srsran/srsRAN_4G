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

#include "srslte/phy/channel/fading.h"
#include "srslte/phy/utils/vector.h"
#include <complex.h>
#include <math.h>
#include <memory.h>
#include <srslte/phy/utils/debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

//#undef ENABLE_GUI /* Disable GUI by default */
#ifdef ENABLE_GUI
#include "srsgui/srsgui.h"
static bool enable_gui = false;
#endif /* ENABLE_GUI */

static srslte_channel_fading_t channel_fading;

static char     default_model[] = "epa5";
static uint32_t duration_ms     = 1000;
static char*    model           = default_model;
static uint32_t srate           = (uint32_t)30.72e6;
static uint32_t random_seed     = 0x12345678; // Default seed, deterministic channel

#define INPUT_TYPE 0 /* 0: Dirac Delta; Otherwise: Random*/

static void usage(char* prog)
{
  printf("Usage: %s [mts]\n", prog);
  printf("\t-m Channel model: epa5, eva70, etu300 [Default %s]\n", model);
  printf("\t-t Simulation time in ms: [Default %d]\n", duration_ms);
  printf("\t-s Sampling rate in Hz: [Default %d]\n", srate);
  printf("\t-r Random generator seed: [Default %d]\n", random_seed);
#ifdef ENABLE_GUI
  printf("\t-g Enable GUI: [Default %s]\n", enable_gui ? "enabled" : "disabled");
#endif /* ENABLE_GUI */
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "mtsrg")) != -1) {
    switch (opt) {
      case 'm':
        model = argv[optind];
        break;
      case 't':
        duration_ms = (uint32_t)strtof(argv[optind], NULL);
        break;
      case 's':
        srate = (uint32_t)strtof(argv[optind], NULL);
        break;
      case 'r':
        random_seed = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'g':
#ifdef ENABLE_GUI
        enable_gui = (enable_gui) ? false : true;
#endif /* ENABLE_GUI */
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  int            ret           = SRSLTE_ERROR;
  cf_t*          input_buffer  = NULL;
  cf_t*          output_buffer = NULL;
  struct timeval t[3]          = {};
  uint64_t       time_usec     = 0;

  parse_args(argc, argv);

  srslte_dft_plan_t ifft;
  srslte_dft_plan_c(&ifft, srate / 1000, SRSLTE_DFT_BACKWARD);

#ifdef ENABLE_GUI
  plot_real_t plot_fft = NULL;
  plot_real_t plot_h   = NULL;
  plot_real_t plot_imp = NULL;

  srslte_dft_plan_t fft        = {};
  cf_t*             fft_buffer = NULL;
  float*            fft_mag    = NULL;
  float*            imp        = NULL;

  if (enable_gui) {
    sdrgui_init();
    sdrgui_init_title("SRS Fading channel");

    plot_real_init(&plot_h);
    plot_real_setTitle(&plot_h, "Ideal channel");
    plot_real_setYAxisScale(&plot_h, -60, 20);
    plot_real_addToWindowGrid(&plot_h, (char*)"fading", 0, 0);

    plot_real_init(&plot_fft);
    plot_real_setTitle(&plot_fft, "FFT measured");
    plot_real_setYAxisScale(&plot_fft, -60, 20);
    plot_real_addToWindowGrid(&plot_fft, (char*)"fading", 0, 1);

    plot_real_init(&plot_imp);
    plot_real_setTitle(&plot_imp, "Impulse");
    plot_real_setYAxisScale(&plot_imp, 0, 5);
    plot_real_addToWindowGrid(&plot_imp, (char*)"fading", 0, 2);

    srslte_dft_plan_c(&fft, srate / 1000, SRSLTE_DFT_FORWARD);

    fft_buffer = srslte_vec_cf_malloc(srate / 1000);
    if (!fft_buffer) {
      fprintf(stderr, "Error: malloc\n");
      goto clean_exit;
    }

    fft_mag = srslte_vec_f_malloc(srate / 1000);
    if (!fft_mag) {
      fprintf(stderr, "Error: malloc\n");
      goto clean_exit;
    }

    imp = srslte_vec_f_malloc(srate / 1000);
    if (!imp) {
      fprintf(stderr, "Error: malloc\n");
      goto clean_exit;
    }
  }
#endif /* ENABLE_GUI */

  // Initialise channel
  if (srslte_channel_fading_init(&channel_fading, srate, model, 0x12345678)) {
    fprintf(stderr, "Error: initialising fading channel. model=%s, srate=%d\n", model, srate);
    goto clean_exit;
  }

  // Allocate buffers
  input_buffer = srslte_vec_cf_malloc(srate / 1000);
  if (!input_buffer) {
    fprintf(stderr, "Error: allocating input buffer\n");
    goto clean_exit;
  }

#if INPUT_TYPE == 0
  srslte_vec_cf_zero(input_buffer, srate / 1000);
  input_buffer[0] = 1;
#else
  for (int i = 0; i < srate / 1000; i++) {
    input_buffer[i] = cexpf(_Complex_I * ((double)rand() / (double)RAND_MAX) * M_PI / 2.0) / ((double)srate / 1000.0);
  }
  srslte_dft_run_c(&ifft, input_buffer, input_buffer);
#endif

  output_buffer = srslte_vec_cf_malloc(srate / 1000);
  if (!output_buffer) {
    fprintf(stderr, "Error: allocating output buffer\n");
    goto clean_exit;
  }

  printf("-- Starting Fading channel simulator. srate=%.2fMHz; model=%s; duration=%dms\n",
         (double)srate / 1e6,
         model,
         duration_ms);

  for (int i = 0; i < duration_ms; i++) {
    gettimeofday(&t[1], NULL);
    srslte_channel_fading_execute(&channel_fading, input_buffer, output_buffer, srate / 1000, (double)i / 1000.0);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    time_usec += (uint64_t)(t->tv_sec * 1e6 + t->tv_usec);

#ifdef ENABLE_GUI
    if (enable_gui) {
      srslte_dft_run_c_zerocopy(&fft, output_buffer, fft_buffer);
      srslte_vec_prod_conj_ccc(fft_buffer, fft_buffer, fft_buffer, srate / 1000);
      for (int j = 0; j < srate / 1000; j++) {
        fft_mag[j] = srslte_convert_power_to_dB(__real__ fft_buffer[j]);
      }
      plot_real_setNewData(&plot_fft, fft_mag, srate / 1000);

      for (int j = 0; j < channel_fading.N; j++) {
        fft_mag[j] = srslte_convert_amplitude_to_dB(cabsf(channel_fading.h_freq[j]));
      }
      plot_real_setNewData(&plot_h, fft_mag, channel_fading.N);

      for (int j = 0; j < srate / 1000; j++) {
        imp[j] = cabsf(output_buffer[j]);
      }
      plot_real_setNewData(&plot_imp, imp, channel_fading.N);

      usleep(1000);
    }
#endif /* ENABLE_GUI */
  }

  ret = SRSLTE_SUCCESS;

clean_exit:
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok ... %.1f MSps\n", duration_ms * (srate / 1000.0) / (double)time_usec);
  }

  srslte_dft_plan_free(&ifft);

#ifdef ENABLE_GUI
  if (enable_gui) {
    sdrgui_exit();
    if (fft_mag) {
      free(fft_mag);
    }
    if (imp) {
      free(imp);
    }
    if (fft_buffer) {
      free(fft_buffer);
    }
    srslte_dft_plan_free(&fft);
  }
#endif /* ENABLE_GUI */
  if (input_buffer) {
    free(input_buffer);
  }
  if (output_buffer) {
    free(output_buffer);
  }
  srslte_channel_fading_free(&channel_fading);
  return ret;
}
