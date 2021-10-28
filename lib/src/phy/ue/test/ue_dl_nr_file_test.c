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

#include "srsran/phy/io/filesource.h"
#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/ue/ue_dl_nr.h"
#include "srsran/phy/utils/debug.h"
#include <getopt.h>

static srsran_carrier_nr_t carrier = SRSRAN_DEFAULT_CARRIER_NR;

static char*                  filename     = NULL;
static srsran_pdcch_cfg_nr_t  pdcch_cfg    = {};
static srsran_sch_hl_cfg_nr_t pdsch_hl_cfg = {};
static uint16_t               rnti         = 0x1234;
static srsran_rnti_type_t     rnti_type    = srsran_rnti_type_c;
static srsran_slot_cfg_t      slot_cfg     = {};

static srsran_softbuffer_rx_t softbuffer = {};
static uint8_t*               data       = NULL;

static void usage(char* prog)
{
  printf("Usage: %s [pTLR] \n", prog);
  printf("\t-f File name [Default none]\n");
  printf("\t-P Number of BWP (Carrier) PRB [Default %d]\n", carrier.nof_prb);
  printf("\t-i Physical cell identifier [Default %d]\n", carrier.pci);
  printf("\t-n Slot index [Default %d]\n", slot_cfg.idx);
  printf("\t-R RNTI in hexadecimal [Default 0x%x]\n", rnti);
  printf("\t-T RNTI type (c, ra) [Default %s]\n", srsran_rnti_type_str(rnti_type));
  printf("\t-S Use standard rates [Default %s]\n", srsran_symbol_size_is_standard() ? "yes" : "no");

  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

static int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "fPivnSRT")) != -1) {
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
      case 'T':
        if (strcmp(argv[optind], "c") == 0) {
          rnti_type = srsran_rnti_type_c;
        } else if (strcmp(argv[optind], "ra") == 0) {
          rnti_type = srsran_rnti_type_ra;
        } else {
          printf("Invalid RNTI type '%s'\n", argv[optind]);
          usage(argv[0]);
          return SRSRAN_ERROR;
        }
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
    return SRSRAN_SUCCESS;
  }

  char str[1024] = {};
  srsran_dci_dl_nr_to_str(&ue_dl->dci, &dci_dl_rx, str, (uint32_t)sizeof(str));
  printf("Found DCI: %s\n", str);

  // Convert DCI to PDSCH transmission
  srsran_sch_cfg_nr_t pdsch_cfg = {};
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

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  int               ret                      = SRSRAN_ERROR;
  srsran_ue_dl_nr_t ue_dl                    = {};
  cf_t*             buffer[SRSRAN_MAX_PORTS] = {};

  uint32_t sf_len = SRSRAN_SF_LEN_PRB(carrier.nof_prb);
  buffer[0]       = srsran_vec_cf_malloc(sf_len);
  if (buffer[0] == NULL) {
    ERROR("Error malloc");
    goto clean_exit;
  }

  if (srsran_softbuffer_rx_init_guru(&softbuffer, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
      SRSRAN_SUCCESS) {
    ERROR("Error init soft-buffer");
    goto clean_exit;
  }

  data = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
  if (data == NULL) {
    ERROR("Error malloc");
    goto clean_exit;
  }

  srsran_ue_dl_nr_args_t ue_dl_args        = {};
  ue_dl_args.nof_rx_antennas               = 1;
  ue_dl_args.pdsch.sch.disable_simd        = false;
  ue_dl_args.pdsch.sch.decoder_use_flooded = false;
  ue_dl_args.pdsch.measure_evm             = true;
  ue_dl_args.pdcch.disable_simd            = false;
  ue_dl_args.pdcch.measure_evm             = true;
  ue_dl_args.nof_max_prb                   = carrier.nof_prb;

  // Set default PDSCH configuration
  if (parse_args(argc, argv) < SRSRAN_SUCCESS) {
    goto clean_exit;
  }

  // Check for filename
  if (filename == NULL) {
    ERROR("Filename was not provided");
    goto clean_exit;
  }

  // Open filesource
  srsran_filesource_t filesource = {};
  if (srsran_filesource_init(&filesource, filename, SRSRAN_COMPLEX_FLOAT_BIN) < SRSRAN_SUCCESS) {
    ERROR("Error opening filesource");
    goto clean_exit;
  }

  // Configure CORESET
  srsran_coreset_t* coreset    = &pdcch_cfg.coreset[1];
  pdcch_cfg.coreset_present[1] = true;
  coreset->duration            = 2;
  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    coreset->freq_resources[i] = i < carrier.nof_prb / 6;
  }

  // Configure Search Space
  srsran_search_space_t* search_space = &pdcch_cfg.search_space[0];
  pdcch_cfg.search_space_present[0]   = true;
  search_space->id                    = 0;
  search_space->coreset_id            = 1;
  search_space->type                  = srsran_search_space_type_common_3;
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
    goto clean_exit;
  }

  if (srsran_ue_dl_nr_set_carrier(&ue_dl, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
  }

  // Read baseband from file
  if (srsran_filesource_read(&filesource, buffer[0], (int)ue_dl.fft->sf_sz) < SRSRAN_SUCCESS) {
    ERROR("Error reading baseband");
    goto clean_exit;
  }

  srsran_dci_cfg_nr_t dci_cfg = {};
  dci_cfg.bwp_dl_initial_bw   = carrier.nof_prb;
  dci_cfg.bwp_ul_initial_bw   = carrier.nof_prb;
  dci_cfg.monitor_common_0_0  = true;
  if (srsran_ue_dl_nr_set_pdcch_config(&ue_dl, &pdcch_cfg, &dci_cfg)) {
    ERROR("Error setting CORESET");
    goto clean_exit;
  }

  // Actual decode
  work_ue_dl(&ue_dl, &slot_cfg);

  ret = SRSRAN_SUCCESS;

clean_exit:
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
