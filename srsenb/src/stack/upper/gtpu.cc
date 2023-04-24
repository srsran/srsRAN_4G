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

#include "srsran/upper/gtpu.h"
#include "srsenb/hdr/stack/upper/gtpu.h"
#include "srsran/common/common_nr.h"
#include "srsran/common/network_utils.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/enb_interfaces.h"
#include "srsran/interfaces/enb_pdcp_interfaces.h"
#include "srsran/support/srsran_assert.h"

#include <errno.h>
#include <linux/ip.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace srsran;

namespace srsenb {

// ensure consistent formatting
#define TEID_IN_FMT "TEID In=0x%x"
#define TEID_OUT_FMT "TEID Out=0x%x"

gtpu_tunnel_manager::gtpu_tunnel_manager(srsran::task_sched_handle task_sched_,
                                         srslog::basic_logger&     logger,
                                         srsran::srsran_rat_t      ran_type_) :
  logger(logger), ran_type(ran_type_), task_sched(task_sched_), tunnels(1)
{
}

void gtpu_tunnel_manager::init(const gtpu_args_t& args, pdcp_interface_gtpu* pdcp_)
{
  gtpu_args = &args;
  pdcp      = pdcp_;
}

const gtpu_tunnel_manager::tunnel* gtpu_tunnel_manager::find_tunnel(uint32_t teid)
{
  auto it = tunnels.find(teid);
  return it != tunnels.end() ? &it->second : nullptr;
}

gtpu_tunnel_manager::ue_bearer_tunnel_list* gtpu_tunnel_manager::find_rnti_tunnels(uint16_t rnti)
{
  auto it = ue_teidin_db.find(rnti);
  return it != ue_teidin_db.end() ? &ue_teidin_db[rnti] : nullptr;
}

srsran::span<gtpu_tunnel_manager::bearer_teid_pair>
gtpu_tunnel_manager::find_rnti_bearer_tunnels(uint16_t rnti, uint32_t eps_bearer_id)
{
  if (ran_type == srsran::srsran_rat_t::lte and not is_eps_bearer_id(eps_bearer_id)) {
    logger.warning("Searching for bearer with invalid eps-BearerID=%d", eps_bearer_id);
    return {};
  }
  if (ran_type == srsran::srsran_rat_t::nr and not is_nr_pdu_session_id(eps_bearer_id)) {
    logger.warning("Searching for bearer with invalid PDU Session Id=%d", eps_bearer_id);
    return {};
  }
  auto* ue_ptr = find_rnti_tunnels(rnti);
  if (ue_ptr == nullptr) {
    return {};
  }
  auto bearer_it_begin = std::lower_bound(ue_ptr->begin(), ue_ptr->end(), bearer_teid_pair{eps_bearer_id, 0});
  auto bearer_it_end   = std::lower_bound(ue_ptr->begin(), ue_ptr->end(), bearer_teid_pair{eps_bearer_id + 1, 0});

  return srsran::span<bearer_teid_pair>(&(*bearer_it_begin), &(*bearer_it_end));
}

const gtpu_tunnel*
gtpu_tunnel_manager::add_tunnel(uint16_t rnti, uint32_t eps_bearer_id, uint32_t teidout, uint32_t spgw_addr)
{
  if (ran_type == srsran::srsran_rat_t::lte and not is_eps_bearer_id(eps_bearer_id)) {
    logger.warning("Adding TEID with invalid eps-BearerID=%d", eps_bearer_id);
    return nullptr;
  }
  if (ran_type == srsran::srsran_rat_t::nr and not is_nr_pdu_session_id(eps_bearer_id)) {
    logger.warning("Adding TEID with invalid PDU Session Id=%d", eps_bearer_id);
    return nullptr;
  }
  auto ret_pair = tunnels.insert(tunnel());
  if (not ret_pair) {
    logger.warning("Unable to create new GTPU TEID In");
    return nullptr;
  }
  tunnel* tun        = &tunnels[ret_pair.value()];
  tun->teid_in       = ret_pair.value();
  tun->rnti          = rnti;
  tun->eps_bearer_id = eps_bearer_id;
  tun->teid_out      = teidout;
  tun->spgw_addr     = spgw_addr;

  if (ue_teidin_db.find(rnti) == ue_teidin_db.end()) {
    auto ret = ue_teidin_db.emplace(rnti, ue_bearer_tunnel_list());
    if (!ret.second) {
      logger.error("Failed to allocate rnti=0x%x", rnti);
      return nullptr;
    }
  }
  auto& ue_tunnels = ue_teidin_db[rnti];

  if (ue_tunnels.full()) {
    logger.error("The number of TEIDs per UE exceeded for rnti=0x%x", rnti);
    tunnels.erase(tun->teid_in);
    return nullptr;
  }
  ue_tunnels.push_back(bearer_teid_pair{eps_bearer_id, tun->teid_in});
  std::sort(ue_tunnels.begin(), ue_tunnels.end());

  fmt::memory_buffer str_buffer;
  srsran::gtpu_ntoa(str_buffer, htonl(spgw_addr));
  logger.info("New tunnel created - " TEID_IN_FMT ", " TEID_OUT_FMT ", rnti=0x%x, eps-BearerID=%d, remote addr=%s",
              tun->teid_in,
              teidout,
              rnti,
              eps_bearer_id,
              srsran::to_c_str(str_buffer));

  return tun;
}

bool gtpu_tunnel_manager::update_rnti(uint16_t old_rnti, uint16_t new_rnti)
{
  auto* old_rnti_ptr = find_rnti_tunnels(old_rnti);
  auto* new_rnti_ptr = find_rnti_tunnels(new_rnti);
  if (old_rnti_ptr == nullptr or (new_rnti_ptr != nullptr and not new_rnti_ptr->empty())) {
    // The old rnti must exist and the new rnti TEID list must be empty
    logger.error("Modifying bearer rnti. Old rnti=0x%x, new rnti=0x%x", old_rnti, new_rnti);
    return false;
  }
  logger.info("Modifying bearer rnti. Old rnti: 0x%x, new rnti: 0x%x", old_rnti, new_rnti);

  // create new RNTI and update TEIDs of old rnti to reflect new rnti
  if (new_rnti_ptr == nullptr and not ue_teidin_db.insert({new_rnti, ue_bearer_tunnel_list()}).second) {
    logger.error("Failure to create new rnti=0x%x", new_rnti);
    return false;
  }
  std::swap(ue_teidin_db[new_rnti], *old_rnti_ptr);
  ue_bearer_tunnel_list&                               new_rnti_obj = ue_teidin_db[new_rnti];
  srsran::bounded_vector<uint32_t, MAX_TUNNELS_PER_UE> to_remove;
  for (bearer_teid_pair& bearer : new_rnti_obj) {
    tunnels[bearer.teid].rnti = new_rnti;
    // Remove forwarding path
    if (tunnels[bearer.teid].state == tunnel_state::forward_to) {
      tunnels[bearer.teid].state      = tunnel_state::pdcp_active;
      tunnels[bearer.teid].fwd_tunnel = nullptr;
      logger.info("Taking down forwarding tunnel for rnti=0x%x, eps-BearerID=%d. New default " TEID_IN_FMT,
                  new_rnti,
                  bearer.eps_bearer_id,
                  bearer.teid);
    } else if (tunnels[bearer.teid].state == tunnel_state::forwarded_from) {
      to_remove.push_back(bearer.teid);
    }
  }
  while (not to_remove.empty()) {
    remove_tunnel(to_remove.back());
    to_remove.pop_back();
  }

  // Leave old_rnti as zombie to be removed later
  old_rnti_ptr->clear();

  return true;
}

bool gtpu_tunnel_manager::remove_tunnel(uint32_t teidin)
{
  tunnel& tun = tunnels[teidin];

  // update forwarding paths if required
  deactivate_tunnel(teidin);

  // erase keeping the relative order
  auto& ue        = ue_teidin_db[tun.rnti];
  auto  bearer_it = std::lower_bound(ue.begin(), ue.end(), bearer_teid_pair{tun.eps_bearer_id, tun.teid_in});
  srsran_assert(bearer_it->teid == tun.teid_in and bearer_it->eps_bearer_id == tun.eps_bearer_id,
                "TEID in undefined state");
  ue.erase(bearer_it);

  logger.info("Removed rnti=0x%x,eps-BearerID=%d tunnel with " TEID_IN_FMT, tun.rnti, tun.eps_bearer_id, teidin);
  tunnels.erase(teidin);
  return true;
}

bool gtpu_tunnel_manager::remove_rnti(uint16_t rnti)
{
  auto it = ue_teidin_db.find(rnti);
  if (it == ue_teidin_db.end()) {
    logger.warning("Removing rnti. rnti=0x%x not found.", rnti);
    return false;
  }
  logger.info("Removing rnti=0x%x", rnti);

  while (not ue_teidin_db[rnti].empty()) {
    uint32_t teid = ue_teidin_db[rnti].front().teid;
    bool     ret  = remove_tunnel(teid);
    srsran_expect(
        ret, "Inconsistency detected between internal data structures for rnti=0x%x," TEID_IN_FMT, rnti, teid);
  }
  ue_teidin_db.erase(rnti);
  return true;
}

void gtpu_tunnel_manager::activate_tunnel(uint32_t teid)
{
  tunnel& tun = tunnels[teid];
  if (tun.state == tunnel_state::pdcp_active) {
    // nothing happens
    return;
  }

  logger.info("Activating GTPU tunnel rnti=0x%x, " TEID_IN_FMT ". %d SDUs currently buffered",
              tun.rnti,
              tun.teid_in,
              tun.buffer->size());
  // Forward buffered SDUs to lower layers and delete buffer
  auto lower_sn = [](const std::pair<uint32_t, srsran::unique_byte_buffer_t>& lhs,
                     const std::pair<uint32_t, srsran::unique_byte_buffer_t>& rhs) { return lhs.first < rhs.first; };
  std::stable_sort(tun.buffer->begin(), tun.buffer->end(), lower_sn);

  for (auto& sdu_pair : *tun.buffer) {
    uint32_t pdcp_sn = sdu_pair.first;
    pdcp->write_sdu(
        tun.rnti, tun.eps_bearer_id, std::move(sdu_pair.second), pdcp_sn == undefined_pdcp_sn ? -1 : pdcp_sn);
  }
  tun.buffer.reset();
  tun.state = tunnel_state::pdcp_active;
}

void gtpu_tunnel_manager::suspend_tunnel(uint32_t teid)
{
  tunnel& tun = tunnels[teid];
  if (tun.state != tunnel_state::pdcp_active) {
    logger.error("Invalid TEID transition detected");
    return;
  }
  // Create a container for buffering SDUs
  tun.buffer.emplace();
  tun.state = tunnel_state::buffering;
}

void gtpu_tunnel_manager::deactivate_tunnel(uint32_t teid)
{
  tunnel& tun = tunnels[teid];

  if (tun.state == tunnel_state::forwarded_from) {
    // Deactivate respective MME->SeNB forwarding tunnel
    for (auto bearer_tun : find_rnti_bearer_tunnels(tun.rnti, tun.eps_bearer_id)) {
      if (bearer_tun.teid != tun.teid_in) {
        const gtpu_tunnel_manager::tunnel* mmeenb_tun = find_tunnel(bearer_tun.teid);
        if (mmeenb_tun->state == gtpu_tunnel_manager::tunnel_state::forward_to and mmeenb_tun->fwd_tunnel == &tun) {
          deactivate_tunnel(mmeenb_tun->teid_in);
        }
      }
    }
  }

  tun.state = tunnel_state::inactive;
}

void gtpu_tunnel_manager::set_tunnel_priority(uint32_t before_teid, uint32_t after_teid)
{
  tunnel& before_tun = tunnels[before_teid];

  // GTPU should not forward SDUs from main tunnel until the SeNB-TeNB tunnel has been flushed
  suspend_tunnel(after_teid);

  before_tun.on_removal = [this, after_teid]() {
    if (tunnels.contains(after_teid)) {
      // In Handover, TeNB switches paths, and flushes PDUs that have been buffered
      activate_tunnel(after_teid);
    }
  };

  // Schedule auto-removal of the indirect tunnel in case the End Marker is not received
  // TS 36.300 - On detection of the "end marker", the target eNB may also initiate the release of the data forwarding
  //             resource. However, the release of the data forwarding resource is implementation dependent and could
  //             also be based on other mechanisms (e.g. timer-based mechanism).
  if (gtpu_args->indirect_tunnel_timeout_msec > 0) {
    before_tun.rx_timer = task_sched.get_unique_timer();
    before_tun.rx_timer.set(gtpu_args->indirect_tunnel_timeout_msec, [this, before_teid](uint32_t tid) {
      // Note: This will self-destruct the callback object
      logger.info("Forwarding tunnel " TEID_IN_FMT "being closed after timeout=%d msec",
                  before_teid,
                  gtpu_args->indirect_tunnel_timeout_msec);
      remove_tunnel(before_teid);
    });
    before_tun.rx_timer.run();
  }
}

void gtpu_tunnel_manager::handle_rx_pdcp_sdu(uint32_t teid)
{
  tunnel& rx_tun = tunnels[teid];

  // Reset Rx timer when a PDCP SDU is received
  if (rx_tun.rx_timer.is_valid() and rx_tun.rx_timer.is_running()) {
    rx_tun.rx_timer.run();
  }
}

void gtpu_tunnel_manager::buffer_pdcp_sdu(uint32_t teid, uint32_t pdcp_sn, srsran::unique_byte_buffer_t sdu)
{
  tunnel& rx_tun = tunnels[teid];

  srsran_assert(rx_tun.state == tunnel_state::buffering, "Buffering of PDCP SDUs only enabled when PDCP is not active");
  if (not rx_tun.buffer->full()) {
    rx_tun.buffer->push_back(std::make_pair(pdcp_sn, std::move(sdu)));
  } else {
    fmt::memory_buffer str_buffer;
    if (pdcp_sn != undefined_pdcp_sn) {
      fmt::format_to(str_buffer, " PDCP SN={}", pdcp_sn);
    }
    logger.warning("GTPU tunnel " TEID_IN_FMT " internal buffer of size=%zd is full. Discarding SDU%s.",
                   teid,
                   rx_tun.buffer->size(),
                   to_c_str(str_buffer));
  }
}

void gtpu_tunnel_manager::setup_forwarding(uint32_t rx_teid, uint32_t tx_teid)
{
  tunnel& rx_tun = tunnels[rx_teid];
  tunnel& tx_tun = tunnels[tx_teid];

  rx_tun.state      = tunnel_state::forward_to;
  rx_tun.fwd_tunnel = &tx_tun;
  tx_tun.state      = tunnel_state::forwarded_from;

  // Auto-removes indirect tunnel when the main tunnel is removed
  rx_tun.on_removal = [this, tx_teid]() {
    if (tunnels.contains(tx_teid)) {
      remove_tunnel(tx_teid);
    }
  };

  fmt::memory_buffer addrbuf;
  srsran::gtpu_ntoa(addrbuf, htonl(rx_tun.spgw_addr));
  fmt::format_to(addrbuf, ":0x{:x} > ", rx_tun.teid_out);
  srsran::gtpu_ntoa(addrbuf, htonl(tx_tun.spgw_addr));
  fmt::format_to(addrbuf, ":0x{:x}", tx_tun.teid_out);
  logger.info("Created forwarding tunnel for rnti=0x%x, eps-BearerID=%d, %s",
              rx_tun.rnti,
              rx_tun.eps_bearer_id,
              srsran::to_c_str(addrbuf));
}

/********************
 *    GTPU class
 *******************/

gtpu::gtpu(srsran::task_sched_handle   task_sched_,
           srslog::basic_logger&       logger,
           srsran::srsran_rat_t        ran_type_,
           srsran::socket_manager_itf* rx_socket_handler_) :
  m1u(this),
  task_sched(task_sched_),
  logger(logger),
  ran_type(ran_type_),
  tunnels(task_sched_, logger, ran_type),

