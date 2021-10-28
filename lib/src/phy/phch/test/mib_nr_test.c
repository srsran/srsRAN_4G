/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/phy/phch/pbch_msg_nr.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/random.h"
#include "srsran/support/srsran_test.h"
#include <getopt.h>
#include <string.h>

static uint32_t        nof_repetitions = 16;
static srsran_random_t random_gen      = NULL;

static int test_packing_unpacking()
{
  for (uint32_t r = 0; r < nof_repetitions; r++) {
    srsran_mib_nr_t mib = {};
    mib.sfn             = srsran_random_uniform_int_dist(random_gen, 0, 1023);
    mib.ssb_idx         = srsran_random_uniform_int_dist(random_gen, 0, 127);
    mib.hrf             = srsran_random_bool(random_gen, 0.5f);
    mib.scs_common =
        srsran_random_bool(random_gen, 0.5f) ? srsran_subcarrier_spacing_15kHz : srsran_subcarrier_spacing_30kHz;
    mib.ssb_offset = srsran_random_uniform_int_dist(random_gen, 0, 31);
    mib.dmrs_typeA_pos =
        srsran_random_bool(random_gen, 0.5f) ? srsran_dmrs_sch_typeA_pos_2 : srsran_dmrs_sch_typeA_pos_3;
    mib.coreset0_idx           = srsran_random_uniform_int_dist(random_gen, 0, 15);
    mib.ss0_idx                = srsran_random_uniform_int_dist(random_gen, 0, 15);
    mib.cell_barred            = srsran_random_bool(random_gen, 0.5f);
    mib.intra_freq_reselection = srsran_random_bool(random_gen, 0.5f);
    mib.spare                  = srsran_random_uniform_int_dist(random_gen, 0, 1);

    srsran_pbch_msg_nr_t pbch_msg = {};
    TESTASSERT(srsran_pbch_msg_nr_mib_pack(&mib, &pbch_msg) == SRSRAN_SUCCESS);

    TESTASSERT(srsran_pbch_msg_nr_is_mib(&pbch_msg));

    srsran_mib_nr_t mib2 = {};
    TESTASSERT(srsran_pbch_msg_nr_mib_unpack(&pbch_msg, &mib2) == SRSRAN_SUCCESS);

    char str1[256];
    char str2[256];
    char strp[256];
    srsran_pbch_msg_nr_mib_info(&mib, str1, (uint32_t)sizeof(str1));
    srsran_pbch_msg_nr_mib_info(&mib2, str2, (uint32_t)sizeof(str2));
    srsran_pbch_msg_info(&pbch_msg, strp, (uint32_t)sizeof(strp));

    if (memcmp(&mib, &mib2, sizeof(srsran_mib_nr_t)) != 0) {
      ERROR("Failed packing/unpacking MIB");
      printf("  Source: %s\n", str1);
      printf("Unpacked: %s\n", str2);
      printf("  Packed: %s\n", strp);
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

static void usage(char* prog)
{
  printf("Usage: %s [cpndv]\n", prog);
  printf("\t-v Increase verbose [default none]\n");
  printf("\t-R Set number of Packing/Unpacking [default %d]\n", nof_repetitions);
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "vR")) != -1) {
    switch (opt) {
      case 'v':
        increase_srsran_verbose_level();
        break;
      case 'R':
        nof_repetitions = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  int ret    = SRSRAN_ERROR;
  random_gen = srsran_random_init(1234);

  if (test_packing_unpacking() < SRSRAN_SUCCESS) {
    goto clean_exit;
  }

  ret = SRSRAN_SUCCESS;

clean_exit:
  srsran_random_free(random_gen);
  return ret;
}
