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

#include "srsue/hdr/stack/mac_nr/mux_nr.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"

namespace srsue {

mux_nr::mux_nr(mac_interface_mux_nr& mac_, srslog::basic_logger& logger_) : mac(mac_), logger(logger_) {}

int32_t mux_nr::init(rlc_interface_mac* rlc_)
{
  rlc = rlc_;

  msg3_buff = srsran::make_byte_buffer();
  if (msg3_buff == nullptr) {
    return SRSRAN_ERROR;
  }

  rlc_buff = srsran::make_byte_buffer();
  if (rlc_buff == nullptr) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void mux_nr::reset()
{
  std::lock_guard<std::mutex> lock(mutex);
  this->logical_channels.clear();
}

int mux_nr::setup_lcid(const srsran::logical_channel_config_t& config)
{
  std::lock_guard<std::mutex> lock(mutex);
  return mux_base::setup_lcid(config);
}

srsran::unique_byte_buffer_t mux_nr::pdu_get_nolock(uint32_t max_pdu_len)
{
  // initialize MAC PDU
  srsran::unique_byte_buffer_t phy_tx_pdu = srsran::make_byte_buffer();
  if (phy_tx_pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return nullptr;
  }

  // verify buffer is large enough for UL grant
  if (phy_tx_pdu->get_tailroom() < max_pdu_len) {
    logger.error("Can't provide MAC PDU. Grant too big (%d < %d).", phy_tx_pdu->get_tailroom(), max_pdu_len);
    return nullptr;
  }

  logger.debug("Building new MAC PDU (%d B)", max_pdu_len);
  tx_pdu.init_tx(phy_tx_pdu.get(), max_pdu_len, true);

  if (msg3_is_pending() && mac.has_crnti()) {
    tx_pdu.add_crnti_ce(mac.get_crnti());
    msg3_transmitted();
  }

  // Pack normal UL data PDU
  int32_t remaining_len = tx_pdu.get_remaing_len(); // local variable to reserve space for CEs

  if (!msg3_is_pending() && add_bsr_ce == sbsr_ce) {
    // reserve space for SBSR
    remaining_len -= 2;
  }

  // First add MAC SDUs
  for (const auto& lc : logical_channels) {
    // TODO: Add proper priority handling
    logger.debug("Adding SDUs for LCID=%d (max %d B)", lc.lcid, remaining_len);
    while (remaining_len >= MIN_RLC_PDU_LEN) {
      // read RLC PDU
      rlc_buff->clear();
      uint8_t* rd = rlc_buff->msg;

      // Determine space for RLC
      int32_t subpdu_header_len = (remaining_len >= srsran::mac_sch_subpdu_nr::MAC_SUBHEADER_LEN_THRESHOLD ? 3 : 2);

      // Read PDU from RLC (account for subPDU header)
      int pdu_len = rlc->read_pdu(lc.lcid, rd, remaining_len - subpdu_header_len);

      if (pdu_len > remaining_len) {
        logger.error("Can't add SDU of %d B. Available space %d B", pdu_len, remaining_len);
        break;
      } else {
        // Add SDU if RLC has something to tx
        if (pdu_len > 0) {
          rlc_buff->N_bytes = pdu_len;
          logger.debug(rlc_buff->msg, rlc_buff->N_bytes, "Read %d B from RLC", rlc_buff->N_bytes);

          // add to MAC PDU and pack
          if (tx_pdu.add_sdu(lc.lcid, rlc_buff->msg, rlc_buff->N_bytes) != SRSRAN_SUCCESS) {
            logger.error("Error packing MAC PDU");
            break;
          }

          if (lc.lcid == 0 && msg3_is_pending()) {
            // TODO:
            msg3_transmitted();
          }

        } else {
          // couldn't read PDU from RLC
          break;
        }

        remaining_len -= (pdu_len + subpdu_header_len);
        logger.debug("%d B remaining PDU", remaining_len);
      }
    }
  }

  // check if
  if (add_bsr_ce == no_bsr) {
    // tell BSR proc we still have space in PDU and let it decide to create a padding BSR
    mac.set_padding_bytes(tx_pdu.get_remaing_len());
  }

  // Second add fixed-sized MAC CEs (e.g. SBSR)
  if (add_bsr_ce == sbsr_ce) {
    tx_pdu.add_sbsr_ce(mac.generate_sbsr());
    add_bsr_ce = no_bsr;
  } else if (add_bsr_ce == lbsr_ce) {
    // TODO: implement LBSR support
    tx_pdu.add_sbsr_ce(mac.generate_sbsr());
    add_bsr_ce = no_bsr;
  }

  // Lastly, add variable-sized MAC CEs

  // Pack PDU
  tx_pdu.pack();

  if (logger.info.enabled()) {
    // log pretty printed PDU
    fmt::memory_buffer buff;
    tx_pdu.to_string(buff);
    logger.info("%s", srsran::to_c_str(buff));
    logger.debug(phy_tx_pdu->msg, phy_tx_pdu->N_bytes, "Generated MAC PDU (%d B)", phy_tx_pdu->N_bytes);
  }

  return phy_tx_pdu;
}

void mux_nr::msg3_flush()
{
  msg3_buff->clear();
  msg3_state = msg3_state_t::none;
}

void mux_nr::msg3_prepare()
{
  msg3_state = msg3_state_t::pending;
}

void mux_nr::msg3_transmitted()
{
  msg3_state = msg3_state_t::transmitted;
}

bool mux_nr::msg3_is_transmitted()
{
  return msg3_state == msg3_state_t::transmitted;
}

bool mux_nr::msg3_is_pending()
{
  return msg3_state == msg3_state_t::pending;
}

bool mux_nr::msg3_is_empty()
{
  return msg3_buff->N_bytes == 0;
}

srsran::unique_byte_buffer_t mux_nr::get_pdu(uint32_t max_pdu_len)
{
  // Lock MAC PDU from current access from PHY workers (will be moved to UL HARQ)
  std::lock_guard<std::mutex> lock(mutex);
  return pdu_get_nolock(max_pdu_len);
}

srsran::unique_byte_buffer_t mux_nr::get_msg3(uint32_t max_pdu_len)
{
  // Lock MAC PDU from current access from PHY workers (will be moved to UL HARQ)
  std::lock_guard<std::mutex>  lock(mutex);
  srsran::unique_byte_buffer_t phy_tx_pdu = srsran::make_byte_buffer();

  if (max_pdu_len < msg3_buff->get_tailroom()) {
    if (msg3_is_empty()) {
      msg3_buff = pdu_get_nolock(max_pdu_len);
      if (msg3_buff == nullptr) {
        logger.error("Moving PDU from Mux unit to Msg3 buffer");
        return NULL;
      }
    }
    *phy_tx_pdu = *msg3_buff;
    return phy_tx_pdu;
  } else {
    logger.error("Msg3 size exceeds buffer");
    return nullptr;
  }
}

void mux_nr::generate_bsr_mac_ce(const srsran::bsr_format_nr_t& format)
{
  switch (format) {
    case srsran::SHORT_BSR:
      add_bsr_ce = sbsr_ce;
      break;
    case srsran::LONG_BSR:
      add_bsr_ce = lbsr_ce;
      break;
    default:
      logger.error("MUX can only be instructred to generate short or long BSRs.");
  }
}

} // namespace srsue
