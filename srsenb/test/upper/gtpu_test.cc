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

#include "srsran/asn1/s1ap.h"
#include <linux/ip.h>
#include <numeric>
#include <random>

#include "srsenb/hdr/stack/upper/gtpu.h"
#include "srsenb/test/common/dummy_classes.h"
#include "srsran/common/network_utils.h"
#include "srsran/common/test_common.h"
#include "srsran/upper/gtpu.h"

namespace srsenb {

static const size_t PDU_HEADER_SIZE = 20;

class pdcp_tester : public pdcp_dummy
{
public:
  void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu, int pdcp_sn) override
  {
    last_sdu     = std::move(sdu);
    last_pdcp_sn = pdcp_sn;
    last_rnti    = rnti;
    last_lcid    = lcid;
  }
  std::map<uint32_t, srsran::unique_byte_buffer_t> get_buffered_pdus(uint16_t rnti, uint32_t lcid) override
  {
    return std::move(buffered_pdus);
  }
  void send_status_report(uint16_t rnti) override {}
  void send_status_report(uint16_t rnti, uint32_t lcid) override {}

  void push_buffered_pdu(uint32_t sn, srsran::unique_byte_buffer_t pdu) { buffered_pdus[sn] = std::move(pdu); }

  void clear()
  {
    last_sdu     = nullptr;
    last_pdcp_sn = -1;
    last_lcid    = 0;
    last_rnti    = SRSRAN_INVALID_RNTI;
  }

  std::map<uint32_t, srsran::unique_byte_buffer_t> buffered_pdus;
  srsran::unique_byte_buffer_t                     last_sdu;
  int                                              last_pdcp_sn = -1;
  uint16_t                                         last_rnti    = SRSRAN_INVALID_RNTI;
  uint32_t                                         last_lcid    = 0;
};

struct dummy_socket_manager : public srsran::socket_manager_itf {
  dummy_socket_manager() : srsran::socket_manager_itf(srslog::fetch_basic_logger("TEST")) {}

  /// Register (fd, callback). callback is called within socket thread when fd has data.
  bool add_socket_handler(int fd, recv_callback_t handler) final
  {
    if (s1u_fd >= 0) {
      return false;
    }
    s1u_fd   = fd;
    callback = std::move(handler);
    return true;
  }

  /// remove registered socket fd
  bool remove_socket(int fd) final
  {
    if (s1u_fd < 0) {
      return false;
    }
    s1u_fd = -1;
    return true;
  }

  int             s1u_fd = -1;
  recv_callback_t callback;
};

const int GTPU_PORT = 2152;

srsran::unique_byte_buffer_t encode_ipv4_packet(srsran::span<uint8_t>     data,
                                                uint32_t                  teid,
                                                const struct sockaddr_in& src_sockaddr_in,
                                                const struct sockaddr_in& dest_sockaddr_in)
{
  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();

  struct iphdr ip_pkt = {};
  ip_pkt.version      = 4;
  ip_pkt.tot_len      = htons(data.size() + sizeof(struct iphdr));
  ip_pkt.saddr        = src_sockaddr_in.sin_addr.s_addr;
  ip_pkt.daddr        = dest_sockaddr_in.sin_addr.s_addr;
  pdu->append_bytes((uint8_t*)&ip_pkt, sizeof(struct iphdr));
  pdu->append_bytes(data.data(), data.size());

  return pdu;
}

srsran::unique_byte_buffer_t encode_gtpu_packet(srsran::span<uint8_t>     data,
                                                uint32_t                  teid,
                                                const struct sockaddr_in& src_sockaddr_in,
                                                const struct sockaddr_in& dest_sockaddr_in)
{
  srsran::unique_byte_buffer_t pdu = encode_ipv4_packet(data, teid, src_sockaddr_in, dest_sockaddr_in);

  // header
  srsran::gtpu_header_t header = {};
  header.flags                 = GTPU_FLAGS_VERSION_V1 | GTPU_FLAGS_GTP_PROTOCOL;
  header.message_type          = GTPU_MSG_DATA_PDU;
  header.length                = pdu->N_bytes;
  header.teid                  = teid;

  gtpu_write_header(&header, pdu.get(), srslog::fetch_basic_logger("GTPU"));
  return pdu;
}

