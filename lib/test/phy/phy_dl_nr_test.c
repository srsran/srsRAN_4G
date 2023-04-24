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

#include "srsran/phy/gnb/gnb_dl.h"
#include "srsran/phy/phch/ra_dl_nr.h"
#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/ue/ue_dl_nr.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/random.h"
#include "srsran/phy/utils/vector.h"
#include <getopt.h>

static srsran_carrier_nr_t       carrier                          = SRSRAN_DEFAULT_CARRIER_NR;
static uint32_t                  n_prb                            = 0;  // Set to 0 for steering
static uint32_t                  mcs                              = 30; // Set to 30 for steering
static srsran_sch_cfg_nr_t       pdsch_cfg                        = {};
static uint32_t                  nof_slots                        = 10;
static uint32_t                  rv_idx                           = 0;
static uint32_t                  delay_n                          = 0;    // Integer delay
static float                     cfo_hz                           = 0.0f; // CFO Hz
static srsran_dmrs_sch_type_t    dmrs_type                        = srsran_dmrs_sch_type_1;
static srsran_dmrs_sch_add_pos_t dmrs_add_pos                     = srsran_dmrs_sch_add_pos_2;
static bool                      interleaved_pdcch                = false;
static uint32_t                  nof_dmrs_cdm_groups_without_data = 1;

