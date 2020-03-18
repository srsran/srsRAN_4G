/*
 * Copyright 2020 Software Radio Systems Limited
 * Author: Vadim Yanitskiy <axilirator@gmail.com>
 * Sponsored by Positive Technologies
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

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/nas_pcap.h"
#include "srslte/common/security.h"
#include "srslte/common/stack_procedure.h"
#include "srslte/interfaces/ue_interfaces.h"

#include "srsue/hdr/stack/upper/nas.h"
#include "srsue/hdr/stack/upper/nas_common.h"
#include "srsue/hdr/stack/upper/nas_ext.h"
#include "srsue/hdr/stack/upper/nas_extif_unix.h"
#include "srsue/hdr/stack/upper/nas_metrics.h"

using namespace srslte;

namespace srsue {

void nas_ext::init(usim_interface_nas* usim_, rrc_interface_nas* rrc_, gw_interface_nas* gw_)
{
  usim = usim_;
  rrc  = rrc_;
  gw   = gw_;

  // RRCTL PDU handler
  auto rrctl_rx_cb = [this](const srslte::byte_buffer_t& pdu) {
    rrctl::proto::msg_type type;
    rrctl::proto::msg_disc disc;
    const uint8_t* payload;
    std::string desc;
    uint16_t length;

    // Parse the message header
    try {
      payload = rrctl::codec::dec_hdr(pdu, type, disc, length);
      desc = rrctl::proto::msg_hdr_desc(type, disc, length);
      nas_log->info("Got RRCTL message: %s\n", desc.c_str());
    } catch (const rrctl::codec::error& e) {
      nas_log->warning("Got malformed RRCTL message: %s\n", e.what());
      return;
    }

    // Call the corresponding handler
    switch (type) {
    case rrctl::proto::RRCTL_RESET:
      handle_rrctl_reset(disc, payload, length);
      break;
    case rrctl::proto::RRCTL_PLMN_SEARCH:
      handle_rrctl_plmn_search(disc, payload, length);
      break;
    case rrctl::proto::RRCTL_PLMN_SELECT:
      handle_rrctl_plmn_select(disc, payload, length);
      break;
    case rrctl::proto::RRCTL_CONN_ESTABLISH:
      handle_rrctl_conn_establish(disc, payload, length);
      break;
    case rrctl::proto::RRCTL_DATA:
      handle_rrctl_data(disc, payload, length);
      break;
    case rrctl::proto::RRCTL_CONN_RELEASE:
    default:
      nas_log->warning("%s is not handled\n", desc.c_str());
    }
  };

  std::unique_ptr<nas_extif_unix> iface_(new nas_extif_unix(nas_log, rrctl_rx_cb, cfg.sock_path));
  iface = std::move(iface_);
}

void nas_ext::rrctl_send_confirm(rrctl::proto::msg_type type)
{
  srslte::byte_buffer_t pdu;

  rrctl::codec::enc_hdr(pdu, type, rrctl::proto::RRCTL_CNF);
  iface->write(pdu);
}

void nas_ext::rrctl_send_error(rrctl::proto::msg_type type)
{
  srslte::byte_buffer_t pdu;

  rrctl::codec::enc_hdr(pdu, type, rrctl::proto::RRCTL_ERR);
  iface->write(pdu);
}

void nas_ext::handle_rrctl_reset(rrctl::proto::msg_disc disc, const uint8_t* msg, size_t len)
{
  // TODO: do we need to reset anything?
  rrctl_send_confirm(rrctl::proto::RRCTL_RESET);
}

void nas_ext::handle_rrctl_plmn_search(rrctl::proto::msg_disc disc, const uint8_t* msg, size_t len)
{
  // Response to be sent when RRC responds
  rrc->plmn_search();
}

void nas_ext::handle_rrctl_plmn_select(rrctl::proto::msg_disc disc, const uint8_t* msg, size_t len)
{
  std::pair<uint16_t, uint16_t> mcc_mnc;
  srslte::plmn_id_t plmn_id;

  // Parse PLMN ID
  try {
    rrctl::codec::dec_plmn_select_req(mcc_mnc, msg, len);
  } catch (const rrctl::codec::error& e) {
    nas_log->warning("Failed to parse RRCTL message: %s\n", e.what());
    rrctl_send_error(rrctl::proto::RRCTL_PLMN_SELECT);
    return;
  }

  if (plmn_id.from_number(mcc_mnc.first, mcc_mnc.second) != SRSLTE_SUCCESS) {
    nas_log->warning("Failed to parse PLMN ID from PLMN Select Request\n");
    rrctl_send_error(rrctl::proto::RRCTL_PLMN_SELECT);
  }

  rrc->plmn_select(plmn_id);
  rrctl_send_confirm(rrctl::proto::RRCTL_PLMN_SELECT);
}

void nas_ext::handle_rrctl_conn_establish(rrctl::proto::msg_disc disc, const uint8_t* msg, size_t len)
{
  srslte::establishment_cause_t cause;
  const uint8_t* pdu;
  size_t pdu_len;

  try {
    rrctl::codec::dec_conn_establish_req(cause, pdu, pdu_len, msg, len);
  } catch (const rrctl::codec::error& e) {
    nas_log->warning("Failed to parse RRCTL message: %s\n", e.what());
    rrctl_send_error(rrctl::proto::RRCTL_CONN_ESTABLISH);
    return;
  }

  // Allocate a new NAS PDU on heap
  unique_byte_buffer_t nas_pdu = srslte::allocate_unique_buffer(*pool, true);
  nas_pdu->append_bytes(pdu, pdu_len);

  rrc->connection_request(cause, std::move(nas_pdu));
}

void nas_ext::handle_rrctl_data(rrctl::proto::msg_disc disc, const uint8_t* msg, size_t len)
{
  if (not rrc->is_connected()) {
    nas_log->warning("Received DATA.req, but there is no active connection\n");
    rrctl_send_error(rrctl::proto::RRCTL_DATA);
    return;
  }

  // Allocate a new NAS PDU on heap
  unique_byte_buffer_t nas_pdu = srslte::allocate_unique_buffer(*pool, true);
  nas_pdu->append_bytes(msg, len);

  rrc->write_sdu(std::move(nas_pdu));
}

void nas_ext::get_metrics(nas_metrics_t* m)
{
  nas_metrics_t metrics = {};
  // FIXME: is there anything we could fill in?
  *m = metrics;
}

void nas_ext::stop()
{
  // Close the UNIX domain socket connection
  iface->close();
  iface.release();
}

/*******************************************************************************
 * UE interface (dummy)
 ******************************************************************************/

