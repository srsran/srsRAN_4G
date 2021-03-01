/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "srslte/upper/gtpu.h"
#include "srsenb/hdr/stack/upper/gtpu.h"
#include "srslte/common/network_utils.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_pdcp_interfaces.h"
#include <errno.h>
#include <linux/ip.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace srslte;
namespace srsenb {

gtpu::gtpu(srslog::basic_logger& logger) : m1u(this), logger(logger) {}

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
    return SRSLTE_ERROR;
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
    srslte::console("Failed to bind on address %s, port %d: %s\n", gtp_bind_addr.c_str(), int(GTPU_PORT), errbuf);
    return SRSLTE_ERROR;
  }

  stack->add_gtpu_s1u_socket_handler(fd);

  // Start MCH socket if enabled
  enable_mbsfn = enable_mbsfn_;
  if (enable_mbsfn) {
    if (not m1u.init(m1u_multiaddr_, m1u_if_addr_)) {
      return SRSLTE_ERROR;
    }
  }
  return SRSLTE_SUCCESS;
}

void gtpu::stop()
{
  if (fd) {
    close(fd);
  }
}

// gtpu_interface_pdcp
void gtpu::write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  srslte::span<uint32_t> teids = get_lcid_teids(rnti, lcid);
  if (teids.empty()) {
    return;
  }
  tunnel& tx_tun = tunnels[teids[0]];
  log_message(tx_tun, false, srslte::make_span(pdu));
  send_pdu_to_tunnel(tx_tun, std::move(pdu));
}

void gtpu::send_pdu_to_tunnel(tunnel& tx_tun, srslte::unique_byte_buffer_t pdu, int pdcp_sn)
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
  uint32_t teid_in  = ++next_teid_in;
  tunnel&  new_tun  = tunnels[teid_in];
  new_tun.teid_in   = teid_in;
  new_tun.rnti      = rnti;
  new_tun.lcid      = lcid;
  new_tun.spgw_addr = addr;
  new_tun.teid_out  = teid_out;

  ue_teidin_db[rnti][lcid].push_back(teid_in);

  logger.info("Adding bearer for rnti: 0x%x, lcid: %d, addr: 0x%x, teid_out: 0x%x, teid_in: 0x%x",
              rnti,
              lcid,
              addr,
              teid_out,
              teid_in);

  if (props != nullptr) {
    if (props->flush_before_teidin_present) {
      // GTPU should wait for the bearer ctxt to arrive before sending SDUs from DL tunnel to PDCP
      new_tun.dl_enabled = false;
      // GTPU should not forward SDUs from main tunnel until the SeNB-TeNB tunnel has been flushed
      tunnel& after_tun               = tunnels.at(props->flush_before_teidin);
      after_tun.dl_enabled            = false;
      after_tun.prior_teid_in_present = true;
      after_tun.prior_teid_in         = teid_in;
    }

    // Connect tunnels if forwarding is activated
    if (props->forward_from_teidin_present) {
      if (create_dl_fwd_tunnel(props->forward_from_teidin, teid_in) != SRSLTE_SUCCESS) {
        rem_tunnel(teid_in);
        return 0;
      }
    }
  }

  return teid_in;
}

void gtpu::set_tunnel_status(uint32_t teidin, bool dl_active)
{
  auto tun_it = tunnels.find(teidin);
  if (tun_it == tunnels.end()) {
    logger.warning("Setting TEID=%d status", teidin);
    return;
  }
  tun_it->second.dl_enabled = dl_active;
  if (dl_active) {
    logger.info("Activing GTPU tunnel rnti=0x%x,TEID=%d. %d SDUs currently buffered",
                tun_it->second.rnti,
                teidin,
                tun_it->second.buffer.size());
    for (auto& sdu_it : tun_it->second.buffer) {
      pdcp->write_sdu(tun_it->second.rnti,
                      tun_it->second.lcid,
                      std::move(sdu_it.second),
                      sdu_it.first == undefined_pdcp_sn ? -1 : sdu_it.first);
    }
    tun_it->second.buffer.clear();
  }
}

