/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsenb/hdr/stack/upper/sdap.h"

namespace srsenb {

sdap::sdap() : m_log("SDAP") {}

bool sdap::init(pdcp_interface_sdap_nr* pdcp_, gtpu_interface_sdap_nr* gtpu_, srsue::gw_interface_pdcp* gw_)
{
  m_gtpu = gtpu_;
  m_pdcp = pdcp_;
  m_gw   = gw_;

  running = true;
  return true;
}

void sdap::stop()
{
  if (running) {
    running = false;
  }
}

void sdap::write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  // for now just forwards it
  if (m_gw) {
    m_gw->write_pdu(lcid, std::move(pdu));
  } else if (m_gtpu) {
    m_gtpu->write_pdu(rnti, lcid, std::move(pdu));
  }
}

void sdap::write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  m_pdcp->write_sdu(rnti, lcid, std::move(pdu));
}

} // namespace srsenb
