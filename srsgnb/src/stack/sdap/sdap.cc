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

#include "srsgnb/hdr/stack/sdap/sdap.h"

namespace srsenb {

sdap::sdap() {}

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

void sdap::write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  // for now just forwards it
  if (m_gw) {
    m_gw->write_pdu(lcid, std::move(pdu));
  } else if (m_gtpu) {
    m_gtpu->write_pdu(rnti, lcid, std::move(pdu));
  }
}

void sdap::write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  m_pdcp->write_sdu(rnti, lcid, std::move(pdu));
}

} // namespace srsenb
