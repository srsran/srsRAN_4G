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

#include "srsran/phy/ch_estimation/csi_rs.h"
#include "srsran/phy/utils/vector.h"
#include "srsran/support/srsran_test.h"
#include <getopt.h>
#include <srsran/srsran.h>
#include <stdlib.h>

static srsran_carrier_nr_t carrier = SRSRAN_DEFAULT_CARRIER_NR;

static float    snr_dB               = 20.0;
static float    power_control_offset = NAN;
static uint32_t start_rb             = UINT32_MAX;
static uint32_t nof_rb               = UINT32_MAX;
static uint32_t first_symbol         = UINT32_MAX;

static int nzp_test_case(const srsran_slot_cfg_t*            slot_cfg,
                         const srsran_csi_rs_nzp_resource_t* resource,
                         srsran_channel_awgn_t*              awgn,
                         cf_t*                               grid)
{
  srsran_csi_trs_measurements_t measure = {};

  // Put NZP-CSI-RS
  TESTASSERT(srsran_csi_rs_nzp_put_resource(&carrier, slot_cfg, resource, grid) == SRSRAN_SUCCESS);

  // Configure N0 and add Noise
  TESTASSERT(srsran_channel_awgn_set_n0(awgn, (float)resource->power_control_offset - snr_dB) == SRSRAN_SUCCESS);
  srsran_channel_awgn_run_c(awgn, grid, grid, SRSRAN_SLOT_LEN_RE_NR(carrier.nof_prb));

  TESTASSERT(srsran_csi_rs_nzp_measure(&carrier, slot_cfg, resource, grid, &measure) == SRSRAN_SUCCESS);

  const float rsrp_dB_gold = (float)resource->power_control_offset;
  const float epre_dB_gold =
      srsran_convert_power_to_dB(srsran_convert_dB_to_power(rsrp_dB_gold) + awgn->std_dev * awgn->std_dev);
  const float n0_dB_gold = srsran_convert_amplitude_to_dB(awgn->std_dev);

  if (get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO) {
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

static int nzp_test_brute(srsran_channel_awgn_t* awgn, cf_t* grid)
{
  // Slot configuration
  srsran_slot_cfg_t slot_cfg = {};

  // Initialise NZP-CSI-RS fix parameters, other params are not implemented
  srsran_csi_rs_nzp_resource_t resource = {};
  resource.resource_mapping.cdm         = srsran_csi_rs_cdm_nocdm;
  resource.resource_mapping.density     = srsran_csi_rs_resource_mapping_density_three;
  resource.resource_mapping.row         = srsran_csi_rs_resource_mapping_row_1;
  resource.resource_mapping.nof_ports   = 1;

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
              TESTASSERT(nzp_test_case(&slot_cfg, &resource, awgn, grid) == SRSRAN_SUCCESS);
            }
          }
        }
      }
    }
  }

  return SRSRAN_SUCCESS;
}

