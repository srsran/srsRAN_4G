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

int mux_nr::setup_lcid(const srsran::logical_channel_config_t& config)
{
  std::lock_guard<std::mutex> lock(mutex);
  return mux_base::setup_lcid(config);
}

srsran::unique_byte_buffer_t mux_nr::get_pdu(uint32_t max_pdu_len)
{
  // initialize MAC PDU
  srsran::unique_byte_buffer_t phy_tx_pdu = srsran::make_byte_buffer();
  if (phy_tx_pdu == nullptr) {
    return nullptr;
  }

  tx_pdu.init_tx(phy_tx_pdu.get(), max_pdu_len, true);

  if (msg3_is_pending()) {
    // If message 3 is pending pack message 3 for uplink transmission
    // Use the CRNTI which is provided in the RRC reconfiguration (only for DC mode maybe other)
    tx_pdu.add_crnti_ce(mac.get_crnti());
    srsran::mac_sch_subpdu_nr::lcg_bsr_t sbsr = {};
    sbsr.lcg_id                               = 0;
    sbsr.buffer_size                          = 1;
    tx_pdu.add_sbsr_ce(sbsr);
    logger.info("Generated msg3 with RNTI 0x%x", mac.get_crnti());
    msg3_transmitted();
  } else {
    // Pack normal UL data PDU

    // TODO: Add proper priority handling
    for (const auto& lc : logical_channels) {
      while (tx_pdu.get_remaing_len() >= MIN_RLC_PDU_LEN) {
        // read RLC PDU
        rlc_buff->clear();
        uint8_t* rd      = rlc_buff->msg;
        int      pdu_len = 0;
        pdu_len          = rlc->read_pdu(lc.lcid, rd, tx_pdu.get_remaing_len() - 2);

        // Add SDU if RLC has something to tx
        if (pdu_len > 0) {
          rlc_buff->N_bytes = pdu_len;
          logger.info(rlc_buff->msg, rlc_buff->N_bytes, "Read %d B from RLC", rlc_buff->N_bytes);

          // add to MAC PDU and pack
          if (tx_pdu.add_sdu(lc.lcid, rlc_buff->msg, rlc_buff->N_bytes) != SRSRAN_SUCCESS) {
            logger.error("Error packing MAC PDU");
          }
        } else {
          break;
        }
      }
    }
  }

  // Pack PDU
  tx_pdu.pack();

  logger.info(phy_tx_pdu->msg, phy_tx_pdu->N_bytes, "Generated MAC PDU (%d B)", phy_tx_pdu->N_bytes);

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

void mux_nr::generate_bsr_mac_ce() {}

} // namespace srsue
