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

#ifndef SRSRAN_UE_GW_INTERFACES_H
#define SRSRAN_UE_GW_INTERFACES_H

#include "srsran/asn1/liblte_mme.h"
#include "srsran/common/byte_buffer.h"

namespace srsue {

class gw_interface_nas
{
public:
  /**
   * Informs GW about new EPS default bearer being created after attach accept
   * along with the assigned IP address.
   */
  virtual int
  setup_if_addr(uint32_t eps_bearer_id, uint8_t pdn_type, uint32_t ip_addr, uint8_t* ipv6_if_id, char* err_str) = 0;

  /**
   * Inform GW about the deactivation of a EPS bearer, e.g. during
   * detach
   */
  virtual int deactivate_eps_bearer(const uint32_t eps_bearer_id) = 0;

  /**
   * Informs GW about new traffic flow templates and their associated EPS bearer ID
   * All TFT are applied to a dedicated EPS bearer that has a linked default bearer
   */
  virtual int apply_traffic_flow_template(const uint8_t&                                 eps_bearer_id,
                                          const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft) = 0;

  typedef enum {
    TEST_LOOP_INACTIVE = 0,
    TEST_LOOP_MODE_A_ACTIVE,
    TEST_LOOP_MODE_B_ACTIVE,
    TEST_LOOP_MODE_C_ACTIVE
  } test_loop_mode_state_t;

  /**
   * Updates the test loop mode. The IP delay parameter is only valid for Mode B.
   * @param mode
   * @param ip_pdu_delay_ms The PDU delay in ms
   */
  virtual void set_test_loop_mode(const test_loop_mode_state_t mode, const uint32_t ip_pdu_delay_ms = 0) = 0;
};

class gw_interface_rrc
{
public:
  virtual void add_mch_port(uint32_t lcid, uint32_t port)             = 0;
  virtual bool is_running() = 0;
};

class gw_interface_pdcp
{
public:
  virtual void write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu)     = 0;
  virtual void write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
};

class gw_interface_stack : public gw_interface_nas, public gw_interface_rrc, public gw_interface_pdcp
{};

} // namespace srsue

#endif // SRSRAN_UE_GW_INTERFACES_H
