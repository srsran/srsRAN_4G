// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_ENB_RRC_INTERFACE_PDCP_H
#define SRSRAN_ENB_RRC_INTERFACE_PDCP_H

#include "srsran/common/byte_buffer.h"

namespace srsenb {

/// RRC interface for PDCP
class rrc_interface_pdcp
{
public:
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
  virtual void notify_pdcp_integrity_error(uint16_t rnti, uint32_t lcid)                 = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_RRC_INTERFACE_PDCP_H
