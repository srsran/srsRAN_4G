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
#include "srsran/phy/ch_estimation/csi_rs.h"
#include "srsran/phy/utils/vector.h"
#include <getopt.h>
#include <srsran/srsran.h>
#include <stdlib.h>

static srsran_carrier_nr_t carrier = {
    1,                               // pci
    0,                               // absolute_frequency_ssb
    0,                               // absolute_frequency_point_a
    0,                               // offset_to_carrier
    srsran_subcarrier_spacing_15kHz, // scs
    50,                              // nof_prb
    0,                               // start
    1                                // max_mimo_layers
};

static float    snr_dB               = 20.0;
static float    power_control_offset = NAN;
static uint32_t start_rb             = UINT32_MAX;
static uint32_t nof_rb               = UINT32_MAX;
static uint32_t first_symbol         = UINT32_MAX;

static int test(const srsran_slot_cfg_t*            slot_cfg,
                const srsran_csi_rs_nzp_resource_t* resource,
                srsran_channel_awgn_t*              awgn,
                cf_t*                               grid)
{
  srsran_csi_rs_measure_t measure = {};

  // Put NZP-CSI-RS
  TESTASSERT(srsran_csi_rs_nzp_put(&carrier, slot_cfg, resource, grid) == SRSRAN_SUCCESS);

  // Configure N0 and add Noise
  TESTASSERT(srsran_channel_awgn_set_n0(awgn, (float)resource->power_control_offset - snr_dB) == SRSRAN_SUCCESS);
  srsran_channel_awgn_run_c(awgn, grid, grid, SRSRAN_SLOT_LEN_RE_NR(carrier.nof_prb));

  TESTASSERT(srsran_csi_rs_nzp_measure(&carrier, slot_cfg, resource, grid, &measure) == SRSRAN_SUCCESS);

  const float rsrp_dB_gold = (float)resource->power_control_offset;
  const float epre_dB_gold =
      srsran_convert_power_to_dB(srsran_convert_dB_to_power(rsrp_dB_gold) + awgn->std_dev * awgn->std_dev);
  const float n0_dB_gold = srsran_convert_amplitude_to_dB(awgn->std_dev);

  if (srsran_verbose >= SRSRAN_VERBOSE_INFO) {
    char str[128] = {};
    srsran_csi_rs_measure_info(&measure, str, sizeof(str));
    INFO("Measure: %s", str);
  }

  TESTASSERT(fabsf(measure.rsrp_dB - rsrp_dB_gold) < 1.0f);
  TESTASSERT(fabsf(measure.epre_dB - epre_dB_gold) < 1.0f);
  TESTASSERT(fabsf(measure.n0_dB - n0_dB_gold) < 2.0f);
  TESTASSERT(fabsf(measure.snr_dB - snr_dB) < 2.0f);

  return SRSRAN_SUCCESS;
}

