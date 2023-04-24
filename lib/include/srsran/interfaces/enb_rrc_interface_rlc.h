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

#ifndef SRSRAN_ENB_RRC_INTERFACE_RLC_H
#define SRSRAN_ENB_RRC_INTERFACE_RLC_H

#include "srsran/common/byte_buffer.h"

namespace srsenb {

/// RRC interface for RLC
class rrc_interface_rlc
{
public:
  virtual void max_retx_attempted(uint16_t rnti)                                         = 0;
  virtual void protocol_failure(uint16_t rnti)                                           = 0;
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu) = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_RRC_INTERFACE_RLC_H
