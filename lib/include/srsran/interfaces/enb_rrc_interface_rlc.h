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
