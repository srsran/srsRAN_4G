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

#include "srsran/upper/gtpu.h"
#include "srsenb/hdr/stack/upper/gtpu.h"
#include "srsran/common/network_utils.h"
#include "srsran/common/srsran_assert.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/enb_interfaces.h"
#include "srsran/interfaces/enb_pdcp_interfaces.h"

#include <errno.h>
#include <linux/ip.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace srsran;

namespace srsenb {

gtpu_tunnel_manager::gtpu_tunnel_manager() : logger(srslog::fetch_basic_logger("GTPU")) {}

gtpu_tunnel* gtpu_tunnel_manager::find_tunnel(uint32_t teid)
{
  auto it = tunnels.find(teid);
  return it != tunnels.end() ? &it->second : nullptr;
}

gtpu_tunnel_manager::ue_lcid_tunnel_list* gtpu_tunnel_manager::find_rnti_tunnels(uint16_t rnti)
{
  if (not ue_teidin_db.contains(rnti)) {
    return nullptr;
  }
  return &ue_teidin_db[rnti];
}

srsran::span<gtpu_tunnel_manager::lcid_tunnel> gtpu_tunnel_manager::find_rnti_lcid_tunnels(uint16_t rnti, uint32_t lcid)
{
  if (lcid < SRSENB_N_SRB or lcid >= SRSENB_N_RADIO_BEARERS) {
    logger.warning("Searching for bearer with invalid lcid=%d", lcid);
    return {};
  }
  auto* ue_ptr = find_rnti_tunnels(rnti);
  if (ue_ptr == nullptr) {
    return {};
  }
  auto lcid_it_begin = std::lower_bound(ue_ptr->begin(), ue_ptr->end(), lcid_tunnel{lcid, 0});
  auto lcid_it_end   = std::lower_bound(ue_ptr->begin(), ue_ptr->end(), lcid_tunnel{lcid + 1, 0});

  return srsran::span<lcid_tunnel>(&(*lcid_it_begin), &(*lcid_it_end));
}

gtpu_tunnel* gtpu_tunnel_manager::add_tunnel(uint16_t rnti, uint32_t lcid, uint32_t teidout, uint32_t spgw_addr)
{
  if (lcid < SRSENB_N_SRB or lcid >= SRSENB_N_RADIO_BEARERS) {
    logger.warning("Adding TEID with invalid parmaters");
    return nullptr;
  }
  auto ret_pair = tunnels.insert(gtpu_tunnel());
  if (not ret_pair) {
    logger.warning("Adding new GTPU TEID In");
    return nullptr;
  }
  gtpu_tunnel* tun = &tunnels[ret_pair.value()];
  tun->teid_in     = ret_pair.value();
  tun->rnti        = rnti;
  tun->lcid        = lcid;
  tun->teid_out    = teidout;
  tun->spgw_addr   = spgw_addr;

  if (not ue_teidin_db.contains(rnti)) {
    ue_teidin_db.insert(rnti, ue_lcid_tunnel_list());
  }
  auto& ue_tunnels = ue_teidin_db[rnti];

  if (ue_tunnels.full()) {
    logger.error("The number of TEIDs per UE exceeded for rnti=0x%x", rnti);
    tunnels.erase(tun->teid_in);
    return nullptr;
  }
  ue_tunnels.push_back(lcid_tunnel{lcid, tun->teid_in});
  std::sort(ue_tunnels.begin(), ue_tunnels.end());

  fmt::memory_buffer str_buffer;
  srsran::gtpu_ntoa(str_buffer, htonl(spgw_addr));
  logger.info("New tunnel teid_in=0x%x, teid_out=0x%x, rnti=0x%x, lcid=%d, addr=%s",
              tun->teid_in,
              teidout,
              rnti,
              lcid,
              srsran::to_c_str(str_buffer));

  return tun;
}

bool gtpu_tunnel_manager::update_rnti(uint16_t old_rnti, uint16_t new_rnti)
{
  auto* old_rnti_ptr = find_rnti_tunnels(old_rnti);
  if (old_rnti_ptr == nullptr or find_rnti_tunnels(new_rnti) != nullptr) {
    logger.error("Modifying bearer rnti. Old rnti=0x%x, new rnti=0x%x", old_rnti, new_rnti);
    return false;
  }
  logger.info("Modifying bearer rnti. Old rnti: 0x%x, new rnti: 0x%x", old_rnti, new_rnti);

  // Change RNTI bearers map
  ue_teidin_db.insert(new_rnti, std::move(*old_rnti_ptr));
  ue_teidin_db.erase(old_rnti);

  // Change TEID in existing tunnels
  auto* new_rnti_ptr = find_rnti_tunnels(new_rnti);
  for (lcid_tunnel& bearer : *new_rnti_ptr) {
    tunnels[bearer.teid].rnti = new_rnti;
  }

  return true;
}

bool gtpu_tunnel_manager::remove_tunnel(uint32_t teidin)
{
  auto it = tunnels.find(teidin);
  if (it == tunnels.end()) {
    logger.warning("Removing GTPU tunnel TEID In=0x%x", teidin);
    return false;
  }
  gtpu_tunnel& tun = it->second;

  // Erase any present forwarding tunnel
  if (tun.fwd_teid_in_present) {
    remove_tunnel(tun.fwd_teid_in);
  }

  // erase keeping the relative order
  auto& ue      = ue_teidin_db[tun.rnti];
  auto  lcid_it = std::find(ue.begin(), ue.end(), lcid_tunnel{tun.lcid, tun.teid_in});
  ue.erase(lcid_it);

  logger.info("TEID In=%d for rnti=0x%x erased", teidin, tun.rnti);
  tunnels.erase(it);
  return true;
}

bool gtpu_tunnel_manager::remove_bearer(uint16_t rnti, uint32_t lcid)
{
  srsran::span<lcid_tunnel> to_rem = find_rnti_lcid_tunnels(rnti, lcid);
  if (to_rem.empty()) {
    return false;
  }
  logger.info("Removing rnti=0x%x,lcid=%d", rnti, lcid);

  for (lcid_tunnel& lcid_tun : to_rem) {
    srsran_expect(tunnels.erase(lcid_tun.teid) > 0, "Inconsistency detected between two internal data structures");
  }
  ue_teidin_db[rnti].erase(to_rem.begin(), to_rem.end());
  return true;
}

bool gtpu_tunnel_manager::remove_rnti(uint16_t rnti)
{
  if (not ue_teidin_db.contains(rnti)) {
    logger.warning("Removing rnti. rnti=0x%x not found.", rnti);
    return false;
  }
  logger.info("Removing rnti=0x%x", rnti);

  for (lcid_tunnel& ue_tuns : ue_teidin_db[rnti]) {
    srsran_expect(tunnels.erase(ue_tuns.teid) > 0, "Inconsistency detected between two internal data structures");
  }
  ue_teidin_db.erase(rnti);
  return true;
}

/********************
 *    GTPU class
 *******************/

gtpu::gtpu(srsran::task_sched_handle task_sched_, srslog::basic_logger& logger) :
  m1u(this), task_sched(task_sched_), logger(logger)
{}

gtpu::~gtpu()
{
  stop();
}

int gtpu::init(std::string                  gtp_bind_addr_,
               std::string                  mme_addr_,
               std::string                  m1u_multiaddr_,
               std::string                  m1u_if_addr_,
               srsenb::pdcp_interface_gtpu* pdcp_,
               stack_interface_gtpu_lte*    stack_,
               bool                         enable_mbsfn_)
{
  pdcp          = pdcp_;
  gtp_bind_addr = gtp_bind_addr_;
  mme_addr      = mme_addr_;
  stack         = stack_;

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
  bindaddr.sin_family      = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr(gtp_bind_addr.c_str());
  bindaddr.sin_port        = htons(GTPU_PORT);

  if (bind(fd, (struct sockaddr*)&bindaddr, sizeof(struct sockaddr_in))) {
    snprintf(errbuf, sizeof(errbuf), "%s", strerror(errno));
    logger.error("Failed to bind on address %s, port %d: %s", gtp_bind_addr.c_str(), int(GTPU_PORT), errbuf);
    srsran::console("Failed to bind on address %s, port %d: %s\n", gtp_bind_addr.c_str(), int(GTPU_PORT), errbuf);
    return SRSRAN_ERROR;
  }

  stack->add_gtpu_s1u_socket_handler(fd);

  // Start MCH socket if enabled
  enable_mbsfn = enable_mbsfn_;
  if (enable_mbsfn) {
    if (not m1u.init(m1u_multiaddr_, m1u_if_addr_)) {
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
void gtpu::write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  srsran::span<gtpu_tunnel_manager::lcid_tunnel> teids = tunnels.find_rnti_lcid_tunnels(rnti, lcid);
  if (teids.empty()) {
    logger.warning("The rnti=0x%x,lcid=%d does not have any active tunnel", rnti, lcid);
    return;
  }
  gtpu_tunnel& tx_tun = *tunnels.find_tunnel(teids[0].teid);
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

uint32_t gtpu::add_bearer(uint16_t rnti, uint32_t lcid, uint32_t addr, uint32_t teid_out, const bearer_props* props)
{
  // Allocate a TEID for the incoming tunnel
  gtpu_tunnel* new_tun = tunnels.add_tunnel(rnti, lcid, teid_out, addr);
  if (new_tun == nullptr) {
    return -1;
  }
  uint32_t teid_in = new_tun->teid_in;

  if (props != nullptr) {
    if (props->flush_before_teidin_present) {
      // GTPU should wait for the bearer ctxt to arrive before sending SDUs from DL tunnel to PDCP
      new_tun->dl_enabled = false;
      new_tun->buffer.emplace();
      // GTPU should not forward SDUs from main tunnel until the SeNB-TeNB tunnel has been flushed
      gtpu_tunnel* after_tun = tunnels.find_tunnel(props->flush_before_teidin);
      if (after_tun == nullptr) {
        logger.error("Setting priority relation between tunnels. TEID=0x%x not found", props->flush_before_teidin);
        tunnels.remove_tunnel(teid_in);
        return -1;
      }
      after_tun->dl_enabled = false;
      after_tun->buffer.emplace();
      after_tun->prior_teid_in_present = true;
      after_tun->prior_teid_in         = teid_in;

      // Schedule autoremoval of this indirect tunnel
      uint32_t after_teidin  = after_tun->teid_in;
      uint32_t before_teidin = new_tun->teid_in;
      new_tun->rx_timer      = task_sched.get_unique_timer();
      new_tun->rx_timer.set(500, [this, before_teidin, after_teidin](uint32_t tid) {
        gtpu_tunnel* after_tun = tunnels.find_tunnel(after_teidin);
        if (after_tun != nullptr) {
          if (after_tun->prior_teid_in_present) {
            after_tun->prior_teid_in_present = false;
            set_tunnel_status(after_tun->teid_in, true);
          }
          // else: indirect tunnel already removed
        } else {
          logger.info("Callback to automatic indirect tunnel deletion called for non-existent TEID=%d", after_teidin);
        }
        // This will self-destruct the callback object
        rem_tunnel(before_teidin);
      });
      new_tun->rx_timer.run();
    }

    // Connect tunnels if forwarding is activated
    if (props->forward_from_teidin_present) {
      if (create_dl_fwd_tunnel(props->forward_from_teidin, teid_in) != SRSRAN_SUCCESS) {
        rem_tunnel(teid_in);
        return 0;
      }
    }
  }

  return teid_in;
}

void gtpu::set_tunnel_status(uint32_t teidin, bool dl_active)
{
  gtpu_tunnel* tun = tunnels.find_tunnel(teidin);
  if (tun == nullptr) {
    logger.warning("Setting TEID=%d status", teidin);
    return;
  }

  bool old_state  = tun->dl_enabled;
  tun->dl_enabled = dl_active;
  if (dl_active and not old_state) {
    logger.info(
        "Activating GTPU tunnel rnti=0x%x,TEID=%d. %d SDUs currently buffered", tun->rnti, teidin, tun->buffer->size());
    std::stable_sort(
        tun->buffer->begin(),
        tun->buffer->end(),
        [](const std::pair<uint32_t, srsran::unique_byte_buffer_t>& lhs,
           const std::pair<uint32_t, srsran::unique_byte_buffer_t>& rhs) { return lhs.first < rhs.first; });
    for (auto& sdu_it : *tun->buffer) {
      pdcp->write_sdu(
          tun->rnti, tun->lcid, std::move(sdu_it.second), sdu_it.first == undefined_pdcp_sn ? -1 : sdu_it.first);
    }
    tun->buffer.reset();
  }
}

void gtpu::rem_bearer(uint16_t rnti, uint32_t lcid)
{
  tunnels.remove_bearer(rnti, lcid);
}

void gtpu::mod_bearer_rnti(uint16_t old_rnti, uint16_t new_rnti)
{
  tunnels.update_rnti(old_rnti, new_rnti);
}

void gtpu::rem_tunnel(uint32_t teidin)
{
  tunnels.remove_tunnel(teidin);
}

void gtpu::rem_user(uint16_t rnti)
{
  tunnels.remove_rnti(rnti);
}

void gtpu::handle_end_marker(const gtpu_tunnel& rx_tunnel)
{
  uint16_t rnti = rx_tunnel.rnti;
  logger.info("Received GTPU End Marker for rnti=0x%x.", rnti);

  // TS 36.300, Sec 10.1.2.2.1 - Path Switch upon handover
  if (rx_tunnel.fwd_teid_in_present) {
    // END MARKER should be forwarded to TeNB if forwarding is activated
    send_end_marker(rx_tunnel.fwd_teid_in);
    rem_tunnel(rx_tunnel.teid_in);
  } else {
    // TeNB switches paths, and flushes PDUs that have been buffered
    srsran::span<gtpu_tunnel_manager::lcid_tunnel> lcid_tunnels = tunnels.find_rnti_lcid_tunnels(rnti, rx_tunnel.lcid);
    for (auto& lcid_tun : lcid_tunnels) {
      gtpu_tunnel* new_tun = tunnels.find_tunnel(lcid_tun.teid);
      if (new_tun->teid_in != rx_tunnel.teid_in and new_tun->prior_teid_in_present and
          new_tun->prior_teid_in == rx_tunnel.teid_in) {
        rem_tunnel(new_tun->prior_teid_in);
        new_tun->prior_teid_in_present = false;
        set_tunnel_status(new_tun->teid_in, true);
        break;
      }
    }
  }
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

  // Find TEID present in GTPU Header
  gtpu_tunnel* tun_ptr = tunnels.find_tunnel(header.teid);
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

void gtpu::handle_msg_data_pdu(const gtpu_header_t& header, gtpu_tunnel& rx_tunnel, srsran::unique_byte_buffer_t pdu)
{
  struct iphdr* ip_pkt = (struct iphdr*)pdu->msg;
  if (ip_pkt->version != 4 && ip_pkt->version != 6) {
    logger.error("Received SDU with invalid IP version=%d", (int)ip_pkt->version);
    return;
  }

  if (rx_tunnel.rx_timer.is_valid()) {
    // Restart Rx timer
    rx_tunnel.rx_timer.run();
  }

  uint16_t rnti = rx_tunnel.rnti;
  uint16_t lcid = rx_tunnel.lcid;

  log_message(rx_tunnel, true, srsran::make_span(pdu));

  if (rx_tunnel.fwd_teid_in_present) {
    // Forward SDU to direct/indirect tunnel during Handover
    const gtpu_tunnel* tx_tun_ptr = tunnels.find_tunnel(rx_tunnel.fwd_teid_in);
    if (tx_tun_ptr == nullptr) {
      logger.error("Forwarding tunnel TEID=%d does not exist", rx_tunnel.fwd_teid_in);
      return;
    }
    send_pdu_to_tunnel(*tx_tun_ptr, std::move(pdu));

  } else {
    // Forward SDU to PDCP or buffer it if tunnel is disabled
    uint32_t pdcp_sn = undefined_pdcp_sn;
    if ((header.flags & GTPU_FLAGS_EXTENDED_HDR) != 0 and header.next_ext_hdr_type == GTPU_EXT_HEADER_PDCP_PDU_NUMBER) {
      pdcp_sn = (header.ext_buffer[1] << 8U) + header.ext_buffer[2];
    }
    if (not rx_tunnel.dl_enabled) {
      rx_tunnel.buffer->push_back(std::make_pair(pdcp_sn, std::move(pdu)));
    } else {
      pdcp->write_sdu(rnti, lcid, std::move(pdu), pdcp_sn == undefined_pdcp_sn ? -1 : (int)pdcp_sn);
    }
  }
}

void gtpu::handle_gtpu_m1u_rx_packet(srsran::unique_byte_buffer_t pdu, const sockaddr_in& addr)
{
  m1u.handle_rx_packet(std::move(pdu), addr);
}

/// Connect created tunnel with pre-existing tunnel for data forwarding
int gtpu::create_dl_fwd_tunnel(uint32_t rx_teid_in, uint32_t tx_teid_in)
{
  gtpu_tunnel* rx_tun = tunnels.find_tunnel(rx_teid_in);
  gtpu_tunnel* tx_tun = tunnels.find_tunnel(tx_teid_in);
  if (rx_tun == nullptr or tx_tun == nullptr) {
    logger.error("Failed to create forwarding tunnel between teids 0x%x and 0x%x", rx_teid_in, tx_teid_in);
    return SRSRAN_ERROR;
  }

  rx_tun->fwd_teid_in_present = true;
  rx_tun->fwd_teid_in         = tx_teid_in;
  logger.info("Creating forwarding tunnel for rnti=0x%x, lcid=%d, in={0x%x, 0x%x}->out={0x%x, 0x%x}",
              rx_tun->rnti,
              rx_tun->lcid,
              rx_tun->teid_out,
              rx_tun->spgw_addr,
              tx_tun->teid_out,
              tx_tun->spgw_addr);

  // Get all buffered PDCP PDUs, and forward them through tx tunnel
  std::map<uint32_t, srsran::unique_byte_buffer_t> pdus = pdcp->get_buffered_pdus(rx_tun->rnti, rx_tun->lcid);
  for (auto& pdu_pair : pdus) {
    log_message(*tx_tun, false, srsran::make_span(pdu_pair.second), pdu_pair.first);
    send_pdu_to_tunnel(*tx_tun, std::move(pdu_pair.second), pdu_pair.first);
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
  logger.info("TX GTPU End Marker.");
  const gtpu_tunnel* tx_tun = tunnels.find_tunnel(teidin);
  if (tx_tun == nullptr) {
    logger.error("TEID=%d not found to send the end marker to", teidin);
    return false;
  }

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

  sendto(fd, pdu->msg, pdu->N_bytes, MSG_EOR, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in));
  return true;
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
    if (not tun.dl_enabled) {
      fmt::format_to(strbuf2, "DL (buffered), ");
    } else if (tun.fwd_teid_in_present) {
      const gtpu_tunnel* tx_tun = tunnels.find_tunnel(tun.fwd_teid_in);
      srsran_assert(tx_tun != nullptr, "Invalid teid=%d", tun.fwd_teid_in);
      addrbuf.clear();
      srsran::gtpu_ntoa(addrbuf, htonl(tx_tun->spgw_addr));
      fmt::format_to(strbuf2, "{}:0x{:0x} (forwarded), ", srsran::to_c_str(addrbuf), tx_tun->teid_in);
    } else {
      fmt::format_to(strbuf2, "DL, ");
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
                 "{} S1-U SDU, {}rnti=0x{:0x}, lcid={}, n_bytes={}, IPv{}",
                 dir,
                 fmt::to_string(strbuf2),
                 tun.rnti,
                 tun.lcid,
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
  mreq.imr_multiaddr.s_addr = inet_addr(m1u_multiaddr.c_str()); // Multicast address of the service
  mreq.imr_interface.s_addr = inet_addr(m1u_if_addr.c_str());   // Address of the IF the socket will listen to.
  if (setsockopt(m1u_sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    logger.error("Register musticast group for M1-U");
    logger.error("M1-U infterface IP: %s, M1-U Multicast Address %s", m1u_if_addr.c_str(), m1u_multiaddr.c_str());
    return false;
  }
  logger.info("M1-U initialized");

  initiated    = true;
  lcid_counter = 1;

  // Register socket in stack rx sockets thread
  parent->stack->add_gtpu_m1u_socket_handler(m1u_sd);

  return true;
}

void gtpu::m1u_handler::handle_rx_packet(srsran::unique_byte_buffer_t pdu, const sockaddr_in& addr)
{
  logger.debug("Received %d bytes from M1-U interface", pdu->N_bytes);

  gtpu_header_t header;
  gtpu_read_header(pdu.get(), &header, logger);
  pdcp->write_sdu(SRSRAN_MRNTI, lcid_counter, std::move(pdu));
}

} // namespace srsenb
