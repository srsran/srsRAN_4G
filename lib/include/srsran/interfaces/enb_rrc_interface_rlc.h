// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