  rx_socket_handler(rx_socket_handler_)
{
  gtpu_queue = task_sched.make_task_queue();
}

gtpu::~gtpu()
{
  stop();
}

int gtpu::init(const gtpu_args_t& gtpu_args, pdcp_interface_gtpu* pdcp_)
{
  args          = gtpu_args;
  pdcp          = pdcp_;
  gtp_bind_addr = gtpu_args.gtp_bind_addr;
  mme_addr      = gtpu_args.mme_addr;

  tunnels.init(args, pdcp);

  char errbuf[128] = {};

  // Set up socket
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    logger.error("Failed to create socket");
    return SRSRAN_ERROR;
  }
  int enable = 1;
#if defined(SO_REUSEADDR)
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    logger.error("setsockopt(SO_REUSEADDR) failed");
#endif
#if defined(SO_REUSEPORT)
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
    logger.error("setsockopt(SO_REUSEPORT) failed");
#endif

  struct sockaddr_in bindaddr;
  bzero(&bindaddr, sizeof(struct sockaddr_in));
  // Bind socket
  if (not net_utils::bind_addr(fd, gtp_bind_addr.c_str(), GTPU_PORT, &bindaddr)) {
    snprintf(errbuf, sizeof(errbuf), "%s", strerror(errno));
    srsran::console("Failed to bind on address %s, port %d: %s\n", gtp_bind_addr.c_str(), int(GTPU_PORT), errbuf);
    return SRSRAN_ERROR;
  }

