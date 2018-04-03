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

#ifndef SRSUE_USIM_H
#define SRSUE_USIM_H

#include <string>
#include "srslte/common/log.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/security.h"

namespace srsue {

typedef enum{
  auth_algo_milenage = 0,
  auth_algo_xor,
}auth_algo_t;

typedef struct{
  std::string algo;
  std::string op;
  std::string imsi;
  std::string imei;
  std::string k;
}usim_args_t;

class usim
    :public usim_interface_nas
    ,public usim_interface_rrc
{
public:
  usim();
  void init(usim_args_t *args, srslte::log *usim_log_);
  void stop();

  // NAS interface
  std::string get_imsi_str();
  std::string get_imei_str();

  bool get_imsi_vec(uint8_t* imsi_, uint32_t n);
  bool get_imei_vec(uint8_t* imei_, uint32_t n);
  bool get_home_plmn_id(LIBLTE_RRC_PLMN_IDENTITY_STRUCT *home_plmn_id);

  void generate_authentication_response(uint8_t  *rand,
                                        uint8_t  *autn_enb,
                                        uint16_t  mcc,
                                        uint16_t  mnc,
                                        bool     *net_valid,
                                        uint8_t  *res,
                                        uint8_t  *k_asme);

  void generate_nas_keys(uint8_t *k_asme,
                         uint8_t *k_nas_enc,
                         uint8_t *k_nas_int,
                         srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                         srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo);

  // RRC interface
  void generate_as_keys(uint8_t *k_asme,
                        uint32_t count_ul,
                        uint8_t *k_rrc_enc,
                        uint8_t *k_rrc_int,
                        uint8_t *k_up_enc,
                        uint8_t *k_up_int,
                        srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                        srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo);

  void generate_as_keys_ho(uint32_t pci,
                           uint32_t earfcn,
                           int ncc,
                           uint8_t *k_rrc_enc,
                           uint8_t *k_rrc_int,
                           uint8_t *k_up_enc,
                           uint8_t *k_up_int,
                           srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                           srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo);


private:
  void gen_auth_res_milenage( uint8_t  *rand,
                              uint8_t  *autn_enb,
                              uint16_t  mcc,
                              uint16_t  mnc,
                              bool     *net_valid,
                              uint8_t  *res,
                              uint8_t  *k_asme);
  void gen_auth_res_xor(      uint8_t  *rand,
                              uint8_t  *autn_enb,
                              uint16_t  mcc,
                              uint16_t  mnc,
                              bool     *net_valid,
                              uint8_t  *res,
                              uint8_t  *k_asme);
  void str_to_hex(std::string str, uint8_t *hex);

  srslte::log *usim_log;

  // User data
  auth_algo_t auth_algo;
  uint8_t     amf[2];  // 3GPP 33.102 v10.0.0 Annex H
  uint8_t     op[16];
  uint64_t    imsi;
  uint64_t    imei;
  uint8_t     k[16];

  std::string imsi_str;
  std::string imei_str;

  // Security variables
  uint8_t     rand[16];
  uint8_t     ck[16];
  uint8_t     ik[16];
  uint8_t     ak[6];
  uint8_t     mac[8];
  uint8_t     autn[16];
  uint8_t     k_asme[32];
  uint8_t     nh[32];
  uint8_t     k_enb[32];
  uint8_t     k_enb_star[32];

  uint32_t    current_ncc;

  bool initiated;

};

} // namespace srsue


#endif // SRSUE_USIM_H
