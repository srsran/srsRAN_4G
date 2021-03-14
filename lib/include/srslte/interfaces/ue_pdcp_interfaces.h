/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSLTE_UE_PDCP_INTERFACES_H
#define SRSLTE_UE_PDCP_INTERFACES_H

#include "pdcp_interface_types.h"
#include "srslte/common/byte_buffer.h"

namespace srsue {

class pdcp_interface_rrc
{
public:
  virtual void reestablish()                                                                                        = 0;
  virtual void reestablish(uint32_t lcid)                                                                           = 0;
  virtual void reset()                                                                                              = 0;
  virtual void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, int sn = -1)                              = 0;
  virtual void add_bearer(uint32_t lcid, srslte::pdcp_config_t cnfg)                                                = 0;
  virtual void change_lcid(uint32_t old_lcid, uint32_t new_lcid)                                                    = 0;
  virtual void config_security(uint32_t lcid, srslte::as_security_config_t sec_cfg)                                 = 0;
  virtual void config_security_all(srslte::as_security_config_t sec_cfg)                                            = 0;
  virtual void enable_integrity(uint32_t lcid, srslte::srslte_direction_t direction)                                = 0;
  virtual void enable_encryption(uint32_t                   lcid,
                                 srslte::srslte_direction_t direction = srslte::srslte_direction_t::DIRECTION_TXRX) = 0;
  virtual void send_status_report()                                                                                 = 0;
  virtual void send_status_report(uint32_t lcid)                                                                    = 0;
};

class pdcp_interface_rlc
{
public:
  /* RLC calls PDCP to push a PDCP PDU. */
  virtual void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu)              = 0;
  virtual void write_pdu_bcch_bch(srslte::unique_byte_buffer_t sdu)                    = 0;
  virtual void write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t sdu)                  = 0;
  virtual void write_pdu_pcch(srslte::unique_byte_buffer_t sdu)                        = 0;
  virtual void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t sdu)          = 0;
  virtual void notify_delivery(uint32_t lcid, const srslte::pdcp_sn_vector_t& pdcp_sn) = 0;
  virtual void notify_failure(uint32_t lcid, const srslte::pdcp_sn_vector_t& pdcp_sn)  = 0;
};

class pdcp_interface_gw
{
public:
  virtual void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu) = 0;
  virtual bool is_lcid_enabled(uint32_t lcid)                             = 0;
};

// STACK interface for GW
class stack_interface_gw : public pdcp_interface_gw
{
public:
  virtual bool is_registered()         = 0;
  virtual bool start_service_request() = 0;
};

} // namespace srsue

#endif // SRSLTE_UE_PDCP_INTERFACES_H
