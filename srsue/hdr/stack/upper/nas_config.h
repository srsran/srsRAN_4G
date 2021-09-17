/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSUE_NAS_CONFIG_H
#define SRSUE_NAS_CONFIG_H

#include "srsran/interfaces/ue_nas_interfaces.h"
#include <string>

namespace srsue {

typedef struct {
  int airplane_t_on_ms  = -1;
  int airplane_t_off_ms = -1;
} nas_sim_args_t;

class nas_args_t
{
public:
  nas_args_t() : force_imsi_attach(false) {}
  ~nas_args_t() = default;
  std::string    apn_name;
  std::string    apn_protocol;
  std::string    apn_user;
  std::string    apn_pass;
  bool           force_imsi_attach;
  std::string    eia;
  std::string    eea;
  nas_sim_args_t sim;

  // 5G args
  std::string    ia5g;
  std::string    ea5g;
  std::vector<pdu_session_cfg_t> pdu_session_cfgs;
};

class nas_5g_args_t
{
public:
  nas_5g_args_t() : force_imsi_attach(false) {}
  ~nas_5g_args_t() = default;
  bool force_imsi_attach;

  // Need EPS sec capabilities in 5G
  std::string eia;
  std::string eea;

  // 5G Security capabilities
  std::string                    ia5g;
  std::string                    ea5g;
  std::vector<pdu_session_cfg_t> pdu_session_cfgs;
};

} // namespace srsue
#endif // SRSUE_NAS_COMMON_H
