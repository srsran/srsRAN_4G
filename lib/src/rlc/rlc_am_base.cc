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

#include "srsran/rlc/rlc_am_base.h"
#include <sstream>

namespace srsran {

bool rlc_am_is_control_pdu(uint8_t* payload)
{
  return ((*(payload) >> 7) & 0x01) == RLC_DC_FIELD_CONTROL_PDU;
}

bool rlc_am_is_control_pdu(byte_buffer_t* pdu)
{
  return rlc_am_is_control_pdu(pdu->msg);
}

} // namespace srsran
