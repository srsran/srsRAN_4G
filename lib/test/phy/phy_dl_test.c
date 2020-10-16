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

#include <srslte/phy/utils/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "srslte/srslte.h"

#define MAX_DATABUFFER_SIZE (6144 * 16 * 3 / 8)

srslte_cell_t cell = {.nof_prb         = 100,
                      .nof_ports       = 1,
                      .id              = 1,
                      .cp              = SRSLTE_CP_NORM,
                      .phich_resources = SRSLTE_PHICH_R_1,
                      .phich_length    = SRSLTE_PHICH_NORM};

static uint32_t transmission_mode = 1;
static uint32_t cfi               = 1;
static uint32_t nof_rx_ant        = 1;
static uint32_t nof_subframes     = 0;
static uint16_t rnti              = 0x1234;
static bool     print_dci_table;
static uint32_t mcs                     = 20;
static int      cross_carrier_indicator = -1;
static bool     enable_256qam           = false;

void usage(char* prog)
{
  printf("Usage: %s [cfpndvs]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-p cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-s number of subframes to simulate [Default %d]\n", nof_subframes);
  printf("\t-d Print DCI table [Default %s]\n", print_dci_table ? "yes" : "no");
  printf("\t-t Transmission mode: 1,2,3,4 [Default %d]\n", transmission_mode + 1);
  printf("\t-m mcs [Default %d]\n", mcs);
  printf("\tAdvanced parameters:\n");
  if (cross_carrier_indicator >= 0) {
    printf("\t\t-a carrier-indicator [Default %d]\n", cross_carrier_indicator);
  } else {
    printf("\t\t-a carrier-indicator [Default none]\n");
  }
  printf("\t-v [set srslte_verbose to debug, default none]\n");
  printf("\t-q Enable/Disable 256QAM modulation (default %s)\n", enable_256qam ? "enabled" : "disabled");
}

void parse_extensive_param(char* param, char* arg)
{
  int ext_code = SRSLTE_SUCCESS;
  if (!strcmp(param, "carrier-indicator")) {
    cross_carrier_indicator = (uint32_t)strtol(arg, NULL, 10);
  } else {
    ext_code = SRSLTE_ERROR;
  }

  if (ext_code) {
    ERROR("Error parsing parameter '%s' and argument '%s'\n", param, arg);
    exit(ext_code);
  }
}