  // Assign a handler to rx S1U packets
  auto rx_callback = [this](srsran::unique_byte_buffer_t pdu, const sockaddr_in& from) {
    handle_gtpu_s1u_rx_packet(std::move(pdu), from);
  };
  rx_socket_handler->add_socket_handler(fd, srsran::make_sdu_handler(logger, gtpu_queue, rx_callback));

  // Start MCH socket if enabled
  if (args.embms_enable) {
    if (not m1u.init(args.embms_m1u_multiaddr, args.embms_m1u_if_addr)) {
      return SRSRAN_ERROR;
    }
  }
  return SRSRAN_SUCCESS;
}

void gtpu::stop()
{
  if (fd > 0) {
    close(fd);
    fd = -1;
  }
}

// gtpu_interface_pdcp
void gtpu::write_pdu(uint16_t rnti, uint32_t eps_bearer_id, srsran::unique_byte_buffer_t pdu)
{
  srsran::span<gtpu_tunnel_manager::bearer_teid_pair> teids = tunnels.find_rnti_bearer_tunnels(rnti, eps_bearer_id);
  if (teids.empty()) {
    logger.warning("The rnti=0x%x, eps-BearerID=%d does not have any pdcp_active tunnel", rnti, eps_bearer_id);
    return;
  }
  const gtpu_tunnel& tx_tun = *tunnels.find_tunnel(teids[0].teid);
  log_message(tx_tun, false, srsran::make_span(pdu));
  send_pdu_to_tunnel(tx_tun, std::move(pdu));
}