void nas_ext::start_attach_request(srslte::proc_state_t* result, srslte::establishment_cause_t cause_)
{
  nas_log->info("The UE has requested us to perform Attach Request, however we ignore it\n");
  if (result != nullptr) {
    result->set_val();
  }
}

bool nas_ext::detach_request(const bool switch_off)
{
  nas_log->info("The UE has requested us to perform Detach Request, however we ignore it\n");
  return false;
}

void nas_ext::timer_expired(uint32_t timeout_id)
{
  nas_log->info("Timer id=%u is expired, however we ignore it\n", timeout_id);
}

/*******************************************************************************
 * RRC interface
 ******************************************************************************/

// TODO: investigate the meaning of these signals
void nas_ext::set_barring(barring_t barring) {}
void nas_ext::run_tti(uint32_t tti) {}
void nas_ext::left_rrc_connected() {}

bool nas_ext::paging(srslte::s_tmsi_t* ue_identity)
{
  nas_log->info("Received paging from RRC\n");
  // TODO: send PAGING.ind to the external entity
  return false;
}

void nas_ext::write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  srslte::byte_buffer_t msg;

  nas_log->info_hex(pdu->msg, pdu->N_bytes, "Received DL %s PDU from RRC\n", rrc->get_rb_name(lcid).c_str());

  rrctl::codec::enc_data_ind(msg, pdu->msg, pdu->N_bytes, lcid);
  iface->write(msg);
}

uint32_t nas_ext::get_k_enb_count()
{
  // FIXME: we probably need to maintain a security context
  return 0; // return a dummy value for now
}

bool nas_ext::is_attached()
{
  // FIXME: we probably need to maintain the state
  return false; // return a dummy value for now
}

bool nas_ext::get_k_asme(uint8_t* k_asme_, uint32_t n)
{
  // FIXME: we probably need to maintain a security context
  return false; // return a dummy value for now
}

uint32_t nas_ext::get_ipv4_addr()
{
  // FIXME: where can we get it? maybe from GW?
  return 0x00000000;
}

bool nas_ext::get_ipv6_addr(uint8_t* ipv6_addr)
{
  // FIXME: where can we get it? maybe from GW?
  return false;
}

void nas_ext::plmn_search_completed(
    const rrc_interface_nas::found_plmn_t found_plmns[rrc_interface_nas::MAX_FOUND_PLMNS],
    int                                   nof_plmns)
{
  srslte::byte_buffer_t pdu;

  nas_log->info("RRC has completed PLMN search, %d carriers found\n", nof_plmns);

  // Send PLMN_SEARCH.res to an external entity
  if (nof_plmns >= 0) {
    rrctl::codec::enc_plmn_search_res(pdu, found_plmns, nof_plmns);
    iface->write(pdu);
  } else {
    nas_log->warning("PLMN search completed with an error\n");
    rrctl_send_error(rrctl::proto::RRCTL_PLMN_SEARCH);
  }
}

bool nas_ext::connection_request_completed(bool outcome)
{
  nas_log->info("RRC has %s connection establisment\n", outcome ? "completed" : "failed");

  if (outcome)
    rrctl_send_confirm(rrctl::proto::RRCTL_CONN_ESTABLISH);
  else
    rrctl_send_error(rrctl::proto::RRCTL_CONN_ESTABLISH);

  return false; // FIXME: what should we return here?
}

} // namespace srsue
