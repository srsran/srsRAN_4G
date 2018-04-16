/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <iostream>
#include "srsue/hdr/upper/pcsc_usim.h"
#include "srslte/common/log_filter.h"
#include <assert.h>
#include <srsue/hdr/upper/usim_base.h>

using namespace srsue;
using namespace std;


uint8_t rand_enb[] = {0xbc, 0x4c, 0xb0, 0x27, 0xb3, 0x4b, 0x7f, 0x51, 0x21, 0x5e, 0x56, 0x5f, 0x67, 0x3f, 0xde, 0x4f};
uint8_t autn_enb[] = {0x5a, 0x17, 0x77, 0x3c, 0x62, 0x57, 0x90, 0x01, 0xcf, 0x47, 0xf7, 0x6d, 0xb3, 0xa0, 0x19, 0x46};

int main(int argc, char **argv)
{
  srslte::log_filter usim_log("USIM");
  usim_log.set_level(srslte::LOG_LEVEL_DEBUG);
  usim_log.set_hex_limit(100000);
  uint8_t res[16];
  int res_len;
  uint8_t k_asme[32];
  uint16 mcc = 0;
  uint16 mnc = 0;

  usim_args_t args;
  args.pin = "6129";
  args.imei = "353490069873319";

  srsue::pcsc_usim usim;
  if (usim.init(&args, &usim_log)) {
    printf("Error initializing PC/SC USIM.\n");
    return SRSLTE_ERROR;
  };

  std::string imsi = usim.get_imsi_str();
  cout << "IMSI: " << imsi << endl;
  auth_result_t result = usim.generate_authentication_response(rand_enb, autn_enb, mcc, mnc, res, &res_len, k_asme);

  return SRSLTE_SUCCESS;
}
