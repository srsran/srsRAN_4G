/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/phy/enb/enb_dl_nr.h"
#include "srslte/phy/phch/ra_nr.h"
#include "srslte/phy/ue/ue_dl_nr.h"
#include "srslte/phy/ue/ue_dl_nr_data.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/random.h"
#include "srslte/phy/utils/vector.h"
#include <getopt.h>

static srslte_carrier_nr_t carrier = {
    0,   // cell_id
    0,   // numerology
    100, // nof_prb
    0,   // start
    1    // max_mimo_layers

};

static uint32_t              n_prb       = 0;  // Set to 0 for steering
static uint32_t              mcs         = 30; // Set to 30 for steering
static srslte_sch_cfg_nr_t   pdsch_cfg   = {};
static srslte_sch_grant_nr_t pdsch_grant = {};
static uint16_t              rnti        = 0x1234;
static uint32_t              nof_slots   = 10;

void usage(char* prog)
{
  printf("Usage: %s [pTL] \n", prog);
  printf("\t-P Number of BWP (Carrier) PRB [Default %d]\n", carrier.nof_prb);
  printf("\t-p Number of grant PRB, set to 0 for steering [Default %d]\n", n_prb);
  printf("\t-n Number of slots to simulate [Default %d]\n", nof_slots);
  printf("\t-m MCS PRB, set to >28 for steering [Default %d]\n", mcs);
  printf("\t-T Provide MCS table (64qam, 256qam, 64qamLowSE) [Default %s]\n",
         srslte_mcs_table_to_str(pdsch_cfg.sch_cfg.mcs_table));
  printf("\t-L Provide number of layers [Default %d]\n", carrier.max_mimo_layers);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "PpmnTLv")) != -1) {
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
      case 'T':
        pdsch_cfg.sch_cfg.mcs_table = srslte_mcs_table_from_str(argv[optind]);
        break;
      case 'L':
        carrier.max_mimo_layers = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        return SRSLTE_ERROR;
    }
  }

  return SRSLTE_SUCCESS;
}

int work_gnb_dl(srslte_enb_dl_nr_t*    enb_dl,
                srslte_dl_slot_cfg_t*  slot,
                srslte_search_space_t* search_space,
                srslte_dci_dl_nr_t*    dci_dl,
                srslte_dci_location_t* dci_location,
                uint8_t**              data_tx)
{
  if (srslte_enb_dl_nr_base_zero(enb_dl) < SRSLTE_SUCCESS) {
    ERROR("Error setting base to zero\n");
    return SRSLTE_ERROR;
  }

  // Put actual DCI
  if (srslte_enb_dl_nr_pdcch_put(enb_dl, slot, search_space, dci_dl, dci_location, rnti) < SRSLTE_SUCCESS) {
    ERROR("Error putting PDCCH\n");
    return SRSLTE_ERROR;
  }

  // Put PDSCH transmission
  if (srslte_enb_dl_nr_pdsch_put(enb_dl, slot, &pdsch_cfg, &pdsch_grant, data_tx) < SRSLTE_SUCCESS) {
    ERROR("Error putting PDSCH\n");
    return SRSLTE_ERROR;
  }

  srslte_enb_dl_nr_gen_signal(enb_dl);

  return SRSLTE_SUCCESS;
}

