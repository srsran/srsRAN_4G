/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsue/hdr/stack/mac_nr/demux_nr.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"

namespace srsue {

demux_nr::demux_nr(srslog::basic_logger& logger_) : logger(logger_) {}

demux_nr::~demux_nr() {}

int32_t demux_nr::init(rlc_interface_mac* rlc_)
{
  rlc = rlc_;
  return SRSRAN_SUCCESS;
}

// Enqueues PDU and returns quickly
void demux_nr::push_pdu(srsran::unique_byte_buffer_t pdu, uint32_t tti)
{
  pdu_queue.push(std::move(pdu));
}

void demux_nr::process_pdus()
{
  while (not pdu_queue.empty()) {
    srsran::unique_byte_buffer_t pdu = pdu_queue.wait_pop();
    handle_pdu(std::move(pdu));
  }
}

/// Handling of DLSCH PDUs only
void demux_nr::handle_pdu(srsran::unique_byte_buffer_t pdu)
{
  logger.debug(pdu->msg, pdu->N_bytes, "Handling MAC PDU (%d B)", pdu->N_bytes);

  rx_pdu.init_rx();
  if (rx_pdu.unpack(pdu->msg, pdu->N_bytes) != SRSRAN_SUCCESS) {
    return;
  }

  if (logger.info.enabled()) {
    fmt::memory_buffer str_buffer;
    rx_pdu.to_string(str_buffer);
    logger.info("%s", srsran::to_c_str(str_buffer));
  }

  for (uint32_t i = 0; i < rx_pdu.get_num_subpdus(); ++i) {
    srsran::mac_sch_subpdu_nr subpdu = rx_pdu.get_subpdu(i);
    logger.debug("Handling subPDU %d/%d: rnti=0x%x lcid=%d, sdu_len=%d",
                 i + 1,
                 rx_pdu.get_num_subpdus(),
                 subpdu.get_c_rnti(),
                 subpdu.get_lcid(),
                 subpdu.get_sdu_length());

    // Handle Timing Advance CE
    switch (subpdu.get_lcid()) {
      case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::DRX_CMD:
        logger.info("DRX CE not implemented.");
        break;
      case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::TA_CMD:
        logger.info("Timing Advance CE not implemented.");
        break;
      case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::CON_RES_ID:
        logger.info("Contention Resolution CE not implemented.");
        break;
      default:
        if (subpdu.is_sdu()) {
          rlc->write_pdu(subpdu.get_lcid(), subpdu.get_sdu(), subpdu.get_sdu_length());
        }
    }
  }
}

} // namespace srsue