static int nzp_test_trs(srsran_channel_awgn_t* awgn, cf_t* grid)
{
  // Slot configuration
  srsran_slot_cfg_t slot_cfg = {};

  //  Item 1
  //      NZP-CSI-RS-Resource
  //          nzp-CSI-RS-ResourceId: 1
  //          resourceMapping
  //              frequencyDomainAllocation: row1 (0)
  //                  row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal value 1]
  //              nrofPorts: p1 (0)
  //              firstOFDMSymbolInTimeDomain: 4
  //              cdm-Type: noCDM (0)
  //              density: three (2)
  //                  three: NULL
  //              freqBand
  //                  startingRB: 0
  //                  nrofRBs: 52
  //          powerControlOffset: 0dB
  //          powerControlOffsetSS: db0 (1)
  //          scramblingID: 0
  //          periodicityAndOffset: slots40 (7)
  //              slots40: 11
  //          qcl-InfoPeriodicCSI-RS: 0
  srsran_csi_rs_nzp_resource_t resource1               = {};
  resource1.id                                         = 1;
  resource1.resource_mapping.frequency_domain_alloc[0] = 0;
  resource1.resource_mapping.frequency_domain_alloc[1] = 0;
  resource1.resource_mapping.frequency_domain_alloc[2] = 0;
  resource1.resource_mapping.frequency_domain_alloc[3] = 1;
  resource1.resource_mapping.nof_ports                 = 1;
  resource1.resource_mapping.first_symbol_idx          = 4;
  resource1.resource_mapping.cdm                       = srsran_csi_rs_cdm_nocdm;
  resource1.resource_mapping.density                   = srsran_csi_rs_resource_mapping_density_three;
  resource1.resource_mapping.freq_band.start_rb        = 0;
  resource1.resource_mapping.freq_band.nof_rb          = carrier.nof_prb;
  resource1.power_control_offset                       = 0;
  resource1.power_control_offset_ss                    = 0;
  resource1.periodicity.period                         = 40;
  resource1.periodicity.offset                         = 11;

  //  Item 2
  //      NZP-CSI-RS-Resource
  //          nzp-CSI-RS-ResourceId: 2
  //          resourceMapping
  //              frequencyDomainAllocation: row1 (0)
  //                  row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal value 1]
  //              nrofPorts: p1 (0)
  //              firstOFDMSymbolInTimeDomain: 8
  //              cdm-Type: noCDM (0)
  //              density: three (2)
  //                  three: NULL
  //              freqBand
  //                  startingRB: 0
  //                  nrofRBs: 52
  //          powerControlOffset: 0dB
  //          powerControlOffsetSS: db0 (1)
  //          scramblingID: 0
  //          periodicityAndOffset: slots40 (7)
  //              slots40: 11
  //          qcl-InfoPeriodicCSI-RS: 0
  srsran_csi_rs_nzp_resource_t resource2               = {};
  resource2.id                                         = 1;
  resource2.resource_mapping.frequency_domain_alloc[0] = 0;
  resource2.resource_mapping.frequency_domain_alloc[1] = 0;
  resource2.resource_mapping.frequency_domain_alloc[2] = 0;
  resource2.resource_mapping.frequency_domain_alloc[3] = 1;
  resource2.resource_mapping.nof_ports                 = 1;
  resource2.resource_mapping.first_symbol_idx          = 8;
  resource2.resource_mapping.cdm                       = srsran_csi_rs_cdm_nocdm;
  resource2.resource_mapping.density                   = srsran_csi_rs_resource_mapping_density_three;
  resource2.resource_mapping.freq_band.start_rb        = 0;
  resource2.resource_mapping.freq_band.nof_rb          = carrier.nof_prb;
  resource2.power_control_offset                       = 0;
  resource2.power_control_offset_ss                    = 0;
  resource2.periodicity.period                         = 40;
  resource2.periodicity.offset                         = 11;

  //  Item 3
  //      NZP-CSI-RS-Resource
  //          nzp-CSI-RS-ResourceId: 3
  //          resourceMapping
  //              frequencyDomainAllocation: row1 (0)
  //                  row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal value 1]
  //              nrofPorts: p1 (0)
  //              firstOFDMSymbolInTimeDomain: 4
  //              cdm-Type: noCDM (0)
  //              density: three (2)
  //                  three: NULL
  //              freqBand
  //                  startingRB: 0
  //                  nrofRBs: 52
  //          powerControlOffset: 0dB
  //          powerControlOffsetSS: db0 (1)
  //          scramblingID: 0
  //          periodicityAndOffset: slots40 (7)
  //              slots40: 12
  //          qcl-InfoPeriodicCSI-RS: 0
  srsran_csi_rs_nzp_resource_t resource3               = {};
  resource3.id                                         = 1;
  resource3.resource_mapping.frequency_domain_alloc[0] = 0;
  resource3.resource_mapping.frequency_domain_alloc[1] = 0;
  resource3.resource_mapping.frequency_domain_alloc[2] = 0;
  resource3.resource_mapping.frequency_domain_alloc[3] = 1;
  resource3.resource_mapping.nof_ports                 = 1;
  resource3.resource_mapping.first_symbol_idx          = 4;
  resource3.resource_mapping.cdm                       = srsran_csi_rs_cdm_nocdm;
  resource3.resource_mapping.density                   = srsran_csi_rs_resource_mapping_density_three;
  resource3.resource_mapping.freq_band.start_rb        = 0;
  resource3.resource_mapping.freq_band.nof_rb          = carrier.nof_prb;
  resource3.power_control_offset                       = 0;
  resource3.power_control_offset_ss                    = 0;
  resource3.periodicity.period                         = 40;
  resource3.periodicity.offset                         = 12;

  //  Item 4
  //      NZP-CSI-RS-Resource
  //          nzp-CSI-RS-ResourceId: 4
  //          resourceMapping
  //              frequencyDomainAllocation: row1 (0)
  //                  row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal value 1]
  //              nrofPorts: p1 (0)
  //              firstOFDMSymbolInTimeDomain: 8
  //              cdm-Type: noCDM (0)
  //              density: three (2)
  //                  three: NULL
  //              freqBand
  //                  startingRB: 0
  //                  nrofRBs: 52
  //          powerControlOffset: 0dB
  //          powerControlOffsetSS: db0 (1)
  //          scramblingID: 0
  //          periodicityAndOffset: slots40 (7)
  //              slots40: 12
  //          qcl-InfoPeriodicCSI-RS: 0
  srsran_csi_rs_nzp_resource_t resource4               = {};
  resource4.id                                         = 1;
  resource4.resource_mapping.frequency_domain_alloc[0] = 0;
  resource4.resource_mapping.frequency_domain_alloc[1] = 0;
  resource4.resource_mapping.frequency_domain_alloc[2] = 0;
  resource4.resource_mapping.frequency_domain_alloc[3] = 1;
  resource4.resource_mapping.nof_ports                 = 1;
  resource4.resource_mapping.first_symbol_idx          = 8;
  resource4.resource_mapping.cdm                       = srsran_csi_rs_cdm_nocdm;
  resource4.resource_mapping.density                   = srsran_csi_rs_resource_mapping_density_three;
  resource4.resource_mapping.freq_band.start_rb        = 0;
  resource4.resource_mapping.freq_band.nof_rb          = carrier.nof_prb;
  resource4.power_control_offset                       = 0;
  resource4.power_control_offset_ss                    = 0;
  resource4.periodicity.period                         = 40;
  resource4.periodicity.offset                         = 12;

  // NZP-CSI-RS-ResourceSet
  //    nzp-CSI-ResourceSetId: 1
  //    nzp-CSI-RS-Resources: 4 items
  //        Item 0
  //            NZP-CSI-RS-ResourceId: 1
  //        Item 1
  //            NZP-CSI-RS-ResourceId: 2
  //        Item 2
  //            NZP-CSI-RS-ResourceId: 3
  //        Item 3
  //            NZP-CSI-RS-ResourceId: 4
  //    trs-Info: true (0)
  srsran_csi_rs_nzp_set_t set = {};
  set.data[set.count++]       = resource1;
  set.data[set.count++]       = resource2;
  set.data[set.count++]       = resource3;
  set.data[set.count++]       = resource4;
  set.trs_info                = true;

  for (slot_cfg.idx = 0; slot_cfg.idx < resource1.periodicity.period; slot_cfg.idx++) {
    // Put NZP-CSI-RS TRS signals
    int ret = srsran_csi_rs_nzp_put_set(&carrier, &slot_cfg, &set, grid);

    // Check return
    if (slot_cfg.idx == 11 || slot_cfg.idx == 12) {
      TESTASSERT(ret == 2);
    } else {
      TESTASSERT(ret == 0);
    }

    // Configure N0 and add Noise
    TESTASSERT(srsran_channel_awgn_set_n0(awgn, (float)set.data[0].power_control_offset - snr_dB) == SRSRAN_SUCCESS);
    srsran_channel_awgn_run_c(awgn, grid, grid, SRSRAN_SLOT_LEN_RE_NR(carrier.nof_prb));

    // Measure
    srsran_csi_trs_measurements_t measure = {};
    ret                                   = srsran_csi_rs_nzp_measure_trs(&carrier, &slot_cfg, &set, grid, &measure);

    // Check return and assert measurement
    if (slot_cfg.idx == 11 || slot_cfg.idx == 12) {
      TESTASSERT(ret == 2);
    } else {
      TESTASSERT(ret == 0);
    }
  }

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
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  int                   ret  = SRSRAN_ERROR;
  srsran_channel_awgn_t awgn = {};

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

  if (nzp_test_brute(&awgn, grid) < SRSRAN_SUCCESS) {
    goto clean_exit;
  }

  if (nzp_test_trs(&awgn, grid) < SRSRAN_SUCCESS) {
    goto clean_exit;
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
