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

#ifndef SRSENB_S1AP_H
#define SRSENB_S1AP_H

#include <map>

#include "srsenb/hdr/common/common_enb.h"
#include "srsran/adt/circular_map.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/s1ap_pcap.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/enb_interfaces.h"
#include "srsran/interfaces/enb_s1ap_interfaces.h"

#include "s1ap_metrics.h"
#include "srsran/adt/optional.h"
#include "srsran/asn1/s1ap.h"
#include "srsran/common/network_utils.h"
#include "srsran/common/stack_procedure.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/srslog/srslog.h"
#include <unordered_map>

namespace srsenb {

class rrc_interface_s1ap;

struct ue_ctxt_t {
  static const uint32_t invalid_enb_id = std::numeric_limits<uint32_t>::max();

  uint16_t                   rnti           = SRSRAN_INVALID_RNTI;
  uint32_t                   enb_ue_s1ap_id = invalid_enb_id;
  srsran::optional<uint32_t> mme_ue_s1ap_id;
  uint32_t                   enb_cc_idx     = 0;
  struct timeval             init_timestamp = {};
};

class s1ap : public s1ap_interface_rrc
{
  using s1ap_proc_id_t = asn1::s1ap::s1ap_elem_procs_o::init_msg_c::types_opts::options;

public:
  using erab_id_list   = srsran::bounded_vector<uint16_t, ASN1_S1AP_MAXNOOF_ERABS>;
  using erab_item_list = srsran::bounded_vector<asn1::s1ap::erab_item_s, ASN1_S1AP_MAXNOOF_ERABS>;

  static const uint32_t ts1_reloc_prep_timeout_ms    = 10000;
  static const uint32_t ts1_reloc_overall_timeout_ms = 10000;

  s1ap(srsran::task_sched_handle   task_sched_,
       srslog::basic_logger&       logger,
       srsran::socket_manager_itf* rx_socket_handler);
  int  init(const s1ap_args_t& args_, rrc_interface_s1ap* rrc_);
  void stop();
  void get_metrics(s1ap_metrics_t& m);

  // RRC interface
  void initial_ue(uint16_t                              rnti,
                  uint32_t                              enb_cc_idx,
                  asn1::s1ap::rrc_establishment_cause_e cause,
                  srsran::unique_byte_buffer_t          pdu) override;
  void initial_ue(uint16_t                              rnti,
                  uint32_t                              enb_cc_idx,
                  asn1::s1ap::rrc_establishment_cause_e cause,
                  srsran::unique_byte_buffer_t          pdu,
                  uint32_t                              m_tmsi,
                  uint8_t                               mmec) override;
  void write_pdu(uint16_t rnti, srsran::unique_byte_buffer_t pdu) override;
  bool user_exists(uint16_t rnti) override;
  void user_mod(uint16_t old_rnti, uint16_t new_rnti) override;
  bool user_release(uint16_t rnti, asn1::s1ap::cause_radio_network_e cause_radio) override;
  void ue_ctxt_setup_complete(uint16_t rnti) override;
  bool is_mme_connected() override;
  bool send_ho_required(uint16_t                     rnti,
                        uint32_t                     target_eci,
                        srsran::plmn_id_t            target_plmn,
                        srsran::span<uint32_t>       fwd_erabs,
                        srsran::unique_byte_buffer_t rrc_container,
                        bool                         has_direct_fwd_path) override;
  bool send_enb_status_transfer_proc(uint16_t rnti, std::vector<bearer_status_info>& bearer_status_list) override;
  bool send_ho_req_ack(const asn1::s1ap::ho_request_s&                msg,
                       uint16_t                                       rnti,
                       uint32_t                                       enb_cc_idx,
                       srsran::unique_byte_buffer_t                   ho_cmd,
                       srsran::span<asn1::s1ap::erab_admitted_item_s> admitted_bearers,
                       srsran::const_span<asn1::s1ap::erab_item_s>    not_admitted_bearers) override;
  void send_ho_cancel(uint16_t rnti, const asn1::s1ap::cause_c& cause) override;
  bool release_erabs(uint16_t rnti, const std::vector<uint16_t>& erabs_successfully_released) override;
  bool send_error_indication(const asn1::s1ap::cause_c& cause,
                             srsran::optional<uint32_t> enb_ue_s1ap_id = {},
                             srsran::optional<uint32_t> mme_ue_s1ap_id = {});
  bool send_ue_cap_info_indication(uint16_t rnti, srsran::unique_byte_buffer_t ue_radio_cap) override;

