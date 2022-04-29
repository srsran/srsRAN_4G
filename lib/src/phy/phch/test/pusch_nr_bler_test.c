/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

/**
 * \file pusch_nr_bler_test.c
 * \brief BLER and throughput test for PUSCH NR.
 *
 * This program simulates several PUSCH transmissions in order to estimate its performance in terms of the receiver BLER
 * and throughput (expressed as a percentage of the transmitted one). Specifically, the simulation runs until 100
 * transmissions fail (or after 2,000,000 transmitted transport blocks). Failures are detected by CRC verification.
 *
 * The simulation setup can be controlled by means of the following arguments.
 *  - <tt>-p num</tt>: sets the number of granted PUSCH PRBs to \c num.
 *  - <tt>-T tab</tt>: sets the modulation and coding scheme table (valid options: \c 64qam, \c 256qam, \c 64qamLowSE).
 *  - <tt>-m mcs</tt>: sets the modulation and coding scheme index to \c mcs.
 *  - <tt>-L num</tt>: sets the number of transmission layers to \c num.
 *  - <tt>-A num</tt>: sets the number of HARQ-ACK bits to \c num.
 *  - <tt>-C num</tt>: sets the number of CSI bits to \c num.
 *  - <tt>-N num</tt>: sets the maximum number of simulated transport blocks to \c num.
 *  - <tt>-s val</tt>: sets the nominal SNR to \c val (in dB).
 *  - <tt>-f </tt>: activates full BLER simulations (Tx--Rx comparison as opposed to CRC-verification only).
 *  - <tt>-v </tt>: activates verbose output.
 *
 * Example:
 * \code{.cpp}
 * pusch_nr_bler_test -p 52 -m 2 -T 64qam -s -1.8 -f
 * \endcode
 *
 */

#include "srsran/phy/channel/ch_awgn.h"
#include "srsran/phy/phch/pusch_nr.h"
#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/phch/ra_ul_nr.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/random.h"
#include "srsran/phy/utils/vector.h"
#include <getopt.h>

static srsran_carrier_nr_t carrier      = SRSRAN_DEFAULT_CARRIER_NR;
static uint32_t            n_prb        = 0;
static uint32_t            mcs          = 30;
static srsran_sch_cfg_nr_t pusch_cfg    = {};
static uint16_t            rnti         = 0x1234;
static uint32_t            nof_ack_bits = 0;
static uint32_t            nof_csi_bits = 0;
static uint32_t            max_blocks   = 2e6; // max number of simulated transport blocks
static float               snr          = 10;
static bool                full_check   = false;