static void usage(char* prog)
{
  printf("Usage: %s [recov]\n", prog);
  printf("\t-p nof_prb [Default %d]\n", carrier.nof_prb);
  printf("\t-c cell_id [Default %d]\n", carrier.pci);
  printf("\t-s SNR in dB [Default %.2f]\n", snr_dB);
  printf("\t-S Start RB index [Default %d]\n", start_rb);
  printf("\t-L Number of RB [Default %d]\n", nof_rb);
  printf("\t-f First symbol index [Default %d]\n", first_symbol);
  printf("\t-o Power control offset [Default %.2f]\n", power_control_offset);
  printf("\t-v increase verbosity\n");
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "pcosSLfv")) != -1) {
    switch (opt) {
      case 'p':
        carrier.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        carrier.pci = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'o':
        power_control_offset = strtof(argv[optind], NULL);
        break;
      case 's':
        snr_dB = strtof(argv[optind], NULL);
        break;
      case 'S':
        start_rb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'L':
        nof_rb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        first_symbol = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srsran_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  int                          ret      = SRSRAN_ERROR;
  srsran_slot_cfg_t            slot_cfg = {};
  srsran_csi_rs_nzp_resource_t resource = {};
  srsran_channel_awgn_t        awgn     = {};

  parse_args(argc, argv);

  cf_t* grid = srsran_vec_cf_malloc(SRSRAN_SLOT_LEN_RE_NR(carrier.nof_prb));
  if (grid == NULL) {
    ERROR("Alloc");
    goto clean_exit;
  }

  if (srsran_channel_awgn_init(&awgn, 1234) < SRSRAN_SUCCESS) {
    ERROR("AWGN Init");
    goto clean_exit;
  }

  // Fixed parameters, other params are not implemented
  resource.resource_mapping.cdm       = srsran_csi_rs_cdm_nocdm;
  resource.resource_mapping.density   = srsran_csi_rs_resource_mapping_density_three;
  resource.resource_mapping.row       = srsran_csi_rs_resource_mapping_row_1;
  resource.resource_mapping.nof_ports = 1;

  // Row 1 supported only!
  uint32_t nof_freq_dom_alloc = SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW1;

  uint32_t first_symbol_begin = (first_symbol != UINT32_MAX) ? first_symbol : 0;
  uint32_t first_symbol_end   = (first_symbol != UINT32_MAX) ? first_symbol : 13;
  for (resource.resource_mapping.first_symbol_idx = first_symbol_begin;
       resource.resource_mapping.first_symbol_idx <= first_symbol_end;
       resource.resource_mapping.first_symbol_idx++) {
    // Iterate over possible power control offset
    float power_control_offset_begin = isnormal(power_control_offset) ? power_control_offset : -8.0f;
    float power_control_offset_end   = isnormal(power_control_offset) ? power_control_offset : 15.0f;
    for (resource.power_control_offset = power_control_offset_begin;
         resource.power_control_offset <= power_control_offset_end;
         resource.power_control_offset += 1.0f) {
      // Iterate over all possible starting number of PRB
      uint32_t start_rb_begin = (start_rb != UINT32_MAX) ? start_rb : 0;
      uint32_t start_rb_end   = (start_rb != UINT32_MAX) ? start_rb : carrier.nof_prb - 24;
      for (resource.resource_mapping.freq_band.start_rb = start_rb_begin;
           resource.resource_mapping.freq_band.start_rb <= start_rb_end;
           resource.resource_mapping.freq_band.start_rb += 4) {
        // Iterate over all possible number of PRB
        uint32_t nof_rb_begin = (nof_rb != UINT32_MAX) ? nof_rb : 24;
        uint32_t nof_rb_end =
            (nof_rb != UINT32_MAX) ? nof_rb : (carrier.nof_prb - resource.resource_mapping.freq_band.start_rb);
        for (resource.resource_mapping.freq_band.nof_rb = nof_rb_begin;
             resource.resource_mapping.freq_band.nof_rb <= nof_rb_end;
             resource.resource_mapping.freq_band.nof_rb += 4) {
          // Iterate for all slot numbers
          for (slot_cfg.idx = 0; slot_cfg.idx < SRSRAN_NSLOTS_PER_FRAME_NR(carrier.scs); slot_cfg.idx++) {
            // Steer Frequency allocation
            for (uint32_t freq_dom_alloc = 0; freq_dom_alloc < nof_freq_dom_alloc; freq_dom_alloc++) {
              for (uint32_t i = 0; i < nof_freq_dom_alloc; i++) {
                resource.resource_mapping.frequency_domain_alloc[i] = i == freq_dom_alloc;
              }

              // Call actual test
              if (test(&slot_cfg, &resource, &awgn, grid) < SRSRAN_SUCCESS) {
                goto clean_exit;
              }
            }
          }
        }
      }
    }
  }

  ret = SRSRAN_SUCCESS;

clean_exit:
  if (grid) {
    free(grid);
  }

  srsran_channel_awgn_free(&awgn);

  if (ret == SRSRAN_SUCCESS) {
    printf("Passed!\n");
  } else {
    printf("Failed!\n");
  }

  return ret;
}