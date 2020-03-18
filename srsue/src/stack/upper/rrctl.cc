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

#include <stdint.h>
#include <arpa/inet.h>

#include "srsue/hdr/stack/upper/rrctl.h"

namespace rrctl {

namespace proto {

std::string msg_hdr_desc(proto::msg_type type, proto::msg_disc disc, uint16_t len)
{
  std::string desc;

  switch (type) {
  case RRCTL_RESET:
    desc += "Reset";
    break;
  case RRCTL_PLMN_SEARCH:
    desc += "PLMN Search";
    break;
  case RRCTL_PLMN_SELECT:
    desc += "PLMN Select";
    break;
  case RRCTL_CONN_ESTABLISH:
    desc += "Connection Establish";
    break;
  case RRCTL_CONN_RELEASE:
    desc += "Connection Release";
    break;
  case RRCTL_DATA:
    desc += "Data (PDU)";
    break;
  default:
    desc += "<UNKNOWN>";
  }

  desc += " ";

  switch (disc) {
  case RRCTL_REQ:
    desc += "Request";
    break;
  case RRCTL_IND:
    desc += "Indication";
    break;
  case RRCTL_CNF:
    desc += "Confirmation";
    break;
  case RRCTL_ERR:
    desc += "Error";
    break;
  }

  if (len > 0) {
    desc += " (length ";
    desc += std::to_string(len);
    desc += ")";
  }

  return desc;
}

}

namespace codec {

void enc_hdr(srslte::byte_buffer_t& buf,
             proto::msg_type type,
             proto::msg_disc disc,
             uint16_t len)
{
  struct proto::msg_hdr hdr = {
    .type = (uint8_t) type,
    .disc = (uint8_t) disc,
    .len = ntohs(len),
  };

  buf.append_bytes((const uint8_t*) &hdr, sizeof(hdr));
}

const uint8_t* dec_hdr(const srslte::byte_buffer_t& buf,
                       proto::msg_type& type,
                       proto::msg_disc& disc,
                       uint16_t& len)
{
  const struct proto::msg_hdr* hdr;

  // Make sure at least header is present
  if (buf.N_bytes < sizeof(*hdr))
    throw codec::error("header is too short");

  hdr = reinterpret_cast<const struct proto::msg_hdr*> (buf.msg);
  type = static_cast<proto::msg_type> (hdr->type);
  disc = static_cast<proto::msg_disc> (hdr->disc);
  len = htons(hdr->len);

  // Make sure the whole message fits
  if (buf.N_bytes < sizeof(*hdr) + len)
    throw codec::error("body is too short");

  // Return pointer to the payload (if present)
  return len ? hdr->data : NULL;
}

void enc_plmn_search_res(srslte::byte_buffer_t& buf,
                         const srsue::rrc_interface_nas::found_plmn_t* plmns,
                         size_t nof_plmns)
{
  struct proto::msg_plmn_search_res msg;
  uint16_t msg_len;

  if (nof_plmns > 16)
    throw codec::error("too many PLMNS to encode");
  msg.nof_plmns = static_cast<uint8_t> (nof_plmns);

  for (size_t i = 0; i < nof_plmns; i++) {
    std::pair<uint16_t, uint16_t> mcc_mnc = plmns[i].plmn_id.to_number();
    msg.plmns[i].mcc = htons(mcc_mnc.first);
    msg.plmns[i].mnc = htons(mcc_mnc.second);
    msg.plmns[i].tac = htons(plmns[i].tac);
  }

  msg_len = sizeof(proto::msg_plmn_search_res::plmn) * nof_plmns + 1;
  enc_hdr(buf, proto::RRCTL_PLMN_SEARCH, proto::RRCTL_CNF, msg_len);
  buf.append_bytes((uint8_t *) &msg, msg_len);
}

void dec_plmn_select_req(std::pair<uint16_t, uint16_t>& mcc_mnc,
                         const uint8_t* payload, size_t len)
{
  const struct proto::msg_plmn_select_req* msg;

  if (len < sizeof(*msg))
    throw codec::error("body is too short");

  msg = reinterpret_cast<const struct proto::msg_plmn_select_req*> (payload);
  mcc_mnc.first  = htons(msg->mcc);
  mcc_mnc.second = htons(msg->mnc);
}

void dec_conn_establish_req(srslte::establishment_cause_t& cause,
                            const uint8_t*& pdu, size_t& pdu_len,
                            const uint8_t* payload, size_t len)
{
  const struct proto::msg_conn_establish_req* msg;

  if (len < sizeof(*msg))
    throw codec::error("body is too short");

  msg = reinterpret_cast<const struct proto::msg_conn_establish_req*> (payload);
  cause = static_cast<srslte::establishment_cause_t> (msg->cause);
  pdu_len = len - 1;
  pdu = msg->pdu;
}

void enc_data_ind(srslte::byte_buffer_t& buf,
                  const uint8_t *pdu, size_t pdu_len,
                  uint32_t lcid)
{
  struct proto::msg_data msg;

  msg.lcid = htonl(lcid);

  enc_hdr(buf, proto::RRCTL_DATA, proto::RRCTL_IND, sizeof(msg) + pdu_len);
  buf.append_bytes((const uint8_t*) &msg, sizeof(msg));
  buf.append_bytes(pdu, pdu_len);
}

} // namespace codec

} // namespace rrctl
