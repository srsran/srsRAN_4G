/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/common/test_common.h"
#include "srsran/phy/channel/ch_awgn.h"
#include "srsran/phy/sync/ssb.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include <getopt.h>
#include <stdlib.h>

// NR parameters
static uint32_t                    carrier_nof_prb = 52;
static srsran_subcarrier_spacing_t carrier_scs     = srsran_subcarrier_spacing_15kHz;
static double                      carrier_freq_hz = 3.5e9 + 960e3;
static srsran_subcarrier_spacing_t ssb_scs         = srsran_subcarrier_spacing_30kHz;
static double                      ssb_freq_hz     = 3.5e9;
static srsran_ssb_pattern_t        ssb_pattern     = SRSRAN_SSB_PATTERN_A;

// Channel parameters
static int32_t delay_n = 2;
static float   cfo_hz  = 100.0f;
static float   n0_dB   = -30.0f;

// Test context
static srsran_channel_awgn_t awgn     = {};
static double                srate_hz = 0.0f; // Base-band sampling rate
static float                 delay_us = 0.0f; // Base-band sampling rate
static uint32_t              sf_len   = 0;    // Subframe length
static cf_t*                 buffer   = NULL; // Base-band buffer

#define RSRP_MAX_ERROR 1.0f
#define EPRE_MAX_ERROR 1.0f
#define N0_MAX_ERROR 3.0f
#define SNR_MAX_ERROR 3.0f
#define CFO_MAX_ERROR (cfo_hz * 0.3f)
#define DELAY_MAX_ERROR (delay_us * 0.1f)

static void usage(char* prog)
{
  printf("Usage: %s [v]\n", prog);
  printf("\t-s SSB subcarrier spacing [default, %s kHz]\n", srsran_subcarrier_spacing_to_str(ssb_scs));
  printf("\t-f SSB center frequency [default, %.3f MHz]\n", ssb_freq_hz / 1e6);
  printf("\t-S cell/carrier subcarrier spacing [default, %s kHz]\n", srsran_subcarrier_spacing_to_str(carrier_scs));
  printf("\t-F cell/carrier center frequency in Hz [default, %.3f MHz]\n", carrier_freq_hz / 1e6);
  printf("\t-P SSB pattern [default, %s]\n", srsran_ssb_pattern_to_str(ssb_pattern));
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}


