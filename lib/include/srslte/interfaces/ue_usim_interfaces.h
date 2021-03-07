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

#ifndef SRSLTE_UE_USIM_INTERFACES_H
#define SRSLTE_UE_USIM_INTERFACES_H

#include "rrc_interface_types.h"
#include <string>

namespace srsue {

enum auth_result_t { AUTH_OK, AUTH_FAILED, AUTH_SYNCH_FAILURE };

// USIM interface for NAS
class usim_interface_nas
{
public:
  virtual std::string   get_imsi_str()                                                    = 0;
  virtual std::string   get_imei_str()                                                    = 0;
  virtual bool          get_imsi_vec(uint8_t* imsi_, uint32_t n)                          = 0;
  virtual bool          get_imei_vec(uint8_t* imei_, uint32_t n)                          = 0;
  virtual bool          get_home_plmn_id(srslte::plmn_id_t* home_plmn_id)                 = 0;
  virtual auth_result_t generate_authentication_response(uint8_t* rand,
                                                         uint8_t* autn_enb,
                                                         uint16_t mcc,
                                                         uint16_t mnc,
                                                         uint8_t* res,
                                                         int*     res_len,
                                                         uint8_t* k_asme)                 = 0;
  virtual void          generate_nas_keys(uint8_t*                            k_asme,
                                          uint8_t*                            k_nas_enc,
                                          uint8_t*                            k_nas_int,
                                          srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                          srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo) = 0;
};

// USIM interface for RRC
class usim_interface_rrc
{
public:
  virtual void generate_as_keys(uint8_t* k_asme, uint32_t count_ul, srslte::as_security_config_t* sec_cfg)        = 0;
  virtual void generate_as_keys_ho(uint32_t pci, uint32_t earfcn, int ncc, srslte::as_security_config_t* sec_cfg) = 0;
  virtual void store_keys_before_ho(const srslte::as_security_config_t& as_cfg)                                   = 0;
  virtual void restore_keys_from_failed_ho(srslte::as_security_config_t* as_cfg)                                  = 0;
};

class usim_interface_rrc_nr
{
public:
  virtual void generate_nr_context(uint16_t sk_counter, srslte::as_security_config_t* sec_cfg) = 0;
  virtual void update_nr_context(srslte::as_security_config_t* sec_cfg)                        = 0;
};

} // namespace srsue

#endif // SRSLTE_UE_USIM_INTERFACES_H
