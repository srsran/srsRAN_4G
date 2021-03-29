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
#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/ue/ue_dl_nr.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/random.h"
#include "srsran/phy/utils/vector.h"
#include <getopt.h>

static srsran_carrier_nr_t carrier = {
    501, // cell_id
    0,   // numerology
    52,  // nof_prb
    0,   // start
    1    // max_mimo_layers

};

static uint32_t            n_prb     = 0;  // Set to 0 for steering
static uint32_t            mcs       = 30; // Set to 30 for steering
static srsran_sch_cfg_nr_t pdsch_cfg = {};
static uint32_t            nof_slots = 10;
static uint32_t            rv_idx    = 0;

static void usage(char* prog)
{
  printf("Usage: %s [pTL] \n", prog);
  printf("\t-P Number of BWP (Carrier) PRB [Default %d]\n", carrier.nof_prb);
  printf("\t-p Number of grant PRB, set to 0 for steering [Default %d]\n", n_prb);
  printf("\t-n Number of slots to simulate [Default %d]\n", nof_slots);
  printf("\t-m MCS PRB, set to >28 for steering [Default %d]\n", mcs);
  printf("\t-r Redundancy version, set to >28 for steering [Default %d]\n", mcs);
  printf("\t-T Provide MCS table (64qam, 256qam, 64qamLowSE) [Default %s]\n",
         srsran_mcs_table_to_str(pdsch_cfg.sch_cfg.mcs_table));
  printf("\t-R Reserve RE: [rb_begin] [rb_end] [rb_stride] [sc_mask] [symbol_mask]\n");
  printf("\t-L Provide number of layers [Default %d]\n", carrier.max_mimo_layers);
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

static int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "rRPpmnTLv")) != -1) {
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
      case 'L':
        carrier.max_mimo_layers = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srsran_verbose++;
        break;
      default:
        usage(argv[0]);
        return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

static int work_gnb_dl(srsran_enb_dl_nr_t*    enb_dl,
                       srsran_slot_cfg_t*     slot,
                       srsran_search_space_t* search_space,
                       srsran_dci_location_t* dci_location,
                       uint8_t**              data_tx)
{
  if (srsran_enb_dl_nr_base_zero(enb_dl) < SRSRAN_SUCCESS) {
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
  dci_dl.freq_domain_assigment = 0;
  dci_dl.time_domain_assigment = 0;
  dci_dl.vrb_to_prb_mapping    = 0;
  dci_dl.mcs                   = mcs;
  dci_dl.rv                    = 0;

  // Put actual DCI
  if (srsran_enb_dl_nr_pdcch_put(enb_dl, slot, &dci_dl) < SRSRAN_SUCCESS) {
    ERROR("Error putting PDCCH");
    return SRSRAN_ERROR;
  }

  // Put PDSCH transmission
  if (srsran_enb_dl_nr_pdsch_put(enb_dl, slot, &pdsch_cfg, data_tx) < SRSRAN_SUCCESS) {
    ERROR("Error putting PDSCH");
    return SRSRAN_ERROR;
  }

  srsran_enb_dl_nr_gen_signal(enb_dl);

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
  int                   ret                      = SRSRAN_ERROR;
  srsran_enb_dl_nr_t    enb_dl                   = {};
  srsran_ue_dl_nr_t     ue_dl                    = {};
  srsran_pdsch_res_nr_t pdsch_res[SRSRAN_MAX_TB] = {};
  srsran_random_t       rand_gen                 = srsran_random_init(1234);
  srsran_slot_cfg_t     slot                     = {};
  struct timeval        t[3]                     = {};
  uint64_t              pdsch_encode_us          = 0;
  uint64_t              pdsch_decode_us          = 0;
  uint64_t              nof_bits                 = 0;

  uint8_t* data_tx[SRSRAN_MAX_TB]        = {};
  uint8_t* data_rx[SRSRAN_MAX_CODEWORDS] = {};
  cf_t*    buffer                        = NULL;

  buffer = srsran_vec_cf_malloc(SRSRAN_SF_LEN_PRB(carrier.nof_prb));
  if (buffer == NULL) {
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

  srsran_enb_dl_nr_args_t enb_dl_args = {};
  enb_dl_args.nof_tx_antennas         = 1;
  enb_dl_args.pdsch.sch.disable_simd  = false;
  enb_dl_args.pdcch.disable_simd      = false;
  enb_dl_args.nof_max_prb             = carrier.nof_prb;

  // Set default PDSCH configuration
  pdsch_cfg.sch_cfg.mcs_table = srsran_mcs_table_64qam;
  if (parse_args(argc, argv) < SRSRAN_SUCCESS) {
    goto clean_exit;
  }

  srsran_pdcch_cfg_nr_t pdcch_cfg = {};

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

  if (srsran_ue_dl_nr_init(&ue_dl, &buffer, &ue_dl_args)) {
    ERROR("Error UE DL");
    goto clean_exit;
  }

  if (srsran_enb_dl_nr_init(&enb_dl, &buffer, &enb_dl_args)) {
    ERROR("Error UE DL");
    goto clean_exit;
  }

  if (srsran_ue_dl_nr_set_carrier(&ue_dl, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
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

  if (srsran_enb_dl_nr_set_carrier(&enb_dl, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
  }

  if (srsran_enb_dl_nr_set_pdcch_config(&enb_dl, &pdcch_cfg, &dci_cfg)) {
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

    pdsch_res[i].payload = data_rx[i];
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
  pdsch_cfg.dmrs.typeA_pos = srsran_dmrs_sch_typeA_pos_2;
  if (srsran_ra_dl_nr_time_default_A(0, pdsch_cfg.dmrs.typeA_pos, &pdsch_cfg.grant) < SRSRAN_SUCCESS) {
    ERROR("Error loading default grant");
    goto clean_exit;
  }
  pdsch_cfg.grant.nof_layers                       = carrier.max_mimo_layers;
  pdsch_cfg.grant.dci_format                       = srsran_dci_format_nr_1_0;
  pdsch_cfg.grant.nof_dmrs_cdm_groups_without_data = 1;
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

          for (uint32_t i = 0; i < pdsch_cfg.grant.tb[tb].tbs; i++) {
            data_tx[tb][i] = (uint8_t)srsran_random_uniform_int_dist(rand_gen, 0, UINT8_MAX);
          }
          pdsch_cfg.grant.tb[tb].softbuffer.tx = &softbuffer_tx;
        }

        // Compute PDCCH candidate locations
        uint32_t L                                                          = 1;
        uint32_t ncce_candidates[SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR] = {};
        int      nof_candidates                                             = srsran_pdcch_nr_locations_coreset(coreset,
                                                               search_space,
                                                               pdsch_cfg.grant.rnti,
                                                               L,
                                                               SRSRAN_SLOT_NR_MOD(carrier.numerology, slot.idx),
                                                               ncce_candidates);
        if (nof_candidates < SRSRAN_SUCCESS) {
          ERROR("Error getting PDCCH candidates");
          goto clean_exit;
        }

        srsran_vec_fprint_i(stdout, (int*)ncce_candidates, nof_candidates);

        // Setup DCI location
        srsran_dci_location_t dci_location = {};
        dci_location.ncce                  = ncce_candidates[0];
        dci_location.L                     = L;

        gettimeofday(&t[1], NULL);
        if (work_gnb_dl(&enb_dl, &slot, search_space, &dci_location, data_tx) < SRSRAN_ERROR) {
          ERROR("Error running eNb DL");
          goto clean_exit;
        }
        gettimeofday(&t[2], NULL);
        get_time_interval(t);
        pdsch_encode_us += (size_t)(t[0].tv_sec * 1e6 + t[0].tv_usec);

        for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
          pdsch_cfg.grant.tb[tb].softbuffer.rx = &softbuffer_rx;
          srsran_softbuffer_rx_reset(pdsch_cfg.grant.tb[tb].softbuffer.rx);
        }

        gettimeofday(&t[1], NULL);
        if (work_ue_dl(&ue_dl, &slot, pdsch_res) < SRSRAN_SUCCESS) {
          ERROR("Error running UE DL");
          goto clean_exit;
        }
        gettimeofday(&t[2], NULL);
        get_time_interval(t);
        pdsch_decode_us += (size_t)(t[0].tv_sec * 1e6 + t[0].tv_usec);

        if (pdsch_res->evm > 0.001f) {
          ERROR("Error PDSCH EVM is too high %f", pdsch_res->evm);
          goto clean_exit;
        }

        // Check CRC only for RV=0
        if (rv_idx == 0) {
          if (!pdsch_res[0].crc) {
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

        INFO("n_prb=%d; mcs=%d; TBS=%d; EVM=%f; PASSED!", n_prb, mcs, pdsch_cfg.grant.tb[0].tbs, pdsch_res[0].evm);

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
  srsran_enb_dl_nr_free(&enb_dl);
  srsran_ue_dl_nr_free(&ue_dl);
  for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    if (data_tx[i]) {
      free(data_tx[i]);
    }
    if (data_rx[i]) {
      free(data_rx[i]);
    }
  }
  if (buffer) {
    free(buffer);
  }
  srsran_softbuffer_tx_free(&softbuffer_tx);
  srsran_softbuffer_rx_free(&softbuffer_rx);

  return ret;
}
