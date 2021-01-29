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

#include "srslte/asn1/s1ap.h"
#include <linux/ip.h>
#include <numeric>

#include "srsenb/hdr/stack/upper/gtpu.h"
#include "srsenb/test/common/dummy_classes.h"
#include "srslte/common/network_utils.h"
#include "srslte/common/test_common.h"
#include "srslte/upper/gtpu.h"

namespace srsenb {

class stack_tester : public stack_interface_gtpu_lte
{
public:
  int  s1u_fd;
  void add_gtpu_s1u_socket_handler(int fd) { s1u_fd = fd; }
  void add_gtpu_m1u_socket_handler(int fd) {}
};

class pdcp_tester : public pdcp_dummy
{
public:
  void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu, int pdcp_sn) override
  {
    last_sdu     = std::move(sdu);
    last_pdcp_sn = pdcp_sn;
    last_rnti    = rnti;
    last_lcid    = lcid;
  }

  srslte::unique_byte_buffer_t last_sdu;
  int                          last_pdcp_sn = -1;
  uint16_t                     last_rnti    = SRSLTE_INVALID_RNTI;
  uint32_t                     last_lcid    = 0;
};

int GTPU_PORT = 2152;

srslte::unique_byte_buffer_t encode_gtpu_packet(srslte::span<uint8_t>     data,
                                                uint32_t                  teid,
                                                const struct sockaddr_in& src_sockaddr_in,
                                                const struct sockaddr_in& dest_sockaddr_in)
{
  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*srslte::byte_buffer_pool::get_instance());

  struct iphdr ip_pkt;
  ip_pkt.version = 4;
  ip_pkt.tot_len = htons(data.size() + sizeof(struct iphdr));
  ip_pkt.saddr   = src_sockaddr_in.sin_addr.s_addr;
  ip_pkt.daddr   = dest_sockaddr_in.sin_addr.s_addr;
  memcpy(pdu->msg, &ip_pkt, sizeof(struct iphdr));
  pdu->N_bytes = sizeof(struct iphdr);
  memcpy(pdu->msg + pdu->N_bytes, data.data(), data.size());
  pdu->N_bytes += data.size();

  // header
  srslte::gtpu_header_t header;
  header.flags        = GTPU_FLAGS_VERSION_V1 | GTPU_FLAGS_GTP_PROTOCOL;
  header.message_type = GTPU_MSG_DATA_PDU;
  header.length       = pdu->N_bytes;
  header.teid         = teid;

  gtpu_write_header(&header, pdu.get(), srslte::log_ref("GTPU"));
  return pdu;
}

srslte::unique_byte_buffer_t read_socket(int fd)
{
  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*srslte::byte_buffer_pool::get_instance());
  pdu->N_bytes                     = read(fd, pdu->msg, pdu->get_tailroom());
  return pdu;
}

int test_gtpu_direct_tunneling()
{
  uint16_t           rnti = 0x46, rnti2 = 0x50;
  uint32_t           drb1         = 3;
  uint32_t           mme_teidout1 = 1, mme_teidout2 = 2;
  const char *       mme_addr_str = "127.0.0.1", *senb_addr_str = "127.0.1.1", *tenb_addr_str = "127.0.1.2";
  struct sockaddr_in senb_sockaddr, mme_sockaddr, tenb_sockaddr;
  srslte::net_utils::set_sockaddr(&senb_sockaddr, senb_addr_str, GTPU_PORT);
  srslte::net_utils::set_sockaddr(&mme_sockaddr, mme_addr_str, GTPU_PORT);
  srslte::net_utils::set_sockaddr(&tenb_sockaddr, tenb_addr_str, GTPU_PORT);
  uint32_t tenb_addr = ntohl(tenb_sockaddr.sin_addr.s_addr), mme_addr = ntohl(mme_sockaddr.sin_addr.s_addr);

  // Initiate layers
  srsenb::gtpu senb_gtpu(srslog::fetch_basic_logger("GTPU1")), tenb_gtpu(srslog::fetch_basic_logger("GTPU2"));
  stack_tester senb_stack, tenb_stack;
  pdcp_tester  senb_pdcp, tenb_pdcp;
  senb_gtpu.init(senb_addr_str, mme_addr_str, "", "", &senb_pdcp, &senb_stack, false);
  tenb_gtpu.init(tenb_addr_str, mme_addr_str, "", "", &tenb_pdcp, &tenb_stack, false);

  // create tunnels MME-SeNB and MME-TeNB
  uint32_t senb_teid_in = senb_gtpu.add_bearer(rnti, drb1, mme_addr, mme_teidout1);
  uint32_t tenb_teid_in = tenb_gtpu.add_bearer(rnti, drb1, mme_addr, mme_teidout2);

  // create direct tunnel SeNB-TeNB
  gtpu::bearer_props props;
  props.flush_before_teidin_present = true;
  props.flush_before_teidin         = tenb_teid_in;
  uint32_t dl_tenb_teid_in          = tenb_gtpu.add_bearer(rnti2, drb1, 0, 0, &props);
  props                             = {};
  props.forward_from_teidin_present = true;
  props.forward_from_teidin         = senb_teid_in;
  senb_gtpu.add_bearer(rnti, drb1, tenb_addr, dl_tenb_teid_in, &props);

  std::vector<uint8_t> data_vec(10);
  std::iota(data_vec.begin(), data_vec.end(), 0);

  // TEST: verify that incoming DL data is forwarded through SeNB-TeNB tunnel
  srslte::unique_byte_buffer_t pdu = encode_gtpu_packet(data_vec, senb_teid_in, mme_sockaddr, senb_sockaddr);
  std::vector<uint8_t>         encoded_data(pdu->msg + 8u, pdu->msg + pdu->N_bytes);
  senb_gtpu.handle_gtpu_s1u_rx_packet(std::move(pdu), mme_sockaddr);
  tenb_gtpu.handle_gtpu_s1u_rx_packet(read_socket(tenb_stack.s1u_fd), senb_sockaddr);
  TESTASSERT(tenb_pdcp.last_sdu != nullptr);
  TESTASSERT(tenb_pdcp.last_sdu->N_bytes == encoded_data.size() and
             memcmp(tenb_pdcp.last_sdu->msg, encoded_data.data(), encoded_data.size()) == 0);
  TESTASSERT(tenb_pdcp.last_rnti == rnti2);
  TESTASSERT(tenb_pdcp.last_lcid == drb1);

  return SRSLTE_SUCCESS;
}

} // namespace srsenb

int main()
{
  // Setup logging.
  srslog::sink&          log_sink = srslog::fetch_stdout_sink();
  srslog::log_channel*   chan     = srslog::create_log_channel("gtpu_test", log_sink);
  srslte::srslog_wrapper log_wrapper(*chan);

  // Start the log backend.
  srslog::init();

  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_DEBUG);
  srslte::logmap::set_default_hex_limit(100000);
  TESTASSERT(srsenb::test_gtpu_direct_tunneling() == SRSLTE_SUCCESS);
}
