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

#ifndef SRSUE_USIM_H
#define SRSUE_USIM_H

#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/security.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "usim_base.h"
#include <string>

namespace srsue {

class usim : public usim_base
{
public:
  usim(srslte::log* log_);
  int  init(usim_args_t* args);
  void stop();

  // NAS interface
  auth_result_t generate_authentication_response(uint8_t* rand,
                                                 uint8_t* autn_enb,
                                                 uint16_t mcc,
                                                 uint16_t mnc,
                                                 uint8_t* res,
                                                 int*     res_len,
                                                 uint8_t* k_asme);

private:
  auth_result_t gen_auth_res_milenage(uint8_t* rand,
                                      uint8_t* autn_enb,
                                      uint16_t mcc,
                                      uint16_t mnc,
                                      uint8_t* res,
                                      int*     res_len,
                                      uint8_t* k_asme);
  auth_result_t gen_auth_res_xor(uint8_t* rand,
                                 uint8_t* autn_enb,
                                 uint16_t mcc,
                                 uint16_t mnc,
                                 uint8_t* res,
                                 int*     res_len,
                                 uint8_t* k_asme);

  // Helpers
  virtual std::string get_mnc_str(const uint8_t* imsi_vec, std::string mcc_str) final;
  void                str_to_hex(std::string str, uint8_t* hex);

  // User data
  auth_algo_t auth_algo = auth_algo_milenage;
  uint8_t     amf[2]    = {}; // 3GPP 33.102 v10.0.0 Annex H
  uint8_t     op[16]    = {};
  uint8_t     opc[16]   = {};
  uint8_t     k[16]     = {};

  // Security variables
  uint8_t mac[8]   = {};
  uint8_t autn[16] = {};
};

} // namespace srsue

#endif // SRSUE_USIM_H