void parse_args(int argc, char** argv)
{
  int opt;

  // Load default transmission mode to avoid wrong number of ports/antennas
  if (transmission_mode == 0) {
    cell.nof_ports = 1;
    nof_rx_ant     = 1;
  } else if (transmission_mode < 4) {
    cell.nof_ports = 2;
    nof_rx_ant     = 2;
  }

  while ((opt = getopt(argc, argv, "cfapndvqstm")) != -1) {
    switch (opt) {
      case 't':
        transmission_mode = (uint32_t)strtol(argv[optind], NULL, 10) - 1;
        if (transmission_mode == 0) {
          cell.nof_ports = 1;
          nof_rx_ant     = 1;
        } else if (transmission_mode < 4) {
          cell.nof_ports = 2;
          nof_rx_ant     = 2;
        }
        break;
      case 'f':
        cfi = (uint32_t)(uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        mcs = (uint32_t)(uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        nof_subframes = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        print_dci_table = true;
        break;
      case 'a':
        parse_extensive_param(argv[optind], argv[optind + 1]);
        optind++;
        break;
      case 'v':
        srslte_verbose++;
        break;
      case 'q':
        enable_256qam = (enable_256qam) ? false : true;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int work_enb(srslte_enb_dl_t*         enb_dl,
             srslte_dl_sf_cfg_t*      dl_sf,
             srslte_dci_cfg_t*        dci_cfg,
             srslte_dci_dl_t*         dci,
             srslte_softbuffer_tx_t** softbuffer_tx,
             uint8_t**                data_tx)
{
  int ret = SRSLTE_ERROR;

  srslte_enb_dl_put_base(enb_dl, dl_sf);
  if (srslte_enb_dl_put_pdcch_dl(enb_dl, dci_cfg, dci)) {
    ERROR("Error putting PDCCH sf_idx=%d\n", dl_sf->tti);
    goto quit;
  }

  // Create pdsch config
  srslte_pdsch_cfg_t pdsch_cfg;
  if (srslte_ra_dl_dci_to_grant(&cell, dl_sf, transmission_mode, enable_256qam, dci, &pdsch_cfg.grant)) {
    ERROR("Computing DL grant sf_idx=%d\n", dl_sf->tti);
    goto quit;
  }
  char str[512];
  srslte_dci_dl_info(dci, str, 512);
  INFO("eNb PDCCH: rnti=0x%x, %s\n", rnti, str);

  for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    pdsch_cfg.softbuffers.tx[i] = softbuffer_tx[i];
  }

  // Enable power allocation
  pdsch_cfg.power_scale  = true;
  pdsch_cfg.p_a          = 0.0f;                                     // 0 dB
  pdsch_cfg.p_b          = (transmission_mode > SRSLTE_TM1) ? 1 : 0; // 0 dB
  pdsch_cfg.rnti         = rnti;
  pdsch_cfg.meas_time_en = false;

  if (srslte_enb_dl_put_pdsch(enb_dl, &pdsch_cfg, data_tx) < 0) {
    ERROR("Error putting PDSCH sf_idx=%d\n", dl_sf->tti);
    goto quit;
  }
  srslte_pdsch_tx_info(&pdsch_cfg, str, 512);
  INFO("eNb PDSCH: rnti=0x%x, %s\n", rnti, str);

  srslte_enb_dl_gen_signal(enb_dl);

  ret = SRSLTE_SUCCESS;

quit:
  return ret;
}

int work_ue(srslte_ue_dl_t*     ue_dl,
            srslte_dl_sf_cfg_t* sf_cfg_dl,
            srslte_ue_dl_cfg_t* ue_dl_cfg,
            srslte_dci_dl_t*    dci_dl,
            uint32_t            sf_idx,
            srslte_pdsch_res_t  pdsch_res[SRSLTE_MAX_CODEWORDS])
{
  if (srslte_ue_dl_decode_fft_estimate(ue_dl, sf_cfg_dl, ue_dl_cfg) < 0) {
    ERROR("Getting PDCCH FFT estimate sf_idx=%d\n", sf_idx);
    return SRSLTE_ERROR;
  }

  int nof_grants = srslte_ue_dl_find_dl_dci(ue_dl, sf_cfg_dl, ue_dl_cfg, rnti, dci_dl);
  if (nof_grants < 0) {
    ERROR("Looking for DL grants sf_idx=%d\n", sf_idx);
    return SRSLTE_ERROR;
  } else if (nof_grants == 0) {
    ERROR("Failed to find DCI in sf_idx=%d\n", sf_idx);
    return SRSLTE_ERROR;
  }

  // Enable power allocation
  ue_dl_cfg->cfg.pdsch.power_scale = true;
  ue_dl_cfg->cfg.pdsch.p_a         = 0.0f;                                     // 0 dB
  ue_dl_cfg->cfg.pdsch.p_b         = (transmission_mode > SRSLTE_TM1) ? 1 : 0; // 0 dB
  ue_dl_cfg->cfg.pdsch.rnti        = dci_dl->rnti;
  ue_dl_cfg->cfg.pdsch.csi_enable  = false;
  ue_dl_cfg->cfg.pdsch.meas_evm_en = false;

  if (srslte_verbose >= SRSLTE_VERBOSE_INFO) {
    char str[512];
    srslte_dci_dl_info(&dci_dl[0], str, 512);
    INFO("UE PDCCH: rnti=0x%x, %s\n", rnti, str);
  }

  if (srslte_ra_dl_dci_to_grant(
          &cell, sf_cfg_dl, transmission_mode, enable_256qam, &dci_dl[0], &ue_dl_cfg->cfg.pdsch.grant)) {
    ERROR("Computing DL grant sf_idx=%d\n", sf_idx);
    return SRSLTE_ERROR;
  }

  // Reset softbuffer
  for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (ue_dl_cfg->cfg.pdsch.grant.tb[i].enabled) {
      srslte_softbuffer_rx_reset(ue_dl_cfg->cfg.pdsch.softbuffers.rx[i]);
    }
  }

  if (srslte_ue_dl_decode_pdsch(ue_dl, sf_cfg_dl, &ue_dl_cfg->cfg.pdsch, pdsch_res)) {
    ERROR("ERROR: Decoding PDSCH sf_idx=%d\n", sf_idx);
    return SRSLTE_ERROR;
  }

  if (srslte_verbose >= SRSLTE_VERBOSE_INFO) {
    char str[512];
    srslte_pdsch_rx_info(&ue_dl_cfg->cfg.pdsch, pdsch_res, str, 512);
    INFO("eNb PDSCH: rnti=0x%x, %s\n", rnti, str);
  }

  return SRSLTE_SUCCESS;
}

static int
check_softbits(srslte_enb_dl_t* enb_dl, srslte_ue_dl_t* ue_dl, srslte_ue_dl_cfg_t* ue_dl_cfg, uint32_t sf_idx, int tb)
{
  int ret = SRSLTE_SUCCESS;

  // Generate sequence
  srslte_sequence_pdsch(&ue_dl->pdsch.tmp_seq,
                        rnti,
                        ue_dl_cfg->cfg.pdsch.grant.tb[tb].cw_idx,
                        2 * (sf_idx % 10),
                        cell.id,
                        ue_dl_cfg->cfg.pdsch.grant.tb[tb].nof_bits);

  // Scramble
  if (ue_dl->pdsch.llr_is_8bit) {
    srslte_scrambling_sb_offset(
        &ue_dl->pdsch.tmp_seq, ue_dl->pdsch.e[tb], 0, ue_dl_cfg->cfg.pdsch.grant.tb[tb].nof_bits);
  } else {
    srslte_scrambling_s_offset(
        &ue_dl->pdsch.tmp_seq, ue_dl->pdsch.e[tb], 0, ue_dl_cfg->cfg.pdsch.grant.tb[tb].nof_bits);
  }
  int16_t* rx       = ue_dl->pdsch.e[tb];
  uint8_t* rx_bytes = ue_dl->pdsch.e[tb];
  for (int i = 0, k = 0; i < ue_dl_cfg->cfg.pdsch.grant.tb[tb].nof_bits / 8; i++) {
    uint8_t w = 0;
    for (int j = 0; j < 8; j++, k++) {
      w |= (rx[k] > 0) ? ((uint32_t)1 << (uint32_t)(7 - j)) : 0;
    }
    rx_bytes[i] = w;
  }
  if (memcmp(ue_dl->pdsch.e[tb], enb_dl->pdsch.e[tb], ue_dl_cfg->cfg.pdsch.grant.tb[tb].nof_bits / 8) != 0) {
    ret = SRSLTE_ERROR;
  }

  return ret;
}

static int check_evm(srslte_enb_dl_t* enb_dl, srslte_ue_dl_t* ue_dl, srslte_ue_dl_cfg_t* ue_dl_cfg, int tb)
{
  int ret = SRSLTE_SUCCESS;
  srslte_vec_sub_ccc(enb_dl->pdsch.d[tb], ue_dl->pdsch.d[tb], enb_dl->pdsch.d[tb], ue_dl_cfg->cfg.pdsch.grant.nof_re);
  uint32_t evm_max_i = srslte_vec_max_abs_ci(enb_dl->pdsch.d[tb], ue_dl_cfg->cfg.pdsch.grant.nof_re);
  float    evm       = cabsf(enb_dl->pdsch.d[tb][evm_max_i]);

  if (evm > 0.1f) {
    printf("TB%d Constellation EVM (%.3f) is too high\n", tb, evm);
    ret = SRSLTE_ERROR;
  }

  return ret;
}

int main(int argc, char** argv)
{
  srslte_enb_dl_t*        enb_dl      = srslte_vec_malloc(sizeof(srslte_enb_dl_t));
  srslte_ue_dl_t*         ue_dl       = srslte_vec_malloc(sizeof(srslte_ue_dl_t));
  srslte_random_t         random      = srslte_random_init(0);
  struct timeval          t[3]        = {};
  size_t                  tx_nof_bits = 0, rx_nof_bits = 0;
  srslte_softbuffer_tx_t* softbuffer_tx[SRSLTE_MAX_TB] = {};
  srslte_softbuffer_rx_t* softbuffer_rx[SRSLTE_MAX_TB] = {};
  uint8_t*                data_tx[SRSLTE_MAX_TB]       = {};
  uint8_t*                data_rx[SRSLTE_MAX_TB]       = {};
  uint32_t                count_failures = 0, count_tbs = 0;
  size_t                  pdsch_decode_us = 0;
  size_t                  pdsch_encode_us = 0;

  int ret = -1;

  parse_args(argc, argv);

  cf_t* signal_buffer[SRSLTE_MAX_PORTS] = {NULL};

  /*
   * Allocate Memory
   */
  for (int i = 0; i < cell.nof_ports; i++) {
    signal_buffer[i] = srslte_vec_cf_malloc(SRSLTE_SF_LEN_PRB(cell.nof_prb));
    if (!signal_buffer[i]) {
      ERROR("Error allocating buffer\n");
      goto quit;
    }
  }

  for (int i = 0; i < SRSLTE_MAX_TB; i++) {
    softbuffer_tx[i] = (srslte_softbuffer_tx_t*)calloc(sizeof(srslte_softbuffer_tx_t), 1);
    if (!softbuffer_tx[i]) {
      ERROR("Error allocating softbuffer_tx\n");
      goto quit;
    }

    if (srslte_softbuffer_tx_init(softbuffer_tx[i], cell.nof_prb)) {
      ERROR("Error initiating softbuffer_tx\n");
      goto quit;
    }

    softbuffer_rx[i] = (srslte_softbuffer_rx_t*)calloc(sizeof(srslte_softbuffer_rx_t), 1);
    if (!softbuffer_rx[i]) {
      ERROR("Error allocating softbuffer_rx\n");
      goto quit;
    }

    if (srslte_softbuffer_rx_init(softbuffer_rx[i], cell.nof_prb)) {
      ERROR("Error initiating softbuffer_rx\n");
      goto quit;
    }

    data_tx[i] = srslte_vec_u8_malloc(MAX_DATABUFFER_SIZE);
    if (!data_tx[i]) {
      ERROR("Error allocating data tx\n");
      goto quit;
    }

    data_rx[i] = srslte_vec_u8_malloc(MAX_DATABUFFER_SIZE);
    if (!data_rx[i]) {
      ERROR("Error allocating data tx\n");
      goto quit;
    }
  }

  /*
   * Initialise eNb
   */
  if (srslte_enb_dl_init(enb_dl, signal_buffer, cell.nof_prb)) {
    ERROR("Error initiating eNb downlink\n");
    goto quit;
  }

  if (srslte_enb_dl_set_cell(enb_dl, cell)) {
    ERROR("Error setting eNb DL cell\n");
    goto quit;
  }

  if (srslte_enb_dl_add_rnti(enb_dl, rnti)) {
    ERROR("Error adding RNTI\n");
    goto quit;
  }

  /*
   * Initialise UE
   */
  if (srslte_ue_dl_init(ue_dl, signal_buffer, cell.nof_prb, nof_rx_ant)) {
    ERROR("Error initiating UE downlink\n");
    goto quit;
  }

  if (srslte_ue_dl_set_cell(ue_dl, cell)) {
    ERROR("Error setting UE downlink cell\n");
    goto quit;
  }

  srslte_ue_dl_set_rnti(ue_dl, rnti);

  /*
   * Create PDCCH Allocations
   */
  uint32_t              nof_locations[SRSLTE_NOF_SF_X_FRAME];
  srslte_dci_location_t dci_locations[SRSLTE_NOF_SF_X_FRAME][SRSLTE_MAX_CANDIDATES_UE];
  uint32_t              location_counter = 0;
  for (uint32_t i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
    srslte_dl_sf_cfg_t sf_cfg_dl;
    ZERO_OBJECT(sf_cfg_dl);
    sf_cfg_dl.tti     = i;
    sf_cfg_dl.cfi     = cfi;
    sf_cfg_dl.sf_type = SRSLTE_SF_NORM;

    nof_locations[i] =
        srslte_pdcch_ue_locations(&enb_dl->pdcch, &sf_cfg_dl, dci_locations[i], SRSLTE_MAX_CANDIDATES_UE, rnti);
    location_counter += nof_locations[i];
  }

  if (nof_subframes == 0) {
    nof_subframes = location_counter;
  }

  /*
   *  DCI Configuration
   */
  srslte_dci_dl_t  dci                 = {};
  srslte_dci_cfg_t dci_cfg             = {};
  dci_cfg.srs_request_enabled          = false;
  dci_cfg.ra_format_enabled            = false;
  dci_cfg.multiple_csi_request_enabled = false;
  dci_cfg.is_not_ue_ss                 = false;

  // DCI Fixed values
  dci.pid                 = 0;
  dci.pinfo               = 0;
  dci.rnti                = rnti;
  dci.is_tdd              = false;
  dci.is_dwpts            = false;
  dci.is_ra_order         = false;
  dci.tb_cw_swap          = false;
  dci.pconf               = false;
  dci.power_offset        = false;
  dci.tpc_pucch           = false;
  dci.ra_preamble         = false;
  dci.ra_mask_idx         = false;
  dci.srs_request         = false;
  dci.srs_request_present = false;

  if (cross_carrier_indicator >= 0) {
    dci.cif_present     = true;
    dci_cfg.cif_enabled = true;
    dci.cif             = (uint32_t)cross_carrier_indicator;
  } else {
    dci.cif_present     = false;
    dci_cfg.cif_enabled = false;
  }

  // Set PRB Allocation type
#if 0
  dci.alloc_type      = SRSLTE_RA_ALLOC_TYPE2;
  uint32_t n_prb      = 1; ///< Number of PRB
  uint32_t s_prb      = 0; ///< Start
  dci.type2_alloc.riv = srslte_ra_type2_to_riv(n_prb, s_prb, cell.nof_prb);
#else
  dci.alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
  dci.type0_alloc.rbg_bitmask = 0xffffffff; // All PRB
#endif

  // Set TB
  if (transmission_mode < SRSLTE_TM3) {
    dci.format        = (dci.alloc_type == SRSLTE_RA_ALLOC_TYPE2) ? SRSLTE_DCI_FORMAT1A : SRSLTE_DCI_FORMAT1;
    dci.tb[0].mcs_idx = mcs;
    dci.tb[0].rv      = 0;
    dci.tb[0].ndi     = 0;
    dci.tb[0].cw_idx  = 0;
    dci.tb[1].mcs_idx = 0;
    dci.tb[1].rv      = 1;
  } else if (transmission_mode == SRSLTE_TM3) {
    dci.format = SRSLTE_DCI_FORMAT2A;
    for (uint32_t i = 0; i < SRSLTE_MAX_TB; i++) {
      dci.tb[i].mcs_idx = mcs;
      dci.tb[i].rv      = 0;
      dci.tb[i].ndi     = 0;
      dci.tb[i].cw_idx  = i;
    }
  } else if (transmission_mode == SRSLTE_TM4) {
    dci.format = SRSLTE_DCI_FORMAT2;
    dci.pinfo  = 0;
    for (uint32_t i = 0; i < SRSLTE_MAX_TB; i++) {
      dci.tb[i].mcs_idx = mcs;
      dci.tb[i].rv      = 0;
      dci.tb[i].ndi     = 0;
      dci.tb[i].cw_idx  = i;
    }
  } else {
    ERROR("Wrong transmission mode (%d)\n", transmission_mode);
  }

  /*
   * Loop
   */
  INFO("--- Starting test ---\n");
  for (uint32_t sf_idx = 0; sf_idx < nof_subframes; sf_idx++) {
    /* Generate random data */
    for (int j = 0; j < SRSLTE_MAX_TB; j++) {
      for (int i = 0; i < MAX_DATABUFFER_SIZE; i++) {
        data_tx[j][i] = (uint8_t)srslte_random_uniform_int_dist(random, 0, 255);
      }
    }

    /*
     * Run eNodeB
     */
    srslte_dl_sf_cfg_t sf_cfg_dl = {};
    sf_cfg_dl.tti     = sf_idx % 10;
    sf_cfg_dl.cfi     = cfi;
    sf_cfg_dl.sf_type = SRSLTE_SF_NORM;

    // Set DCI Location
    dci.location = dci_locations[sf_idx % 10][(sf_idx / 10) % nof_locations[sf_idx % 10]];
    if (cell.nof_prb == 6) {
      for (int i = 0; i < SRSLTE_MAX_TB; i++) {
        dci.tb[i].mcs_idx = (sf_idx % 5 == 0) ? 0 : mcs;
      }
    } else if (cell.nof_prb == 15) {
      for (int i = 0; i < SRSLTE_MAX_TB; i++) {
        dci.tb[i].mcs_idx = (sf_idx % 5 == 0) ? SRSLTE_MIN(mcs, 27) : mcs;
      }
    }
    INFO("--- Process eNb ---\n");

    gettimeofday(&t[1], NULL);
    if (work_enb(enb_dl, &sf_cfg_dl, &dci_cfg, &dci, softbuffer_tx, data_tx)) {
      goto quit;
    }
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    pdsch_encode_us += (size_t)(t[0].tv_sec * 1e6 + t[0].tv_usec);

    // MIMO perfect crossed channel
    if (transmission_mode > 1) {
      for (int i = 0; i < SRSLTE_SF_LEN_PRB(cell.nof_prb); i++) {
        cf_t x0 = signal_buffer[0][i];
        cf_t x1 = signal_buffer[1][i];

        cf_t y0 = x0 + x1;
        cf_t y1 = x0 - x1;

        signal_buffer[0][i] = y0;
        signal_buffer[1][i] = y1;
      }
    }

    /*
     * Run UE
     */
    INFO("--- Process  UE ---\n");
    gettimeofday(&t[1], NULL);

    srslte_ue_dl_cfg_t ue_dl_cfg                  = {};
    srslte_dci_dl_t    dci_dl[SRSLTE_MAX_DCI_MSG] = {};

    ue_dl_cfg.cfg.tm                       = transmission_mode;
    ue_dl_cfg.cfg.pdsch.p_a                = 0.0;
    ue_dl_cfg.cfg.pdsch.power_scale        = false;
    ue_dl_cfg.cfg.pdsch.decoder_type       = SRSLTE_MIMO_DECODER_MMSE;
    ue_dl_cfg.cfg.pdsch.max_nof_iterations = 10;
    ue_dl_cfg.cfg.pdsch.meas_time_en       = false;

    ue_dl_cfg.chest_cfg.filter_coef[0]       = 4;
    ue_dl_cfg.chest_cfg.filter_coef[1]       = 1;
    ue_dl_cfg.chest_cfg.filter_type          = SRSLTE_CHEST_FILTER_GAUSS;
    ue_dl_cfg.chest_cfg.noise_alg            = SRSLTE_NOISE_ALG_REFS;
    ue_dl_cfg.chest_cfg.rsrp_neighbour       = false;
    ue_dl_cfg.chest_cfg.estimator_alg        = SRSLTE_ESTIMATOR_ALG_AVERAGE;
    ue_dl_cfg.chest_cfg.cfo_estimate_enable  = false;
    ue_dl_cfg.chest_cfg.cfo_estimate_sf_mask = false;
    ue_dl_cfg.chest_cfg.sync_error_enable    = false;
    ue_dl_cfg.cfg.dci                        = dci_cfg;
    ue_dl_cfg.cfg.pdsch.use_tbs_index_alt    = enable_256qam;

    srslte_pdsch_res_t pdsch_res[SRSLTE_MAX_CODEWORDS];
    for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      pdsch_res[i].payload                  = data_rx[i];
      pdsch_res[i].avg_iterations_block     = 0.0f;
      pdsch_res[i].crc                      = false;
      ue_dl_cfg.cfg.pdsch.softbuffers.rx[i] = softbuffer_rx[i];
    }
    if (work_ue(ue_dl, &sf_cfg_dl, &ue_dl_cfg, dci_dl, sf_idx, pdsch_res)) {
      goto quit;
    }

    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    pdsch_decode_us += (size_t)(t[0].tv_sec * 1e6 + t[0].tv_usec);

    for (int i = 0; i < SRSLTE_MAX_TB; i++) {
      if (ue_dl_cfg.cfg.pdsch.grant.tb[i].enabled) {
        if (check_evm(enb_dl, ue_dl, &ue_dl_cfg, i)) {
          count_failures++;
        } else if (check_softbits(enb_dl, ue_dl, &ue_dl_cfg, sf_idx, i) != SRSLTE_SUCCESS) {
          printf("TB%d: The received softbits in subframe %d DO NOT match the encoded bits (crc=%d)\n",
                 i,
                 sf_idx,
                 pdsch_res[i].crc);
          srslte_vec_fprint_byte(stdout, (uint8_t*)enb_dl->pdsch.e[i], ue_dl_cfg.cfg.pdsch.grant.tb[i].nof_bits / 8);
          srslte_vec_fprint_byte(stdout, (uint8_t*)ue_dl->pdsch.e[i], ue_dl_cfg.cfg.pdsch.grant.tb[i].nof_bits / 8);
          count_failures++;
        } else if (!pdsch_res[i].crc ||
                   memcmp(data_tx[i], data_rx[i], (uint32_t)ue_dl_cfg.cfg.pdsch.grant.tb[i].tbs / 8) != 0) {
          printf("UE Failed decoding tb %d in subframe %d. crc=%d; Bytes:\n", i, sf_idx, pdsch_res[i].crc);
          srslte_vec_fprint_byte(stdout, data_tx[i], (uint32_t)ue_dl_cfg.cfg.pdsch.grant.tb[i].tbs / 8);
          srslte_vec_fprint_byte(stdout, data_rx[i], (uint32_t)ue_dl_cfg.cfg.pdsch.grant.tb[i].tbs / 8);
          count_failures++;
        } else {
          // Decoded Ok
          rx_nof_bits += ue_dl_cfg.cfg.pdsch.grant.tb[i].tbs;
        }
        count_tbs++;
        tx_nof_bits += ue_dl_cfg.cfg.pdsch.grant.tb[i].tbs;
      }
    }
  }

  printf("Finished! The UE failed decoding %d of %d transport blocks.\n", count_failures, count_tbs);
  if (!count_failures) {
    ret = SRSLTE_SUCCESS;
  }

  printf("%zd were transmitted, %zd bits were received.\n", tx_nof_bits, rx_nof_bits);
  printf("[Rates in Mbps] Granted  Processed\n");
  printf("           eNb:   %5.1f      %5.1f\n",
         (float)tx_nof_bits / (float)nof_subframes / 1000.0f,
         (float)rx_nof_bits / pdsch_encode_us);
  printf("            UE:   %5.1f      %5.1f\n",
         (float)rx_nof_bits / (float)nof_subframes / 1000.0f,
         (float)rx_nof_bits / pdsch_decode_us);

  printf("BLER: %5.1f%%\n", (float)count_failures / (float)count_tbs * 100.0f);

quit:
  srslte_enb_dl_free(enb_dl);
  srslte_ue_dl_free(ue_dl);
  srslte_random_free(random);

  for (int i = 0; i < cell.nof_ports; i++) {
    if (signal_buffer[i]) {
      free(signal_buffer[i]);
    }
  }

  for (int i = 0; i < SRSLTE_MAX_TB; i++) {
    if (softbuffer_tx[i]) {
      srslte_softbuffer_tx_free(softbuffer_tx[i]);
      free(softbuffer_tx[i]);
    }

    if (softbuffer_rx[i]) {
      srslte_softbuffer_rx_free(softbuffer_rx[i]);
      free(softbuffer_rx[i]);
    }

    if (data_tx[i]) {
      free(data_tx[i]);
    }

    if (data_rx[i]) {
      free(data_rx[i]);
    }
  }
  if (enb_dl) {
    free(enb_dl);
  }
  if (ue_dl) {
    free(ue_dl);
  }

  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  exit(ret);
}
