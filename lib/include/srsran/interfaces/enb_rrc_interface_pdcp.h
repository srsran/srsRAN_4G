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
