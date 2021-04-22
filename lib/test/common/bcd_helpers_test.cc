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

#include "srsran/common/bcd_helpers.h"
#include "srsran/common/test_common.h"

using namespace srsran;

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
  return SRSRAN_SUCCESS;
}