void gtpu::send_pdu_to_tunnel(const gtpu_tunnel& tx_tun, srsran::unique_byte_buffer_t pdu, int pdcp_sn)
{
  // Check valid IP version
  struct iphdr* ip_pkt = (struct iphdr*)pdu->msg;
  if (ip_pkt->version != 4 && ip_pkt->version != 6) {
    logger.error("Invalid IP version to SPGW");
    return;
  }

  gtpu_header_t header;
  header.flags        = GTPU_FLAGS_VERSION_V1 | GTPU_FLAGS_GTP_PROTOCOL;
  header.message_type = GTPU_MSG_DATA_PDU;
  header.length       = pdu->N_bytes;
  header.teid         = tx_tun.teid_out;

  if (pdcp_sn >= 0) {
    header.flags |= GTPU_FLAGS_EXTENDED_HDR;
    header.next_ext_hdr_type = GTPU_EXT_HEADER_PDCP_PDU_NUMBER;
    header.ext_buffer.resize(4u);
    header.ext_buffer[0] = 0x01u;
    header.ext_buffer[1] = (pdcp_sn >> 8u) & 0xffu;
    header.ext_buffer[2] = pdcp_sn & 0xffu;
    header.ext_buffer[3] = 0;
  }

  struct sockaddr_in servaddr;
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(tx_tun.spgw_addr);
  servaddr.sin_port        = htons(GTPU_PORT);

  if (!gtpu_write_header(&header, pdu.get(), logger)) {
    logger.error("Error writing GTP-U Header. Flags 0x%x, Message Type 0x%x", header.flags, header.message_type);
    return;
  }
  if (sendto(fd, pdu->msg, pdu->N_bytes, MSG_EOR, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in)) < 0) {
    perror("sendto");
  }
}

