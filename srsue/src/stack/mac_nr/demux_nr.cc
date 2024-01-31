/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

int32_t demux_nr::init(rlc_interface_mac* rlc_, phy_interface_mac_nr* phy_, mac_nr_interface_demux* mac_)
{
  rlc = rlc_;
  phy = phy_;
  mac = mac_;
  return SRSRAN_SUCCESS;
}

bool demux_nr::get_uecrid_successful()
{
  return is_uecrid_successful;
}

// Enqueues PDU and returns quickly
void demux_nr::push_pdu(srsran::unique_byte_buffer_t pdu, uint32_t tti)
{
  pdu_queue.push(std::move(pdu));
}

void demux_nr::push_bcch(srsran::unique_byte_buffer_t pdu)
{
  bcch_queue.push(std::move(pdu));
}

/* Demultiplexing of MAC PDU associated with a Temporal C-RNTI. The PDU will
 * remain in buffer until demultiplex_pending_pdu() is called.
 * This features is provided to enable the Random Access Procedure to decide
 * whether the PDU shall pass to upper layers or not, which depends on the
 * Contention Resolution result.
 *
 * Warning: this function does some processing here assuming ACK deadline is not an
 * issue here because Temp C-RNTI messages have small payloads
 */
void demux_nr::push_pdu_temp_crnti(srsran::unique_byte_buffer_t pdu, uint32_t tti)
{
  is_uecrid_successful = false;
  handle_pdu(rx_pdu_tcrnti, std::move(pdu));
}

void demux_nr::process_pdus()
{
  // Handle first BCCH
  while (not bcch_queue.empty()) {
    srsran::unique_byte_buffer_t pdu = bcch_queue.wait_pop();
    logger.debug(pdu->msg, pdu->N_bytes, "Handling MAC BCCH PDU (%d B)", pdu->N_bytes);
    rlc->write_pdu_bcch_dlsch(pdu->msg, pdu->N_bytes);
  }
  // Then user PDUs
  while (not pdu_queue.empty()) {
    srsran::unique_byte_buffer_t pdu = pdu_queue.wait_pop();
    handle_pdu(rx_pdu, std::move(pdu));
  }
}

/// Handling of DLSCH PDUs only
void demux_nr::handle_pdu(srsran::mac_sch_pdu_nr& pdu_buffer, srsran::unique_byte_buffer_t pdu)
{
  logger.debug(pdu->msg, pdu->N_bytes, "Handling MAC PDU (%d B)", pdu->N_bytes);

  pdu_buffer.init_rx();
  if (pdu_buffer.unpack(pdu->msg, pdu->N_bytes) != SRSRAN_SUCCESS) {
    return;
  }

  if (logger.info.enabled()) {
    fmt::memory_buffer str_buffer;
    pdu_buffer.to_string(str_buffer);
    logger.info("%s", srsran::to_c_str(str_buffer));
  }

  bool con_res_rxed = false;
  for (uint32_t i = 0; i < pdu_buffer.get_num_subpdus(); ++i) {
    srsran::mac_sch_subpdu_nr subpdu = pdu_buffer.get_subpdu(i);
    logger.debug("Handling subPDU %d/%d: rnti=0x%x lcid=%d, sdu_len=%d",
                 i + 1,
                 pdu_buffer.get_num_subpdus(),
                 subpdu.get_c_rnti(),
                 subpdu.get_lcid(),
                 subpdu.get_sdu_length());

    // Handle Contention Resolution UE ID and Timing Advance CE
    switch (subpdu.get_lcid()) {
      case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::DRX_CMD:
        logger.info("DRX CE not implemented.");
        break;
      case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::TA_CMD:
        logger.info("Received TA=%d.", subpdu.get_ta().ta_command);
        phy->set_timeadv(0, subpdu.get_ta().ta_command);
        break;
      case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::CON_RES_ID:
        con_res_rxed = true;
        logger.info("Received Contention Resolution ID 0x%lx", subpdu.get_ue_con_res_id_ce_packed());
        if (!is_uecrid_successful) {
          is_uecrid_successful = mac->received_contention_id(subpdu.get_ue_con_res_id_ce_packed());
        }
        break;
      default:
        if (!con_res_rxed or (con_res_rxed and is_uecrid_successful)) {
          if (subpdu.is_sdu()) {
            rlc->write_pdu(subpdu.get_lcid(), subpdu.get_sdu(), subpdu.get_sdu_length());
          }
        }
    }
  }
}

} // namespace srsue
