/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
static srsran_subcarrier_spacing_t carrier_scs     = srsran_subcarrier_spacing_15kHz;
static double                      carrier_freq_hz = 3.5e9 + 960e3;
static srsran_subcarrier_spacing_t ssb_scs         = srsran_subcarrier_spacing_30kHz;
static double                      ssb_freq_hz     = 3.5e9;
static srsran_ssb_pattern_t        ssb_pattern     = SRSRAN_SSB_PATTERN_A;
static uint32_t                    ssb_idx         = 0;   // SSB candidate index to test
static uint32_t                    pci             = 123; // N_id

// Test context
static srsran_random_t random_gen = NULL;
static double          srate_hz   = 0.0f;            // Base-band sampling rate
static cf_t*           grid       = NULL;            // Resource grid
static uint32_t        grid_bw_sc = 52 * SRSRAN_NRE; // Resource grid bandwidth in subcarriers

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

static void gen_pbch_msg(srsran_pbch_msg_nr_t* pbch_msg)
{
  // Default all to zero
  SRSRAN_MEM_ZERO(pbch_msg, srsran_pbch_msg_nr_t, 1);

  // Generate payload
  srsran_random_bit_vector(random_gen, pbch_msg->payload, SRSRAN_PBCH_MSG_NR_SZ);

  pbch_msg->ssb_idx = ssb_idx;
  pbch_msg->crc     = true;
}

static int test_case(srsran_ssb_t* ssb)
{
  // SSB configuration
  srsran_ssb_cfg_t ssb_cfg = {};
  ssb_cfg.srate_hz         = srate_hz;
  ssb_cfg.center_freq_hz   = carrier_freq_hz;
  ssb_cfg.ssb_freq_hz      = ssb_freq_hz;
  ssb_cfg.scs              = ssb_scs;
  ssb_cfg.pattern          = ssb_pattern;

  TESTASSERT(srsran_ssb_set_cfg(ssb, &ssb_cfg) == SRSRAN_SUCCESS);

  // Build PBCH message
  srsran_pbch_msg_nr_t pbch_msg_tx = {};
  gen_pbch_msg(&pbch_msg_tx);

  // Print encoded PBCH message
  char str[512] = {};
  srsran_pbch_msg_info(&pbch_msg_tx, str, sizeof(str));
  INFO("test_case - encoded pci=%d %s", pci, str);

  // Add the SSB base-band
  TESTASSERT(srsran_ssb_put_grid(ssb, pci, &pbch_msg_tx, grid, grid_bw_sc) == SRSRAN_SUCCESS);

  // Decode
  srsran_pbch_msg_nr_t pbch_msg_rx = {};
  TESTASSERT(srsran_ssb_decode_grid(ssb, pci, pbch_msg_tx.hrf, pbch_msg_tx.ssb_idx, grid, grid_bw_sc, &pbch_msg_rx) ==
             SRSRAN_SUCCESS);

  // Print decoded PBCH message
  srsran_pbch_msg_info(&pbch_msg_rx, str, sizeof(str));
  INFO("test_case - decoded pci=%d %s crc=%s", pci, str, pbch_msg_rx.crc ? "OK" : "KO");

  // Assert PBCH message CRC
  TESTASSERT(pbch_msg_rx.crc);
  TESTASSERT(memcmp(&pbch_msg_rx, &pbch_msg_tx, sizeof(srsran_pbch_msg_nr_t)) == 0);

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  int ret = SRSRAN_ERROR;
  parse_args(argc, argv);

  random_gen = srsran_random_init(1234);
  srate_hz   = (double)SRSRAN_SUBC_SPACING_NR(carrier_scs) * srsran_min_symbol_sz_rb(grid_bw_sc / SRSRAN_NRE);
  grid       = srsran_vec_cf_malloc(grid_bw_sc * SRSRAN_NSYMB_PER_SLOT_NR);

  srsran_ssb_t      ssb      = {};
  srsran_ssb_args_t ssb_args = {};
  ssb_args.enable_encode     = true;
  ssb_args.enable_decode     = true;
  ssb_args.enable_search     = true;

  if (grid == NULL) {
    ERROR("Malloc");
    goto clean_exit;
  }

  if (srsran_ssb_init(&ssb, &ssb_args) < SRSRAN_SUCCESS) {
    ERROR("Init");
    goto clean_exit;
  }

  if (test_case(&ssb) != SRSRAN_SUCCESS) {
    ERROR("test case failed");
    goto clean_exit;
  }

  ret = SRSRAN_SUCCESS;

clean_exit:
  srsran_random_free(random_gen);
  srsran_ssb_free(&ssb);

  if (grid) {
    free(grid);
  }

  return ret;
}