srsran::expected<uint32_t> gtpu::add_bearer(uint16_t            rnti,
                                            uint32_t            eps_bearer_id,
                                            uint32_t            addr_out,
                                            uint32_t            teid_out,
                                            uint32_t&           addr_in,
                                            const bearer_props* props)
{
  // Allocate a TEID for the incoming tunnel
  const gtpu_tunnel* new_tun = tunnels.add_tunnel(rnti, eps_bearer_id, teid_out, addr_out);
  if (new_tun == nullptr) {
    return default_error_t();
  }
  uint32_t teid_in = new_tun->teid_in;

  if (props != nullptr) {
    if (props->flush_before_teidin_present) {
      // GTPU should wait for the bearer ctxt to arrive before sending SDUs from DL tunnel to PDCP
      tunnels.suspend_tunnel(teid_in);

      // GTPU should not forward SDUs from main tunnel until the SeNB-TeNB tunnel has been flushed
      tunnels.set_tunnel_priority(teid_in, props->flush_before_teidin);
    }

    // Connect tunnels if forwarding is activated
    if (props->forward_from_teidin_present) {
      if (create_dl_fwd_tunnel(props->forward_from_teidin, teid_in) != SRSRAN_SUCCESS) {
        rem_tunnel(teid_in);
        return default_error_t();
      }
    }
  }

  // Return bind address for S1AP and NGAP setup
  uint8_t addr_in_tmp[4];
  if ((inet_pton(AF_INET, gtp_bind_addr.c_str(), &addr_in_tmp)) < 1) {
    logger.error("Invalid address or failure during conversion: %s\n", gtp_bind_addr.c_str());
  }
  addr_in = 0;
  addr_in = addr_in_tmp[3] | (addr_in_tmp[2] << 8) | (addr_in_tmp[1] << 16) | (addr_in_tmp[0] << 24);

  return teid_in;
}

void gtpu::set_tunnel_status(uint32_t teidin, bool dl_active)
{
  if (not tunnels.has_teid(teidin)) {
    logger.error("Setting status for non-existent " TEID_IN_FMT, teidin);
    return;
  }

  if (dl_active) {
    tunnels.activate_tunnel(teidin);
  } else {
    tunnels.suspend_tunnel(teidin);
  }
}

void gtpu::rem_bearer(uint16_t rnti, uint32_t eps_bearer_id)
{
  srsran::span<gtpu_tunnel_manager::bearer_teid_pair> bearer_tuns =
      tunnels.find_rnti_bearer_tunnels(rnti, eps_bearer_id);
  if (bearer_tuns.empty()) {
    logger.info("Removing bearer rnti=0x%x,eps-BearerID=%d without any active tunnels", rnti, eps_bearer_id);
    return;
  }
  do {
    rem_tunnel(bearer_tuns.front().teid);
    bearer_tuns = tunnels.find_rnti_bearer_tunnels(rnti, eps_bearer_id);
  } while (not bearer_tuns.empty());
}

void gtpu::mod_bearer_rnti(uint16_t old_rnti, uint16_t new_rnti)
{
  tunnels.update_rnti(old_rnti, new_rnti);
}

void gtpu::rem_tunnel(uint32_t teidin)
{
  const gtpu_tunnel_manager::tunnel* tun = tunnels.find_tunnel(teidin);
  if (tun == nullptr) {
    logger.warning("Removing tunnel - " TEID_IN_FMT " does not exist", teidin);
    return;
  }
  if (tun->state == gtpu_tunnel_manager::tunnel_state::forwarded_from) {
    // TS 36.300, Sec 10.1.2.2.1 - Path Switch upon handover
    // END MARKER should be forwarded to TeNB if forwarding is activated
    send_end_marker(tun->teid_in);
  } else if (tun->state == gtpu_tunnel_manager::tunnel_state::forward_to) {
    // Delete respective forwarding SeNB-TeNB tunnel
    send_end_marker(tun->fwd_tunnel->teid_in);
    rem_tunnel(tun->fwd_tunnel->teid_in);
  }
  tunnels.remove_tunnel(teidin);
}

void gtpu::rem_user(uint16_t rnti)
{
  const gtpu_tunnel_manager::ue_bearer_tunnel_list* tun_lst = tunnels.find_rnti_tunnels(rnti);
  if (tun_lst == nullptr) {
    logger.info("Removing user - rnti=0x%x not found.", rnti);
    return;
  }
  while (not tun_lst->empty()) {
    // Note: May send End-Marker to active forwarding tunnels during their removal
    rem_tunnel(tun_lst->front().teid);
  }
  tunnels.remove_rnti(rnti);
}

void gtpu::handle_end_marker(const gtpu_tunnel& rx_tunnel)
{
  uint16_t rnti = rx_tunnel.rnti;
  logger.info("Rx GTPU End Marker, " TEID_IN_FMT ", rnti=0x%x.", rx_tunnel.teid_in, rnti);

  if (rx_tunnel.state == gtpu_tunnel_state::forward_to) {
    // TS 36.300, Sec 10.1.2.2.1 - Path Switch upon handover
    // END MARKER should be forwarded to TeNB if forwarding is activated
    send_end_marker(rx_tunnel.fwd_tunnel->teid_in);
    rem_tunnel(rx_tunnel.fwd_tunnel->teid_in);
  }

  // Remove tunnel that received End Marker
  rem_tunnel(rx_tunnel.teid_in);
}

