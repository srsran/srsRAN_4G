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

#ifndef SRSRAN_ENB_GTPU_INTERFACES_H
#define SRSRAN_ENB_GTPU_INTERFACES_H

#include "srsran/adt/expected.h"
#include "srsran/common/byte_buffer.h"

namespace srsenb {

// GTPU interface for PDCP
class gtpu_interface_pdcp
{
public:
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
};

// GTPU interface for RRC
class gtpu_interface_rrc
{
public:
  struct bearer_props {
    bool     forward_from_teidin_present = false;
    bool     flush_before_teidin_present = false;
    uint32_t forward_from_teidin         = 0;
    uint32_t flush_before_teidin         = 0;
  };

  virtual srsran::expected<uint32_t>
               add_bearer(uint16_t rnti, uint32_t lcid, uint32_t addr, uint32_t teid_out, const bearer_props* props = nullptr) = 0;
  virtual void set_tunnel_status(uint32_t teidin, bool dl_active)    = 0;
  virtual void rem_bearer(uint16_t rnti, uint32_t lcid)              = 0;
  virtual void mod_bearer_rnti(uint16_t old_rnti, uint16_t new_rnti) = 0;
  virtual void rem_user(uint16_t rnti)                               = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_GTPU_INTERFACES_H
