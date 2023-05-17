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

#ifdef __cplusplus
#include <complex.h>
extern "C" {
#include "srsran/phy/io/filesource.h"
#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/ue/ue_dl_nr.h"
#include "srsran/phy/utils/debug.h"
#include <srsran/phy/common/sliv.h>
}
#endif // __cplusplus

#include "srsran/common/band_helper.h"
#include <getopt.h>

static srsran_carrier_nr_t carrier = SRSRAN_DEFAULT_CARRIER_NR;

static char*                  filename     = NULL;
static srsran_pdcch_cfg_nr_t  pdcch_cfg    = {};
static srsran_sch_hl_cfg_nr_t pdsch_hl_cfg = {};
static uint16_t               rnti         = 0x1234;
static srsran_rnti_type_t     rnti_type    = srsran_rnti_type_c;
static srsran_slot_cfg_t      slot_cfg     = {};

static srsran_filesource_t    filesource               = {};
static srsran_ue_dl_nr_t      ue_dl                    = {};
static cf_t*                  buffer[SRSRAN_MAX_PORTS] = {};
static srsran_softbuffer_rx_t softbuffer               = {};
static uint8_t*               data                     = NULL;
static int                    pdsch_time_ra_start      = -1;
static int                    pdsch_time_ra_length     = -1;

static uint32_t coreset0_idx      = 0; // if ss_type=si coreset0 is used and this is the index
static uint32_t coreset_offset_rb = 0;
static bool     interleaved_pdcch = false;
static uint32_t dl_arfcn          = 161200; // center of the NR carrier (default at 806e6 Hz)
static uint32_t ssb_arfcn         = 161290; // center of the SSB within the carrier (default at 806.45e6)

static uint32_t                   coreset_n_rb = 48;
static uint32_t                   coreset_len  = 1;
static srsran_search_space_type_t ss_type      = srsran_search_space_type_common_0;

