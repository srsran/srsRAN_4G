/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include <map>
#include <string.h>

#include "common_enb.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/logmap.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/enb_gtpu_interfaces.h"
#include "srslte/srslog/srslog.h"
#include "srslte/srslte.h"

#ifndef SRSENB_GTPU_H
#define SRSENB_GTPU_H

namespace srsenb {

class pdcp_interface_gtpu;
class stack_interface_gtpu_lte;

class gtpu final : public gtpu_interface_rrc, public gtpu_interface_pdcp
{
public:
  explicit gtpu(srslog::basic_logger& logger);

  int  init(std::string               gtp_bind_addr_,
            std::string               mme_addr_,
            std::string               m1u_multiaddr_,
            std::string               m1u_if_addr_,
            pdcp_interface_gtpu*      pdcp_,
            stack_interface_gtpu_lte* stack_,
            bool                      enable_mbsfn = false);
  void stop();

  // gtpu_interface_rrc
  uint32_t add_bearer(uint16_t            rnti,
                      uint32_t            lcid,
                      uint32_t            addr,
                      uint32_t            teid_out,
                      const bearer_props* props = nullptr) override;
  void     set_tunnel_status(uint32_t teidin, bool dl_active) override;
  void     rem_bearer(uint16_t rnti, uint32_t lcid) override;
  void     mod_bearer_rnti(uint16_t old_rnti, uint16_t new_rnti) override;
  void     rem_user(uint16_t rnti) override;

  // gtpu_interface_pdcp
  void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu) override;

  // stack interface
  void handle_gtpu_s1u_rx_packet(srslte::unique_byte_buffer_t pdu, const sockaddr_in& addr);
  void handle_gtpu_m1u_rx_packet(srslte::unique_byte_buffer_t pdu, const sockaddr_in& addr);

private:
  static const int GTPU_PORT = 2152;

  void rem_tunnel(uint32_t teidin);

  stack_interface_gtpu_lte* stack = nullptr;

  bool                         enable_mbsfn = false;
  std::string                  gtp_bind_addr;
  std::string                  mme_addr;
  srsenb::pdcp_interface_gtpu* pdcp = nullptr;
  srslog::basic_logger&        logger;

  // Class to create
  class m1u_handler
  {
  public:
    explicit m1u_handler(gtpu* gtpu_) : parent(gtpu_), logger(parent->logger) {}
    ~m1u_handler();
    m1u_handler(const m1u_handler&) = delete;
    m1u_handler(m1u_handler&&)      = delete;
    m1u_handler& operator=(const m1u_handler&) = delete;
    m1u_handler& operator=(m1u_handler&&) = delete;
    bool         init(std::string m1u_multiaddr_, std::string m1u_if_addr_);
    void         handle_rx_packet(srslte::unique_byte_buffer_t pdu, const sockaddr_in& addr);

  private:
    gtpu*                 parent = nullptr;
    pdcp_interface_gtpu*  pdcp   = nullptr;
    srslog::basic_logger& logger;
    std::string           m1u_multiaddr;
    std::string           m1u_if_addr;

    bool initiated    = false;
    int  m1u_sd       = -1;
    int  lcid_counter = 0;
  };
  m1u_handler m1u;

  const uint32_t undefined_pdcp_sn = std::numeric_limits<uint32_t>::max();
  struct tunnel {
    bool     dl_enabled            = true;
    bool     fwd_teid_in_present   = false;
    bool     prior_teid_in_present = false;
    uint16_t rnti                  = SRSLTE_INVALID_RNTI;
    uint32_t lcid                  = SRSENB_N_RADIO_BEARERS;
    uint32_t teid_in               = 0;
    uint32_t teid_out              = 0;
    uint32_t spgw_addr             = 0;
    uint32_t fwd_teid_in           = 0; ///< forward Rx SDUs to this TEID
    uint32_t prior_teid_in         = 0; ///< buffer bearer SDUs until this TEID receives an End Marker
    std::multimap<uint32_t, srslte::unique_byte_buffer_t> buffer;
  };
  std::unordered_map<uint32_t, tunnel>                                           tunnels;
  std::map<uint16_t, std::array<std::vector<uint32_t>, SRSENB_N_RADIO_BEARERS> > ue_teidin_db;

  // Tx sequence number for signaling messages
  uint32_t tx_seq = 0;

  // Socket file descriptor
  int fd = -1;

  void send_pdu_to_tunnel(tunnel& tx_tun, srslte::unique_byte_buffer_t pdu, int pdcp_sn = -1);

  void echo_response(in_addr_t addr, in_port_t port, uint16_t seq);
  void error_indication(in_addr_t addr, in_port_t port, uint32_t err_teid);
  void end_marker(uint32_t teidin);

  int create_dl_fwd_tunnel(uint32_t rx_teid_in, uint32_t tx_teid_in);

  /****************************************************************************
   * TEID to RNIT/LCID helper functions
   ***************************************************************************/
  uint32_t next_teid_in = 0;

  tunnel*                get_tunnel(uint32_t teidin);
  srslte::span<uint32_t> get_lcid_teids(uint16_t rnti, uint32_t lcid);

  void log_message(tunnel& tun, bool is_rx, srslte::span<uint8_t> pdu, int pdcp_sn = -1);
};

} // namespace srsenb

#endif // SRSENB_GTPU_H
