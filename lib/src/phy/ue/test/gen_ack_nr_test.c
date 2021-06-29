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

#include "srsran/common/test_common.h"
#include "srsran/phy/phch/harq_ack.h"
#include "srsran/phy/utils/debug.h"
#include <getopt.h>

static int test_case_1()
{
  // Set configuration
  srsran_harq_ack_cfg_hl_t cfg = {};
  cfg.harq_ack_codebook        = srsran_pdsch_harq_ack_codebook_dynamic;

  // Generate ACK information
  srsran_pdsch_ack_nr_t ack_info = {};
  ack_info.nof_cc                = 1;
  ack_info.use_pusch             = true;

  srsran_harq_ack_m_t m = {};
  m.value[0]            = 1;
  m.present             = true;

  m.resource.k1       = 8;
  m.resource.v_dai_dl = 0;
  m.value[0]          = 1;
  m.present           = true;
  TESTASSERT(srsran_harq_ack_insert_m(&ack_info, &m) == SRSRAN_SUCCESS);

  m.resource.k1       = 5;
  m.resource.v_dai_dl = 2;
  TESTASSERT(srsran_harq_ack_insert_m(&ack_info, &m) == SRSRAN_SUCCESS);

  m.resource.k1       = 6;
  m.resource.v_dai_dl = 1;
  TESTASSERT(srsran_harq_ack_insert_m(&ack_info, &m) == SRSRAN_SUCCESS);

  m.resource.k1       = 4;
  m.resource.v_dai_dl = 3;
  TESTASSERT(srsran_harq_ack_insert_m(&ack_info, &m) == SRSRAN_SUCCESS);

  m.resource.k1       = 3;
  m.resource.v_dai_dl = 0;
  TESTASSERT(srsran_harq_ack_insert_m(&ack_info, &m) == SRSRAN_SUCCESS);

  // Print trace
  char str[512] = {};
  TESTASSERT(srsran_harq_ack_info(&ack_info, str, (uint32_t)sizeof(str)) > SRSRAN_SUCCESS);
  INFO("%s", str);

  // Generate UCI data
  srsran_uci_data_nr_t uci_data = {};
  TESTASSERT(srsran_harq_ack_pack(&cfg, &ack_info, &uci_data) == SRSRAN_SUCCESS);

  // Assert UCI data
  TESTASSERT(uci_data.cfg.ack.count == 5);

  return SRSRAN_SUCCESS;
}

static int test_case_2()
{
  // Set configuration
  srsran_harq_ack_cfg_hl_t cfg = {};
  cfg.harq_ack_codebook        = srsran_pdsch_harq_ack_codebook_dynamic;

  // Generate ACK information
  srsran_pdsch_ack_nr_t ack_info = {};
  ack_info.nof_cc                = 1;
  ack_info.use_pusch             = true;

  srsran_harq_ack_m_t m = {};
  m.value[0]            = 1;
  m.present             = true;

  m.resource.k1       = 7;
  m.resource.v_dai_dl = 1;
  TESTASSERT(srsran_harq_ack_insert_m(&ack_info, &m) == SRSRAN_SUCCESS);

  m.resource.k1       = 6;
  m.resource.v_dai_dl = 2;
  TESTASSERT(srsran_harq_ack_insert_m(&ack_info, &m) == SRSRAN_SUCCESS);

  m.resource.k1       = 8;
  m.resource.v_dai_dl = 0;
  TESTASSERT(srsran_harq_ack_insert_m(&ack_info, &m) == SRSRAN_SUCCESS);

  m.resource.k1       = 5;
  m.resource.v_dai_dl = 3;
  TESTASSERT(srsran_harq_ack_insert_m(&ack_info, &m) == SRSRAN_SUCCESS);

  m.resource.k1       = 4;
  m.resource.v_dai_dl = 0;
  TESTASSERT(srsran_harq_ack_insert_m(&ack_info, &m) == SRSRAN_SUCCESS);

  // Print trace
  char str[512] = {};
  TESTASSERT(srsran_harq_ack_info(&ack_info, str, (uint32_t)sizeof(str)) > SRSRAN_SUCCESS);
  INFO("%s", str);

  // Generate UCI data
  srsran_uci_data_nr_t uci_data = {};
  TESTASSERT(srsran_harq_ack_pack(&cfg, &ack_info, &uci_data) == SRSRAN_SUCCESS);

  // Assert UCI data
  TESTASSERT(uci_data.cfg.ack.count == 5);

  // Unpack HARQ ACK UCI data
  srsran_pdsch_ack_nr_t ack_info_rx = ack_info;
  TESTASSERT(srsran_harq_ack_unpack(&cfg, &uci_data, &ack_info_rx) == SRSRAN_SUCCESS);

  // Assert unpacked data
  TESTASSERT(memcmp(&ack_info, &ack_info_rx, sizeof(srsran_pdsch_ack_nr_t)) == 0);

  return SRSRAN_SUCCESS;
}

static void usage(char* prog)
{
  printf("Usage: %s [v]\n", prog);
  printf("\t-v Increase srsran_verbose\n");
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "v")) != -1) {
    switch (opt) {
      case 'v':
        srsran_verbose++;
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

  // Test only until Format1B - CS
  TESTASSERT(test_case_1() == SRSRAN_SUCCESS);
  TESTASSERT(test_case_2() == SRSRAN_SUCCESS);

  printf("Ok\n");
  return SRSRAN_SUCCESS;
}