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

#include "srsran/srsran.h"
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "srsran/srsran.h"

srsran_cell_t cell = {
    100,              // nof_prb
    SRSRAN_MAX_PORTS, // nof_ports
    1,                // cell_id
    SRSRAN_CP_NORM,   // cyclic prefix
    SRSRAN_PHICH_NORM,
    SRSRAN_PHICH_R_1_6, // PHICH length
    SRSRAN_FDD,

};

void usage(char* prog)
{
  printf("Usage: %s [recv]\n", prog);

  printf("\t-r nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e extended cyclic prefix [Default normal]\n");

  printf("\t-c cell_id (1000 tests all). [Default %d]\n", cell.id);

  printf("\t-v increase verbosity\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "recv")) != -1) {
    switch (opt) {
      case 'r':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'e':
        cell.cp = SRSRAN_CP_EXT;
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  srsran_refsignal_ul_t             refs      = {};
  srsran_refsignal_dmrs_pusch_cfg_t pusch_cfg = {};
  cf_t*                             signal    = NULL;
  int                               ret       = -1;

  parse_args(argc, argv);

  if (srsran_refsignal_ul_set_cell(&refs, cell)) {
    ERROR("Error initializing UL reference signal");
    goto do_exit;
  }

  signal = srsran_vec_cf_malloc(2 * SRSRAN_NRE * cell.nof_prb);
  if (!signal) {
    perror("malloc");
    goto do_exit;
  }
  printf("Running tests for %d PRB\n", cell.nof_prb);

  for (int n = 6; n < cell.nof_prb; n++) {
    for (int delta_ss = 29; delta_ss < SRSRAN_NOF_DELTA_SS; delta_ss++) {
      for (int cshift = 0; cshift < SRSRAN_NOF_CSHIFT; cshift++) {
        for (int h = 0; h < 3; h++) {
          for (int sf_idx = 0; sf_idx < 10; sf_idx++) {
            for (int cshift_dmrs = 0; cshift_dmrs < SRSRAN_NOF_CSHIFT; cshift_dmrs++) {
              uint32_t nof_prb         = n;
              pusch_cfg.cyclic_shift   = cshift;
              pusch_cfg.delta_ss       = delta_ss;
              bool group_hopping_en    = false;
              bool sequence_hopping_en = false;

              if (!h) {
                group_hopping_en    = false;
                sequence_hopping_en = false;
              } else if (h == 1) {
                group_hopping_en    = false;
                sequence_hopping_en = true;
              } else if (h == 2) {
                group_hopping_en    = true;
                sequence_hopping_en = false;
              }

              printf("nof_prb: %d, ", nof_prb);
              printf("cyclic_shift: %d, ", pusch_cfg.cyclic_shift);
              printf("cyclic_shift_for_dmrs: %d, ", cshift_dmrs);
              printf("delta_ss: %d, ", pusch_cfg.delta_ss);
              printf("SF_idx: %d\n", sf_idx);
              struct timeval t[3];

              gettimeofday(&t[1], NULL);
              pusch_cfg.group_hopping_en    = group_hopping_en;
              pusch_cfg.sequence_hopping_en = sequence_hopping_en;

              srsran_refsignal_dmrs_pusch_gen(&refs, &pusch_cfg, nof_prb, sf_idx, cshift_dmrs, signal);
              gettimeofday(&t[2], NULL);
              get_time_interval(t);
              printf("DMRS ExecTime: %ld us\n", t[0].tv_usec);

              srsran_refsignal_srs_cfg_t srs_cfg;
              ZERO_OBJECT(srs_cfg);

              gettimeofday(&t[1], NULL);
              srsran_refsignal_srs_gen(&refs, &srs_cfg, &pusch_cfg, sf_idx, signal);
              gettimeofday(&t[2], NULL);
              get_time_interval(t);
              printf("SRS ExecTime: %ld us\n", t[0].tv_usec);
            }
          }
        }
      }
    }
  }

  ret = 0;

do_exit:

  if (signal) {
    free(signal);
  }

  if (!ret) {
    printf("OK\n");
  }
  exit(ret);
}