static void usage(char* prog)
{
  printf("Usage: %s [rRPdpmnTILDCv] \n", prog);
  printf("\t-P Number of BWP (Carrier) PRB [Default %d]\n", carrier.nof_prb);
  printf("\t-p Number of grant PRB, set to 0 for steering [Default %d]\n", n_prb);
  printf("\t-n Number of slots to simulate [Default %d]\n", nof_slots);
  printf("\t-m MCS PRB, set to >28 for steering [Default %d]\n", mcs);
  printf("\t-r Redundancy version, set to >28 for steering [Default %d]\n", mcs);
  printf("\t-d DMRS configuration [type 1-2] [add_pos 2-3] [CDM groups 1-3] [Default %d]\n", mcs);
  printf("\t-T Provide MCS table (64qam, 256qam, 64qamLowSE) [Default %s]\n",
         srsran_mcs_table_to_str(pdsch_cfg.sch_cfg.mcs_table));
  printf("\t-R Reserve RE: [rb_begin] [rb_end] [rb_stride] [sc_mask] [symbol_mask]\n");
  printf("\t-I Enable interleaved CCE-to-REG [Default %s]\n", interleaved_pdcch ? "Enabled" : "Disabled");
  printf("\t-L Provide number of layers [Default %d]\n", carrier.max_mimo_layers);
  printf("\t-D Delay signal an integer number of samples [Default %d samples]\n", delay_n);
  printf("\t-C Frequency shift (CFO) signal in Hz [Default %+.0f Hz]\n", cfo_hz);
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

static int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "rRIPdpmnTLDCv")) != -1) {
    switch (opt) {
      case 'P':
        carrier.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        n_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        nof_slots = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        mcs = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        rv_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        switch (strtol(argv[optind++], NULL, 10)) {
          case 1:
            dmrs_type = srsran_dmrs_sch_type_1;
            break;
          case 2:
            dmrs_type = srsran_dmrs_sch_type_2;
            break;
        }
        switch (strtol(argv[optind++], NULL, 10)) {
          case 0:
            dmrs_add_pos = srsran_dmrs_sch_add_pos_0;
            break;
          case 1:
            dmrs_add_pos = srsran_dmrs_sch_add_pos_1;
            break;
          case 2:
            dmrs_add_pos = srsran_dmrs_sch_add_pos_2;
            break;
          case 3:
            dmrs_add_pos = srsran_dmrs_sch_add_pos_3;
            break;
        }
        nof_dmrs_cdm_groups_without_data = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'T':
        pdsch_cfg.sch_cfg.mcs_table = srsran_mcs_table_from_str(argv[optind]);
        break;
      case 'R': {
        srsran_re_pattern_t pattern = {};
        pattern.rb_begin            = (uint32_t)strtol(argv[optind++], NULL, 10);
        pattern.rb_end              = (uint32_t)strtol(argv[optind++], NULL, 10);
        pattern.rb_stride           = (uint32_t)strtol(argv[optind++], NULL, 10);
        uint32_t sc_mask            = (uint32_t)strtol(argv[optind++], NULL, 2);
        for (uint32_t i = 0; i < SRSRAN_NRE; i++) {
          pattern.sc[i] = ((sc_mask >> (SRSRAN_NRE - 1U - i)) & 0x1) == 0x1;
        }
        uint32_t symbol_mask = (uint32_t)strtol(argv[optind], NULL, 2);
        for (uint32_t i = 0; i < SRSRAN_NSYMB_PER_SLOT_NR; i++) {
          pattern.symbol[i] = ((symbol_mask >> (SRSRAN_NSYMB_PER_SLOT_NR - 1U - i)) & 0x1) == 0x1;
        }
        if (srsran_re_pattern_merge(&pdsch_cfg.rvd_re, &pattern) < SRSRAN_ERROR) {
          ERROR("Error adding pattern");
          return SRSRAN_ERROR;
        }
      } break;
      case 'I':
        interleaved_pdcch ^= true;
        break;
      case 'L':
        carrier.max_mimo_layers = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'D':
        delay_n = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'C':
        cfo_hz = strtof(argv[optind], NULL);
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

static int work_gnb_dl(srsran_gnb_dl_t*       gnb_dl,
                       srsran_slot_cfg_t*     slot,
                       srsran_search_space_t* search_space,
                       srsran_dci_location_t* dci_location,
                       uint8_t**              data_tx)
{
  if (srsran_gnb_dl_base_zero(gnb_dl) < SRSRAN_SUCCESS) {
    ERROR("Error setting base to zero");
    return SRSRAN_ERROR;
  }

  // Hard-coded values
  srsran_dci_dl_nr_t dci_dl    = {};
  dci_dl.ctx.rnti              = pdsch_cfg.grant.rnti;
  dci_dl.ctx.rnti_type         = pdsch_cfg.grant.rnti_type;
  dci_dl.ctx.format            = srsran_dci_format_nr_1_0;
  dci_dl.ctx.location          = *dci_location;
  dci_dl.ctx.ss_type           = search_space->type;
  dci_dl.ctx.coreset_id        = 1;
  dci_dl.ctx.coreset_start_rb  = 0;
  dci_dl.freq_domain_assigment = 0;
  dci_dl.time_domain_assigment = 0;
  dci_dl.vrb_to_prb_mapping    = 0;
  dci_dl.mcs                   = mcs;
  dci_dl.rv                    = 0;

  // Put actual DCI
  if (srsran_gnb_dl_pdcch_put_dl(gnb_dl, slot, &dci_dl) < SRSRAN_SUCCESS) {
    ERROR("Error putting PDCCH");
    return SRSRAN_ERROR;
  }

  // Put PDSCH transmission
  if (srsran_gnb_dl_pdsch_put(gnb_dl, slot, &pdsch_cfg, data_tx) < SRSRAN_SUCCESS) {
    ERROR("Error putting PDSCH");
    return SRSRAN_ERROR;
  }

  srsran_gnb_dl_gen_signal(gnb_dl);

  return SRSRAN_SUCCESS;
}

static int work_ue_dl(srsran_ue_dl_nr_t* ue_dl, srsran_slot_cfg_t* slot, srsran_pdsch_res_nr_t* pdsch_res)
{
  srsran_ue_dl_nr_estimate_fft(ue_dl, slot);

  srsran_dci_dl_nr_t dci_dl_rx = {};
  int                nof_found_dci =
      srsran_ue_dl_nr_find_dl_dci(ue_dl, slot, pdsch_cfg.grant.rnti, pdsch_cfg.grant.rnti_type, &dci_dl_rx, 1);
  if (nof_found_dci < SRSRAN_SUCCESS) {
    ERROR("Error decoding");
    return SRSRAN_ERROR;
  }

  if (nof_found_dci < 1) {
    ERROR("Error DCI not found");
    return SRSRAN_ERROR;
  }

  if (srsran_ue_dl_nr_decode_pdsch(ue_dl, slot, &pdsch_cfg, pdsch_res) < SRSRAN_SUCCESS) {
    ERROR("Error decoding");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  int                   ret             = SRSRAN_ERROR;
  srsran_gnb_dl_t       gnb_dl          = {};
  srsran_ue_dl_nr_t     ue_dl           = {};
  srsran_pdsch_res_nr_t pdsch_res       = {};
  srsran_random_t       rand_gen        = srsran_random_init(1234);
  srsran_slot_cfg_t     slot            = {};
  struct timeval        t[3]            = {};
  uint64_t              pdsch_encode_us = 0;
  uint64_t              pdsch_decode_us = 0;
  uint64_t              nof_bits        = 0;

  uint8_t* data_tx[SRSRAN_MAX_TB]        = {};
  uint8_t* data_rx[SRSRAN_MAX_CODEWORDS] = {};
  cf_t*    buffer_gnb[SRSRAN_MAX_PORTS]  = {};
  cf_t*    buffer_ue[SRSRAN_MAX_PORTS]   = {};

  // Set default PDSCH configuration
  if (parse_args(argc, argv) < SRSRAN_SUCCESS) {
    goto clean_exit;
  }

  uint32_t sf_len = SRSRAN_SF_LEN_PRB_NR(carrier.nof_prb);
  buffer_gnb[0]   = srsran_vec_cf_malloc(sf_len);
  buffer_ue[0]    = srsran_vec_cf_malloc(sf_len);
  if (buffer_gnb[0] == NULL || buffer_ue[0] == NULL) {
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

  srsran_gnb_dl_args_t gnb_dl_args   = {};
  gnb_dl_args.nof_tx_antennas        = 1;
  gnb_dl_args.pdsch.sch.disable_simd = false;
  gnb_dl_args.pdcch.disable_simd     = false;
  gnb_dl_args.nof_max_prb            = carrier.nof_prb;
  gnb_dl_args.srate_hz               = SRSRAN_SUBC_SPACING_NR(carrier.scs) * srsran_min_symbol_sz_rb(carrier.nof_prb);

  srsran_pdcch_cfg_nr_t pdcch_cfg = {};

  // Configure CORESET
  srsran_coreset_t* coreset    = &pdcch_cfg.coreset[1];
  pdcch_cfg.coreset_present[1] = true;
  coreset->duration            = 1;

  uint32_t coreset_bw_rb = carrier.nof_prb;

  if (interleaved_pdcch) {
    coreset->mapping_type         = srsran_coreset_mapping_type_interleaved;
    coreset->reg_bundle_size      = srsran_coreset_bundle_size_n6;
    coreset->interleaver_size     = srsran_coreset_bundle_size_n2;
    coreset->precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;
    coreset->shift_index          = carrier.pci;
    coreset_bw_rb                 = SRSRAN_FLOOR(carrier.nof_prb, 12) * 12;
  }

  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    coreset->freq_resources[i] = i < coreset_bw_rb / 6;
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

  if (srsran_ue_dl_nr_init(&ue_dl, buffer_ue, &ue_dl_args)) {
    ERROR("Error UE DL");
    goto clean_exit;
  }

  if (srsran_gnb_dl_init(&gnb_dl, buffer_gnb, &gnb_dl_args)) {
    ERROR("Error UE DL");
    goto clean_exit;
  }

  if (srsran_ue_dl_nr_set_carrier(&ue_dl, &carrier)) {
    ERROR("Error setting SCH NR carrier");
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

  if (srsran_gnb_dl_set_carrier(&gnb_dl, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
  }

  if (srsran_gnb_dl_set_pdcch_config(&gnb_dl, &pdcch_cfg, &dci_cfg)) {
    ERROR("Error setting CORESET");
    goto clean_exit;
  }

  for (uint32_t i = 0; i < 1; i++) {
    data_tx[i] = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
    data_rx[i] = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
    if (data_tx[i] == NULL || data_rx[i] == NULL) {
      ERROR("Error malloc");
      goto clean_exit;
    }

    pdsch_res.tb[i].payload = data_rx[i];
  }

  srsran_softbuffer_tx_t softbuffer_tx = {};
  srsran_softbuffer_rx_t softbuffer_rx = {};

  if (srsran_softbuffer_tx_init_guru(&softbuffer_tx, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
      SRSRAN_SUCCESS) {
    ERROR("Error init soft-buffer");
    goto clean_exit;
  }

  if (srsran_softbuffer_rx_init_guru(&softbuffer_rx, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
      SRSRAN_SUCCESS) {
    ERROR("Error init soft-buffer");
    goto clean_exit;
  }

  // Use grant default A time resources with m=0
  pdsch_cfg.dmrs.type                              = dmrs_type;
  pdsch_cfg.dmrs.typeA_pos                         = srsran_dmrs_sch_typeA_pos_2;
  pdsch_cfg.dmrs.additional_pos                    = dmrs_add_pos;
  pdsch_cfg.grant.S                                = 1;
  pdsch_cfg.grant.L                                = 13;
  pdsch_cfg.grant.nof_layers                       = carrier.max_mimo_layers;
  pdsch_cfg.grant.dci_format                       = srsran_dci_format_nr_1_0;
  pdsch_cfg.grant.nof_dmrs_cdm_groups_without_data = nof_dmrs_cdm_groups_without_data;
  pdsch_cfg.grant.beta_dmrs                        = srsran_convert_dB_to_amplitude(3);
  pdsch_cfg.grant.rnti_type                        = srsran_rnti_type_c;
  pdsch_cfg.grant.rnti                             = 0x4601;
  pdsch_cfg.grant.tb[0].rv                         = rv_idx;

  uint32_t n_prb_start = 1;
  uint32_t n_prb_end   = carrier.nof_prb + 1;
  if (n_prb > 0) {
    n_prb_start = SRSRAN_MIN(n_prb, n_prb_end - 1);
    n_prb_end   = SRSRAN_MIN(n_prb + 1, n_prb_end);
  }

  uint32_t mcs_start = 0;
  uint32_t mcs_end   = pdsch_cfg.sch_cfg.mcs_table == srsran_mcs_table_256qam ? 28 : 29;
  if (mcs < mcs_end) {
    mcs_start = SRSRAN_MIN(mcs, mcs_end - 1);
    mcs_end   = SRSRAN_MIN(mcs + 1, mcs_end);
  }

  uint64_t slot_count = 0;
  for (slot.idx = 0; slot.idx < nof_slots; slot.idx++) {
    for (n_prb = n_prb_start; n_prb < n_prb_end; n_prb++) {
      for (mcs = mcs_start; mcs < mcs_end; mcs++, slot_count++) {
        for (uint32_t n = 0; n < SRSRAN_MAX_PRB_NR; n++) {
          pdsch_cfg.grant.prb_idx[n] = (n < n_prb);
        }
        pdsch_cfg.grant.nof_prb = n_prb;

        if (srsran_ra_nr_fill_tb(&pdsch_cfg, &pdsch_cfg.grant, mcs, &pdsch_cfg.grant.tb[0]) < SRSRAN_SUCCESS) {
          ERROR("Error filing tb");
          goto clean_exit;
        }

        for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
          // Skip TB if no allocated
          if (data_tx[tb] == NULL) {
            continue;
          }
          srsran_random_byte_vector(rand_gen, data_tx[tb], pdsch_cfg.grant.tb[tb].tbs / 8);
          pdsch_cfg.grant.tb[tb].softbuffer.tx = &softbuffer_tx;
        }

        // Compute PDCCH candidate locations
        uint32_t L                                                          = 1;
        uint32_t ncce_candidates[SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR] = {};
        int      nof_candidates                                             = srsran_pdcch_nr_locations_coreset(
            coreset, search_space, pdsch_cfg.grant.rnti, L, SRSRAN_SLOT_NR_MOD(carrier.scs, slot.idx), ncce_candidates);
        if (nof_candidates < SRSRAN_SUCCESS) {
          ERROR("Error getting PDCCH candidates");
          goto clean_exit;
        }

        // Setup DCI location
        srsran_dci_location_t dci_location = {};
        dci_location.ncce                  = ncce_candidates[0];
        dci_location.L                     = L;

        gettimeofday(&t[1], NULL);
        if (work_gnb_dl(&gnb_dl, &slot, search_space, &dci_location, data_tx) < SRSRAN_ERROR) {
          ERROR("Error running eNb DL");
          goto clean_exit;
        }
        gettimeofday(&t[2], NULL);
        get_time_interval(t);
        pdsch_encode_us += (size_t)(t[0].tv_sec * 1e6 + t[0].tv_usec);

        // Emulate channel delay
        if (delay_n >= sf_len) {
          ERROR("Delay exceeds SF length");
          goto clean_exit;
        }
        srsran_vec_cf_copy(&buffer_ue[0][0], &buffer_gnb[0][delay_n], sf_len - delay_n);
        srsran_vec_cf_copy(&buffer_ue[0][sf_len - delay_n], &buffer_gnb[0][0], delay_n);

        // Emulate channel CFO
        if (isnormal(cfo_hz) && ue_dl.fft[0].cfg.symbol_sz > 0) {
          srsran_vec_apply_cfo(buffer_ue[0],
                               cfo_hz / (ue_dl.fft[0].cfg.symbol_sz * SRSRAN_SUBC_SPACING_NR(carrier.scs)),
                               buffer_ue[0],
                               sf_len);
        }

        for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
          pdsch_cfg.grant.tb[tb].softbuffer.rx = &softbuffer_rx;
          srsran_softbuffer_rx_reset(pdsch_cfg.grant.tb[tb].softbuffer.rx);
        }

        gettimeofday(&t[1], NULL);
        if (work_ue_dl(&ue_dl, &slot, &pdsch_res) < SRSRAN_SUCCESS) {
          ERROR("Error running UE DL");
          goto clean_exit;
        }
        gettimeofday(&t[2], NULL);
        get_time_interval(t);
        pdsch_decode_us += (size_t)(t[0].tv_sec * 1e6 + t[0].tv_usec);

        if (pdsch_res.evm[0] > 0.02f) {
          ERROR("Error PDSCH EVM is too high %f", pdsch_res.evm[0]);
          goto clean_exit;
        }

        // Check CRC only for RV=0
        if (rv_idx == 0) {
          if (!pdsch_res.tb[0].crc) {
            ERROR("Failed to match CRC; n_prb=%d; mcs=%d; TBS=%d;", n_prb, mcs, pdsch_cfg.grant.tb[0].tbs);
            goto clean_exit;
          }

          if (memcmp(data_tx[0], data_rx[0], pdsch_cfg.grant.tb[0].tbs / 8) != 0) {
            ERROR("Failed to match Tx/Rx data; n_prb=%d; mcs=%d; TBS=%d;", n_prb, mcs, pdsch_cfg.grant.tb[0].tbs);
            printf("Tx data: ");
            srsran_vec_fprint_byte(stdout, data_tx[0], pdsch_cfg.grant.tb[0].tbs / 8);
            printf("Rx data: ");
            srsran_vec_fprint_byte(stdout, data_rx[0], pdsch_cfg.grant.tb[0].tbs / 8);
            goto clean_exit;
          }
        }

        if (get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO) {
          char                  str[512];
          srsran_pdsch_res_nr_t pdsch_res_vec[SRSRAN_MAX_CODEWORDS] = {};
          pdsch_res_vec[0]                                          = pdsch_res;
          srsran_ue_dl_nr_pdsch_info(&ue_dl, &pdsch_cfg, pdsch_res_vec, str, (uint32_t)sizeof(str));

          char str_extra[2048];
          srsran_sch_cfg_nr_info(&pdsch_cfg, str_extra, (uint32_t)sizeof(str_extra));
          INFO("PDSCH: %s\n%s", str, str_extra);
        }

        INFO("n_prb=%d; mcs=%d; TBS=%d; EVM=%f; PASSED!", n_prb, mcs, pdsch_cfg.grant.tb[0].tbs, pdsch_res.evm[0]);

        // Count the Tx/Rx'd number of bits
        nof_bits += pdsch_cfg.grant.tb[0].tbs;
      }
    }
  }

  printf("[Rates in Mbps] Granted  Processed\n");
  printf("           eNb:   %5.1f      %5.1f\n",
         (double)nof_bits / (double)slot_count / 1000.0f,
         (double)nof_bits / pdsch_encode_us);
  printf("            UE:   %5.1f      %5.1f\n",
         (double)nof_bits / (double)slot_count / 1000.0f,
         (double)nof_bits / pdsch_decode_us);

  ret = SRSRAN_SUCCESS;

clean_exit:
  srsran_random_free(rand_gen);
  srsran_gnb_dl_free(&gnb_dl);
  srsran_ue_dl_nr_free(&ue_dl);
  for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    if (data_tx[i]) {
      free(data_tx[i]);
    }
    if (data_rx[i]) {
      free(data_rx[i]);
    }
  }
  if (buffer_gnb[0]) {
    free(buffer_gnb[0]);
  }
  if (buffer_ue[0]) {
    free(buffer_ue[0]);
  }
  srsran_softbuffer_tx_free(&softbuffer_tx);
  srsran_softbuffer_rx_free(&softbuffer_rx);

  return ret;
}