void gtpu::rem_bearer(uint16_t rnti, uint32_t lcid)
{
  auto ue_it = ue_teidin_db.find(rnti);
  if (ue_it == ue_teidin_db.end()) {
    logger.warning("Removing bearer rnti=0x%x, lcid=%d", rnti, lcid);
    return;
  }
  std::vector<uint32_t>& lcid_tuns = ue_it->second[lcid];

  while (not lcid_tuns.empty()) {
    rem_tunnel(lcid_tuns.back());
  }
  logger.info("Removing bearer for rnti: 0x%x, lcid: %d", rnti, lcid);

  bool rem_ue = std::all_of(
      ue_it->second.begin(), ue_it->second.end(), [](const std::vector<uint32_t>& list) { return list.empty(); });
  if (rem_ue) {
    ue_teidin_db.erase(ue_it);
  }
}

void gtpu::mod_bearer_rnti(uint16_t old_rnti, uint16_t new_rnti)
{
  logger.info("Modifying bearer rnti. Old rnti: 0x%x, new rnti: 0x%x", old_rnti, new_rnti);

  if (ue_teidin_db.count(new_rnti) != 0) {
    logger.error("New rnti already exists, aborting.");
    return;
  }
  auto old_it = ue_teidin_db.find(old_rnti);
  if (old_it == ue_teidin_db.end()) {
    logger.error("Old rnti does not exist, aborting.");
    return;
  }

  // Change RNTI bearers map
  ue_teidin_db.insert(std::make_pair(new_rnti, std::move(old_it->second)));
  ue_teidin_db.erase(old_it);

  // Change TEID
  auto new_it = ue_teidin_db.find(new_rnti);
  for (auto& bearer : new_it->second) {
    for (uint32_t teid : bearer) {
      tunnels[teid].rnti = new_rnti;
    }
  }
}

void gtpu::rem_tunnel(uint32_t teidin)
{
  auto it = tunnels.find(teidin);
  if (it == tunnels.end()) {
    logger.warning("Removing GTPU tunnel TEID In=0x%x", teidin);
    return;
  }
  if (it->second.fwd_teid_in_present) {
    // Forward End Marker to forwarding tunnel, before deleting tunnel
    end_marker(it->second.fwd_teid_in);
    it->second.fwd_teid_in_present = false;
  }
  auto                   ue_it        = ue_teidin_db.find(it->second.rnti);
  std::vector<uint32_t>& lcid_tunnels = ue_it->second[it->second.lcid];
  lcid_tunnels.erase(std::remove(lcid_tunnels.begin(), lcid_tunnels.end(), teidin), lcid_tunnels.end());
  tunnels.erase(it);
  logger.debug("TEID In=%d erased", teidin);
}

void gtpu::rem_user(uint16_t rnti)
{
  logger.info("Removing rnti=0x%x", rnti);
  auto ue_it = ue_teidin_db.find(rnti);
  if (ue_it != ue_teidin_db.end()) {
    for (auto& bearer : ue_it->second) {
      while (not bearer.empty()) {
        rem_tunnel(bearer.back());
      }
    }
  }
}