  /// Target eNB Handover
  /// Section 8.4.2 - Handover Resource Allocation
  void send_ho_failure(uint32_t mme_ue_s1ap_id, const asn1::s1ap::cause_c& cause);
  /// Section 8.4.3 - Handover Notification
  void send_ho_notify(uint16_t rnti, uint64_t target_eci) override;

  // Stack interface
  bool
       handle_mme_rx_msg(srsran::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags);
  void start_pcap(srsran::s1ap_pcap* pcap_);

private:
  static const int MME_PORT        = 36412;
  static const int ADDR_FAMILY     = AF_INET;
  static const int SOCK_TYPE       = SOCK_STREAM;
  static const int PROTO           = IPPROTO_SCTP;
  static const int PPID            = 18;
  static const int NONUE_STREAM_ID = 0;

  // args
  rrc_interface_s1ap*         rrc = nullptr;
  s1ap_args_t                 args;
  srslog::basic_logger&       logger;
  srsran::task_sched_handle   task_sched;
  srsran::task_queue_handle   mme_task_queue;
  srsran::socket_manager_itf* rx_socket_handler;

  srsran::unique_socket mme_socket;
  struct sockaddr_in    mme_addr            = {}; // MME address
  bool                  mme_connected       = false;
  bool                  running             = false;
  uint32_t              next_enb_ue_s1ap_id = 1; // Next ENB-side UE identifier
  uint16_t              next_ue_stream_id   = 1; // Next UE SCTP stream identifier
  srsran::unique_timer  mme_connect_timer, s1setup_timeout;

  // Protocol IEs sent with every UL S1AP message
  asn1::s1ap::tai_s        tai;
  asn1::s1ap::eutran_cgi_s eutran_cgi;

  // PCAP
  srsran::s1ap_pcap* pcap = nullptr;

  asn1::s1ap::s1_setup_resp_s s1setupresponse;

  void build_tai_cgi();
  bool connect_mme();
  bool setup_s1();
  bool sctp_send_s1ap_pdu(const asn1::s1ap::s1ap_pdu_c& tx_pdu, uint32_t rnti, const char* procedure_name);

  bool handle_s1ap_rx_pdu(srsran::byte_buffer_t* pdu);
  bool handle_initiatingmessage(const asn1::s1ap::init_msg_s& msg);
  bool handle_successfuloutcome(const asn1::s1ap::successful_outcome_s& msg);
  bool handle_unsuccessfuloutcome(const asn1::s1ap::unsuccessful_outcome_s& msg);
  bool handle_paging(const asn1::s1ap::paging_s& msg);

  bool handle_s1setupresponse(const asn1::s1ap::s1_setup_resp_s& msg);

  bool handle_dlnastransport(const asn1::s1ap::dl_nas_transport_s& msg);
  bool handle_initialctxtsetuprequest(const asn1::s1ap::init_context_setup_request_s& msg);
  bool handle_uectxtreleasecommand(const asn1::s1ap::ue_context_release_cmd_s& msg);
  bool handle_s1setupfailure(const asn1::s1ap::s1_setup_fail_s& msg);
  bool handle_erabsetuprequest(const asn1::s1ap::erab_setup_request_s& msg);
  bool handle_erabmodifyrequest(const asn1::s1ap::erab_modify_request_s& msg);
  bool handle_erabreleasecommand(const asn1::s1ap::erab_release_cmd_s& msg);
  bool handle_uecontextmodifyrequest(const asn1::s1ap::ue_context_mod_request_s& msg);