static void usage(char* prog)
{
  printf("Usage: %s [fPivnSRTscoNlAaIt] \n", prog);
  printf("\t-f File name [Default none]\n");
  printf("\t-P Number of BWP (Carrier) PRB [Default %d]\n", carrier.nof_prb);
  printf("\t-i Physical cell identifier [Default %d]\n", carrier.pci);
  printf("\t-n Slot index [Default %d]\n", slot_cfg.idx);
  printf("\t-R RNTI in hexadecimal [Default 0x%x]\n", rnti);
  printf("\t-T RNTI type (c, ra, si) [Default %s]\n", srsran_rnti_type_str(rnti_type));
  printf("\t-s Search space type (common0, common3, ue) [Default %s]\n", srsran_ss_type_str(ss_type));
  printf("\t-c Coreset0 index (only used if SS type is common0 for SIB) [Default %d]\n", coreset0_idx);
  printf("\t-o Coreset RB offset [Default %d]\n", coreset_offset_rb);
  printf("\t-N Coreset N_RB [Default %d]\n", coreset_n_rb);
  printf("\t-l Coreset duration in symbols [Default %d]\n", coreset_len);
  printf("\t-I Enable interleaved CCE-to-REG [Default %s]\n", interleaved_pdcch ? "Enabled" : "Disabled");
  printf("\t-t PDSCH time resource allocation [start symbol] [length]\n");
  printf("\t-A ARFCN of the NR carrier (center) [Default %d]\n", dl_arfcn);
  printf("\t-a center of the SSB within the carrier [Default %d]\n", ssb_arfcn);

  printf("\t-S Use standard rates [Default %s]\n", srsran_symbol_size_is_standard() ? "yes" : "no");

  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

static int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "fPivnSRTscoNlAaIt")) != -1) {
    switch (opt) {
      case 'f':
        filename = argv[optind];
        break;
      case 'P':
        carrier.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'i':
        carrier.pci = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      case 'n':
        slot_cfg.idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'R':
        rnti = (uint16_t)strtol(argv[optind], NULL, 16);
        break;
      case 't':
        pdsch_time_ra_start  = (int)strtol(argv[optind++], NULL, 10);
        pdsch_time_ra_length = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'T':
        if (strcmp(argv[optind], "c") == 0) {
          rnti_type = srsran_rnti_type_c;
        } else if (strcmp(argv[optind], "ra") == 0) {
          rnti_type = srsran_rnti_type_ra;
        } else if (strcmp(argv[optind], "si") == 0) {
          rnti_type = srsran_rnti_type_si;
        } else {
          printf("Invalid RNTI type '%s'\n", argv[optind]);
          usage(argv[0]);
          return SRSRAN_ERROR;
        }
        break;
      case 's':
        if (strcmp(argv[optind], "common0") == 0) {
          ss_type = srsran_search_space_type_common_0;
        } else if (strcmp(argv[optind], "common1") == 0) {
          ss_type = srsran_search_space_type_common_1;
        } else if (strcmp(argv[optind], "common3") == 0) {
          ss_type = srsran_search_space_type_common_3;
        } else if (strcmp(argv[optind], "ue") == 0) {
          ss_type = srsran_search_space_type_ue;
        } else {
          printf("Invalid SS type '%s'\n", argv[optind]);
          usage(argv[0]);
          return SRSRAN_ERROR;
        }
        break;
      case 'I':
        interleaved_pdcch ^= true;
        break;
      case 'c':
        coreset0_idx = (uint16_t)strtol(argv[optind], NULL, 10);
        break;
      case 'o':
        coreset_offset_rb = (uint16_t)strtol(argv[optind], NULL, 10);
        break;
      case 'N':
        coreset_n_rb = (uint16_t)strtol(argv[optind], NULL, 10);
        break;
      case 'l':
        coreset_len = (uint16_t)strtol(argv[optind], NULL, 10);
        break;
      case 'A':
        dl_arfcn = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'a':
        ssb_arfcn = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'S':
        srsran_use_standard_symbol_size(true);
        break;
      default:
        usage(argv[0]);
        return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

static int work_ue_dl(srsran_ue_dl_nr_t* ue_dl, srsran_slot_cfg_t* slot)
{
  // Run FFT
  srsran_ue_dl_nr_estimate_fft(ue_dl, slot);

  // Blind search
  srsran_dci_dl_nr_t dci_dl_rx     = {};
  int                nof_found_dci = srsran_ue_dl_nr_find_dl_dci(ue_dl, slot, rnti, rnti_type, &dci_dl_rx, 1);
  if (nof_found_dci < SRSRAN_SUCCESS) {
    ERROR("Error in blind search");
    return SRSRAN_ERROR;
  }

  // Print PDCCH blind search candidates
  for (uint32_t i = 0; i < ue_dl->pdcch_info_count; i++) {
    const srsran_ue_dl_nr_pdcch_info_t* info = &ue_dl->pdcch_info[i];
    INFO("PDCCH: %s-rnti=0x%x, crst_id=%d, ss_type=%s, ncce=%d, al=%d, EPRE=%+.2f, RSRP=%+.2f, corr=%.3f; "
         "nof_bits=%d; crc=%s;",
         srsran_rnti_type_str_short(info->dci_ctx.rnti_type),
         info->dci_ctx.rnti,
         info->dci_ctx.coreset_id,
         srsran_ss_type_str(info->dci_ctx.ss_type),
         info->dci_ctx.location.ncce,
         info->dci_ctx.location.L,
         info->measure.epre_dBfs,
         info->measure.rsrp_dBfs,
         info->measure.norm_corr,
         info->nof_bits,
         info->result.crc ? "OK" : "KO");
  }

  if (nof_found_dci < 1) {
    printf("No DCI found :'(\n");
    return SRSRAN_ERROR;
  }

  char str[1024] = {};
  srsran_dci_dl_nr_to_str(&ue_dl->dci, &dci_dl_rx, str, (uint32_t)sizeof(str));
  printf("Found DCI: %s\n", str);

  // Convert DCI to PDSCH transmission
  srsran_sch_cfg_nr_t pdsch_cfg = {};
  if (rnti_type == srsran_rnti_type_ra) {
    pdsch_hl_cfg.common_time_ra[0].k            = 0;
    pdsch_hl_cfg.common_time_ra[0].mapping_type = srsran_sch_mapping_type_A;
    pdsch_hl_cfg.common_time_ra[0].sliv =
        srsran_ra_type2_to_riv(SRSRAN_NSYMB_PER_SLOT_NR - 1, 1, SRSRAN_NSYMB_PER_SLOT_NR);
    pdsch_hl_cfg.nof_common_time_ra = 1;
  }
  if (srsran_ra_dl_dci_to_grant_nr(&carrier, slot, &pdsch_hl_cfg, &dci_dl_rx, &pdsch_cfg, &pdsch_cfg.grant) <
      SRSRAN_SUCCESS) {
    ERROR("Error decoding PDSCH search");
    return SRSRAN_ERROR;
  }

  srsran_sch_cfg_nr_info(&pdsch_cfg, str, (uint32_t)sizeof(str));
  printf("PDSCH: %s\n", str);

  // Set softbuffer
  pdsch_cfg.grant.tb[0].softbuffer.rx = &softbuffer;

  // Prepare PDSCH result
  srsran_pdsch_res_nr_t pdsch_res = {};
  pdsch_res.tb[0].payload         = data;

  // Decode PDSCH
  if (srsran_ue_dl_nr_decode_pdsch(ue_dl, slot, &pdsch_cfg, &pdsch_res) < SRSRAN_SUCCESS) {
    ERROR("Error decoding PDSCH search");
    return SRSRAN_ERROR;
  }

  if (!pdsch_res.tb[0].crc) {
    ERROR("Error decoding PDSCH");
    return SRSRAN_ERROR;
  }

  printf("Decoded PDSCH (%d B)\n", pdsch_cfg.grant.tb[0].tbs / 8);
  srsran_vec_fprint_byte(stdout, pdsch_res.tb[0].payload, pdsch_cfg.grant.tb[0].tbs / 8);

  // check payload is not all null
  bool all_zero = true;
  for (int i = 0; i < pdsch_cfg.grant.tb[0].tbs / 8; ++i) {
    if (pdsch_res.tb[0].payload[i] != 0x0) {
      all_zero = false;
      break;
    }
  }
  if (all_zero) {
    ERROR("PDSCH payload is all zeros");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

// helper to avoid goto in C++
int clean_exit(int ret)
{
  if (buffer[0] != NULL) {
    free(buffer[0]);
  }
  if (data != NULL) {
    free(data);
  }
  srsran_ue_dl_nr_free(&ue_dl);
  srsran_filesource_free(&filesource);
  srsran_softbuffer_rx_free(&softbuffer);
  return ret;
}

int main(int argc, char** argv)
{
  int ret = SRSRAN_ERROR;

  // parse args
  if (parse_args(argc, argv) < SRSRAN_SUCCESS) {
    return clean_exit(ret);
  }

  uint32_t sf_len = SRSRAN_SF_LEN_PRB(carrier.nof_prb);
  buffer[0]       = srsran_vec_cf_malloc(sf_len);
  if (buffer[0] == NULL) {
    ERROR("Error malloc");
    return clean_exit(ret);
  }

  if (srsran_softbuffer_rx_init_guru(&softbuffer, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
      SRSRAN_SUCCESS) {
    ERROR("Error init soft-buffer");
    return clean_exit(ret);
  }

  data = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
  if (data == NULL) {
    ERROR("Error malloc");
    return clean_exit(ret);
  }

  // Set default PDSCH configuration
  srsran_ue_dl_nr_args_t ue_dl_args        = {};
  ue_dl_args.nof_rx_antennas               = 1;
  ue_dl_args.pdsch.sch.disable_simd        = false;
  ue_dl_args.pdsch.sch.decoder_use_flooded = false;
  ue_dl_args.pdsch.measure_evm             = true;
  ue_dl_args.pdcch.disable_simd            = false;
  ue_dl_args.pdcch.measure_evm             = true;
  ue_dl_args.nof_max_prb                   = carrier.nof_prb;

  // Check for filename
  if (filename == NULL) {
    ERROR("Filename was not provided");
    return clean_exit(ret);
  }

  // Open filesource
  if (srsran_filesource_init(&filesource, filename, SRSRAN_COMPLEX_FLOAT_BIN) < SRSRAN_SUCCESS) {
    ERROR("Error opening filesource");
    return clean_exit(ret);
  }

  // initial DCI config
  srsran_dci_cfg_nr_t dci_cfg = {};
  dci_cfg.bwp_dl_initial_bw   = carrier.nof_prb;
  dci_cfg.bwp_ul_initial_bw   = carrier.nof_prb;
  dci_cfg.bwp_dl_active_bw    = carrier.nof_prb;
  dci_cfg.bwp_ul_active_bw    = carrier.nof_prb;
  dci_cfg.monitor_common_0_0  = true;
  dci_cfg.monitor_0_0_and_1_0 = true;


  // derive absolute frequencies from ARFCNs
  srsran::srsran_band_helper band_helper;
  carrier.ssb_center_freq_hz     = band_helper.nr_arfcn_to_freq(ssb_arfcn);
  carrier.dl_center_frequency_hz = band_helper.nr_arfcn_to_freq(dl_arfcn);

  srsran_coreset_t* coreset = NULL;

  // Configure CORESET
  if (rnti_type == srsran_rnti_type_si || rnti_type == srsran_rnti_type_ra) {
    // configure to use coreset0
    coreset                      = &pdcch_cfg.coreset[0];
    pdcch_cfg.coreset_present[0] = true;

    // Get pointA and SSB absolute frequencies
    double pointA_abs_freq_Hz =
            carrier.dl_center_frequency_hz - carrier.nof_prb * SRSRAN_NRE * SRSRAN_SUBC_SPACING_NR(carrier.scs) / 2;
    double ssb_abs_freq_Hz = carrier.ssb_center_freq_hz;
    // Calculate integer SSB to pointA frequency offset in Hz
    uint32_t ssb_pointA_freq_offset_Hz =
            (ssb_abs_freq_Hz > pointA_abs_freq_Hz) ? (uint32_t)(ssb_abs_freq_Hz - pointA_abs_freq_Hz) : 0;

    // derive coreset0 parameters
    if (srsran_coreset_zero(carrier.pci, ssb_pointA_freq_offset_Hz, carrier.scs, carrier.scs, coreset0_idx, coreset) !=
        SRSRAN_SUCCESS) {
      printf("Not possible to create CORESET Zero (ssb_scs=%s, pdcch_scs=%s, idx=%d)",
             srsran_subcarrier_spacing_to_str(carrier.scs),
             srsran_subcarrier_spacing_to_str(carrier.scs),
             coreset0_idx);
      return clean_exit(ret);
    }

    // Setup PDSCH DMRS (also signaled through MIB)
    pdsch_hl_cfg.typeA_pos = srsran_dmrs_sch_typeA_pos_2;

    // set coreset0 bandwidth
    dci_cfg.coreset0_bw = srsran_coreset_get_bw(coreset);
  } else {
    // configure to use coreset1
    coreset                      = &pdcch_cfg.coreset[1];
    coreset->id                  = 1;
    pdcch_cfg.coreset_present[1] = true;
    coreset->duration            = coreset_len;
    coreset->offset_rb           = coreset_offset_rb;
    for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
      coreset->freq_resources[i] = i < coreset_n_rb / 6;
    }
    if (interleaved_pdcch) {
      coreset->mapping_type         = srsran_coreset_mapping_type_interleaved;
      coreset->reg_bundle_size      = srsran_coreset_bundle_size_n6;
      coreset->interleaver_size     = srsran_coreset_bundle_size_n2;
      coreset->precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;
      coreset->shift_index          = carrier.pci;
    }
    // set coreset0 bandwidth (it is used in RA when ss_type = common3)
    dci_cfg.coreset0_bw = coreset_n_rb;

    // SCH configuration parameters
    if (pdsch_time_ra_start >= 0 && pdsch_time_ra_length >= 0) {
      auto last_pdsch_symbol = (uint16_t)(pdsch_time_ra_start + pdsch_time_ra_length);

      if (last_pdsch_symbol > SRSRAN_NSYMB_PER_SLOT_NR) {
        ERROR("incorrect PDSCH start symbol or length provided");
        return clean_exit(ret);
      }
      uint32_t sliv = srsran_ra_nr_type1_riv(SRSRAN_NSYMB_PER_SLOT_NR, pdsch_time_ra_start, pdsch_time_ra_length);

      pdsch_hl_cfg.nof_dedicated_time_ra             = 1;
      pdsch_hl_cfg.dedicated_time_ra[0].mapping_type = srsran_sch_mapping_type_A;
      pdsch_hl_cfg.dedicated_time_ra[0].k            = 0;
      pdsch_hl_cfg.dedicated_time_ra[0].sliv         = sliv;
    }
  }

  char coreset_info[512] = {};
  srsran_coreset_to_str(coreset, coreset_info, sizeof(coreset_info));
  INFO("Coreset parameter: %s", coreset_info);

  // Configure Search Space
  srsran_search_space_t* search_space = &pdcch_cfg.search_space[0];
  pdcch_cfg.search_space_present[0]   = true;
  search_space->id                    = 0;
  search_space->coreset_id            = (rnti_type == srsran_rnti_type_si || rnti_type == srsran_rnti_type_ra) ? 0 : 1;
  search_space->type                  = ss_type;
  search_space->formats[0]            = srsran_dci_format_nr_0_0;
  search_space->formats[1]            = srsran_dci_format_nr_1_0;
  search_space->nof_formats           = 2;
  for (uint32_t L = 0; L < SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR; L++) {
    search_space->nof_candidates[L] = srsran_pdcch_nr_max_candidates_coreset(coreset, L);
  }

  //  Configure RA search space
  pdcch_cfg.ra_search_space_present = true;
  pdcch_cfg.ra_search_space         = *search_space;
  pdcch_cfg.ra_search_space.type    = srsran_search_space_type_common_1;

  if (srsran_ue_dl_nr_init(&ue_dl, buffer, &ue_dl_args)) {
    ERROR("Error UE DL");
    return clean_exit(ret);
  }

  if (srsran_ue_dl_nr_set_carrier(&ue_dl, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    return clean_exit(ret);
  }

  // Read baseband from file
  if (srsran_filesource_read(&filesource, buffer[0], (int)ue_dl.fft->sf_sz) < SRSRAN_SUCCESS) {
    ERROR("Error reading baseband");
    return clean_exit(ret);
  }

  if (srsran_ue_dl_nr_set_pdcch_config(&ue_dl, &pdcch_cfg, &dci_cfg)) {
    ERROR("Error setting CORESET");
    return clean_exit(ret);
  }

  // Actual decode
  ret = work_ue_dl(&ue_dl, &slot_cfg);

  // free memory and return last value of ret
  return clean_exit(ret);
}