void gtpu::handle_gtpu_s1u_rx_packet(srslte::unique_byte_buffer_t pdu, const sockaddr_in& addr)
{
  logger.debug("Received %d bytes from S1-U interface", pdu->N_bytes);
  pdu->set_timestamp();

  gtpu_header_t header;
  if (not gtpu_read_header(pdu.get(), &header, logger)) {
    return;
  }

  if (header.teid != 0 && tunnels.count(header.teid) == 0) {
    // Received G-PDU for non-existing and non-zero TEID.
    // Sending GTP-U error indication
    error_indication(addr.sin_addr.s_addr, addr.sin_port, header.teid);
    return;
  }

  switch (header.message_type) {
    case GTPU_MSG_ECHO_REQUEST:
      // Echo request - send response
      echo_response(addr.sin_addr.s_addr, addr.sin_port, header.seq_number);
      break;
    case GTPU_MSG_DATA_PDU: {
      auto&    rx_tun = tunnels.find(header.teid)->second;
      uint16_t rnti   = rx_tun.rnti;
      uint16_t lcid   = rx_tun.lcid;

      log_message(rx_tun, true, srslte::make_span(pdu));

      if (lcid < SRSENB_N_SRB || lcid >= SRSENB_N_RADIO_BEARERS) {
        logger.error("Invalid LCID for DL PDU: %d - dropping packet", lcid);
        return;
      }
      struct iphdr* ip_pkt = (struct iphdr*)pdu->msg;
      if (ip_pkt->version != 4 && ip_pkt->version != 6) {
        return;
      }

      if (rx_tun.fwd_teid_in_present) {
        tunnel& tx_tun = tunnels.at(rx_tun.fwd_teid_in);
        send_pdu_to_tunnel(tx_tun, std::move(pdu));
      } else {
        uint32_t pdcp_sn = undefined_pdcp_sn;
        if (header.flags & GTPU_FLAGS_EXTENDED_HDR and header.next_ext_hdr_type == GTPU_EXT_HEADER_PDCP_PDU_NUMBER) {
          pdcp_sn = (header.ext_buffer[1] << 8u) + header.ext_buffer[2];
        }
        if (not rx_tun.dl_enabled) {
          rx_tun.buffer.insert(std::make_pair(pdcp_sn, std::move(pdu)));
        } else {
          pdcp->write_sdu(rnti, lcid, std::move(pdu), pdcp_sn == undefined_pdcp_sn ? -1 : pdcp_sn);
        }
      }
    } break;
    case GTPU_MSG_END_MARKER: {
      tunnel&  old_tun = tunnels.find(header.teid)->second;
      uint16_t rnti    = old_tun.rnti;
      logger.info("Received GTPU End Marker for rnti=0x%x.", rnti);

      // TS 36.300, Sec 10.1.2.2.1 - Path Switch upon handover
      if (old_tun.fwd_teid_in_present) {
        // END MARKER should be forwarded to TeNB if forwarding is activated
        end_marker(old_tun.fwd_teid_in);
        old_tun.fwd_teid_in_present = false;
      } else {
        // TeNB switches paths, and flush PDUs that have been buffered
        std::vector<uint32_t>& bearer_tunnels = ue_teidin_db.find(old_tun.rnti)->second[old_tun.lcid];
        for (uint32_t new_teidin : bearer_tunnels) {
          tunnel& new_tun = tunnels.at(new_teidin);
          if (new_teidin != old_tun.teid_in and new_tun.prior_teid_in_present and
              new_tun.prior_teid_in == old_tun.teid_in) {
            new_tun.prior_teid_in_present = false;
            set_tunnel_status(new_tun.teid_in, true);
          }
        }
      }
      break;
    }
    default:
      break;
  }
}

void gtpu::handle_gtpu_m1u_rx_packet(srslte::unique_byte_buffer_t pdu, const sockaddr_in& addr)
{
  m1u.handle_rx_packet(std::move(pdu), addr);
}

/// Connect created tunnel with pre-existing tunnel for data forwarding
int gtpu::create_dl_fwd_tunnel(uint32_t rx_teid_in, uint32_t tx_teid_in)
{
  auto rx_tun_pair = tunnels.find(rx_teid_in);
  auto tx_tun_pair = tunnels.find(tx_teid_in);
  if (rx_tun_pair == tunnels.end() or tx_tun_pair == tunnels.end()) {
    logger.error("Failed to create forwarding tunnel between teids 0x%x and 0x%x", rx_teid_in, tx_teid_in);
    return SRSLTE_ERROR;
  }

  tunnel &rx_tun = rx_tun_pair->second, &tx_tun = tx_tun_pair->second;
  rx_tun.fwd_teid_in_present = true;
  rx_tun.fwd_teid_in         = tx_teid_in;
  logger.info("Creating forwarding tunnel for rnti=0x%x, lcid=%d, in={0x%x, 0x%x}->out={0x%x, 0x%x}",
              rx_tun.rnti,
              rx_tun.lcid,
              rx_tun.teid_out,
              rx_tun.spgw_addr,
              tx_tun.teid_out,
              tx_tun.spgw_addr);

  // Get all buffered PDCP PDUs, and forward them through tx tunnel
  std::map<uint32_t, srslte::unique_byte_buffer_t> pdus = pdcp->get_buffered_pdus(rx_tun.rnti, rx_tun.lcid);
  for (auto& pdu_pair : pdus) {
    log_message(tx_tun, false, srslte::make_span(pdu_pair.second), pdu_pair.first);
    send_pdu_to_tunnel(tx_tun, std::move(pdu_pair.second), pdu_pair.first);
  }

  return SRSLTE_SUCCESS;
}

/****************************************************************************
 * GTP-U Error Indication
 ***************************************************************************/