srsran::unique_byte_buffer_t encode_end_marker(uint32_t teid)
{
  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();

  // header
  srsran::gtpu_header_t header = {};
  header.flags                 = GTPU_FLAGS_VERSION_V1 | GTPU_FLAGS_GTP_PROTOCOL;
  header.message_type          = GTPU_MSG_END_MARKER;
  header.length                = 0;
  header.teid                  = teid;

  gtpu_write_header(&header, pdu.get(), srslog::fetch_basic_logger("GTPU"));
  return pdu;
}

srsran::unique_byte_buffer_t read_socket(int fd)
{
  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  pdu->N_bytes                     = read(fd, pdu->msg, pdu->get_tailroom());
  return pdu;
}

void test_gtpu_tunnel_manager()
{
  const char*        sgw_addr_str = "127.0.0.1";
  struct sockaddr_in sgw_sockaddr = {};
  srsran::net_utils::set_sockaddr(&sgw_sockaddr, sgw_addr_str, GTPU_PORT);
  uint32_t               sgw_addr  = ntohl(sgw_sockaddr.sin_addr.s_addr);
  const uint32_t         drb1_lcid = 3;
  srsran::task_scheduler task_sched;

  gtpu_tunnel_manager tunnels(&task_sched, srslog::fetch_basic_logger("GTPU"));
  TESTASSERT(tunnels.find_tunnel(0) == nullptr);
  TESTASSERT(tunnels.find_rnti_lcid_tunnels(0x46, drb1_lcid).empty());
  TESTASSERT(tunnels.find_rnti_tunnels(0x46) == nullptr);

  // Creation of tunnels for different users and lcids
  const gtpu_tunnel* tun = tunnels.add_tunnel(0x46, drb1_lcid, 5, sgw_addr);
  TESTASSERT(tun != nullptr);
  TESTASSERT(tunnels.find_tunnel(tun->teid_in) == tun);
  const gtpu_tunnel* tun2 = tunnels.add_tunnel(0x47, drb1_lcid, 6, sgw_addr);
  TESTASSERT(tun2 != nullptr);
  TESTASSERT(tunnels.find_tunnel(tun2->teid_in) == tun2);
  tun2 = tunnels.add_tunnel(0x47, drb1_lcid + 1, 7, sgw_addr);
  TESTASSERT(tun2 != nullptr);
  TESTASSERT(tunnels.find_tunnel(tun2->teid_in) == tun2);
  TESTASSERT(tunnels.find_rnti_lcid_tunnels(0x46, drb1_lcid).size() == 1);
  TESTASSERT(tunnels.find_rnti_lcid_tunnels(0x47, drb1_lcid).size() == 1);
  TESTASSERT(tunnels.find_rnti_lcid_tunnels(0x47, drb1_lcid + 1).size() == 1);

  // TEST: Creation/Removal of indirect tunnel
  const gtpu_tunnel* fwd_tun = tunnels.add_tunnel(0x46, drb1_lcid, 8, sgw_addr);
  TESTASSERT(fwd_tun != nullptr);
  TESTASSERT(tunnels.find_tunnel(fwd_tun->teid_in) == fwd_tun);
  tunnels.setup_forwarding(tun->teid_in, fwd_tun->teid_in);
  TESTASSERT(tunnels.find_rnti_lcid_tunnels(0x46, drb1_lcid).size() == 2);
  // Removing a tunnel also clears any associated forwarding tunnel
  TESTASSERT(tunnels.remove_tunnel(tun->teid_in));
  TESTASSERT(tunnels.find_rnti_lcid_tunnels(0x46, drb1_lcid).empty());

  // TEST: Prioritization of one TEID over another
  const gtpu_tunnel* before_tun = tunnels.add_tunnel(0x46, drb1_lcid, 7, sgw_addr);
  const gtpu_tunnel* after_tun  = tunnels.add_tunnel(0x46, drb1_lcid, 8, sgw_addr);
  TESTASSERT(before_tun != nullptr and after_tun != nullptr);
  tunnels.set_tunnel_priority(before_tun->teid_in, after_tun->teid_in);
  for (uint32_t i = 0; i < 1000; ++i) {
    TESTASSERT(before_tun->state == gtpu_tunnel_manager::tunnel_state::pdcp_active);
    TESTASSERT(after_tun->state == gtpu_tunnel_manager::tunnel_state::buffering);
    // while Rx packets are received, active forwarding TEID should not be removed
    tunnels.handle_rx_pdcp_sdu(before_tun->teid_in);
  }
  // Removing active TEID, will automatically switch TEID paths
  TESTASSERT(tunnels.find_rnti_lcid_tunnels(0x46, drb1_lcid).size() == 2);
  tunnels.remove_tunnel(before_tun->teid_in);
  TESTASSERT(tunnels.find_rnti_lcid_tunnels(0x46, drb1_lcid).size() == 1);
  TESTASSERT(after_tun->state == gtpu_tunnel_manager::tunnel_state::pdcp_active);
}

