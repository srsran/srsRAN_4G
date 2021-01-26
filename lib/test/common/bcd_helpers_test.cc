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

#include "srslte/common/bcd_helpers.h"
#include "srslte/common/test_common.h"

using namespace srslte;

int main(int argc, char** argv)
{
  std::string mcc_str = "001";
  std::string mnc_str = "001";
  uint16_t    mcc;
  uint16_t    mnc;

  // String to code

  TESTASSERT(string_to_mcc(mcc_str, &mcc));
  TESTASSERT(mcc == 0xF001);

  TESTASSERT(string_to_mnc(mnc_str, &mnc));
  TESTASSERT(mnc == 0xF001);

  mnc_str = "01";
  TESTASSERT(string_to_mnc(mnc_str, &mnc));
  TESTASSERT(mnc == 0xFF01);

  // Code to string

  mcc_str = "";
  mnc_str = "";
  mcc     = 0xF001;
  mnc     = 0xF001;

  TESTASSERT(mcc_to_string(mcc, &mcc_str));
  TESTASSERT(mcc_str.compare("001") == 0);

  TESTASSERT(mnc_to_string(mnc, &mnc_str));
  TESTASSERT(mnc_str.compare("001") == 0);

  mnc = 0xFF01;
  TESTASSERT(mnc_to_string(mnc, &mnc_str));
  TESTASSERT(mnc_str.compare("01") == 0);
  return SRSLTE_SUCCESS;
}