static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "SsFfPv")) != -1) {
    switch (opt) {
      case 's':
        ssb_scs = srsran_subcarrier_spacing_from_str(argv[optind]);
        if (ssb_scs == srsran_subcarrier_spacing_invalid) {
          ERROR("Invalid SSB subcarrier spacing %s\n", argv[optind]);
          exit(-1);
        }
        break;
      case 'f':
        ssb_freq_hz = strtod(argv[optind], NULL);
        break;
      case 'S':
        carrier_scs = srsran_subcarrier_spacing_from_str(argv[optind]);
        if (carrier_scs == srsran_subcarrier_spacing_invalid) {
          ERROR("Invalid Cell/Carrier subcarrier spacing %s\n", argv[optind]);
          exit(-1);
        }
        break;
      case 'F':
        carrier_freq_hz = strtod(argv[optind], NULL);
        break;
      case 'P':
        ssb_pattern = srsran_ssb_pattern_fom_str(argv[optind]);
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

static void run_channel()
{
  // Delay
  for (uint32_t i = 0; i < sf_len; i++) {
    buffer[i] = buffer[(i + delay_n) % sf_len];
  }

  // CFO
  srsran_vec_apply_cfo(buffer, cfo_hz / srate_hz, buffer, sf_len);

  // AWGN
  srsran_channel_awgn_run_c(&awgn, buffer, buffer, sf_len);
}

static int assert_measure(const srsran_csi_trs_measurements_t* meas)
{
  TESTASSERT(fabsf(meas->rsrp_dB - 0.0f) < RSRP_MAX_ERROR);
  TESTASSERT(fabsf(meas->epre_dB - 0.0f) < EPRE_MAX_ERROR);
  TESTASSERT(fabsf(meas->n0_dB - n0_dB) < N0_MAX_ERROR);
  TESTASSERT(fabsf(meas->snr_dB + n0_dB) < SNR_MAX_ERROR);
  TESTASSERT(fabsf(meas->cfo_hz - cfo_hz) < CFO_MAX_ERROR);
  TESTASSERT(fabsf(meas->delay_us + delay_us) < DELAY_MAX_ERROR);
  return SRSRAN_SUCCESS;
}

static int test_case_1(srsran_ssb_t* ssb)
{
  // For benchmarking purposes
  uint64_t t_add_usec  = 0;
  uint64_t t_find_usec = 0;
  uint64_t t_meas_usec = 0;

  // SSB configuration
  srsran_ssb_cfg_t ssb_cfg = {};
  ssb_cfg.srate_hz         = srate_hz;
  ssb_cfg.center_freq_hz   = carrier_freq_hz;
  ssb_cfg.ssb_freq_hz      = ssb_freq_hz;
  ssb_cfg.scs              = ssb_scs;
  ssb_cfg.pattern          = ssb_pattern;

  TESTASSERT(srsran_ssb_set_cfg(ssb, &ssb_cfg) == SRSRAN_SUCCESS);

  // Build PBCH message
  srsran_pbch_msg_nr_t pbch_msg = {};

  for (uint32_t pci = 0; pci < SRSRAN_NOF_NID_NR; pci++) {
    struct timeval t[3] = {};

    // Initialise baseband
    srsran_vec_cf_zero(buffer, sf_len);

    // Add the SSB base-band
    gettimeofday(&t[1], NULL);
    TESTASSERT(srsran_ssb_add(ssb, pci, &pbch_msg, buffer, buffer) == SRSRAN_SUCCESS);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    t_add_usec += t[0].tv_usec + t[0].tv_sec * 1000000UL;

    // Run channel
    run_channel();

    // Find
    gettimeofday(&t[1], NULL);
    uint32_t                      N_id_found  = 0;
    srsran_csi_trs_measurements_t meas_search = {};
    TESTASSERT(srsran_ssb_csi_search(ssb, buffer, sf_len, &N_id_found, &meas_search) == SRSRAN_SUCCESS);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    t_find_usec += t[0].tv_usec + t[0].tv_sec * 1000000UL;

    // Print measurement
    char str[512] = {};
    srsran_csi_meas_info(&meas_search, str, sizeof(str));
    INFO("test_case_1 - search pci=%d %s", pci, str);

    // Assert find
    TESTASSERT(N_id_found == pci);

    // Measure
    gettimeofday(&t[1], NULL);
    srsran_csi_trs_measurements_t meas = {};
    TESTASSERT(srsran_ssb_csi_measure(ssb, pci, 0, buffer, &meas) == SRSRAN_SUCCESS);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    t_meas_usec += t[0].tv_usec + t[0].tv_sec * 1000000UL;

    srsran_csi_meas_info(&meas, str, sizeof(str));
    INFO("test_case_1 - measure pci=%d %s", pci, str);

    // Assert measurements
    TESTASSERT(assert_measure(&meas) == SRSRAN_SUCCESS);
  }

  INFO("test_case_1 - %.1f usec/encode; %.1f usec/search; Max srate %.1f MSps; %.1f usec/measurement",
       (double)t_add_usec / (double)SRSRAN_NOF_NID_NR,
       (double)t_find_usec / (double)SRSRAN_NOF_NID_NR,
       (double)sf_len * (double)SRSRAN_NOF_NID_NR / (double)t_find_usec,
       (double)t_meas_usec / (double)SRSRAN_NOF_NID_NR);

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  int ret = SRSRAN_ERROR;
  parse_args(argc, argv);

  srate_hz = (double)SRSRAN_SUBC_SPACING_NR(carrier_scs) * srsran_min_symbol_sz_rb(carrier_nof_prb);
  delay_us = 1e6f * delay_n / (float)srate_hz;
  sf_len   = (uint32_t)ceil(srate_hz / 1000.0);
  buffer   = srsran_vec_cf_malloc(sf_len);

  srsran_ssb_t      ssb      = {};
  srsran_ssb_args_t ssb_args = {};
  ssb_args.enable_encode     = true;
  ssb_args.enable_measure    = true;
  ssb_args.enable_search     = true;

  if (buffer == NULL) {
    ERROR("Malloc");
    goto clean_exit;
  }

  if (srsran_channel_awgn_init(&awgn, 0x0) < SRSRAN_SUCCESS) {
    ERROR("AWGN");
    goto clean_exit;
  }

  if (srsran_channel_awgn_set_n0(&awgn, n0_dB) < SRSRAN_SUCCESS) {
    ERROR("AWGN");
    goto clean_exit;
  }

  if (srsran_ssb_init(&ssb, &ssb_args) < SRSRAN_SUCCESS) {
    ERROR("Init");
    goto clean_exit;
  }

  if (test_case_1(&ssb) != SRSRAN_SUCCESS) {
    ERROR("test case failed");
    goto clean_exit;
  }

  ret = SRSRAN_SUCCESS;

clean_exit:
  srsran_ssb_free(&ssb);

  srsran_channel_awgn_free(&awgn);

  if (buffer) {
    free(buffer);
  }

  return ret;
}