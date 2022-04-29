/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_UE_USIM_INTERFACES_H
#define SRSRAN_UE_USIM_INTERFACES_H

#include "rrc_interface_types.h"
#include <string>

namespace srsue {

enum auth_result_t { AUTH_OK, AUTH_FAILED, AUTH_SYNCH_FAILURE };

// USIM interface for NAS
class usim_interface_nas
{
public:
  virtual std::string get_imsi_str()                                    = 0;
  virtual std::string get_imei_str()                                    = 0;
  virtual bool        get_imsi_vec(uint8_t* imsi_, uint32_t n)          = 0;
  virtual bool        get_imei_vec(uint8_t* imei_, uint32_t n)          = 0;
  virtual bool        get_home_plmn_id(srsran::plmn_id_t* home_plmn_id) = 0;
  // Get the home mcc as bytes array
  virtual bool get_home_mcc_bytes(uint8_t* mcc_, uint32_t n) = 0;
  // Get the home mnc as byte array
  virtual bool get_home_mnc_bytes(uint8_t* mnc_, uint32_t n) = 0;
  // Get the home msin in bytes array encoded as bcd
  virtual bool          get_home_msin_bcd(uint8_t* msin_, uint32_t n)     = 0;
  virtual auth_result_t generate_authentication_response(uint8_t* rand,
                                                         uint8_t* autn_enb,
                                                         uint16_t mcc,
                                                         uint16_t mnc,
                                                         uint8_t* res,
                                                         int*     res_len,
                                                         uint8_t* k_asme) = 0;

  virtual auth_result_t generate_authentication_response_5g(uint8_t*    rand,
                                                            uint8_t*    autn_enb,
                                                            const char* serving_network_name,
                                                            uint8_t*    abba,
                                                            uint32_t    abba_len,
                                                            uint8_t*    res_star,
                                                            uint8_t*    k_amf) = 0;

  virtual void generate_nas_keys(uint8_t*                            k_asme,
                                 uint8_t*                            k_nas_enc,
                                 uint8_t*                            k_nas_int,
                                 srsran::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                 srsran::INTEGRITY_ALGORITHM_ID_ENUM integ_algo) = 0;

  virtual bool generate_nas_keys_5g(uint8_t*                            k_amf,
                                    uint8_t*                            k_nas_enc,
                                    uint8_t*                            k_nas_int,
                                    srsran::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                    srsran::INTEGRITY_ALGORITHM_ID_ENUM integ_algo) = 0;
};

// USIM interface for RRC
class usim_interface_rrc
{
public:
  virtual void generate_as_keys(uint8_t* k_asme, uint32_t count_ul, srsran::as_security_config_t* sec_cfg)        = 0;
  virtual void generate_as_keys_ho(uint32_t pci, uint32_t earfcn, int ncc, srsran::as_security_config_t* sec_cfg) = 0;
  virtual void store_keys_before_ho(const srsran::as_security_config_t& as_cfg)                                   = 0;
  virtual void restore_keys_from_failed_ho(srsran::as_security_config_t* as_cfg)                                  = 0;
};

class usim_interface_rrc_nr
{
public:
  virtual void
  generate_nr_as_keys(srsran::as_key_t& k_amf, uint32_t count_ul, srsran::as_security_config_t* sec_cfg) = 0;
  virtual bool generate_nr_context(uint16_t sk_counter, srsran::as_security_config_t* sec_cfg)           = 0;
  virtual bool update_nr_context(srsran::as_security_config_t* sec_cfg)                                  = 0;
};

} // namespace srsue

#endif // SRSRAN_UE_USIM_INTERFACES_H
