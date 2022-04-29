/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include <map>
#include <unordered_map>
#include <string.h>

#include "srsenb/hdr/common/common_enb.h"
#include "srsran/adt/bounded_vector.h"
#include "srsran/adt/circular_map.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/network_utils.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/enb_gtpu_interfaces.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/srslog/srslog.h"

#include <netinet/in.h>

#ifndef SRSENB_GTPU_H
#define SRSENB_GTPU_H

namespace srsran {
struct gtpu_header_t;
}

namespace srsenb {

class pdcp_interface_gtpu;

class gtpu_tunnel_manager
{
  // Buffer used to store SDUs while PDCP is still getting configured during handover.
  // Note: The buffer cannot be too large, otherwise it risks depleting the byte buffer pool.
  const static size_t BUFFER_SIZE = 512;
  using buffered_sdu_list = srsran::bounded_vector<std::pair<uint32_t, srsran::unique_byte_buffer_t>, BUFFER_SIZE>;

  static const uint32_t undefined_pdcp_sn = std::numeric_limits<uint32_t>::max();

public:
  // A UE should have <= 3 DRBs active, and each DRB should have two tunnels active at the same time at most
  const static size_t MAX_TUNNELS_PER_UE = 10;

  enum class tunnel_state { pdcp_active, buffering, forward_to, forwarded_from, inactive };

  struct tunnel {
    uint16_t rnti          = SRSRAN_INVALID_RNTI;
    uint32_t eps_bearer_id = srsran::INVALID_EPS_BEARER_ID;
    uint32_t teid_in       = 0;
    uint32_t teid_out      = 0;
    uint32_t spgw_addr     = 0;

    tunnel_state                                    state = tunnel_state::pdcp_active;
    srsran::unique_timer                            rx_timer;
    srsran::byte_buffer_pool_ptr<buffered_sdu_list> buffer;
    tunnel*                                         fwd_tunnel = nullptr; ///< forward Rx SDUs to this TEID
    srsran::move_callback<void()>                   on_removal;

    tunnel()                  = default;
    tunnel(tunnel&&) noexcept = default;
    tunnel& operator=(tunnel&&) noexcept = default;
    ~tunnel()
    {
      if (not on_removal.is_empty()) {
        on_removal();
      }
    }
  };

  struct bearer_teid_pair {
    uint32_t eps_bearer_id;
    uint32_t teid;

    bool operator<(const bearer_teid_pair& other) const
    {
      return eps_bearer_id < other.eps_bearer_id or (eps_bearer_id == other.eps_bearer_id and teid < other.teid);
    }
    bool operator==(const bearer_teid_pair& other) const
    {
      return eps_bearer_id == other.eps_bearer_id and teid == other.teid;
    }
  };
  using ue_bearer_tunnel_list = srsran::bounded_vector<bearer_teid_pair, MAX_TUNNELS_PER_UE>;

  explicit gtpu_tunnel_manager(srsran::task_sched_handle task_sched_, srslog::basic_logger& logger);
  void init(const gtpu_args_t& gtpu_args, pdcp_interface_gtpu* pdcp_);

  bool                           has_teid(uint32_t teid) const { return tunnels.contains(teid); }
  const tunnel*                  find_tunnel(uint32_t teid);
  ue_bearer_tunnel_list*         find_rnti_tunnels(uint16_t rnti);
  srsran::span<bearer_teid_pair> find_rnti_bearer_tunnels(uint16_t rnti, uint32_t eps_bearer_id);

  const tunnel* add_tunnel(uint16_t rnti, uint32_t eps_bearer_id, uint32_t teidout, uint32_t spgw_addr);
  bool          update_rnti(uint16_t old_rnti, uint16_t new_rnti);

  void activate_tunnel(uint32_t teid);
  void suspend_tunnel(uint32_t teid);
  void deactivate_tunnel(uint32_t teid);
  void set_tunnel_priority(uint32_t first_teid, uint32_t second_teid);
  void handle_rx_pdcp_sdu(uint32_t teid);
  void buffer_pdcp_sdu(uint32_t teid, uint32_t pdcp_sn, srsran::unique_byte_buffer_t sdu);
  void setup_forwarding(uint32_t rx_teid, uint32_t tx_teid);

  bool remove_tunnel(uint32_t teid);
  bool remove_rnti(uint16_t rnti);

private:
  using tunnel_list_t  = srsran::static_id_obj_pool<uint32_t, tunnel, SRSENB_MAX_UES * MAX_TUNNELS_PER_UE>;
  using tunnel_ctxt_it = typename tunnel_list_t::iterator;

