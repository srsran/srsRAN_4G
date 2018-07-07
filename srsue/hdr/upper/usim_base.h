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

#ifndef SRSUE_USIM_BASE_H
#define SRSUE_USIM_BASE_H

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
  std::string mode;
  std::string algo;
  bool using_op;
  std::string op;
  std::string opc;
  std::string imsi;
  std::string imei;
  std::string k;
  std::string pin;
  std::string reader;
}usim_args_t;

class usim_base
    :public usim_interface_nas
    ,public usim_interface_rrc
{
public:
  usim_base();
  virtual ~usim_base();
  static usim_base* get_instance(usim_args_t *args, srslte::log *usim_log_);

  virtual int init(usim_args_t *args, srslte::log *usim_log_) = 0;
  virtual void stop() = 0;

  // NAS interface
  virtual std::string get_imsi_str() = 0;
  virtual std::string get_imei_str() = 0;

  virtual bool get_imsi_vec(uint8_t* imsi_, uint32_t n) = 0;
  virtual bool get_imei_vec(uint8_t* imei_, uint32_t n) = 0;
  virtual bool get_home_plmn_id(LIBLTE_RRC_PLMN_IDENTITY_STRUCT *home_plmn_id) = 0;

  virtual auth_result_t generate_authentication_response(uint8_t  *rand,
                                                         uint8_t  *autn_enb,
                                                         uint16_t  mcc,
                                                         uint16_t  mnc,
                                                         uint8_t  *res,
                                                         int      *res_len,
                                                         uint8_t  *k_asme) = 0;

  virtual void generate_nas_keys(uint8_t *k_asme,
                         uint8_t *k_nas_enc,
                         uint8_t *k_nas_int,
                         srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                         srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo) = 0;

  // RRC interface
  virtual void generate_as_keys(uint8_t *k_asme,
                        uint32_t count_ul,
                        uint8_t *k_rrc_enc,
                        uint8_t *k_rrc_int,
                        uint8_t *k_up_enc,
                        uint8_t *k_up_int,
                        srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                        srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo) = 0;

  virtual void generate_as_keys_ho(uint32_t pci,
                           uint32_t earfcn,
                           int ncc,
                           uint8_t *k_rrc_enc,
                           uint8_t *k_rrc_int,
                           uint8_t *k_up_enc,
                           uint8_t *k_up_int,
                           srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                           srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo) = 0;
};

} // namespace srsue

#endif // SRSUE_USIM_BASE_H