int work_ue_dl(srslte_ue_dl_nr_t*     ue_dl,
               srslte_dl_slot_cfg_t*  slot,
               srslte_search_space_t* search_space,
               srslte_pdsch_res_nr_t* pdsch_res)
{
  srslte_ue_dl_nr_estimate_fft(ue_dl, slot);

  srslte_dci_dl_nr_t dci_dl_rx     = {};
  int                nof_found_dci = srslte_ue_dl_nr_find_dl_dci(ue_dl, search_space, slot, rnti, &dci_dl_rx, 1);
  if (nof_found_dci < SRSLTE_SUCCESS) {
    ERROR("Error decoding\n");
    return SRSLTE_ERROR;
  }

  if (nof_found_dci < 1) {
    ERROR("Error DCI not found\n");
    return SRSLTE_ERROR;
  }

  if (srslte_ue_dl_nr_pdsch_get(ue_dl, slot, &pdsch_cfg, &pdsch_grant, pdsch_res) < SRSLTE_SUCCESS) {
    ERROR("Error decoding\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  int                   ret                      = SRSLTE_ERROR;
  srslte_enb_dl_nr_t    enb_dl                   = {};
  srslte_ue_dl_nr_t     ue_dl                    = {};
  srslte_pdsch_res_nr_t pdsch_res[SRSLTE_MAX_TB] = {};
  srslte_random_t       rand_gen                 = srslte_random_init(1234);
  srslte_dl_slot_cfg_t  slot                     = {};
  struct timeval        t[3]                     = {};
  uint64_t              pdsch_encode_us          = 0;
  uint64_t              pdsch_decode_us          = 0;
  uint64_t              nof_bits                 = 0;

  uint8_t* data_tx[SRSLTE_MAX_TB]        = {};
  uint8_t* data_rx[SRSLTE_MAX_CODEWORDS] = {};
  cf_t*    buffer                        = NULL;

  buffer = srslte_vec_cf_malloc(SRSLTE_SF_LEN_PRB(carrier.nof_prb));
  if (buffer == NULL) {
    ERROR("Error malloc\n");
    goto clean_exit;
  }

  srslte_ue_dl_nr_args_t ue_dl_args        = {};
  ue_dl_args.nof_rx_antennas               = 1;
  ue_dl_args.pdsch.sch.disable_simd        = false;
  ue_dl_args.pdsch.sch.decoder_use_flooded = false;
  ue_dl_args.pdsch.measure_evm             = true;
  ue_dl_args.pdcch.disable_simd            = false;
  ue_dl_args.pdcch.measure_evm             = true;
  ue_dl_args.nof_max_prb                   = carrier.nof_prb;

  srslte_enb_dl_nr_args_t enb_dl_args = {};
  enb_dl_args.nof_tx_antennas         = 1;
  enb_dl_args.pdsch.sch.disable_simd  = false;
  enb_dl_args.pdcch.disable_simd      = false;
  enb_dl_args.nof_max_prb             = carrier.nof_prb;

  // Set default PDSCH configuration
  pdsch_cfg.sch_cfg.mcs_table = srslte_mcs_table_64qam;
  if (parse_args(argc, argv) < SRSLTE_SUCCESS) {
    goto clean_exit;
  }

  // Configure CORESET
  srslte_coreset_t coreset = {};
  coreset.duration         = 2;
  for (uint32_t i = 0; i < SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    coreset.freq_resources[i] = i < carrier.nof_prb / 6;
  }

  // Configure Search Space
  srslte_search_space_t search_space = {};
  search_space.type                  = srslte_search_space_type_ue;
  for (uint32_t L = 0; L < SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR; L++) {
    search_space.nof_candidates[L] = srslte_pdcch_nr_max_candidates_coreset(&coreset, L);
  }

  if (srslte_ue_dl_nr_init(&ue_dl, &buffer, &ue_dl_args)) {
    ERROR("Error UE DL\n");
    goto clean_exit;
  }

  if (srslte_enb_dl_nr_init(&enb_dl, &buffer, &enb_dl_args)) {
    ERROR("Error UE DL\n");
    goto clean_exit;
  }

  if (srslte_ue_dl_nr_set_carrier(&ue_dl, &carrier)) {
    ERROR("Error setting SCH NR carrier\n");
    goto clean_exit;
  }

  if (srslte_ue_dl_nr_set_coreset(&ue_dl, &coreset)) {
    ERROR("Error setting CORESET\n");
    goto clean_exit;
  }

  if (srslte_enb_dl_nr_set_carrier(&enb_dl, &carrier)) {
    ERROR("Error setting SCH NR carrier\n");
    goto clean_exit;
  }

  if (srslte_enb_dl_nr_set_coreset(&enb_dl, &coreset)) {
    ERROR("Error setting CORESET\n");
    goto clean_exit;
  }

  for (uint32_t i = 0; i < 1; i++) {
    data_tx[i] = srslte_vec_u8_malloc(SRSLTE_SLOT_MAX_NOF_BITS_NR);
    data_rx[i] = srslte_vec_u8_malloc(SRSLTE_SLOT_MAX_NOF_BITS_NR);
    if (data_tx[i] == NULL || data_rx[i] == NULL) {
      ERROR("Error malloc\n");
      goto clean_exit;
    }

    pdsch_res[i].payload = data_rx[i];
  }

  srslte_softbuffer_tx_t softbuffer_tx = {};
  srslte_softbuffer_rx_t softbuffer_rx = {};

  if (srslte_softbuffer_tx_init_guru(&softbuffer_tx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer\n");
    goto clean_exit;
  }

  if (srslte_softbuffer_rx_init_guru(&softbuffer_rx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer\n");
    goto clean_exit;
  }

  // Use grant default A time resources with m=0
  if (srslte_ue_dl_nr_pdsch_time_resource_default_A(0, pdsch_cfg.dmrs_typeA.typeA_pos, &pdsch_grant) < SRSLTE_SUCCESS) {
    ERROR("Error loading default grant\n");
    goto clean_exit;
  }
  pdsch_grant.nof_layers                       = carrier.max_mimo_layers;
  pdsch_grant.dci_format                       = srslte_dci_format_nr_1_0;
  pdsch_grant.nof_dmrs_cdm_groups_without_data = 1;

  uint32_t n_prb_start = 1;
  uint32_t n_prb_end   = carrier.nof_prb + 1;
  if (n_prb > 0) {
    n_prb_start = SRSLTE_MIN(n_prb, n_prb_end - 1);
    n_prb_end   = SRSLTE_MIN(n_prb + 1, n_prb_end);
  }

  uint32_t mcs_start = 0;
  uint32_t mcs_end   = pdsch_cfg.sch_cfg.mcs_table == srslte_mcs_table_256qam ? 28 : 29;
  if (mcs < mcs_end) {
    mcs_start = SRSLTE_MIN(mcs, mcs_end - 1);
    mcs_end   = SRSLTE_MIN(mcs + 1, mcs_end);
  }

  uint64_t slot_count = 0;
  for (slot.idx = 0; slot.idx < nof_slots; slot.idx++) {
    for (n_prb = n_prb_start; n_prb < n_prb_end; n_prb++) {
      for (mcs = mcs_start; mcs < mcs_end; mcs++, slot_count++) {

        for (uint32_t n = 0; n < SRSLTE_MAX_PRB_NR; n++) {
          pdsch_grant.prb_idx[n] = (n < n_prb);
        }

        if (srslte_ra_nr_fill_tb(&pdsch_cfg, &pdsch_grant, mcs, &pdsch_grant.tb[0]) < SRSLTE_SUCCESS) {
          ERROR("Error filing tb\n");
          goto clean_exit;
        }

        for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
          // Skip TB if no allocated
          if (data_tx[tb] == NULL) {
            continue;
          }

          for (uint32_t i = 0; i < pdsch_grant.tb[tb].tbs; i++) {
            data_tx[tb][i] = (uint8_t)srslte_random_uniform_int_dist(rand_gen, 0, UINT8_MAX);
          }
          pdsch_grant.tb[tb].softbuffer.tx = &softbuffer_tx;
        }

        // Compute PDCCH candidate locations
        uint32_t L                                                          = 0;
        uint32_t ncce_candidates[SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR] = {};
        int      nof_candidates =
            srslte_pdcch_nr_locations_coreset(&coreset, &search_space, rnti, L, slot.idx, ncce_candidates);
        if (nof_candidates < SRSLTE_SUCCESS) {
          ERROR("Error getting PDCCH candidates\n");
          goto clean_exit;
        }

        // Setup DCI location
        srslte_dci_location_t dci_location = {};
        dci_location.ncce                  = ncce_candidates[0];
        dci_location.L                     = L;

        // Setup DCI
        srslte_dci_dl_nr_t dci_dl = {};

        gettimeofday(&t[1], NULL);
        if (work_gnb_dl(&enb_dl, &slot, &search_space, &dci_dl, &dci_location, data_tx) < SRSLTE_ERROR) {
          ERROR("Error running eNb DL\n");
          goto clean_exit;
        }
        gettimeofday(&t[2], NULL);
        get_time_interval(t);
        pdsch_encode_us += (size_t)(t[0].tv_sec * 1e6 + t[0].tv_usec);

        for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
          pdsch_grant.tb[tb].softbuffer.rx = &softbuffer_rx;
          srslte_softbuffer_rx_reset(pdsch_grant.tb[tb].softbuffer.rx);
        }

        gettimeofday(&t[1], NULL);
        if (work_ue_dl(&ue_dl, &slot, &search_space, pdsch_res) < SRSLTE_SUCCESS) {
          ERROR("Error running UE DL\n");
          goto clean_exit;
        }
        gettimeofday(&t[2], NULL);
        get_time_interval(t);
        pdsch_decode_us += (size_t)(t[0].tv_sec * 1e6 + t[0].tv_usec);

        if (pdsch_res->evm > 0.001f) {
          ERROR("Error PDSCH EVM is too high %f\n", pdsch_res->evm);
          goto clean_exit;
        }

        if (!pdsch_res[0].crc) {
          ERROR("Failed to match CRC; n_prb=%d; mcs=%d; TBS=%d;\n", n_prb, mcs, pdsch_grant.tb[0].tbs);
          goto clean_exit;
        }

        if (memcmp(data_tx[0], data_rx[0], pdsch_grant.tb[0].tbs / 8) != 0) {
          ERROR("Failed to match Tx/Rx data; n_prb=%d; mcs=%d; TBS=%d;\n", n_prb, mcs, pdsch_grant.tb[0].tbs);
          printf("Tx data: ");
          srslte_vec_fprint_byte(stdout, data_tx[0], pdsch_grant.tb[0].tbs / 8);
          printf("Rx data: ");
          srslte_vec_fprint_byte(stdout, data_rx[0], pdsch_grant.tb[0].tbs / 8);
          goto clean_exit;
        }

        INFO("n_prb=%d; mcs=%d; TBS=%d; EVM=%f; PASSED!\n", n_prb, mcs, pdsch_grant.tb[0].tbs, pdsch_res[0].evm);

        // Count the Tx/Rx'd number of bits
        nof_bits += pdsch_grant.tb[0].tbs;
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

  ret = SRSLTE_SUCCESS;

clean_exit:
  srslte_random_free(rand_gen);
  srslte_enb_dl_nr_free(&enb_dl);
  srslte_ue_dl_nr_free(&ue_dl);
  for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
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
  srslte_softbuffer_tx_free(&softbuffer_tx);
  srslte_softbuffer_rx_free(&softbuffer_rx);

  return ret;
}
