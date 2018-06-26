/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#ifndef SRSLTE_INTERFACES_COMMON_H
#define SRSLTE_INTERFACES_COMMON_H

#include "srslte/common/timers.h"
#include "srslte/common/security.h"
#include "srslte/asn1/liblte_rrc.h"
#include <string>


namespace srslte {

class srslte_nas_config_t
{
public:
  srslte_nas_config_t(uint32_t lcid_ = 0, std::string apn_ = "", std::string user_ = "", std::string pass_ = "", bool force_imsi_attach_ = false)
    :lcid(lcid_),
     apn(apn_),
     user(user_),
     pass(pass_),
     force_imsi_attach(force_imsi_attach_)
    {}

  uint32_t    lcid;
  std::string apn;
  std::string user;
  std::string pass;
  bool        force_imsi_attach;
};


class srslte_gw_config_t
{
public:
  srslte_gw_config_t(uint32_t lcid_ = 0)
  :lcid(lcid_)
  {}

  uint32_t lcid;
};


class srslte_pdcp_config_t
{
public:
  srslte_pdcp_config_t(bool is_control_ = false, bool is_data_ = false, uint8_t direction_ = SECURITY_DIRECTION_UPLINK)
    :direction(direction_)
    ,is_control(is_control_)
    ,is_data(is_data_)
    ,sn_len(12) {}

  uint8_t  direction;
  bool     is_control;
  bool     is_data;
  uint8_t  sn_len;

  // TODO: Support the following configurations
  // bool do_rohc;
};

class mac_interface_timers
{
public: 
  /* Timer services with ms resolution. 
   * timer_id must be lower than MAC_NOF_UPPER_TIMERS
   */
  virtual timers::timer* timer_get(uint32_t timer_id)  = 0;
  virtual void           timer_release_id(uint32_t timer_id) = 0;
  virtual uint32_t       timer_get_unique_id() = 0;
};

class read_pdu_interface
{
public:
  virtual int read_pdu(uint32_t lcid, uint8_t *payload, uint32_t requested_bytes) = 0; 
};

}

#endif // SRSLTE_INTERFACES_COMMON_H