void gtpu::error_indication(in_addr_t addr, in_port_t port, uint32_t err_teid)
{
  logger.info("TX GTPU Error Indication. Seq: %d, Error TEID: %d", tx_seq, err_teid);

  gtpu_header_t        header = {};
  unique_byte_buffer_t pdu    = make_byte_buffer();

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
void gtpu::end_marker(uint32_t teidin)
{
  logger.info("TX GTPU End Marker.");
  tunnel& tunnel = tunnels.find(teidin)->second;

  gtpu_header_t        header = {};
  unique_byte_buffer_t pdu    = make_byte_buffer();

  // header
  header.flags        = GTPU_FLAGS_VERSION_V1 | GTPU_FLAGS_GTP_PROTOCOL;
  header.message_type = GTPU_MSG_END_MARKER;
  header.teid         = tunnel.teid_out;
  header.length       = 0;

  gtpu_write_header(&header, pdu.get(), logger);

  struct sockaddr_in servaddr = {};
  servaddr.sin_family         = AF_INET;
  servaddr.sin_addr.s_addr    = htonl(tunnel.spgw_addr);
  servaddr.sin_port           = htons(GTPU_PORT);

  sendto(fd, pdu->msg, pdu->N_bytes, MSG_EOR, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in));
}

/****************************************************************************
 * TEID to RNTI/LCID helper functions
 ***************************************************************************/

gtpu::tunnel* gtpu::get_tunnel(uint32_t teidin)
{
  auto it = tunnels.find(teidin);
  if (it == tunnels.end()) {
    logger.error("TEID=%d In does not exist.", teidin);
    return nullptr;
  }
  return &it->second;
}

srslte::span<uint32_t> gtpu::get_lcid_teids(uint16_t rnti, uint32_t lcid)
{
  auto ue_it = ue_teidin_db.find(rnti);
  if (ue_it == ue_teidin_db.end() or lcid < SRSENB_N_SRB or lcid >= SRSENB_N_RADIO_BEARERS or
      ue_it->second[lcid].empty()) {
    logger.error("Could not find bearer rnti=0x%x, lcid=%d", rnti, lcid);
    return {};
  }
  return ue_it->second[lcid];
}

void gtpu::log_message(tunnel& tun, bool is_rx, srslte::span<uint8_t> pdu, int pdcp_sn)
{
  fmt::basic_memory_buffer<char, 1024> strbuf;
  struct iphdr*                        ip_pkt = (struct iphdr*)pdu.data();
  if (ip_pkt->version != 4 && ip_pkt->version != 6) {
    logger.error("%s SDU with invalid IP version %s SPGW", is_rx ? "Received" : "Sending", is_rx ? "from" : "to");
    return;
  }

  const char*        dir = "Tx";
  fmt::memory_buffer strbuf2;
  if (is_rx) {
    dir = "Rx";
    fmt::format_to(strbuf2, "{}:0x{:0x} > ", srslte::gtpu_ntoa(htonl(tun.spgw_addr)), tun.teid_in);
    if (not tun.dl_enabled) {
      fmt::format_to(strbuf2, "DL (buffered), ");
    } else if (tun.fwd_teid_in_present) {
      tunnel& tx_tun = tunnels.at(tun.fwd_teid_in);
      fmt::format_to(strbuf2, "{}:0x{:0x} (forwarded), ", srslte::gtpu_ntoa(htonl(tx_tun.spgw_addr)), tx_tun.teid_in);
    } else {
      fmt::format_to(strbuf2, "DL, ");
    }
  } else {
    if (pdcp_sn >= 0) {
      fmt::format_to(strbuf2, "DL PDCP SDU SN={} ", pdcp_sn);
    } else {
      fmt::format_to(strbuf2, "UL ");
    }
    fmt::format_to(strbuf2, "> {}:0x{:0x}, ", srslte::gtpu_ntoa(htonl(tun.spgw_addr)), tun.teid_in);
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
    fmt::format_to(strbuf, " {} > {}", srslte::gtpu_ntoa(ip_pkt->saddr), srslte::gtpu_ntoa(ip_pkt->daddr));
    if (ntohs(ip_pkt->tot_len) != pdu.size()) {
      logger.error("IP Len and PDU N_bytes mismatch");
    }
  }
  logger.info(pdu.data(), pdu.size(), fmt::to_string(strbuf));
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

void gtpu::m1u_handler::handle_rx_packet(srslte::unique_byte_buffer_t pdu, const sockaddr_in& addr)
{
  logger.debug("Received %d bytes from M1-U interface", pdu->N_bytes);

  gtpu_header_t header;
  gtpu_read_header(pdu.get(), &header, logger);
  pdcp->write_sdu(SRSLTE_MRNTI, lcid_counter, std::move(pdu));
}

} // namespace srsenb
