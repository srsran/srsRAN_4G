/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "srslte/phy/ch_estimation/chest_sl.h"
#include "srslte/phy/channel/ch_awgn.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/io/filesource.h"
#include "srslte/phy/phch/pscch.h"
#include "srslte/phy/phch/sci.h"
#include "srslte/phy/sync/cfo.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

char*            input_file_name;
srslte_cell_sl_t cell                   = {.nof_prb = 6, .tm = SRSLTE_SIDELINK_TM2, .cp = SRSLTE_CP_NORM};
bool             use_standard_lte_rates = false;
uint32_t         size_sub_channel       = 10;
uint32_t         num_sub_channel        = 5;
uint32_t         file_offset            = 0;

uint32_t          sf_n_samples;
uint32_t          sf_n_re;
cf_t*             sf_buffer;
cf_t*             equalized_sf_buffer;
cf_t*             input_buffer;
srslte_sci_t      sci;
srslte_pscch_t    pscch;
srslte_chest_sl_t pscch_chest;
srslte_ofdm_t     fft;

srslte_filesource_t fsrc;

void usage(char* prog)
{
  printf("Usage: %s [deioptxzn]\n", prog);
  printf("\t-i input_file_name\n");
  printf("\t-o File offset samples [Default %d]\n", file_offset);
  printf("\t-p nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e Extended CP [Default normal]\n");
  printf("\t-d use_standard_lte_rates [Default %i]\n", use_standard_lte_rates);
  printf("\t-t Sidelink transmission mode {1,2,3,4} [Default %d]\n", (cell.tm + 1));
  printf("\t-z Size of sub-channels [Default %i]\n", size_sub_channel);
  printf("\t-n Number of sub-channels [Default %i]\n", num_sub_channel);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "deioptznv")) != -1) {
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
      case 'p':
        cell.nof_prb = (int32_t)strtol(argv[optind], NULL, 10);
        break;
      case 't':
        switch ((int32_t)strtol(argv[optind], NULL, 10)) {
          case 1:
            cell.tm = SRSLTE_SIDELINK_TM1;
            break;
          case 2:
            cell.tm = SRSLTE_SIDELINK_TM2;
            break;
          case 3:
            cell.tm = SRSLTE_SIDELINK_TM3;
            break;
          case 4:
            cell.tm = SRSLTE_SIDELINK_TM4;
            break;
          default:
            usage(argv[0]);
            exit(-1);
            break;
        }
        break;
      case 'z':
        size_sub_channel = (int32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        num_sub_channel = (int32_t)strtol(argv[optind], NULL, 10);
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int base_init()
{
  sf_n_samples = srslte_symbol_sz(cell.nof_prb) * 15;

  sf_n_re = SRSLTE_CP_NSYMB(cell.cp) * SRSLTE_NRE * 2 * cell.nof_prb;

  sf_buffer = srslte_vec_malloc(sizeof(cf_t) * sf_n_re);
  if (!sf_buffer) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  equalized_sf_buffer = srslte_vec_malloc(sizeof(cf_t) * sf_n_re);
  if (!equalized_sf_buffer) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  input_buffer = srslte_vec_malloc(sizeof(cf_t) * sf_n_samples);
  if (!input_buffer) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  srslte_sci_init(&sci, cell.nof_prb, cell.tm, size_sub_channel, num_sub_channel);

  if (srslte_pscch_init(&pscch, cell) != SRSLTE_SUCCESS) {
    ERROR("Error in PSCCH init\n");
    return SRSLTE_ERROR;
  }

  if (srslte_chest_sl_init_pscch_dmrs(&pscch_chest) != SRSLTE_SUCCESS) {
    ERROR("Error in PSCCH DMRS init\n");
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

  uint32_t num_decoded_sci                 = 0;
  char     sci_msg[SRSLTE_SCI_MSG_MAX_LEN] = "";

  int max_num_subframes = 128;
  int num_subframes     = 0;
  int nread             = 0;

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

    // CFO estimation and correction
    srslte_sl_cfo_est_corr_cp(input_buffer, cell.nof_prb, cell.cp);
    srslte_ofdm_rx_sf(&fft);

    if (cell.tm == SRSLTE_SIDELINK_TM1 || cell.tm == SRSLTE_SIDELINK_TM2) {
      uint32_t prb_num      = (uint32_t)ceil(cell.nof_prb / 2);
      uint32_t prb_start    = 0;
      uint32_t prb_end      = cell.nof_prb - 1;
      uint32_t cyclic_shift = 0;

      srslte_chest_sl_gen_pscch_dmrs(&pscch_chest, cyclic_shift, cell.tm);

      for (uint32_t pscch_prb_idx = prb_start; pscch_prb_idx <= prb_end; pscch_prb_idx++) {
        if (pscch_prb_idx == (prb_start + prb_num)) {
          pscch_prb_idx = (prb_end + 1) - prb_num;
        }

        // PSCCH Channel estimation
        srslte_chest_sl_pscch_ls_estimate_equalize(
            &pscch_chest, sf_buffer, pscch_prb_idx, equalized_sf_buffer, cell.nof_prb, cell.tm, cell.cp);

        if (srslte_pscch_decode(&pscch, equalized_sf_buffer, sci_rx, pscch_prb_idx) == SRSLTE_SUCCESS) {
          if (srslte_sci_format0_unpack(&sci, sci_rx) != SRSLTE_SUCCESS) {
            printf("Error unpacking sci format 0\n");
            return SRSLTE_ERROR;
          }

          srslte_sci_info(sci_msg, &sci);
          fprintf(stdout, "%s", sci_msg);

          num_decoded_sci++;
        }
      }
    } else if (cell.tm == SRSLTE_SIDELINK_TM3 || cell.tm == SRSLTE_SIDELINK_TM4) {
      for (int i = 0; i < num_sub_channel; i++) {
        uint32_t pscch_prb_idx = size_sub_channel * i;

        for (uint32_t cyclic_shift = 0; cyclic_shift <= 9; cyclic_shift += 3) {
          // PSCCH Channel estimation
          srslte_chest_sl_gen_pscch_dmrs(&pscch_chest, cyclic_shift, cell.tm);
          srslte_chest_sl_pscch_ls_estimate_equalize(
              &pscch_chest, sf_buffer, pscch_prb_idx, equalized_sf_buffer, cell.nof_prb, cell.tm, cell.cp);

          if (srslte_pscch_decode(&pscch, equalized_sf_buffer, sci_rx, pscch_prb_idx) == SRSLTE_SUCCESS) {
            if (srslte_sci_format1_unpack(&sci, sci_rx) != SRSLTE_SUCCESS) {
              printf("Error unpacking sci format 1\n");
              return SRSLTE_ERROR;
            }

            srslte_sci_info(sci_msg, &sci);
            fprintf(stdout, "%s", sci_msg);

            num_decoded_sci++;
          }
          if (SRSLTE_VERBOSE_ISDEBUG()) {
            char filename[64];
            snprintf(
                filename, 64, "pscch_rx_syms_sf%d_shift%d_prbidx%d.bin", num_subframes, cyclic_shift, pscch_prb_idx);
            printf("Saving PSCCH symbols (%d) to %s\n", pscch.E / SRSLTE_PSCCH_QM, filename);
            srslte_vec_save_file(filename, pscch.mod_symbols, pscch.E / SRSLTE_PSCCH_QM * sizeof(cf_t));
          }
        }
      }
    }
    num_subframes++;
  } while (nread > 0 && num_subframes < max_num_subframes);

clean_exit:

  base_free();

  printf("num_decoded_sci=%d\n", num_decoded_sci);

  ret = (num_decoded_sci > 0) ? SRSLTE_SUCCESS : SRSLTE_ERROR;

  return ret;
}
