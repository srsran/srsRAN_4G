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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "srslte/phy/ch_estimation/chest_sl.h"
#include "srslte/phy/channel/ch_awgn.h"
#include "srslte/phy/common/phy_common_sl.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/io/filesource.h"
#include "srslte/phy/phch/pscch.h"
#include "srslte/phy/phch/pssch.h"
#include "srslte/phy/phch/ra_sl.h"
#include "srslte/phy/phch/sci.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

static char*            input_file_name = NULL;
static srslte_cell_sl_t cell            = {.nof_prb = 6, .N_sl_id = 0, .tm = SRSLTE_SIDELINK_TM2, .cp = SRSLTE_CP_NORM};
static bool             use_standard_lte_rates = false;
static uint32_t         file_offset            = 0;

static uint32_t                       sf_n_samples          = 0;
static uint32_t                       sf_n_re               = 0;
static cf_t*                          sf_buffer             = NULL;
static cf_t*                          equalized_sf_buffer   = NULL;
static cf_t*                          input_buffer          = NULL;
static srslte_sci_t                   sci                   = {};
static srslte_pscch_t                 pscch                 = {};
static srslte_chest_sl_t              pscch_chest           = {};
static srslte_pssch_t                 pssch                 = {};
static srslte_chest_sl_t              pssch_chest           = {};
static srslte_ofdm_t                  fft                   = {};
static srslte_sl_comm_resource_pool_t sl_comm_resource_pool = {};
static uint32_t                       size_sub_channel      = 10;
static uint32_t                       num_sub_channel       = 5;
static uint32_t                       current_sf_idx        = 0;

static srslte_chest_sl_cfg_t pscch_chest_sl_cfg = {};
static srslte_chest_sl_cfg_t pssch_chest_sl_cfg = {};

static srslte_filesource_t fsrc = {};

void usage(char* prog)
{
  printf("Usage: %s [deinopstv]\n", prog);
  printf("\t-i input_file_name\n");
  printf("\t-o File offset samples [Default %d]\n", file_offset);
  printf("\t-p nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-s size_sub_channel [Default for 50 prbs %d]\n", size_sub_channel);
  printf("\t-n num_sub_channel [Default for 50 prbs %d]\n", num_sub_channel);
  printf("\t-m Subframe index [Default for %d]\n", current_sf_idx);
  printf("\t-e Extended CP [Default normal]\n");
  printf("\t-t Sidelink transmission mode {1,2,3,4} [Default %d]\n", (cell.tm + 1));
  printf("\t-d use_standard_lte_rates [Default %i]\n", use_standard_lte_rates);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "deinmopstv")) != -1) {
    switch (opt) {
      case 'd':
        use_standard_lte_rates = true;
        break;
      case 'o':
        file_offset = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'e':
        cell.cp = SRSLTE_CP_EXT;
        break;
      case 'i':
        input_file_name = argv[optind];
        break;
      case 's':
        size_sub_channel = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        num_sub_channel = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        current_sf_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 't':
        if (srslte_sl_tm_to_cell_sl_tm_t(&cell, strtol(argv[optind], NULL, 10)) != SRSLTE_SUCCESS) {
          usage(argv[0]);
          exit(-1);
        }
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (SRSLTE_CP_ISEXT(cell.cp) && cell.tm >= SRSLTE_SIDELINK_TM3) {
    ERROR("Selected TM does not support extended CP");
    usage(argv[0]);
    exit(-1);
  }
}

int base_init()
{
  sf_n_samples = srslte_symbol_sz(cell.nof_prb) * 15;
  sf_n_re      = SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);

  if (srslte_sl_comm_resource_pool_get_default_config(&sl_comm_resource_pool, cell) != SRSLTE_SUCCESS) {
    ERROR("Error initializing sl_comm_resource_pool\n");
    return SRSLTE_ERROR;
  }
  sl_comm_resource_pool.num_sub_channel  = num_sub_channel;
  sl_comm_resource_pool.size_sub_channel = size_sub_channel;

  sf_buffer = srslte_vec_cf_malloc(sf_n_re);
  if (!sf_buffer) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  srslte_vec_cf_zero(sf_buffer, sf_n_re);

  equalized_sf_buffer = srslte_vec_cf_malloc(sf_n_re);
  if (!equalized_sf_buffer) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  srslte_vec_cf_zero(equalized_sf_buffer, sf_n_re);

  input_buffer = srslte_vec_cf_malloc(sf_n_samples);
  if (!input_buffer) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  srslte_vec_cf_zero(input_buffer, sf_n_samples);

  srslte_sci_init(&sci, cell, sl_comm_resource_pool);

  if (srslte_pscch_init(&pscch, SRSLTE_MAX_PRB) != SRSLTE_SUCCESS) {
    ERROR("Error in PSCCH init\n");
    return SRSLTE_ERROR;
  }

  if (srslte_pscch_set_cell(&pscch, cell) != SRSLTE_SUCCESS) {
    ERROR("Error in PSCCH set cell\n");
    return SRSLTE_ERROR;
  }

  if (srslte_chest_sl_init(&pscch_chest, SRSLTE_SIDELINK_PSCCH, cell, sl_comm_resource_pool) != SRSLTE_SUCCESS) {
    ERROR("Error in PSCCH DMRS init\n");
    return SRSLTE_ERROR;
  }

  if (srslte_pssch_init(&pssch, cell, sl_comm_resource_pool) != SRSLTE_SUCCESS) {
    ERROR("Error initializing PSSCH\n");
    return SRSLTE_ERROR;
  }

  if (srslte_chest_sl_init(&pssch_chest, SRSLTE_SIDELINK_PSSCH, cell, sl_comm_resource_pool) != SRSLTE_SUCCESS) {
    ERROR("Error in chest PSSCH init\n");
    return SRSLTE_ERROR;
  }

  if (input_file_name) {
    if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
      printf("Error opening file %s\n", input_file_name);
      return SRSLTE_ERROR;
    }
  } else {
    ERROR("Invalid input file name\n");
    return SRSLTE_ERROR;
  }

  if (srslte_ofdm_rx_init(&fft, cell.cp, input_buffer, sf_buffer, cell.nof_prb)) {
    fprintf(stderr, "Error creating FFT object\n");
    return SRSLTE_ERROR;
  }
  srslte_ofdm_set_normalize(&fft, true);
  srslte_ofdm_set_freq_shift(&fft, -0.5);

  return SRSLTE_SUCCESS;
}