void gtpu::handle_gtpu_s1u_rx_packet(srsran::unique_byte_buffer_t pdu, const sockaddr_in& addr)
{
  srsran_assert(pdu != nullptr, "Called with null PDU");

  logger.debug("Received %d bytes from S1-U interface", pdu->N_bytes);
  pdu->set_timestamp();

  // Decode GTPU Header
  gtpu_header_t header;
  if (not gtpu_read_header(pdu.get(), &header, logger)) {
    return;
  }

  if (header.message_type == GTPU_MSG_ECHO_REQUEST) {
    // Echo request - send response
    echo_response(addr.sin_addr.s_addr, addr.sin_port, header.seq_number);
    return;
  }
  if (header.message_type == GTPU_MSG_ERROR_INDICATION) {
    logger.warning("Received Error Indication");
    return;
  }
  if (header.teid == 0) {
    logger.warning("Received GTPU S1-U message with " TEID_IN_FMT, header.teid);
  }

  // Find TEID present in GTPU Header
  const gtpu_tunnel* tun_ptr = tunnels.find_tunnel(header.teid);
  if (tun_ptr == nullptr) {
    // Received G-PDU for non-existing and non-zero TEID.
    // Sending GTP-U error indication
    error_indication(addr.sin_addr.s_addr, addr.sin_port, header.teid);
    return;
  }

  switch (header.message_type) {
    case GTPU_MSG_DATA_PDU: {
      handle_msg_data_pdu(header, *tun_ptr, std::move(pdu));
    } break;
    case GTPU_MSG_END_MARKER:
      handle_end_marker(*tun_ptr);
      break;
    default:
      logger.warning("Unhandled GTPU message type=%d", header.message_type);
      break;
  }
}

void gtpu::handle_msg_data_pdu(const gtpu_header_t&         header,
                               const gtpu_tunnel&           rx_tunnel,
                               srsran::unique_byte_buffer_t pdu)
{
  struct iphdr* ip_pkt = (struct iphdr*)pdu->msg;
  if (ip_pkt->version != 4 && ip_pkt->version != 6) {
    logger.error("Received SDU with invalid IP version=%d", (int)ip_pkt->version);
    return;
  }

  // Forward SDU to PDCP or buffer it if tunnel is disabled
  uint32_t pdcp_sn = undefined_pdcp_sn;
  if ((header.flags & GTPU_FLAGS_EXTENDED_HDR) != 0 and header.next_ext_hdr_type == GTPU_EXT_HEADER_PDCP_PDU_NUMBER) {
    pdcp_sn = (header.ext_buffer[1] << 8U) + header.ext_buffer[2];
  }

  uint16_t rnti          = rx_tunnel.rnti;
  uint16_t eps_bearer_id = rx_tunnel.eps_bearer_id;

  log_message(rx_tunnel, true, srsran::make_span(pdu));

  tunnels.handle_rx_pdcp_sdu(rx_tunnel.teid_in);

  switch (rx_tunnel.state) {
    case gtpu_tunnel_manager::tunnel_state::forward_to: {
      // Forward SDU to direct/indirect tunnel during Handover
      send_pdu_to_tunnel(*rx_tunnel.fwd_tunnel, std::move(pdu));
      break;
    }
    case gtpu_tunnel_manager::tunnel_state::buffering: {
      tunnels.buffer_pdcp_sdu(rx_tunnel.teid_in, pdcp_sn, std::move(pdu));
      break;
    }
    case gtpu_tunnel_manager::tunnel_state::pdcp_active: {
      pdcp->write_sdu(rnti, eps_bearer_id, std::move(pdu), pdcp_sn == undefined_pdcp_sn ? -1 : (int)pdcp_sn);
      break;
    }
    case gtpu_tunnel_manager::tunnel_state::forwarded_from:
    default:
      logger.error(TEID_IN_FMT " found in invalid state", rx_tunnel.teid_in);
      break;
  }
}

void gtpu::handle_gtpu_m1u_rx_packet(srsran::unique_byte_buffer_t pdu, const sockaddr_in& addr)
{
  m1u.handle_rx_packet(std::move(pdu), addr);
}

/// Connect created tunnel with pre-existing tunnel for data forwarding
int gtpu::create_dl_fwd_tunnel(uint32_t rx_teid_in, uint32_t tx_teid_in)
{
  const gtpu_tunnel* rx_tun = tunnels.find_tunnel(rx_teid_in);
  const gtpu_tunnel* tx_tun = tunnels.find_tunnel(tx_teid_in);
  if (rx_tun == nullptr or tx_tun == nullptr) {
    logger.error("Failed to create forwarding tunnel between teids 0x%x and 0x%x", rx_teid_in, tx_teid_in);
    return SRSRAN_ERROR;
  }

  tunnels.setup_forwarding(rx_teid_in, tx_teid_in);

  // Get all buffered PDCP PDUs, and forward them through tx tunnel
  std::map<uint32_t, srsran::unique_byte_buffer_t> pdus = pdcp->get_buffered_pdus(rx_tun->rnti, rx_tun->eps_bearer_id);
  for (auto& pdu_pair : pdus) {
    uint32_t pdcp_sn = pdu_pair.first;
    log_message(*tx_tun, false, srsran::make_span(pdu_pair.second), pdcp_sn);
    send_pdu_to_tunnel(*tx_tun, std::move(pdu_pair.second), pdcp_sn);
  }

  return SRSRAN_SUCCESS;
}