void usage(char* prog)
{
  printf("Usage: %s [pmTLACNsfv] \n", prog);
  printf("\t-p Number of grant PRB [Default %d]\n", n_prb);
  printf("\t-m MCS PRB [Default %d]\n", mcs);
  printf("\t-T Provide MCS table (64qam, 256qam, 64qamLowSE) [Default %s]\n",
         srsran_mcs_table_to_str(pusch_cfg.sch_cfg.mcs_table));
  printf("\t-L Provide number of layers [Default %d]\n", carrier.max_mimo_layers);
  printf("\t-A Provide a number of HARQ-ACK bits [Default %d]\n", nof_ack_bits);
  printf("\t-C Provide a number of CSI bits [Default %d]\n", nof_csi_bits);
  printf("\t-N Maximum number of simulated transport blocks [Default %d]\n", max_blocks);
  printf("\t-s Signal-to-Noise Ratio in dB [Default %.1f]\n", snr);
  printf("\t-f Perform full BLER check instead of CRC only [Default %s]\n", full_check ? "true" : "false");
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

int parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "p:m:T:L:A:C:N:s:fv")) != -1) {
    switch (opt) {
      case 'p':
        n_prb = (uint32_t)strtol(optarg, NULL, 10);
        break;
      case 'm':
        mcs = (uint32_t)strtol(optarg, NULL, 10);
        break;
      case 'T':
        pusch_cfg.sch_cfg.mcs_table = srsran_mcs_table_from_str(optarg);
        break;
      case 'L':
        carrier.max_mimo_layers = (uint32_t)strtol(optarg, NULL, 10);
        break;
      case 'A':
        nof_ack_bits = (uint32_t)strtol(optarg, NULL, 10);
        break;
      case 'C':
        nof_csi_bits = (uint32_t)strtol(optarg, NULL, 10);
        break;
      case 'N':
        max_blocks = (uint32_t)strtol(optarg, NULL, 10);
        break;
      case 's':
        snr = strtof(optarg, NULL);
        break;
      case 'f':
        full_check = true;
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

int main(int argc, char** argv)
{
  int                   ret      = SRSRAN_ERROR;
  srsran_pusch_nr_t     pusch_tx = {};
  srsran_pusch_nr_t     pusch_rx = {};
  srsran_chest_dl_res_t chest    = {};
  srsran_random_t       rand_gen = srsran_random_init(1234);

  srsran_pusch_data_nr_t data_tx                             = {};
  srsran_pusch_res_nr_t  data_rx                             = {};
  cf_t*                  sf_symbols_tx[SRSRAN_MAX_LAYERS_NR] = {};
  cf_t*                  sf_symbols_rx[SRSRAN_MAX_LAYERS_NR] = {};

  // Set default PUSCH configuration
  pusch_cfg.sch_cfg.mcs_table = srsran_mcs_table_64qam;

  if (parse_args(argc, argv) < SRSRAN_SUCCESS) {
    goto clean_exit;
  }

  srsran_pusch_nr_args_t pusch_args = {};
  pusch_args.sch.disable_simd       = false;
  pusch_args.measure_evm            = true;

  if (srsran_pusch_nr_init_ue(&pusch_tx, &pusch_args) < SRSRAN_SUCCESS) {
    ERROR("Error initiating PUSCH for Tx");
    goto clean_exit;
  }

  if (srsran_pusch_nr_init_gnb(&pusch_rx, &pusch_args) < SRSRAN_SUCCESS) {
    ERROR("Error initiating SCH NR for Rx");
    goto clean_exit;
  }

  if (srsran_pusch_nr_set_carrier(&pusch_tx, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
  }

  if (srsran_pusch_nr_set_carrier(&pusch_rx, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
  }

  uint32_t slot_length = SRSRAN_SLOT_LEN_RE_NR(carrier.nof_prb);
  for (uint32_t i = 0; i < carrier.max_mimo_layers; i++) {
    sf_symbols_tx[i] = srsran_vec_cf_malloc(slot_length);
    sf_symbols_rx[i] = srsran_vec_cf_malloc(slot_length);
    if (sf_symbols_tx[i] == NULL || sf_symbols_rx[i] == NULL) {
      ERROR("Error malloc");
      goto clean_exit;
    }
  }

  for (uint32_t i = 0; i < pusch_tx.max_cw; i++) {
    data_tx.payload[i]    = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
    data_rx.tb[i].payload = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
    if (data_tx.payload[i] == NULL || data_rx.tb[i].payload == NULL) {
      ERROR("Error malloc");
      goto clean_exit;
    }
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
  if (srsran_ra_ul_nr_pusch_time_resource_default_A(carrier.scs, 0, &pusch_cfg.grant) < SRSRAN_SUCCESS) {
    ERROR("Error loading default grant");
    goto clean_exit;
  }

  // Set PUSCH grant without considering any procedure
  pusch_cfg.grant.nof_dmrs_cdm_groups_without_data = 1; // No need for MIMO
  pusch_cfg.grant.nof_layers                       = carrier.max_mimo_layers;
  pusch_cfg.grant.dci_format                       = srsran_dci_format_nr_1_0;
  pusch_cfg.grant.rnti                             = rnti;

  // Check input: PRB
  if (n_prb > carrier.nof_prb) {
    ERROR("Invalid number of PRB");
    goto clean_exit;
  }

  // Check input: MCS
  uint32_t mcs_end = pusch_cfg.sch_cfg.mcs_table == srsran_mcs_table_256qam ? 28 : 29;
  if (mcs > mcs_end) {
    ERROR("Invalid MCS");
    goto clean_exit;
  }

  srsran_sch_hl_cfg_nr_t sch_hl_cfg = {};
  sch_hl_cfg.scaling                = 1.0F;
  sch_hl_cfg.beta_offsets.fix_ack   = 12.625F;
  sch_hl_cfg.beta_offsets.fix_csi1  = 2.25F;
  sch_hl_cfg.beta_offsets.fix_csi2  = 2.25F;

  if (srsran_chest_dl_res_init(&chest, carrier.nof_prb) < SRSRAN_SUCCESS) {
    ERROR("Initiating chest");
    goto clean_exit;
  }

  for (uint32_t n = 0; n < SRSRAN_MAX_PRB_NR; n++) {
    pusch_cfg.grant.prb_idx[n] = (n < n_prb);
  }
  pusch_cfg.grant.nof_prb = n_prb;

  pusch_cfg.grant.dci_format                       = srsran_dci_format_nr_0_0;
  pusch_cfg.grant.nof_dmrs_cdm_groups_without_data = 2;
  pusch_cfg.dmrs.type                              = srsran_dmrs_sch_type_1;
  pusch_cfg.dmrs.length                            = srsran_dmrs_sch_len_1;
  pusch_cfg.dmrs.additional_pos                    = srsran_dmrs_sch_add_pos_2;
  if (srsran_ra_nr_fill_tb(&pusch_cfg, &pusch_cfg.grant, mcs, &pusch_cfg.grant.tb[0]) < SRSRAN_SUCCESS) {
    ERROR("Error filling tb");
    goto clean_exit;
  }

  uint32_t n_blocks      = 0;
  uint32_t n_errors      = 0;
  uint32_t crc_false_pos = 0;
  uint32_t crc_false_neg = 0;
  float    evm           = 0;
  for (; n_blocks < max_blocks && n_errors < 100; n_blocks++) {
    // Generate SCH payload
    for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
      // Skip TB if no allocated
      if (data_tx.payload[tb] == NULL) {
        continue;
      }

      // load payload with bytes
      for (uint32_t i = 0; i < pusch_cfg.grant.tb[tb].tbs / 8 + 1; i++) {
        data_tx.payload[tb][i] = (uint8_t)srsran_random_uniform_int_dist(rand_gen, 0, UINT8_MAX);
      }
      pusch_cfg.grant.tb[tb].softbuffer.tx = &softbuffer_tx;
    }

    // Generate HARQ ACK bits
    if (nof_ack_bits > 0) {
      pusch_cfg.uci.ack.count = nof_ack_bits;
      for (uint32_t i = 0; i < nof_ack_bits; i++) {
        data_tx.uci.ack[i] = (uint8_t)srsran_random_uniform_int_dist(rand_gen, 0, 1);
      }
    }

    // Generate CSI report bits
    uint8_t csi_report_tx[SRSRAN_UCI_NR_MAX_CSI1_BITS] = {};
    uint8_t csi_report_rx[SRSRAN_UCI_NR_MAX_CSI1_BITS] = {};
    if (nof_csi_bits > 0) {
      pusch_cfg.uci.csi[0].cfg.quantity = SRSRAN_CSI_REPORT_QUANTITY_NONE;
      pusch_cfg.uci.csi[0].K_csi_rs     = nof_csi_bits;
      pusch_cfg.uci.nof_csi             = 1;
      data_tx.uci.csi[0].none           = csi_report_tx;
      for (uint32_t i = 0; i < nof_csi_bits; i++) {
        csi_report_tx[i] = (uint8_t)srsran_random_uniform_int_dist(rand_gen, 0, 1);
      }

      data_rx.uci.csi[0].none = csi_report_rx;
    }

    if (srsran_ra_ul_set_grant_uci_nr(&carrier, &sch_hl_cfg, &pusch_cfg.uci, &pusch_cfg) < SRSRAN_SUCCESS) {
      ERROR("Setting UCI");
      goto clean_exit;
    }

    if (srsran_pusch_nr_encode(&pusch_tx, &pusch_cfg, &pusch_cfg.grant, &data_tx, sf_symbols_tx) < SRSRAN_SUCCESS) {
      ERROR("Error encoding");
      goto clean_exit;
    }

    float noise_var = srsran_convert_dB_to_power(-snr);
    for (uint32_t i = 0; i < carrier.max_mimo_layers; i++) {
      srsran_ch_awgn_c(sf_symbols_tx[i], sf_symbols_rx[i], noise_var, slot_length);
      // memcpy(sf_symbols_rx[i], sf_symbols_tx[i], slot_length * sizeof(cf_t));
    }

    if (get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO) {
      uint32_t nof_re_total = carrier.nof_prb * SRSRAN_NRE;
      uint32_t nof_re_used  = pusch_cfg.grant.nof_prb * SRSRAN_NRE;
      for (int i_layer = 0; i_layer < carrier.max_mimo_layers; i_layer++) {
        INFO("Layer %d", i_layer);
        float   tx_power  = 0;
        float   rx_power  = 0;
        uint8_t n_symbols = 0;
        for (int i = 0; i < SRSRAN_NSYMB_PER_SLOT_NR; i++) {
          if (!pusch_tx.dmrs_re_pattern.symbol[i]) {
            n_symbols++;
            tx_power += srsran_vec_avg_power_cf(sf_symbols_tx[0] + i * nof_re_total, nof_re_total);
            rx_power += srsran_vec_avg_power_cf(sf_symbols_rx[0] + i * nof_re_total, nof_re_total);
          }
        }
        tx_power *= (float)nof_re_total / nof_re_used; // compensate for unused REs
        INFO("    Tx power: %.3f", tx_power / n_symbols);
        INFO("    Rx power: %.3f", rx_power / n_symbols);
        INFO("    SNR: %.3f dB", srsran_convert_power_to_dB(tx_power / (rx_power - tx_power)));
      }
    }

    for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
      pusch_cfg.grant.tb[tb].softbuffer.rx = &softbuffer_rx;
      srsran_softbuffer_rx_reset(pusch_cfg.grant.tb[tb].softbuffer.rx);
    }

    // assume perfect channel estimation (including noise variance)
    for (uint32_t i = 0; i < pusch_cfg.grant.tb->nof_re; i++) {
      chest.ce[0][0][i] = 1.0F;
    }
    chest.nof_re         = pusch_cfg.grant.tb->nof_re;
    chest.noise_estimate = 2 * noise_var;

    if (srsran_pusch_nr_decode(&pusch_rx, &pusch_cfg, &pusch_cfg.grant, &chest, sf_symbols_rx, &data_rx) <
        SRSRAN_SUCCESS) {
      ERROR("Error decoding");
      goto clean_exit;
    }

    evm += data_rx.evm[0];
    // Validate UL-SCH CRC check
    if (!data_rx.tb[0].crc) {
      n_errors++;
      printf("*");
      fflush(stdout);
      if (n_errors % 20 == 0) {
        printf("\n");
      }
    }

    if (full_check) {
      // Validate by comparing payload (recall, payload is represented in bytes)
      if ((memcmp(data_rx.tb[0].payload, data_tx.payload[0], pusch_cfg.grant.tb[0].tbs * sizeof(uint8_t) / 8) == 0) &&
          !data_rx.tb[0].crc) {
        printf("\nWARNING! Codeword OK but CRC KO!\n");
        crc_false_pos++;
      } else if ((memcmp(data_rx.tb[0].payload, data_tx.payload[0], pusch_cfg.grant.tb[0].tbs * sizeof(uint8_t) / 8) !=
                  0) &&
                 data_rx.tb[0].crc) {
        printf("\nWarning! Codeword KO but CRC OK!\n");
        crc_false_neg++;
      }
    }
  }
  char str[512];
  srsran_pusch_nr_rx_info(&pusch_rx, &pusch_cfg, &pusch_cfg.grant, &data_rx, str, (uint32_t)sizeof(str));

  char str_extra[2048];
  srsran_sch_cfg_nr_info(&pusch_cfg, str_extra, (uint32_t)sizeof(str_extra));
  printf("\nPUSCH: %s\n%s", str, str_extra);

  printf("\nNominal SNR: %.1f dB\n", snr);
  printf("Average EVM: %.3f\n", evm / n_blocks);

  printf("BLER: %.3e (%d errors out of %d blocks)\n", (double)n_errors / n_blocks, n_errors, n_blocks);
  printf("Tx Throughput: %.3e Mbps -- Rx Throughput: %.3e Mbps (%.2f%%)\n",
         pusch_cfg.grant.tb[0].tbs / 1e3,
         (n_blocks - n_errors) / 1e3 * pusch_cfg.grant.tb[0].tbs / n_blocks,
         100.0F * (n_blocks - n_errors) / n_blocks);

  if (full_check) {
    uint32_t true_errors = n_errors + crc_false_neg - crc_false_pos;
    printf("CRC: missed detection/Type I err. %.2f%% (%d out of %d)",
           100.0F * crc_false_neg / true_errors,
           crc_false_neg,
           true_errors);
    printf(" -- false alarm %.2f%% (%d out of %d)", 100.0F * crc_false_pos / n_errors, crc_false_pos, n_errors);
    printf(" -- Type II err. %.2f%% (%d out of %d)\n",
           100.0F * crc_false_pos / (n_blocks - true_errors),
           crc_false_pos,
           n_blocks - true_errors);
  }

  ret = SRSRAN_SUCCESS;

clean_exit:
  srsran_chest_dl_res_free(&chest);
  srsran_random_free(rand_gen);
  srsran_pusch_nr_free(&pusch_tx);
  srsran_pusch_nr_free(&pusch_rx);
  for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    if (data_tx.payload[i]) {
      free(data_tx.payload[i]);
    }
    if (data_rx.tb[i].payload) {
      free(data_rx.tb[i].payload);
    }
  }
  for (uint32_t i = 0; i < SRSRAN_MAX_LAYERS_NR; i++) {
    if (sf_symbols_tx[i]) {
      free(sf_symbols_tx[i]);
    }
    if (sf_symbols_rx[i]) {
      free(sf_symbols_rx[i]);
    }
  }
  srsran_softbuffer_tx_free(&softbuffer_tx);
  srsran_softbuffer_rx_free(&softbuffer_rx);

  return ret;
}
