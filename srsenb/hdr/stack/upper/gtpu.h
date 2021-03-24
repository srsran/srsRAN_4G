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

#include <map>
#include <string.h>

#include "common_enb.h"
#include "srsran/adt/bounded_vector.h"
#include "srsran/adt/circular_map.h"
#include "srsran/common/buffer_pool.h"
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
class stack_interface_gtpu_lte;

struct gtpu_tunnel {
  bool                 dl_enabled            = true;
  bool                 fwd_teid_in_present   = false;
  bool                 prior_teid_in_present = false;
  uint16_t             rnti                  = SRSRAN_INVALID_RNTI;
  uint32_t             lcid                  = SRSENB_N_RADIO_BEARERS;
  uint32_t             teid_in               = 0;
  uint32_t             teid_out              = 0;
  uint32_t             spgw_addr             = 0;
  uint32_t             fwd_teid_in           = 0; ///< forward Rx SDUs to this TEID
  uint32_t             prior_teid_in         = 0; ///< buffer bearer SDUs until this TEID receives an End Marker
  srsran::unique_timer rx_timer;
  std::vector<std::pair<uint32_t, srsran::unique_byte_buffer_t> > buffer;
};

class gtpu_tunnel_manager
{
public:
  const static size_t MAX_TUNNELS_PER_UE = 4;
  struct lcid_tunnel {
    uint32_t lcid;
    uint32_t teid;

    bool operator<(const lcid_tunnel& other) const
    {
      return lcid < other.lcid or (lcid == other.lcid and teid < other.teid);
    }
    bool operator==(const lcid_tunnel& other) const { return lcid == other.lcid and teid == other.teid; }
  };
  using ue_lcid_tunnel_list = srsran::bounded_vector<lcid_tunnel, MAX_TUNNELS_PER_UE>;

  gtpu_tunnel_manager();

  gtpu_tunnel*              find_tunnel(uint32_t teid);
  ue_lcid_tunnel_list*      find_rnti_tunnels(uint16_t rnti);
  srsran::span<lcid_tunnel> find_rnti_lcid_tunnels(uint16_t rnti, uint32_t lcid);

  gtpu_tunnel* add_tunnel(uint16_t rnti, uint32_t lcid, uint32_t teidout, uint32_t spgw_addr);
  bool         update_rnti(uint16_t old_rnti, uint16_t new_rnti);

  bool remove_tunnel(uint32_t teid);
  bool remove_bearer(uint16_t rnti, uint32_t lcid);
  bool remove_rnti(uint16_t rnti);

private:
  srslog::basic_logger& logger;

  srsran::static_id_obj_pool<uint32_t, gtpu_tunnel, SRSENB_MAX_UES * MAX_TUNNELS_PER_UE> tunnels;
  srsran::static_circular_map<uint16_t, ue_lcid_tunnel_list, SRSENB_MAX_UES>             ue_teidin_db;
};

class gtpu final : public gtpu_interface_rrc, public gtpu_interface_pdcp
{
public:
  explicit gtpu(srsran::task_sched_handle task_sched_, srslog::basic_logger& logger);
  ~gtpu();

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
  void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) override;

  // stack interface
  void handle_gtpu_s1u_rx_packet(srsran::unique_byte_buffer_t pdu, const sockaddr_in& addr);
  void handle_gtpu_m1u_rx_packet(srsran::unique_byte_buffer_t pdu, const sockaddr_in& addr);

private:
  static const int GTPU_PORT = 2152;

  void rem_tunnel(uint32_t teidin);

  stack_interface_gtpu_lte* stack = nullptr;

  bool                         enable_mbsfn = false;
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

    bool initiated    = false;
    int  m1u_sd       = -1;
    int  lcid_counter = 0;
  };
  m1u_handler m1u;

  const uint32_t      undefined_pdcp_sn = std::numeric_limits<uint32_t>::max();
  gtpu_tunnel_manager tunnels;

  // Tx sequence number for signaling messages
  uint32_t tx_seq = 0;

  // Socket file descriptor
  int fd = -1;

  void send_pdu_to_tunnel(const gtpu_tunnel& tx_tun, srsran::unique_byte_buffer_t pdu, int pdcp_sn = -1);

  void echo_response(in_addr_t addr, in_port_t port, uint16_t seq);
  void error_indication(in_addr_t addr, in_port_t port, uint32_t err_teid);
  bool send_end_marker(uint32_t teidin);

  void handle_end_marker(const gtpu_tunnel& rx_tunnel);
  void
  handle_msg_data_pdu(const srsran::gtpu_header_t& header, gtpu_tunnel& rx_tunnel, srsran::unique_byte_buffer_t pdu);

  int create_dl_fwd_tunnel(uint32_t rx_teid_in, uint32_t tx_teid_in);

  /****************************************************************************
   * TEID to RNIT/LCID helper functions
   ***************************************************************************/
  void log_message(const gtpu_tunnel& tun, bool is_rx, srsran::span<uint8_t> pdu, int pdcp_sn = -1);
};

} // namespace srsenb

#endif // SRSENB_GTPU_H