/****************************************************************************
 * GTP-U Error Indication
 ***************************************************************************/
void gtpu::error_indication(in_addr_t addr, in_port_t port, uint32_t err_teid)
{
  logger.info("TX GTPU Error Indication. Seq: %d, Error TEID: %d", tx_seq, err_teid);

  gtpu_header_t        header = {};
  unique_byte_buffer_t pdu    = make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Could not allocate byte buffer for error indication");
    return;
  }

  // header
  header.flags             = GTPU_FLAGS_VERSION_V1 | GTPU_FLAGS_GTP_PROTOCOL | GTPU_FLAGS_SEQUENCE;
  header.message_type      = GTPU_MSG_ERROR_INDICATION;
  header.teid              = err_teid;
  header.length            = 4;
  header.seq_number        = tx_seq;
  header.n_pdu             = 0;
  header.next_ext_hdr_type = 0;

  gtpu_write_header(&header, pdu.get(), logger);

  struct sockaddr_in servaddr;
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = addr;
  servaddr.sin_port        = port;

  sendto(fd, pdu->msg, 12, MSG_EOR, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in));
  tx_seq++;
}

/****************************************************************************
 * GTP-U Echo Request/Response
 ***************************************************************************/
void gtpu::echo_response(in_addr_t addr, in_port_t port, uint16_t seq)
{
  logger.info("TX GTPU Echo Response, Seq: %d", seq);

  gtpu_header_t        header = {};
  unique_byte_buffer_t pdu    = make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Could not allocate byte buffer for echo response");
    return;
  }

  // header
  header.flags             = GTPU_FLAGS_VERSION_V1 | GTPU_FLAGS_GTP_PROTOCOL | GTPU_FLAGS_SEQUENCE;
  header.message_type      = GTPU_MSG_ECHO_RESPONSE;
  header.teid              = 0;
  header.length            = 4;
  header.seq_number        = seq;
  header.n_pdu             = 0;
  header.next_ext_hdr_type = 0;

  gtpu_write_header(&header, pdu.get(), logger);

  struct sockaddr_in servaddr;
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = addr;
  servaddr.sin_port        = port;

  sendto(fd, pdu->msg, 12, MSG_EOR, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in));
}

/****************************************************************************
 * GTP-U END MARKER
 ***************************************************************************/
bool gtpu::send_end_marker(uint32_t teidin)
{
  const gtpu_tunnel* tx_tun = tunnels.find_tunnel(teidin);
  if (tx_tun == nullptr) {
    logger.error("TEID=%d not found to send the end marker to", teidin);
    return false;
  }
  logger.info("Tx GTPU End Marker, " TEID_IN_FMT ", rnti=0x%x", teidin, tx_tun->rnti);

  gtpu_header_t        header = {};
  unique_byte_buffer_t pdu    = make_byte_buffer();
  if (pdu == nullptr) {
    logger.warning("Failed to allocate buffer to send End Marker to TEID=%d", teidin);
    return false;
  }

  // header
  header.flags        = GTPU_FLAGS_VERSION_V1 | GTPU_FLAGS_GTP_PROTOCOL;
  header.message_type = GTPU_MSG_END_MARKER;
  header.teid         = tx_tun->teid_out;
  header.length       = 0;

  gtpu_write_header(&header, pdu.get(), logger);

  struct sockaddr_in servaddr = {};
  servaddr.sin_family         = AF_INET;
  servaddr.sin_addr.s_addr    = htonl(tx_tun->spgw_addr);
  servaddr.sin_port           = htons(GTPU_PORT);

  bool success =
      sendto(fd, pdu->msg, pdu->N_bytes, MSG_EOR, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in)) > 0;
  if (success) {
    tunnels.deactivate_tunnel(tx_tun->teid_in);
  }
  return success;
}

/****************************************************************************
 * TEID to RNTI/LCID helper functions
 ***************************************************************************/