  // handover
  /**
   * Source eNB Handler for S1AP "HANDOVER PREPARATION FAILURE" Message
   * MME ---> Source eNB
   * @remark TS 36.413, 8.4.1.3 - S1AP Procedures | Handover Signalling | Handover Preparation | Unsuccessful Operation
   * @param msg HANDOVER COMMAND S1AP PDU
   * @return true if the HANDOVER COMMAND content is valid. False otherwise
   */
  bool handle_handover_preparation_failure(const asn1::s1ap::ho_prep_fail_s& msg);
  /**
   * Source eNB Handler for S1AP "HANDOVER COMMAND" Message
   * MME ---> Source eNB
   * @remark TS 36.413, 8.4.1.2 - S1AP Procedures | Handover Signalling | Handover Preparation | Successful Operation
   * @param msg HANDOVER COMMAND S1AP PDU
   * @return true if the HANDOVER COMMAND content is valid. False otherwise
   */
  bool handle_handover_command(const asn1::s1ap::ho_cmd_s& msg);

  /**
   * Target eNB Handler for S1AP "HANDOVER REQUEST" Message
   * MME ---> Target eNB
   * @remark TS 36.413, 8.4.7.2 - S1AP Procedures | Handover Signalling | Handover Resource Allocation
   * @param msg HANDOVER REQUEST S1AP PDU
   * @return true if the new user resources were successfully allocated
   */
  bool handle_handover_request(const asn1::s1ap::ho_request_s& msg);
  /**
   * Target eNB Handler for S1AP "MME STATUS TRANSFER" Message
   * MME ---> Target eNB
   * @remark TS 36.413, 8.4.7.2 - S1AP Procedures | Handover Signalling | MME Status Transfer | Successful Operation
   * @param msg MME STATUS TRANSFER S1AP PDU
   * @return true if the msg content is valid. False otherwise
   */
  bool handle_mme_status_transfer(const asn1::s1ap::mme_status_transfer_s& msg);

  // UE-specific data and procedures
  struct ue {
    //! TS 36.413, Section 8.4.1 - Handover Preparation Procedure
    class ho_prep_proc_t
    {
    public:
      struct ts1_reloc_prep_expired {};
      ho_prep_proc_t(s1ap::ue* ue_);
      srsran::proc_outcome_t init(uint32_t                     target_eci_,
                                  srsran::plmn_id_t            target_plmn_,
                                  srsran::span<uint32_t>       fwd_erabs,
                                  srsran::unique_byte_buffer_t rrc_container,
                                  bool                         has_direct_fwd_path);
      srsran::proc_outcome_t step() { return srsran::proc_outcome_t::yield; }
      srsran::proc_outcome_t react(ts1_reloc_prep_expired e);
      srsran::proc_outcome_t react(const asn1::s1ap::ho_prep_fail_s& msg);
      srsran::proc_outcome_t react(const asn1::s1ap::ho_cmd_s& msg);
      void                   then(const srsran::proc_state_t& result);
      const char*            name() { return "HandoverPreparation"; }

    private:
      s1ap::ue* ue_ptr   = nullptr;
      s1ap*     s1ap_ptr = nullptr;

      uint32_t                     target_eci = 0;
      srsran::plmn_id_t            target_plmn;
      srsran::unique_byte_buffer_t rrc_container;
      const asn1::s1ap::ho_cmd_s*  ho_cmd_msg = nullptr;
    };

    explicit ue(s1ap* s1ap_ptr_);

    bool send_enb_status_transfer_proc(std::vector<bearer_status_info>& bearer_status_list);
    bool send_ulnastransport(srsran::unique_byte_buffer_t pdu);
    bool send_uectxtreleaserequest(const asn1::s1ap::cause_c& cause);
    bool send_uectxtmodifyfailure(const asn1::s1ap::cause_c& cause);
    bool send_uectxtmodifyresp();
    bool send_uectxtreleasecomplete();
    bool send_initialuemessage(asn1::s1ap::rrc_establishment_cause_e cause,
                               srsran::unique_byte_buffer_t          pdu,
                               bool                                  has_tmsi,
                               uint32_t                              m_tmsi = 0,
                               uint8_t                               mmec   = 0);
    void ue_ctxt_setup_complete();
    bool send_erab_setup_response(const erab_id_list& erabs_setup, const erab_item_list& erabs_failed);
    bool send_erab_release_response(const erab_id_list& erabs_released, const erab_item_list& erabs_failed);
    bool send_erab_modify_response(const erab_id_list& erabs_modified, const erab_item_list& erabs_failed);
    bool send_erab_release_indication(const std::vector<uint16_t>& erabs_successfully_released);
    bool send_ue_cap_info_indication(srsran::unique_byte_buffer_t ue_radio_cap);

