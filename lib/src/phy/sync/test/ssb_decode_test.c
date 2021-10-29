/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
#include <complex.h>
#include <getopt.h>
#include <srsran/phy/utils/random.h>
#include <stdlib.h>

// NR parameters
static uint32_t                    carrier_nof_prb = 52;
static srsran_subcarrier_spacing_t carrier_scs     = srsran_subcarrier_spacing_15kHz;
static double                      carrier_freq_hz = 3.5e9 + 960e3;
static srsran_subcarrier_spacing_t ssb_scs         = srsran_subcarrier_spacing_30kHz;
static double                      ssb_freq_hz     = 3.5e9;
static srsran_ssb_patern_t         ssb_pattern     = SRSRAN_SSB_PATTERN_A;

// Channel parameters
static cf_t    wideband_gain = 1.0f + 0.5 * I;
static int32_t delay_n       = 1;
static float   cfo_hz        = 1000.0f;
static float   n0_dB         = -10.0f;

// Test context
static srsran_random_t       random_gen = NULL;
static srsran_channel_awgn_t awgn       = {};
static double                srate_hz   = 0.0f; // Base-band sampling rate
static uint32_t              hf_len     = 0;    // Half-frame length
static cf_t*                 buffer     = NULL; // Base-band buffer

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
  for (uint32_t i = 0; i < hf_len; i++) {
    buffer[i] = buffer[(i + delay_n) % hf_len];
  }

  // CFO
  srsran_vec_apply_cfo(buffer, -cfo_hz / srate_hz, buffer, hf_len);

  // AWGN
  srsran_channel_awgn_run_c(&awgn, buffer, buffer, hf_len);

  // Wideband gain
  srsran_vec_sc_prod_ccc(buffer, wideband_gain, buffer, hf_len);
}

static void gen_pbch_msg(srsran_pbch_msg_nr_t* pbch_msg, uint32_t ssb_idx)
{
  // Default all to zero
  SRSRAN_MEM_ZERO(pbch_msg, srsran_pbch_msg_nr_t, 1);

  // Generate payload
  srsran_random_bit_vector(random_gen, pbch_msg->payload, SRSRAN_PBCH_MSG_NR_SZ);

  pbch_msg->ssb_idx = ssb_idx;
  pbch_msg->crc     = true;
}

static int test_case_1(srsran_ssb_t* ssb)
{
  // For benchmarking purposes
  uint64_t t_encode_usec = 0;
  uint64_t t_decode_usec = 0;
  uint64_t t_search_usec = 0;

  // SSB configuration
  srsran_ssb_cfg_t ssb_cfg = {};
  ssb_cfg.srate_hz         = srate_hz;
  ssb_cfg.center_freq_hz   = carrier_freq_hz;
  ssb_cfg.ssb_freq_hz      = ssb_freq_hz;
  ssb_cfg.scs              = ssb_scs;
  ssb_cfg.pattern          = ssb_pattern;

  TESTASSERT(srsran_ssb_set_cfg(ssb, &ssb_cfg) == SRSRAN_SUCCESS);

  // For each PCI...
  uint64_t count = 0;
  for (uint32_t pci = 0; pci < SRSRAN_NOF_NID_NR; pci += 23) {
    for (uint32_t ssb_idx = 0; ssb_idx < ssb->Lmax; ssb_idx++, count++) {
      struct timeval t[3] = {};

      // Build PBCH message
      srsran_pbch_msg_nr_t pbch_msg_tx = {};
      gen_pbch_msg(&pbch_msg_tx, ssb_idx);

      // Print encoded PBCH message
      char str[512] = {};
      srsran_pbch_msg_info(&pbch_msg_tx, str, sizeof(str));
      INFO("test_case_1 - encoded pci=%d %s", pci, str);

      // Initialise baseband
      srsran_vec_cf_zero(buffer, hf_len);

      // Add the SSB base-band
      gettimeofday(&t[1], NULL);
      TESTASSERT(srsran_ssb_add(ssb, pci, &pbch_msg_tx, buffer, buffer) == SRSRAN_SUCCESS);
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      t_encode_usec += t[0].tv_usec + t[0].tv_sec * 1000000UL;

      // Run channel
      run_channel();

      // Decode
      gettimeofday(&t[1], NULL);
      srsran_pbch_msg_nr_t pbch_msg_rx = {};
      TESTASSERT(srsran_ssb_decode_pbch(ssb, pci, pbch_msg_tx.hrf, pbch_msg_tx.ssb_idx, buffer, &pbch_msg_rx) ==
                 SRSRAN_SUCCESS);
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      t_decode_usec += t[0].tv_usec + t[0].tv_sec * 1000000UL;

      // Print decoded PBCH message
      srsran_pbch_msg_info(&pbch_msg_rx, str, sizeof(str));
      INFO("test_case_1 - decoded pci=%d %s crc=%s", pci, str, pbch_msg_rx.crc ? "OK" : "KO");

      // Assert PBCH message CRC
      TESTASSERT(pbch_msg_rx.crc);
      TESTASSERT(memcmp(&pbch_msg_rx, &pbch_msg_tx, sizeof(srsran_pbch_msg_nr_t)) == 0);

      // Search
      srsran_ssb_search_res_t res = {};
      gettimeofday(&t[1], NULL);
      TESTASSERT(srsran_ssb_search(ssb, buffer, hf_len, &res) == SRSRAN_SUCCESS);
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      t_search_usec += t[0].tv_usec + t[0].tv_sec * 1000000UL;

      // Print decoded PBCH message
      srsran_pbch_msg_info(&res.pbch_msg, str, sizeof(str));
      INFO("test_case_1 - found   pci=%d %s crc=%s", res.N_id, str, res.pbch_msg.crc ? "OK" : "KO");

      // Assert PBCH message CRC
      TESTASSERT(res.pbch_msg.crc);
      TESTASSERT(memcmp(&res.pbch_msg, &pbch_msg_tx, sizeof(srsran_pbch_msg_nr_t)) == 0);
    }
  }

  if (!count) {
    ERROR("Error in test case 1: undefined division");
    return SRSRAN_ERROR;
  }

  INFO("test_case_1 - %.1f usec/encode; %.1f usec/decode; %.1f usec/decode;",
       (double)t_encode_usec / (double)(count),
       (double)t_decode_usec / (double)(count),
       (double)t_search_usec / (double)(count));

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  int ret = SRSRAN_ERROR;
  parse_args(argc, argv);

  random_gen = srsran_random_init(1234);
  srate_hz   = (double)SRSRAN_SUBC_SPACING_NR(carrier_scs) * srsran_min_symbol_sz_rb(carrier_nof_prb);
  hf_len     = (uint32_t)ceil(srate_hz * (5.0 / 1000.0));
  buffer     = srsran_vec_cf_malloc(hf_len);

  srsran_ssb_t      ssb      = {};
  srsran_ssb_args_t ssb_args = {};
  ssb_args.enable_encode     = true;
  ssb_args.enable_decode     = true;
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
  srsran_random_free(random_gen);
  srsran_ssb_free(&ssb);

  srsran_channel_awgn_free(&awgn);

  if (buffer) {
    free(buffer);
  }

  return ret;
}