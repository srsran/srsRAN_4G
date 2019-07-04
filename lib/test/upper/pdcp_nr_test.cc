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

#include "srslte/common/log_filter.h"
#include "srslte/upper/pdcp_entity_nr.h"
#include <iostream>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

// Encription and Integrity Keys
uint8_t k_int[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                   0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};
uint8_t k_enc[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                   0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};

// Test SDUs for tx
uint8_t  sdu1[]   = {0x18, 0xE2};
uint32_t SDU1_LEN = 2;

// fake classes
class rlc_dummy : public srsue::rlc_interface_pdcp
{
public:
  rlc_dummy(srslte::log_filter* log_) : log(log_) {}

private:
  srslte::log_filter* log;
};

class rrc_dummy : public srsue::rrc_interface_pdcp
{
public:
  rrc_dummy(srslte::log_filter* log_) : log(log_) {}

private:
  srslte::log_filter* log;
};

class gw_dummy : public srsue::gw_interface_pdcp
{
public:
  gw_dummy(srslte::log_filter* log_) : log(log_) {}

private:
  srslte::log_filter* log;
};

// Setup all tests
int run_all_tests()
{
  srslte::log_filter log("PDCP NR Test");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(128);
  TESTASSERT(test_tx_basic(&log));
}

/*
 * Test 1: PDCP Entity TX
 */
bool test_tx_basic(srslte::log_filter* log)
{
  srslte::pdcp_entity_nr pdcp;
  srslte::srslte_pdcp_config_nr_t cfg = {0, false, true, SECURITY_DIRECTION_UPLINK, srslte::PDCP_SN_LEN_12};

  rlc_dummy rlc;
  rrc_dummy rrc;
  gw_dummy gw;

  pdcp.init(rlc, rrc, gw, log, 0, cfg);

  return true;
}

int main(int argc, char** argv)
{
  run_all_tests();
  srslte::pool::cleanup();
}