enum class tunnel_test_event { success, wait_end_marker_timeout };

int test_gtpu_direct_tunneling(tunnel_test_event event)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
  logger.info("\n\n**** Test GTPU Direct Tunneling ****\n");
  uint16_t           rnti = 0x46, rnti2 = 0x50;
  uint32_t           drb1         = 3;
  uint32_t           sgw_teidout1 = 1, sgw_teidout2 = 2;
  const char *       sgw_addr_str = "127.0.0.1", *senb_addr_str = "127.0.1.1", *tenb_addr_str = "127.0.1.2";
  struct sockaddr_in senb_sockaddr = {}, sgw_sockaddr = {}, tenb_sockaddr = {};
  srsran::net_utils::set_sockaddr(&senb_sockaddr, senb_addr_str, GTPU_PORT);
  srsran::net_utils::set_sockaddr(&sgw_sockaddr, sgw_addr_str, GTPU_PORT);
  srsran::net_utils::set_sockaddr(&tenb_sockaddr, tenb_addr_str, GTPU_PORT);
  uint32_t tenb_addr = ntohl(tenb_sockaddr.sin_addr.s_addr);
  uint32_t senb_addr = ntohl(senb_sockaddr.sin_addr.s_addr);
  uint32_t sgw_addr  = ntohl(sgw_sockaddr.sin_addr.s_addr);

  srsran::unique_byte_buffer_t pdu;

  // Initiate layers
  srslog::basic_logger& logger1 = srslog::fetch_basic_logger("GTPU1");
  logger1.set_hex_dump_max_size(2048);
  srslog::basic_logger& logger2 = srslog::fetch_basic_logger("GTPU2");
  logger2.set_hex_dump_max_size(2048);
  srsran::task_scheduler task_sched;
  dummy_socket_manager   senb_rx_sockets, tenb_rx_sockets;
  srsenb::gtpu senb_gtpu(&task_sched, logger1, &senb_rx_sockets), tenb_gtpu(&task_sched, logger2, &tenb_rx_sockets);
  pdcp_tester  senb_pdcp, tenb_pdcp;
  senb_gtpu.init(senb_addr_str, sgw_addr_str, "", "", &senb_pdcp, false);
  tenb_gtpu.init(tenb_addr_str, sgw_addr_str, "", "", &tenb_pdcp, false);

  // create tunnels MME-SeNB and MME-TeNB
  uint32_t senb_teid_in = senb_gtpu.add_bearer(rnti, drb1, sgw_addr, sgw_teidout1).value();
  uint32_t tenb_teid_in = tenb_gtpu.add_bearer(rnti2, drb1, sgw_addr, sgw_teidout2).value();

  // Buffer PDUs in SeNB PDCP
  for (size_t sn = 6; sn < 10; ++sn) {
    std::vector<uint8_t> data(10, sn);
    pdu = encode_ipv4_packet(data, senb_teid_in, sgw_sockaddr, senb_sockaddr);
    senb_pdcp.push_buffered_pdu(sn, std::move(pdu));
  }

  // create direct tunnel SeNB-TeNB
  gtpu::bearer_props props;
  props.flush_before_teidin_present = true;
  props.flush_before_teidin         = tenb_teid_in;
  uint32_t dl_tenb_teid_in          = tenb_gtpu.add_bearer(rnti2, drb1, senb_addr, 0, &props).value();
  props                             = {};
  props.forward_from_teidin_present = true;
  props.forward_from_teidin         = senb_teid_in;
  senb_gtpu.add_bearer(rnti, drb1, tenb_addr, dl_tenb_teid_in, &props);

  std::random_device   rd;
  std::mt19937         g(rd());
  std::vector<uint8_t> data_vec(10);
  std::iota(data_vec.begin(), data_vec.end(), 0);
  std::vector<uint8_t>  encoded_data;
  srsran::span<uint8_t> pdu_view{};

  // TEST: GTPU buffers incoming PDCP buffered SNs until the TEID is explicitly activated
  tenb_gtpu.handle_gtpu_s1u_rx_packet(read_socket(tenb_rx_sockets.s1u_fd), senb_sockaddr);
  TESTASSERT(tenb_pdcp.last_sdu == nullptr);
  tenb_gtpu.handle_gtpu_s1u_rx_packet(read_socket(tenb_rx_sockets.s1u_fd), senb_sockaddr);
  TESTASSERT(tenb_pdcp.last_sdu == nullptr);
  tenb_gtpu.set_tunnel_status(dl_tenb_teid_in, true);
  pdu_view = srsran::make_span(tenb_pdcp.last_sdu);
  TESTASSERT(std::count(pdu_view.begin() + PDU_HEADER_SIZE, pdu_view.end(), 7) == 10);
  TESTASSERT(tenb_pdcp.last_rnti == rnti2);
  TESTASSERT(tenb_pdcp.last_lcid == drb1);
  TESTASSERT(tenb_pdcp.last_pdcp_sn == (int)7);

  // TEST: verify that PDCP buffered SNs have been forwarded through SeNB->TeNB tunnel
  for (size_t sn = 8; sn < 10; ++sn) {
    tenb_gtpu.handle_gtpu_s1u_rx_packet(read_socket(tenb_rx_sockets.s1u_fd), senb_sockaddr);
    pdu_view = srsran::make_span(tenb_pdcp.last_sdu);
    TESTASSERT(std::count(pdu_view.begin() + PDU_HEADER_SIZE, pdu_view.end(), sn) == 10);
    TESTASSERT(tenb_pdcp.last_rnti == rnti2);
    TESTASSERT(tenb_pdcp.last_lcid == drb1);
    TESTASSERT(tenb_pdcp.last_pdcp_sn == (int)sn);
  }

  // TEST: verify that incoming DL data MME->SeNB is forwarded through SeNB->TeNB tunnel
  std::shuffle(data_vec.begin(), data_vec.end(), g);
  pdu = encode_gtpu_packet(data_vec, senb_teid_in, sgw_sockaddr, senb_sockaddr);
  encoded_data.assign(pdu->msg + 8u, pdu->msg + pdu->N_bytes);
  senb_gtpu.handle_gtpu_s1u_rx_packet(std::move(pdu), sgw_sockaddr);
  tenb_gtpu.handle_gtpu_s1u_rx_packet(read_socket(tenb_rx_sockets.s1u_fd), senb_sockaddr);
  pdu_view = srsran::make_span(tenb_pdcp.last_sdu);
  TESTASSERT(pdu_view.size() == encoded_data.size() and
             std::equal(pdu_view.begin(), pdu_view.end(), encoded_data.begin()));
  TESTASSERT(tenb_pdcp.last_rnti == rnti2 and tenb_pdcp.last_lcid == drb1);

  // TEST: verify that MME->TeNB packets are buffered until SeNB->TeNB tunnel is closed
  tenb_pdcp.clear();
  size_t N_pdus = std::uniform_int_distribution<size_t>{1, 30}(g);
  for (size_t i = 0; i < N_pdus; ++i) {
    std::fill(data_vec.begin(), data_vec.end(), i);
    pdu = encode_gtpu_packet(data_vec, senb_teid_in, sgw_sockaddr, tenb_sockaddr);
    tenb_gtpu.handle_gtpu_s1u_rx_packet(std::move(pdu), sgw_sockaddr);
    // The PDUs are being buffered
    TESTASSERT(tenb_pdcp.last_sdu == nullptr);
  }
  // PDUs coming from SeNB-TeNB tunnel are forwarded
  std::iota(data_vec.begin(), data_vec.end(), 0);
  std::shuffle(data_vec.begin(), data_vec.end(), g);
  pdu = encode_gtpu_packet(data_vec, senb_teid_in, sgw_sockaddr, senb_sockaddr);
  encoded_data.assign(pdu->msg + 8u, pdu->msg + pdu->N_bytes);
  senb_gtpu.handle_gtpu_s1u_rx_packet(std::move(pdu), sgw_sockaddr);
  tenb_gtpu.handle_gtpu_s1u_rx_packet(read_socket(tenb_rx_sockets.s1u_fd), senb_sockaddr);
  TESTASSERT(tenb_pdcp.last_sdu->N_bytes == encoded_data.size() and
             memcmp(tenb_pdcp.last_sdu->msg, encoded_data.data(), encoded_data.size()) == 0);
  tenb_pdcp.clear();

  TESTASSERT(tenb_pdcp.last_sdu == nullptr);
  if (event == tunnel_test_event::wait_end_marker_timeout) {
    // TEST: EndMarker does not reach TeNB, but there is a timeout that will resume the new GTPU tunnel
    for (size_t i = 0; i < 1000; ++i) {
      task_sched.tic();
    }
  } else {
    // TEST: EndMarker is forwarded via MME->SeNB->TeNB, and TeNB buffered PDUs are flushed
    pdu = encode_end_marker(senb_teid_in);
    senb_gtpu.handle_gtpu_s1u_rx_packet(std::move(pdu), sgw_sockaddr);
    tenb_gtpu.handle_gtpu_s1u_rx_packet(read_socket(tenb_rx_sockets.s1u_fd), senb_sockaddr);
  }
  srsran::span<uint8_t> encoded_data2{tenb_pdcp.last_sdu->msg + 20u, tenb_pdcp.last_sdu->msg + 30u};
  TESTASSERT(std::all_of(encoded_data2.begin(), encoded_data2.end(), [N_pdus](uint8_t b) { return b == N_pdus - 1; }));

  return SRSRAN_SUCCESS;
}

} // namespace srsenb

int main(int argc, char** argv)
{
  // Setup logging.
  auto& logger = srslog::fetch_basic_logger("GTPU", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);

  // Start the log backend.
  srsran::test_init(argc, argv);

  srsenb::test_gtpu_tunnel_manager();
  TESTASSERT(srsenb::test_gtpu_direct_tunneling(srsenb::tunnel_test_event::success) == SRSRAN_SUCCESS);
  TESTASSERT(srsenb::test_gtpu_direct_tunneling(srsenb::tunnel_test_event::wait_end_marker_timeout) == SRSRAN_SUCCESS);

  srslog::flush();

  srsran::console("Success");
}