  srsran::task_sched_handle task_sched;
  const gtpu_args_t*        gtpu_args = nullptr;
  pdcp_interface_gtpu*      pdcp      = nullptr;
  srslog::basic_logger&     logger;

  std::unordered_map<uint16_t, ue_bearer_tunnel_list> ue_teidin_db;
  tunnel_list_t                     tunnels;
};

using gtpu_tunnel_state = gtpu_tunnel_manager::tunnel_state;
using gtpu_tunnel       = gtpu_tunnel_manager::tunnel;

class gtpu final : public gtpu_interface_rrc, public gtpu_interface_pdcp
{
public:
  explicit gtpu(srsran::task_sched_handle   task_sched_,
                srslog::basic_logger&       logger,
                srsran::socket_manager_itf* rx_socket_handler_);
  ~gtpu();

  int  init(const gtpu_args_t& gtpu_args, pdcp_interface_gtpu* pdcp_);
  void stop();

  // gtpu_interface_rrc
  srsran::expected<uint32_t> add_bearer(uint16_t            rnti,
                                        uint32_t            eps_bearer_id,
                                        uint32_t            addr,
                                        uint32_t            teid_out,
                                        uint32_t&           addr_in,
                                        const bearer_props* props = nullptr) override;
  void                       set_tunnel_status(uint32_t teidin, bool dl_active) override;
  void                       rem_bearer(uint16_t rnti, uint32_t eps_bearer_id) override;
  void                       mod_bearer_rnti(uint16_t old_rnti, uint16_t new_rnti) override;
  void                       rem_user(uint16_t rnti) override;

  // gtpu_interface_pdcp
  void write_pdu(uint16_t rnti, uint32_t eps_bearer_id, srsran::unique_byte_buffer_t pdu) override;

  // stack interface
  void handle_gtpu_s1u_rx_packet(srsran::unique_byte_buffer_t pdu, const sockaddr_in& addr);
  void handle_gtpu_m1u_rx_packet(srsran::unique_byte_buffer_t pdu, const sockaddr_in& addr);

private:
  static const int GTPU_PORT = 2152;

  void rem_tunnel(uint32_t teidin);

  srsran::socket_manager_itf* rx_socket_handler = nullptr;
  srsran::task_queue_handle   gtpu_queue;

  gtpu_args_t                  args;
  std::string                  gtp_bind_addr;
  std::string                  mme_addr;
  srsenb::pdcp_interface_gtpu* pdcp = nullptr;
  srslog::basic_logger&        logger;
  srsran::task_sched_handle    task_sched;

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
    void         handle_rx_packet(srsran::unique_byte_buffer_t pdu, const sockaddr_in& addr);

  private:
    gtpu*                 parent = nullptr;
    pdcp_interface_gtpu*  pdcp   = nullptr;
    srslog::basic_logger& logger;
    std::string           m1u_multiaddr;
    std::string           m1u_if_addr;

    bool initiated      = false;
    int  m1u_sd         = -1;
    int  bearer_counter = 0;
  };
  m1u_handler m1u;

  static const uint32_t undefined_pdcp_sn = std::numeric_limits<uint32_t>::max();
  gtpu_tunnel_manager   tunnels;

  // Tx sequence number for signaling messages
  uint32_t tx_seq = 0;

  // Socket file descriptor
  int fd = -1;

  void send_pdu_to_tunnel(const gtpu_tunnel& tx_tun, srsran::unique_byte_buffer_t pdu, int pdcp_sn = -1);

  void echo_response(in_addr_t addr, in_port_t port, uint16_t seq);
  void error_indication(in_addr_t addr, in_port_t port, uint32_t err_teid);
  bool send_end_marker(uint32_t teidin);

  void handle_end_marker(const gtpu_tunnel& rx_tunnel);
  void handle_msg_data_pdu(const srsran::gtpu_header_t& header,
                           const gtpu_tunnel&           rx_tunnel,
                           srsran::unique_byte_buffer_t pdu);

  int create_dl_fwd_tunnel(uint32_t rx_teid_in, uint32_t tx_teid_in);

  /****************************************************************************
   * TEID to RNIT/LCID helper functions
   ***************************************************************************/
  void log_message(const gtpu_tunnel& tun, bool is_rx, srsran::span<uint8_t> pdu, int pdcp_sn = -1);
};

} // namespace srsenb

#endif // SRSENB_GTPU_H
