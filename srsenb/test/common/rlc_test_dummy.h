// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
