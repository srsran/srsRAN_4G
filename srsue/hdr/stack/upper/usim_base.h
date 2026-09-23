/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSUE_USIM_BASE_H
#define SRSUE_USIM_BASE_H

#include "srsran/common/common.h"
#include "srsran/common/security.h"
#include "srsran/interfaces/ue_usim_interfaces.h"
#include "srsran/srslog/srslog.h"
#include <string>

namespace srsue {

#define AKA_RAND_LEN 16
#define AKA_AUTN_LEN 16
#define AKA_AUTS_LEN 14
#define RES_MAX_LEN 16
#define MAC_LEN 8
#define IK_LEN 16
#define CK_LEN 16
#define AK_LEN 6
#define SQN_LEN 6

#define KEY_LEN 32

typedef enum {
  auth_algo_milenage = 0,
  auth_algo_xor,
} auth_algo_t;

class usim_args_t
{
public:
  usim_args_t() : using_op(false) {}
  ~usim_args_t() = default;
  std::string mode;
  std::string algo;
  bool        using_op;
  std::string op;
  std::string opc;
  std::string imsi;
  std::string imei;
  std::string k;
  std::string pin;
  std::string reader;
  // SUCI (SUPI protection, TS 33.501)
  std::string protection_scheme;         // "null" (default) or "profile_a"
  std::string home_network_pubkey;       // 32-byte home network public key as hex (profile A)
  std::string routing_indicator;         // up to 4 decimal digits, default "0000"
  int         home_network_pubkey_id = 0;
};

class usim_base : public usim_interface_nas, public usim_interface_rrc, public usim_interface_rrc_nr
{
public:
  explicit usim_base(srslog::basic_logger& logger);
  virtual ~usim_base();
  static std::unique_ptr<usim_base> get_instance(usim_args_t* args, srslog::basic_logger& logger);

  virtual int  init(usim_args_t* args) = 0;
  virtual void stop()                  = 0;

  // NAS interface
  std::string get_imsi_str() final;
  std::string get_imei_str() final;

  bool get_imsi_vec(uint8_t* imsi_, uint32_t n) final;
  bool get_home_mcc_bytes(uint8_t* mcc_, uint32_t n) final;
  bool get_home_mnc_bytes(uint8_t* mnc_, uint32_t n) final;
  bool get_home_msin_bcd(uint8_t* msin_, uint32_t n) final;
  bool get_imei_vec(uint8_t* imei_, uint32_t n) final;
  bool get_home_plmn_id(srsran::plmn_id_t* home_plmn_id) final;

  // SUCI (SUPI protection, TS 33.501)
  uint8_t get_home_protection_scheme_id() final;
  uint8_t get_home_network_pubkey_id() final;
  void    get_home_routing_indicator(uint8_t routing_indicator[4]) final;
  bool    generate_suci_scheme_output(std::vector<uint8_t>& scheme_output) final;

  virtual auth_result_t generate_authentication_response(uint8_t* rand,
                                                         uint8_t* autn_enb,
                                                         uint16_t mcc,
                                                         uint16_t mnc,
                                                         uint8_t* res,
                                                         int*     res_len,
                                                         uint8_t* k_asme) = 0;

  void generate_nas_keys(uint8_t*                            k_asme,
                         uint8_t*                            k_nas_enc,
                         uint8_t*                            k_nas_int,
                         srsran::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                         srsran::INTEGRITY_ALGORITHM_ID_ENUM integ_algo) final;

  // RRC interface
  void generate_as_keys(uint8_t* k_asme, uint32_t count_ul, srsran::as_security_config_t* sec_cfg) final;
  void generate_as_keys_ho(uint32_t pci, uint32_t earfcn, int ncc, srsran::as_security_config_t* sec_cfg) final;
  void store_keys_before_ho(const srsran::as_security_config_t& as_ctx) final;
  void restore_keys_from_failed_ho(srsran::as_security_config_t* as_ctx) final;

  // NR RRC interface
  void generate_nr_as_keys(srsran::as_key_t& k_amf, uint32_t count_ul, srsran::as_security_config_t* sec_cfg) final;
  bool generate_nr_context(uint16_t sk_counter, srsran::as_security_config_t* sec_cfg) final;
  bool update_nr_context(srsran::as_security_config_t* sec_cfg) final;

  // 5G NAS interface
  virtual auth_result_t generate_authentication_response_5g(uint8_t*    rand,
                                                            uint8_t*    autn_enb,
                                                            const char* serving_network_name,
                                                            uint8_t*    abba,
                                                            uint32_t    abba_len,
                                                            uint8_t*    res_star,
                                                            uint8_t*    k_amf) = 0;

  bool generate_nas_keys_5g(uint8_t*                            k_amf,
                            uint8_t*                            k_nas_enc,
                            uint8_t*                            k_nas_int,
                            srsran::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                            srsran::INTEGRITY_ALGORITHM_ID_ENUM integ_algo);
  // Helpers
  std::string         get_mcc_str(const uint8_t* imsi_vec);
  virtual std::string get_mnc_str(const uint8_t* imsi_vec, std::string mcc_str) = 0;

protected:
  // Parse and validate the SUCI protection configuration from the USIM args.
  void set_suci_config(usim_args_t* args);

  bool initiated = false;

  // Logging
  srslog::basic_logger& logger;

  // User data
  // 3GPP 33.102 v10.0.0 Annex H
  uint64_t    imsi = 0;
  uint64_t    imei = 0;
  std::string imsi_str;
  std::string imei_str;

  // SUCI (SUPI protection, TS 33.501)
  uint8_t suci_protection_scheme = 0; // 0 = null scheme, 1 = ECIES profile A
  uint8_t suci_hn_pubkey[32]     = {};
  bool    suci_hn_pubkey_valid   = false;
  uint8_t suci_hn_pubkey_id      = 0;
  uint8_t suci_routing_indicator[4] = {0, 0, 0, 0};

  // Security variables
  uint8_t ck[CK_LEN]             = {};
  uint8_t ik[IK_LEN]             = {};
  uint8_t ak[AK_LEN]             = {};
  uint8_t k_asme[KEY_LEN]        = {};
  uint8_t k_enb_star[KEY_LEN]    = {};
  uint8_t k_enb_initial[KEY_LEN] = {};
  uint8_t auts[AKA_AUTS_LEN]     = {};

  srsran::as_key_t k_gnb_initial = {};

  // Current K_eNB context (K_eNB, NH and NCC)
  srsran::k_enb_context_t k_enb_ctx = {};
  srsran::k_gnb_context_t k_gnb_ctx = {};

  // Helpers to restore security context if HO fails
  srsran::k_enb_context_t      old_k_enb_ctx = {};
  srsran::as_security_config_t old_as_ctx    = {};
};

} // namespace srsue

#endif // SRSUE_USIM_BASE_H
