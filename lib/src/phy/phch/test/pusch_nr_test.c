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

#include "srslte/phy/phch/pusch_nr.h"
#include "srslte/phy/phch/ra_nr.h"
#include "srslte/phy/phch/ra_ul_nr.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/random.h"
#include "srslte/phy/utils/vector.h"
#include <complex.h>
#include <getopt.h>

static srslte_carrier_nr_t carrier = {
    1,                 // cell_id
    0,                 // numerology
    SRSLTE_MAX_PRB_NR, // nof_prb
    0,                 // start
    1                  // max_mimo_layers
};

static uint32_t              n_prb        = 0;  // Set to 0 for steering
static uint32_t              mcs          = 30; // Set to 30 for steering
static srslte_sch_cfg_nr_t   pusch_cfg    = {};
static srslte_sch_grant_nr_t pusch_grant  = {};
static uint16_t              rnti         = 0x1234;
static uint32_t              nof_ack_bits = 0;
static uint32_t              nof_csi_bits = 0;

void usage(char* prog)
{
  printf("Usage: %s [pTL] \n", prog);
  printf("\t-p Number of grant PRB, set to 0 for steering [Default %d]\n", n_prb);
  printf("\t-m MCS PRB, set to >28 for steering [Default %d]\n", mcs);
  printf("\t-T Provide MCS table (64qam, 256qam, 64qamLowSE) [Default %s]\n",
         srslte_mcs_table_to_str(pusch_cfg.sch_cfg.mcs_table));
  printf("\t-L Provide number of layers [Default %d]\n", carrier.max_mimo_layers);
  printf("\t-A Provide a number of HARQ-ACK bits [Default %d]\n", nof_ack_bits);
  printf("\t-C Provide a number of CSI bits [Default %d]\n", nof_csi_bits);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "pmTLACv")) != -1) {
    switch (opt) {
      case 'p':
        n_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        mcs = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'T':
        pusch_cfg.sch_cfg.mcs_table = srslte_mcs_table_from_str(argv[optind]);
        break;
      case 'L':
        carrier.max_mimo_layers = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'A':
        nof_ack_bits = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'C':
        nof_csi_bits = (uint32_t)strtol(argv[optind], NULL, 10);
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

int main(int argc, char** argv)
{
  int                   ret      = SRSLTE_ERROR;
  srslte_pusch_nr_t     pusch_tx = {};
  srslte_pusch_nr_t     pusch_rx = {};
  srslte_chest_dl_res_t chest    = {};
  srslte_random_t       rand_gen = srslte_random_init(1234);

  srslte_pusch_data_nr_t data_tx[SRSLTE_MAX_TB]           = {};
  srslte_pusch_res_nr_t  data_rx[SRSLTE_MAX_CODEWORDS]    = {};
  cf_t*                  sf_symbols[SRSLTE_MAX_LAYERS_NR] = {};

  // Set default PUSCH configuration
  pusch_cfg.sch_cfg.mcs_table = srslte_mcs_table_64qam;

  if (parse_args(argc, argv) < SRSLTE_SUCCESS) {
    goto clean_exit;
  }

  srslte_pusch_nr_args_t pusch_args = {};
  pusch_args.sch.disable_simd       = false;
  pusch_args.measure_evm            = true;

  if (srslte_pusch_nr_init_ue(&pusch_tx, &pusch_args) < SRSLTE_SUCCESS) {
    ERROR("Error initiating PUSCH for Tx");
    goto clean_exit;
  }

  if (srslte_pusch_nr_init_gnb(&pusch_rx, &pusch_args) < SRSLTE_SUCCESS) {
    ERROR("Error initiating SCH NR for Rx");
    goto clean_exit;
  }

  if (srslte_pusch_nr_set_carrier(&pusch_tx, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
  }

  if (srslte_pusch_nr_set_carrier(&pusch_rx, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
  }

  for (uint32_t i = 0; i < carrier.max_mimo_layers; i++) {
    sf_symbols[i] = srslte_vec_cf_malloc(SRSLTE_SLOT_LEN_RE_NR(carrier.nof_prb));
    if (sf_symbols[i] == NULL) {
      ERROR("Error malloc");
      goto clean_exit;
    }
  }

  for (uint32_t i = 0; i < pusch_tx.max_cw; i++) {
    data_tx[i].payload = srslte_vec_u8_malloc(SRSLTE_SLOT_MAX_NOF_BITS_NR);
    data_rx[i].payload = srslte_vec_u8_malloc(SRSLTE_SLOT_MAX_NOF_BITS_NR);
    if (data_tx[i].payload == NULL || data_rx[i].payload == NULL) {
      ERROR("Error malloc");
      goto clean_exit;
    }
  }

  srslte_softbuffer_tx_t softbuffer_tx = {};
  srslte_softbuffer_rx_t softbuffer_rx = {};

  if (srslte_softbuffer_tx_init_guru(&softbuffer_tx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer");
    goto clean_exit;
  }

  if (srslte_softbuffer_rx_init_guru(&softbuffer_rx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer");
    goto clean_exit;
  }

  // Use grant default A time resources with m=0
  if (srslte_ra_ul_nr_pdsch_time_resource_default_A(carrier.numerology, 0, &pusch_grant) < SRSLTE_SUCCESS) {
    ERROR("Error loading default grant");
    goto clean_exit;
  }

  // Load number of DMRS CDM groups without data
  if (srslte_ra_ul_nr_nof_dmrs_cdm_groups_without_data_format_0_0(&pusch_cfg, &pusch_grant) < SRSLTE_SUCCESS) {
    ERROR("Error loading number of DMRS CDM groups without data");
    goto clean_exit;
  }

  pusch_grant.nof_layers = carrier.max_mimo_layers;
  pusch_grant.dci_format = srslte_dci_format_nr_1_0;
  pusch_grant.rnti       = rnti;

  uint32_t n_prb_start = 1;
  uint32_t n_prb_end   = carrier.nof_prb + 1;
  if (n_prb > 0) {
    n_prb_start = SRSLTE_MIN(n_prb, n_prb_end - 1);
    n_prb_end   = SRSLTE_MIN(n_prb + 1, n_prb_end);
  }

  uint32_t mcs_start = 0;
  uint32_t mcs_end   = pusch_cfg.sch_cfg.mcs_table == srslte_mcs_table_256qam ? 28 : 29;
  if (mcs < mcs_end) {
    mcs_start = SRSLTE_MIN(mcs, mcs_end - 1);
    mcs_end   = SRSLTE_MIN(mcs + 1, mcs_end);
  }

  if (srslte_chest_dl_res_init(&chest, carrier.nof_prb) < SRSLTE_SUCCESS) {
    ERROR("Initiating chest");
    goto clean_exit;
  }

  for (n_prb = n_prb_start; n_prb < n_prb_end; n_prb++) {
    for (mcs = mcs_start; mcs < mcs_end; mcs++) {
      for (uint32_t n = 0; n < SRSLTE_MAX_PRB_NR; n++) {
        pusch_grant.prb_idx[n] = (n < n_prb);
      }

      pusch_grant.dci_format = srslte_dci_format_nr_0_0;
      if (srslte_ra_nr_fill_tb(&pusch_cfg, &pusch_grant, mcs, &pusch_grant.tb[0]) < SRSLTE_SUCCESS) {
        ERROR("Error filing tb");
        goto clean_exit;
      }

      // Generate SCH payload
      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        // Skip TB if no allocated
        if (data_tx[tb].payload == NULL) {
          continue;
        }

        for (uint32_t i = 0; i < pusch_grant.tb[tb].tbs; i++) {
          data_tx[tb].payload[i] = (uint8_t)srslte_random_uniform_int_dist(rand_gen, 0, UINT8_MAX);
        }
        pusch_grant.tb[tb].softbuffer.tx = &softbuffer_tx;
      }

      // Generate HARQ ACK bits
      if (nof_ack_bits > 0) {
        pusch_cfg.uci.o_ack = nof_ack_bits;
        for (uint32_t i = 0; i < nof_ack_bits; i++) {
          data_tx->uci.ack[i] = (uint8_t)srslte_random_uniform_int_dist(rand_gen, 0, 1);
        }
      }

      // Generate CSI report bits
      uint8_t csi_report[SRSLTE_UCI_NR_MAX_CSI1_BITS];
      if (nof_csi_bits > 0) {
        pusch_cfg.uci.csi[0].quantity = SRSLTE_CSI_REPORT_QUANTITY_NONE;
        pusch_cfg.uci.csi[0].K_csi_rs = nof_csi_bits;
        pusch_cfg.uci.nof_csi         = 1;
        data_tx->uci.csi[0].none      = csi_report;
        for (uint32_t i = 0; i < nof_csi_bits; i++) {
          csi_report[i] = (uint8_t)srslte_random_uniform_int_dist(rand_gen, 0, 1);
        }
      }

      if (srslte_pusch_nr_encode(&pusch_tx, &pusch_cfg, &pusch_grant, data_tx, sf_symbols) < SRSLTE_SUCCESS) {
        ERROR("Error encoding");
        goto clean_exit;
      }

      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        pusch_grant.tb[tb].softbuffer.rx = &softbuffer_rx;
        srslte_softbuffer_rx_reset(pusch_grant.tb[tb].softbuffer.rx);
      }

      for (uint32_t i = 0; i < pusch_grant.tb->nof_re; i++) {
        chest.ce[0][0][i] = 1.0f;
      }
      chest.nof_re = pusch_grant.tb->nof_re;

      if (srslte_pusch_nr_decode(&pusch_rx, &pusch_cfg, &pusch_grant, &chest, sf_symbols, data_rx) < SRSLTE_SUCCESS) {
        ERROR("Error encoding");
        goto clean_exit;
      }

      if (data_rx[0].evm > 0.001f) {
        ERROR("Error PUSCH EVM is too high %f", data_rx[0].evm);
        goto clean_exit;
      }

      float    mse    = 0.0f;
      uint32_t nof_re = srslte_ra_dl_nr_slot_nof_re(&pusch_cfg, &pusch_grant);
      for (uint32_t i = 0; i < pusch_grant.nof_layers; i++) {
        for (uint32_t j = 0; j < nof_re; j++) {
          mse += cabsf(pusch_tx.d[i][j] - pusch_rx.d[i][j]);
        }
      }
      if (nof_re * pusch_grant.nof_layers > 0) {
        mse = mse / (nof_re * pusch_grant.nof_layers);
      }
      if (mse > 0.001) {
        ERROR("MSE error (%f) is too high", mse);
        for (uint32_t i = 0; i < pusch_grant.nof_layers; i++) {
          printf("d_tx[%d]=", i);
          srslte_vec_fprint_c(stdout, pusch_tx.d[i], nof_re);
          printf("d_rx[%d]=", i);
          srslte_vec_fprint_c(stdout, pusch_rx.d[i], nof_re);
        }
        goto clean_exit;
      }

      if (!data_rx[0].crc) {
        ERROR("Failed to match CRC; n_prb=%d; mcs=%d; TBS=%d;", n_prb, mcs, pusch_grant.tb[0].tbs);
        goto clean_exit;
      }

      if (memcmp(data_tx[0].payload, data_rx[0].payload, pusch_grant.tb[0].tbs / 8) != 0) {
        ERROR("Failed to match Tx/Rx data; n_prb=%d; mcs=%d; TBS=%d;", n_prb, mcs, pusch_grant.tb[0].tbs);
        printf("Tx data: ");
        srslte_vec_fprint_byte(stdout, data_tx[0].payload, pusch_grant.tb[0].tbs / 8);
        printf("Rx data: ");
        srslte_vec_fprint_byte(stdout, data_tx[0].payload, pusch_grant.tb[0].tbs / 8);
        goto clean_exit;
      }

      printf("n_prb=%d; mcs=%d; TBS=%d; EVM=%f; PASSED!\n", n_prb, mcs, pusch_grant.tb[0].tbs, data_rx[0].evm);
    }
  }

  ret = SRSLTE_SUCCESS;

clean_exit:
  srslte_chest_dl_res_free(&chest);
  srslte_random_free(rand_gen);
  srslte_pusch_nr_free(&pusch_tx);
  srslte_pusch_nr_free(&pusch_rx);
  for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (data_tx[i].payload) {
      free(data_tx[i].payload);
    }
    if (data_rx[i].payload) {
      free(data_rx[i].payload);
    }
  }
  for (uint32_t i = 0; i < SRSLTE_MAX_LAYERS_NR; i++) {
    if (sf_symbols[i]) {
      free(sf_symbols[i]);
    }
  }
  srslte_softbuffer_tx_free(&softbuffer_tx);
  srslte_softbuffer_rx_free(&softbuffer_rx);

  return ret;
}