void base_free()
{
  srslte_filesource_free(&fsrc);
  srslte_ofdm_rx_free(&fft);

  srslte_sci_free(&sci);
  srslte_pscch_free(&pscch);
  srslte_chest_sl_free(&pscch_chest);

  srslte_pssch_free(&pssch);
  srslte_chest_sl_free(&pssch_chest);

  if (sf_buffer) {
    free(sf_buffer);
  }
  if (equalized_sf_buffer) {
    free(equalized_sf_buffer);
  }
  if (input_buffer) {
    free(input_buffer);
  }
}

int main(int argc, char** argv)
{
  uint8_t sci_rx[SRSLTE_SCI_MAX_LEN] = {};
  int     ret                        = SRSLTE_ERROR;

  parse_args(argc, argv);
  srslte_use_standard_symbol_size(use_standard_lte_rates);

  if (base_init()) {
    ERROR("Error initializing\n");
    base_free();
    return SRSLTE_ERROR;
  }

  bool     sci_decoded                     = false;
  uint32_t num_decoded_sci                 = 0;
  char     sci_msg[SRSLTE_SCI_MSG_MAX_LEN] = {};

  uint32_t num_decoded_tb               = 0;
  uint8_t  tb[SRSLTE_SL_SCH_MAX_TB_LEN] = {};

  int max_num_subframes = 128;
  int num_subframes     = 0;
  int nread             = 0;

  uint32_t period_sf_idx        = 0;
  uint32_t allowed_pssch_sf_idx = 0;

  if (file_offset > 0) {
    printf("Offsetting file by %d samples.\n", file_offset);
    srslte_filesource_seek(&fsrc, file_offset * sizeof(cf_t));
  }

  do {
    nread = srslte_filesource_read(&fsrc, input_buffer, sf_n_samples);
    if (nread < 0) {
      fprintf(stderr, "Error reading from file\n");
      return ret;
    } else if (nread == 0) {
      goto clean_exit;
    } else if (nread < sf_n_samples) {
      fprintf(stderr, "Couldn't read entire subframe. Still processing ..\n");
      nread = -1;
    }

    // Convert to frequency domain
    srslte_ofdm_rx_sf(&fft);

    if (cell.tm == SRSLTE_SIDELINK_TM1 || cell.tm == SRSLTE_SIDELINK_TM2) {

      // 3GPP TS 36.213 Section 14.2.1.2 UE procedure for determining subframes
      // and resource blocks for transmitting PSCCH for sidelink transmission mode 2
      if (sl_comm_resource_pool.pscch_sf_bitmap[period_sf_idx] == 1) {

        for (uint32_t pscch_prb_start_idx = sl_comm_resource_pool.prb_start;
             pscch_prb_start_idx <= sl_comm_resource_pool.prb_end;
             pscch_prb_start_idx++) {

          // PSCCH Channel estimation
          pscch_chest_sl_cfg.prb_start_idx = pscch_prb_start_idx;
          srslte_chest_sl_set_cfg(&pscch_chest, pscch_chest_sl_cfg);
          srslte_chest_sl_ls_estimate_equalize(&pscch_chest, sf_buffer, equalized_sf_buffer);

          if (srslte_pscch_decode(&pscch, equalized_sf_buffer, sci_rx, pscch_prb_start_idx) == SRSLTE_SUCCESS) {
            if (srslte_sci_format0_unpack(&sci, sci_rx) == SRSLTE_SUCCESS) {

              srslte_sci_info(&sci, sci_msg, sizeof(sci_msg));
              fprintf(stdout, "%s", sci_msg);

              sci_decoded = true;
              num_decoded_sci++;
            }
          }

          if ((sl_comm_resource_pool.prb_num * 2) <=
              (sl_comm_resource_pool.prb_end - sl_comm_resource_pool.prb_start + 1)) {
            if ((pscch_prb_start_idx + 1) == (sl_comm_resource_pool.prb_start + sl_comm_resource_pool.prb_num)) {
              pscch_prb_start_idx = sl_comm_resource_pool.prb_end - sl_comm_resource_pool.prb_num;
            }
          }
        }
      }

      if ((sl_comm_resource_pool.pssch_sf_bitmap[period_sf_idx] == 1) && (sci_decoded == true)) {
        if (srslte_ra_sl_pssch_allowed_sf(current_sf_idx, sci.trp_idx, SRSLTE_SL_DUPLEX_MODE_FDD, 0)) {

          // Redundancy version
          uint32_t rv_idx = allowed_pssch_sf_idx % 4;

          uint32_t nof_prb_pssch       = 0;
          uint32_t pssch_prb_start_idx = 0;
          srslte_ra_sl_type0_from_riv(sci.riv, cell.nof_prb, &nof_prb_pssch, &pssch_prb_start_idx);
          printf("pssch_start_prb_idx = %i nof_prb = %i\n", pssch_prb_start_idx, nof_prb_pssch);

          // PSSCH Channel estimation
          pssch_chest_sl_cfg.N_x_id        = sci.N_sa_id;
          pssch_chest_sl_cfg.sf_idx        = current_sf_idx;
          pssch_chest_sl_cfg.prb_start_idx = pssch_prb_start_idx;
          pssch_chest_sl_cfg.nof_prb       = nof_prb_pssch;
          srslte_chest_sl_set_cfg(&pssch_chest, pssch_chest_sl_cfg);
          srslte_chest_sl_ls_estimate_equalize(&pssch_chest, sf_buffer, equalized_sf_buffer);

          srslte_pssch_cfg_t pssch_cfg = {
              pssch_prb_start_idx, nof_prb_pssch, sci.N_sa_id, sci.mcs_idx, rv_idx, current_sf_idx};
          if (srslte_pssch_set_cfg(&pssch, pssch_cfg) == SRSLTE_SUCCESS) {
            if (srslte_pssch_decode(&pssch, equalized_sf_buffer, tb, SRSLTE_SL_SCH_MAX_TB_LEN) == SRSLTE_SUCCESS) {
              srslte_vec_fprint_byte(stdout, tb, pssch.sl_sch_tb_len);
              num_decoded_tb++;
              printf("> Transport Block SUCCESS! TB count: %i\n", num_decoded_tb);
            }
          }
          allowed_pssch_sf_idx++;
        }
        current_sf_idx++;
      }
    } else if (cell.tm == SRSLTE_SIDELINK_TM3 || cell.tm == SRSLTE_SIDELINK_TM4) {
      for (int sub_channel_idx = 0; sub_channel_idx < sl_comm_resource_pool.num_sub_channel; sub_channel_idx++) {
        uint32_t pscch_prb_start_idx = sl_comm_resource_pool.size_sub_channel * sub_channel_idx;

        for (uint32_t cyclic_shift = 0; cyclic_shift <= 9; cyclic_shift += 3) {
          // PSCCH Channel estimation
          pscch_chest_sl_cfg.cyclic_shift  = cyclic_shift;
          pscch_chest_sl_cfg.prb_start_idx = pscch_prb_start_idx;
          srslte_chest_sl_set_cfg(&pscch_chest, pscch_chest_sl_cfg);
          srslte_chest_sl_ls_estimate_equalize(&pscch_chest, sf_buffer, equalized_sf_buffer);

          if (srslte_pscch_decode(&pscch, equalized_sf_buffer, sci_rx, pscch_prb_start_idx) == SRSLTE_SUCCESS) {
            if (srslte_sci_format1_unpack(&sci, sci_rx) == SRSLTE_SUCCESS) {
              srslte_sci_info(&sci, sci_msg, sizeof(sci_msg));
              fprintf(stdout, "%s", sci_msg);

              num_decoded_sci++;

              // Decode PSSCH
              uint32_t sub_channel_start_idx = 0;
              uint32_t L_subCH               = 0;
              srslte_ra_sl_type0_from_riv(
                  sci.riv, sl_comm_resource_pool.num_sub_channel, &L_subCH, &sub_channel_start_idx);

              // 3GPP TS 36.213 Section 14.1.1.4C
              uint32_t pssch_prb_start_idx = (sub_channel_idx * sl_comm_resource_pool.size_sub_channel) +
                                             pscch.pscch_nof_prb + sl_comm_resource_pool.start_prb_sub_channel;
              uint32_t nof_prb_pssch = ((L_subCH + sub_channel_idx) * sl_comm_resource_pool.size_sub_channel) -
                                       pssch_prb_start_idx + sl_comm_resource_pool.start_prb_sub_channel;

              // make sure PRBs are valid for DFT precoding
              nof_prb_pssch = srslte_dft_precoding_get_valid_prb(nof_prb_pssch);

              uint32_t N_x_id = 0;
              for (int j = 0; j < SRSLTE_SCI_CRC_LEN; j++) {
                N_x_id += pscch.sci_crc[j] * (1 << (SRSLTE_SCI_CRC_LEN - 1 - j));
              }

              uint32_t rv_idx = 0;
              if (sci.retransmission == true) {
                rv_idx = 1;
              }

              // PSSCH Channel estimation
              pssch_chest_sl_cfg.N_x_id        = N_x_id;
              pssch_chest_sl_cfg.sf_idx        = current_sf_idx;
              pssch_chest_sl_cfg.prb_start_idx = pssch_prb_start_idx;
              pssch_chest_sl_cfg.nof_prb       = nof_prb_pssch;
              srslte_chest_sl_set_cfg(&pssch_chest, pssch_chest_sl_cfg);
              srslte_chest_sl_ls_estimate_equalize(&pssch_chest, sf_buffer, equalized_sf_buffer);

              srslte_pssch_cfg_t pssch_cfg = {
                  pssch_prb_start_idx, nof_prb_pssch, N_x_id, sci.mcs_idx, rv_idx, current_sf_idx};
              if (srslte_pssch_set_cfg(&pssch, pssch_cfg) == SRSLTE_SUCCESS) {
                if (srslte_pssch_decode(&pssch, equalized_sf_buffer, tb, SRSLTE_SL_SCH_MAX_TB_LEN) == SRSLTE_SUCCESS) {
                  srslte_vec_fprint_byte(stdout, tb, pssch.sl_sch_tb_len);
                  num_decoded_tb++;
                }

                if (SRSLTE_VERBOSE_ISDEBUG()) {
                  char filename[64];
                  snprintf(filename, 64, "pssch_rx_syms_sf%d.bin", num_subframes);
                  printf("Saving PSSCH symbols (%d) to %s (current_sf_idx=%d)\n",
                         pssch.G / pssch.Qm,
                         filename,
                         current_sf_idx);
                  srslte_vec_save_file(filename, pssch.symbols, pssch.G / pssch.Qm * sizeof(cf_t));
                }
              }
            }
          }
          if (SRSLTE_VERBOSE_ISDEBUG()) {
            char filename[64];
            snprintf(filename,
                     64,
                     "pscch_rx_syms_sf%d_shift%d_prbidx%d.bin",
                     num_subframes,
                     cyclic_shift,
                     pscch_prb_start_idx);
            printf("Saving PSCCH symbols (%d) to %s\n", pscch.E / SRSLTE_PSCCH_QM, filename);
            srslte_vec_save_file(filename, pscch.mod_symbols, pscch.E / SRSLTE_PSCCH_QM * sizeof(cf_t));
          }
        }
      }
      current_sf_idx = (current_sf_idx + 1) % 10;
    }
    num_subframes++;
    period_sf_idx++;
  } while (nread > 0 && num_subframes < max_num_subframes);

clean_exit:

  base_free();

  printf("num_decoded_sci=%d num_decoded_tb=%d\n", num_decoded_sci, num_decoded_tb);

  ret = (num_decoded_sci > 0) ? SRSLTE_SUCCESS : SRSLTE_ERROR;

  return ret;
}