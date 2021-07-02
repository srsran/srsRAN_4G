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

#include "srsran/phy/enb/enb_dl_nr.h"
#include "srsran/phy/phch/ra_dl_nr.h"
#include "srsran/phy/ue/ue_dl_nr.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/random.h"
#include "srsran/phy/utils/vector.h"
#include <getopt.h>
#include <srsran/phy/io/filesource.h>

static srsran_carrier_nr_t carrier = {
    501,                             // pci
    0,                               // absolute_frequency_ssb
    0,                               // absolute_frequency_point_a
    0,                               // offset_to_carrier
    srsran_subcarrier_spacing_15kHz, // scs
    52,                              // nof_prb
    0,                               // start
    1                                // max_mimo_layers
};

static char*                 filename  = NULL;
static srsran_pdcch_cfg_nr_t pdcch_cfg = {};
static uint16_t              rnti      = 0x1234;
static srsran_rnti_type_t    rnti_type = srsran_rnti_type_c;
static srsran_slot_cfg_t     slot_cfg  = {};

static void usage(char* prog)
{
  printf("Usage: %s [pTLR] \n", prog);
  printf("\t-f File name [Default none]\n");
  printf("\t-p Number of BWP (Carrier) PRB [Default %d]\n", carrier.nof_prb);
  printf("\t-i Physical cell identifier [Default %d]\n", carrier.pci);
  printf("\t-n Slot index [Default %d]\n", slot_cfg.idx);
  printf("\t-R RNTI in hexadecimal [Default 0x%x]\n", rnti);
  printf("\t-S Use standard rates [Default %s]\n", srsran_symbol_size_is_standard() ? "yes" : "no");
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

static int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "fPivnSR")) != -1) {
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
        srsran_verbose++;
        break;
      case 'n':
        slot_cfg.idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'R':
        rnti = (uint16_t)strtol(argv[optind], NULL, 16);
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
    INFO("PDCCH: rnti=0x%x, crst_id=%d, ss_type=%d, ncce=%d, al=%d, EPRE=%+.2f, RSRP=%+.2f, corr=%.3f; "
         "nof_bits=%d; crc=%s;",
         info->dci_ctx.rnti,
         info->dci_ctx.coreset_id,
         info->dci_ctx.ss_type,
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

  char str[512] = {};
  srsran_dci_dl_nr_to_str(&ue_dl->dci, &dci_dl_rx, str, (uint32_t)sizeof(str));
  printf("Found DCI: %s\n", str);

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
  srsran_ue_dl_nr_free(&ue_dl);
  srsran_filesource_free(&filesource);

  return ret;
}
