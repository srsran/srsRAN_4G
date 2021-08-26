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

#ifndef SRSRAN_RLC_TEST_DUMMY_H
#define SRSRAN_RLC_TEST_DUMMY_H

#include "srsran/interfaces/enb_rlc_interfaces.h"

namespace srsenb {

class rlc_dummy : public rlc_interface_mac
{
  int  read_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) { return SRSRAN_SUCCESS; }
  void write_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) {}
};

} // namespace srsenb

#endif // SRSRAN_RLC_TEST_DUMMY_H