void gtpu::log_message(const gtpu_tunnel& tun, bool is_rx, srsran::span<uint8_t> pdu, int pdcp_sn)
{
  struct iphdr* ip_pkt = (struct iphdr*)pdu.data();
  if (ip_pkt->version != 4 && ip_pkt->version != 6) {
    logger.error("%s SDU with invalid IP version %s SPGW", is_rx ? "Received" : "Sending", is_rx ? "from" : "to");
    return;
  }
  if (not logger.info.enabled()) {
    return;
  }

  fmt::basic_memory_buffer<char, 1024> strbuf;
  const char*                          dir = "Tx";
  fmt::memory_buffer                   strbuf2, addrbuf;
  srsran::gtpu_ntoa(addrbuf, htonl(tun.spgw_addr));
  if (is_rx) {
    dir = "Rx";
    fmt::format_to(strbuf2, "{}:0x{:0x} > ", srsran::to_c_str(addrbuf), tun.teid_in);
    switch (tun.state) {
      case gtpu_tunnel_manager::tunnel_state::buffering:
        fmt::format_to(strbuf2, "DL (buffered), ");
        break;
      case gtpu_tunnel_manager::tunnel_state::forward_to: {
        addrbuf.clear();
        srsran::gtpu_ntoa(addrbuf, htonl(tun.fwd_tunnel->spgw_addr));
        fmt::format_to(strbuf2, "{}:0x{:0x} (forwarded), ", srsran::to_c_str(addrbuf), tun.fwd_tunnel->teid_in);
        break;
      }
      case gtpu_tunnel_manager::tunnel_state::pdcp_active:
        fmt::format_to(strbuf2, "DL, ");
        break;
      default:
        logger.error(TEID_IN_FMT " found in invalid state: %d", tun.teid_in, (int)tun.state);
        break;
    }
  } else {
    if (pdcp_sn >= 0) {
      fmt::format_to(strbuf2, "DL PDCP SDU SN={} ", pdcp_sn);
    } else {
      fmt::format_to(strbuf2, "UL ");
    }
    fmt::format_to(strbuf2, "> {}:0x{:0x}, ", srsran::to_c_str(addrbuf), tun.teid_in);
  }
  fmt::format_to(strbuf,
                 "{} S1-U SDU, {}rnti=0x{:0x}, eps-BearerID={}, n_bytes={}, IPv{}",
                 dir,
                 fmt::to_string(strbuf2),
                 tun.rnti,
                 tun.eps_bearer_id,
                 pdu.size(),
                 (int)ip_pkt->version);
  if (ip_pkt->version == 4) {
    addrbuf.clear();
    strbuf2.clear();
    srsran::gtpu_ntoa(addrbuf, ip_pkt->saddr);
    srsran::gtpu_ntoa(strbuf2, ip_pkt->daddr);
    fmt::format_to(strbuf, " {} > {}", srsran::to_c_str(addrbuf), srsran::to_c_str(strbuf2));
    if (ntohs(ip_pkt->tot_len) != pdu.size()) {
      logger.error("IP Len and PDU N_bytes mismatch");
    }
  }
  logger.info(pdu.data(), pdu.size(), "%s", srsran::to_c_str(strbuf));
}

/****************************************************************************
 * Class to handle MCH packet handling
 ***************************************************************************/

gtpu::m1u_handler::~m1u_handler()
{
  if (initiated) {
    close(m1u_sd);
    initiated = false;
  }
}

bool gtpu::m1u_handler::init(std::string m1u_multiaddr_, std::string m1u_if_addr_)
{
  m1u_multiaddr = std::move(m1u_multiaddr_);
  m1u_if_addr   = std::move(m1u_if_addr_);
  pdcp          = parent->pdcp;

  // Set up sink socket
  struct sockaddr_in bindaddr = {};
  m1u_sd                      = socket(AF_INET, SOCK_DGRAM, 0);
  if (m1u_sd < 0) {
    logger.error("Failed to create M1-U sink socket");
    return false;
  }

  /* Bind socket */
  bindaddr.sin_family      = AF_INET;
  bindaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Multicast sockets require bind to INADDR_ANY
  bindaddr.sin_port        = htons(GTPU_PORT + 1);
  if (bind(m1u_sd, (struct sockaddr*)&bindaddr, sizeof(bindaddr)) < 0) {
    logger.error("Failed to bind multicast socket");
    return false;
  }

  /* Send an ADD MEMBERSHIP message via setsockopt */
  struct ip_mreq mreq {};
  // Multicast address of the service
  if (inet_pton(AF_INET, m1u_multiaddr.c_str(), &mreq.imr_multiaddr) != 1) {
    logger.error("Invalid m1u_multiaddr: %s", m1u_multiaddr.c_str());
    srsran::console("Invalid m1u_multiaddr: %s\n", m1u_multiaddr.c_str());
    perror("inet_pton");
    return false;
  }
  // Address of the IF the socket will listen to.
  if (inet_pton(AF_INET, m1u_if_addr.c_str(), &mreq.imr_interface) != 1) {
    logger.error("Invalid m1u_if_addr: %s", m1u_if_addr.c_str());
    srsran::console("Invalid m1u_if_addr: %s\n", m1u_if_addr.c_str());
    perror("inet_pton");
    return false;
  }
  if (setsockopt(m1u_sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    logger.error("Register musticast group for M1-U");
    logger.error("M1-U infterface IP: %s, M1-U Multicast Address %s", m1u_if_addr.c_str(), m1u_multiaddr.c_str());
    return false;
  }
  logger.info("M1-U initialized");

  initiated      = true;
  bearer_counter = 1;

  // Assign a handler to rx M1U packets
  auto rx_callback = [this](srsran::unique_byte_buffer_t pdu, const sockaddr_in& from) {
    parent->handle_gtpu_m1u_rx_packet(std::move(pdu), from);
  };
  parent->rx_socket_handler->add_socket_handler(m1u_sd,
                                                srsran::make_sdu_handler(logger, parent->gtpu_queue, rx_callback));

  return true;
}

void gtpu::m1u_handler::handle_rx_packet(srsran::unique_byte_buffer_t pdu, const sockaddr_in& addr)
{
  logger.debug("Received %d bytes from M1-U interface", pdu->N_bytes);

  gtpu_header_t header;
  gtpu_read_header(pdu.get(), &header, logger);
  pdcp->write_sdu(SRSRAN_MRNTI, bearer_counter, std::move(pdu));
}

} // namespace srsenb
