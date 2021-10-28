/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/common/byte_buffer.h"
#include "srsran/interfaces/pdcp_interface_types.h"
#include <map>

#ifndef SRSRAN_ENB_PDCP_INTERFACES_H
#define SRSRAN_ENB_PDCP_INTERFACES_H

namespace srsenb {

// PDCP interface for GTPU
class pdcp_interface_gtpu
{
public:
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu, int pdcp_sn = -1) = 0;
  virtual std::map<uint32_t, srsran::unique_byte_buffer_t> get_buffered_pdus(uint16_t rnti, uint32_t lcid) = 0;
};

// PDCP interface for RRC
class pdcp_interface_rrc
{
public:
  virtual void set_enabled(uint16_t rnti, uint32_t lcid, bool enable)                                      = 0;
  virtual void reset(uint16_t rnti)                                                                        = 0;
  virtual void add_user(uint16_t rnti)                                                                     = 0;
  virtual void rem_user(uint16_t rnti)                                                                     = 0;
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu, int pdcp_sn = -1) = 0;
  virtual void add_bearer(uint16_t rnti, uint32_t lcid, const srsran::pdcp_config_t& cnfg)                 = 0;
  virtual void del_bearer(uint16_t rnti, uint32_t lcid)                                                    = 0;
  virtual void config_security(uint16_t rnti, uint32_t lcid, const srsran::as_security_config_t& sec_cfg)  = 0;
  virtual void enable_integrity(uint16_t rnti, uint32_t lcid)                                              = 0;
  virtual void enable_encryption(uint16_t rnti, uint32_t lcid)                                             = 0;
  virtual void send_status_report(uint16_t rnti)                                                           = 0;
  virtual void send_status_report(uint16_t rnti, uint32_t lcid)                                            = 0;
  virtual bool get_bearer_state(uint16_t rnti, uint32_t lcid, srsran::pdcp_lte_state_t* state)             = 0;
  virtual bool set_bearer_state(uint16_t rnti, uint32_t lcid, const srsran::pdcp_lte_state_t& state)       = 0;
  virtual void reestablish(uint16_t rnti)                                                                  = 0;
};

// PDCP interface for RLC
class pdcp_interface_rlc
{
public:
  /* RLC calls PDCP to push a PDCP PDU. */
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu)               = 0;
  virtual void notify_delivery(uint16_t rnti, uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sns) = 0;
  virtual void notify_failure(uint16_t rnti, uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sns)  = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_PDCP_INTERFACES_H
