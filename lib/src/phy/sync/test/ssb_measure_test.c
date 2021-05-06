/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/common/test_common.h"
#include "srsran/phy/sync/ssb.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include <getopt.h>
#include <srsran/phy/channel/ch_awgn.h>
#include <stdlib.h>

// NR parameters
static uint32_t                    carrier_nof_prb = 52;
static srsran_subcarrier_spacing_t carrier_scs     = srsran_subcarrier_spacing_15kHz;
static srsran_subcarrier_spacing_t ssb_scs         = srsran_subcarrier_spacing_30kHz;

// Channel parameters
static int32_t delay_n = 1;
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
#define N0_MAX_ERROR 2.0f
#define SNR_MAX_ERROR 2.0f
#define CFO_MAX_ERROR (cfo_hz * 0.3f)
#define DELAY_MAX_ERROR (delay_us * 0.1f)

static void usage(char* prog)
{
  printf("Usage: %s [v]\n", prog);
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "v")) != -1) {
    switch (opt) {
      case 'v':
        srsran_verbose++;
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
  srsran_vec_apply_cfo(buffer, -cfo_hz / srate_hz, buffer, sf_len);

  // AWGN
  srsran_channel_awgn_run_c(&awgn, buffer, buffer, sf_len);
}

static int test_case_1(srsran_ssb_t* ssb)
{
  uint64_t         t_usec  = 0;
  srsran_ssb_cfg_t ssb_cfg = {};
  ssb_cfg.srate_hz         = srate_hz;
  ssb_cfg.freq_offset_hz   = 0.0;
  ssb_cfg.scs              = ssb_scs;

  TESTASSERT(srsran_ssb_set_cfg(ssb, &ssb_cfg) == SRSRAN_SUCCESS);

  // Build PBCH message
  srsran_pbch_msg_nr_t pbch_msg = {};

  for (uint32_t pci = 0; pci < SRSRAN_NOF_NID_NR; pci++) {
    struct timeval t[3] = {};

    // Initialise baseband
    srsran_vec_cf_zero(buffer, sf_len);

    // Add the SSB base-band
    TESTASSERT(srsran_ssb_add(ssb, pci, &pbch_msg, buffer, buffer) == SRSRAN_SUCCESS);

    // Run channel
    run_channel();

    // Measure
    srsran_csi_trs_measurements_t meas = {};
    TESTASSERT(srsran_ssb_csi_measure(ssb, pci, buffer, &meas) == SRSRAN_SUCCESS);

    gettimeofday(&t[1], NULL);
    TESTASSERT(srsran_ssb_csi_measure(ssb, pci, buffer, &meas) == SRSRAN_SUCCESS);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    t_usec += t[0].tv_usec + t[0].tv_sec * 1000000UL;

    // Print measurement
    char str[512];
    srsran_csi_meas_info(&meas, str, sizeof(str));
    INFO("test_case_1 - pci=%d %s", pci, str);

    // Assert measurements
    TESTASSERT(fabsf(meas.rsrp_dB - 0.0f) < RSRP_MAX_ERROR);
    TESTASSERT(fabsf(meas.epre_dB - 0.0f) < EPRE_MAX_ERROR);
    TESTASSERT(fabsf(meas.n0_dB - n0_dB) < N0_MAX_ERROR);
    TESTASSERT(fabsf(meas.snr_dB + n0_dB) < SNR_MAX_ERROR);
    TESTASSERT(fabsf(meas.cfo_hz - cfo_hz) < CFO_MAX_ERROR);
    TESTASSERT(fabsf(meas.delay_us + delay_us) < DELAY_MAX_ERROR);
  }

  INFO("test_case_1 - %.1f usec/measurement", (double)t_usec / (double)SRSRAN_NOF_NID_NR);

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