    bool was_uectxtrelease_requested() const { return release_requested; }

    void
    set_state(s1ap_proc_id_t state, const erab_id_list& erabs_updated, const erab_item_list& erabs_failed_to_update);

    ue_ctxt_t ctxt      = {};
    uint16_t  stream_id = 1;

  private:
    bool send_ho_required(uint32_t                     target_eci_,
                          srsran::plmn_id_t            target_plmn_,
                          srsran::span<uint32_t>       fwd_erabs,
                          srsran::unique_byte_buffer_t rrc_container,
                          bool                         has_direct_fwd_path);
    void get_erab_addr(uint16_t erab_id, transp_addr_t& transp_addr, asn1::fixed_octstring<4, true>& gtpu_teid_id);

    // args
    s1ap*                 s1ap_ptr;
    srslog::basic_logger& logger;

    // state
    bool                 release_requested = false;
    srsran::unique_timer ts1_reloc_prep;    ///< TS1_{RELOCprep} - max time for HO preparation
    srsran::unique_timer ts1_reloc_overall; ///< TS1_{RELOCOverall}

    // Procedure state
    s1ap_proc_id_t                                                           current_state = s1ap_proc_id_t::nulltype;
    erab_id_list                                                             updated_erabs;
    srsran::bounded_vector<asn1::s1ap::erab_item_s, ASN1_S1AP_MAXNOOF_ERABS> failed_cfg_erabs;

  public:
    // user procedures
    srsran::proc_t<ho_prep_proc_t> ho_prep_proc;
  };

  class user_list
  {
  public:
    using value_type     = std::unique_ptr<ue>;
    using iterator       = std::unordered_map<uint32_t, value_type>::iterator;
    using const_iterator = std::unordered_map<uint32_t, value_type>::const_iterator;
    using pair_type      = std::unordered_map<uint32_t, value_type>::value_type;

    ue*            find_ue_rnti(uint16_t rnti);
    ue*            find_ue_enbid(uint32_t enbid);
    ue*            find_ue_mmeid(uint32_t mmeid);
    ue*            add_user(value_type user);
    void           erase(ue* ue_ptr);
    iterator       begin() { return users.begin(); }
    iterator       end() { return users.end(); }
    const_iterator cbegin() const { return users.begin(); }
    const_iterator cend() const { return users.end(); }
    size_t         size() const { return users.size(); }

  private:
    std::unordered_map<uint32_t, std::unique_ptr<ue> > users; // maps ENB_S1AP_ID to user
  };
  user_list users;

  // procedures
  class s1_setup_proc_t
  {
  public:
    struct s1setupresult {
      bool success = false;
      enum class cause_t { timeout, failure } cause;
    };

    explicit s1_setup_proc_t(s1ap* s1ap_) : s1ap_ptr(s1ap_) {}
    srsran::proc_outcome_t init();
    srsran::proc_outcome_t step() { return srsran::proc_outcome_t::yield; }
    srsran::proc_outcome_t react(const s1setupresult& event);
    void                   then(const srsran::proc_state_t& result) const;
    const char*            name() const { return "MME Connection"; }

  private:
    srsran::proc_outcome_t start_mme_connection();

    s1ap* s1ap_ptr = nullptr;
  };

  ue*         handle_s1apmsg_ue_id(uint32_t enb_id, uint32_t mme_id);
  std::string get_cause(const asn1::s1ap::cause_c& c);
  void        log_s1ap_msg(const asn1::s1ap::s1ap_pdu_c& msg, srsran::const_span<uint8_t> sdu, bool is_rx);

  srsran::proc_t<s1_setup_proc_t> s1setup_proc;
};

} // namespace srsenb

#endif // SRSENB_S1AP_H
