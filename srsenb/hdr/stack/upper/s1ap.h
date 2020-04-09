/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSENB_S1AP_H
#define SRSENB_S1AP_H

#include <map>

#include "common_enb.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/logmap.h"
#include "srslte/common/s1ap_pcap.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/enb_interfaces.h"

#include "s1ap_metrics.h"
#include "srslte/common/network_utils.h"
#include "srslte/common/stack_procedure.h"
#include <unordered_map>

namespace srsenb {

struct ue_ctxt_t {
  static const uint32_t invalid_enb_id = std::numeric_limits<uint32_t>::max();

  bool           mme_ue_s1ap_id_present = false;
  uint16_t       rnti                   = SRSLTE_INVALID_RNTI;
  uint32_t       enb_ue_s1ap_id         = invalid_enb_id;
  uint32_t       mme_ue_s1ap_id         = 0;
  struct timeval init_timestamp         = {};
};

class s1ap : public s1ap_interface_rrc
{
public:
  static const uint32_t ts1_reloc_prep_timeout_ms    = 10000;
  static const uint32_t ts1_reloc_overall_timeout_ms = 10000;

  s1ap();
  int  init(s1ap_args_t                       args_,
            rrc_interface_s1ap*               rrc_,
            srslte::timer_handler*            timers_,
            srsenb::stack_interface_s1ap_lte* stack_);
  void stop();
  void get_metrics(s1ap_metrics_t& m);

  // RRC interface
  void
       initial_ue(uint16_t rnti, asn1::s1ap::rrc_establishment_cause_e cause, srslte::unique_byte_buffer_t pdu) override;
  void initial_ue(uint16_t                              rnti,
                  asn1::s1ap::rrc_establishment_cause_e cause,
                  srslte::unique_byte_buffer_t          pdu,
                  uint32_t                              m_tmsi,
                  uint8_t                               mmec) override;
  void write_pdu(uint16_t rnti, srslte::unique_byte_buffer_t pdu) override;
  bool user_exists(uint16_t rnti) override;
  bool user_release(uint16_t rnti, asn1::s1ap::cause_radio_network_e cause_radio) override;
  void ue_ctxt_setup_complete(uint16_t rnti, const asn1::s1ap::init_context_setup_resp_s& res) override;
  void ue_erab_setup_complete(uint16_t rnti, const asn1::s1ap::erab_setup_resp_s& res) override;
  bool is_mme_connected() override;
  bool send_ho_required(uint16_t                     rnti,
                        uint32_t                     target_eci,
                        srslte::plmn_id_t            target_plmn,
                        srslte::unique_byte_buffer_t rrc_container) override;
  bool send_enb_status_transfer_proc(uint16_t rnti, std::vector<bearer_status_info>& bearer_status_list) override;
  // void ue_capabilities(uint16_t rnti, LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *caps);

  // Stack interface
  bool
       handle_mme_rx_msg(srslte::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags);
  void start_pcap(srslte::s1ap_pcap* pcap_);

private:
  static const int MME_PORT        = 36412;
  static const int ADDR_FAMILY     = AF_INET;
  static const int SOCK_TYPE       = SOCK_STREAM;
  static const int PROTO           = IPPROTO_SCTP;
  static const int PPID            = 18;
  static const int NONUE_STREAM_ID = 0;

  // args
  rrc_interface_s1ap*               rrc = nullptr;
  s1ap_args_t                       args;
  srslte::log_ref                   s1ap_log;
  srslte::byte_buffer_pool*         pool  = nullptr;
  srsenb::stack_interface_s1ap_lte* stack = nullptr;

  srslte::socket_handler_t            s1ap_socket;
  struct sockaddr_in                  mme_addr            = {}; // MME address
  bool                                mme_connected       = false;
  bool                                running             = false;
  uint32_t                            next_enb_ue_s1ap_id = 1; // Next ENB-side UE identifier
  uint16_t                            next_ue_stream_id   = 1; // Next UE SCTP stream identifier
  srslte::timer_handler::unique_timer mme_connect_timer, s1setup_timeout;

  // Protocol IEs sent with every UL S1AP message
  asn1::s1ap::tai_s        tai;
  asn1::s1ap::eutran_cgi_s eutran_cgi;

  // PCAP
  srslte::s1ap_pcap* pcap = nullptr;

  asn1::s1ap::s1_setup_resp_s s1setupresponse;

  void build_tai_cgi();
  bool connect_mme();
  bool setup_s1();
  bool sctp_send_s1ap_pdu(const asn1::s1ap::s1ap_pdu_c& tx_pdu, uint32_t rnti, const char* procedure_name);

  bool handle_s1ap_rx_pdu(srslte::byte_buffer_t* pdu);
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
  bool handle_uecontextmodifyrequest(const asn1::s1ap::ue_context_mod_request_s& msg);

  // bool send_ue_capabilities(uint16_t rnti, LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *caps)
  // handover
  bool handle_hopreparationfailure(const asn1::s1ap::ho_prep_fail_s& msg);
  bool handle_s1hocommand(const asn1::s1ap::ho_cmd_s& msg);

  // UE-specific data and procedures
  struct ue {
    //! TS 36.413, Section 8.4.1 - Handover Preparation Procedure
    class ho_prep_proc_t
    {
    public:
      struct ts1_reloc_prep_expired {
      };
      ho_prep_proc_t(s1ap::ue* ue_);
      srslte::proc_outcome_t
                             init(uint32_t target_eci_, srslte::plmn_id_t target_plmn_, srslte::unique_byte_buffer_t rrc_container);
      srslte::proc_outcome_t step() { return srslte::proc_outcome_t::yield; }
      srslte::proc_outcome_t react(ts1_reloc_prep_expired e);
      srslte::proc_outcome_t react(const asn1::s1ap::ho_prep_fail_s& msg);
      srslte::proc_outcome_t react(const asn1::s1ap::ho_cmd_s& msg);
      void                   then(const srslte::proc_state_t& result);
      const char*            name() { return "HandoverPreparation"; }

    private:
      s1ap::ue* ue_ptr   = nullptr;
      s1ap*     s1ap_ptr = nullptr;

      uint32_t                     target_eci = 0;
      srslte::plmn_id_t            target_plmn;
      srslte::unique_byte_buffer_t rrc_container;
    };

    explicit ue(s1ap* s1ap_ptr_);

    bool send_enb_status_transfer_proc(std::vector<bearer_status_info>& bearer_status_list);
    bool send_ulnastransport(srslte::unique_byte_buffer_t pdu);
    bool send_uectxtreleaserequest(const asn1::s1ap::cause_c& cause);
    bool send_uectxtmodifyfailure(const asn1::s1ap::cause_c& cause);
    bool send_uectxtmodifyresp();
    bool send_uectxtreleasecomplete();
    bool send_initialuemessage(asn1::s1ap::rrc_establishment_cause_e cause,
                               srslte::unique_byte_buffer_t          pdu,
                               bool                                  has_tmsi,
                               uint32_t                              m_tmsi = 0,
                               uint8_t                               mmec   = 0);
    bool send_initial_ctxt_setup_response(const asn1::s1ap::init_context_setup_resp_s& res_);
    bool send_initial_ctxt_setup_failure();
    bool send_erab_setup_response(const asn1::s1ap::erab_setup_resp_s& res_);
    bool was_uectxtrelease_requested() const { return release_requested; }

    ue_ctxt_t ctxt      = {};
    uint16_t  stream_id = 1;

    // user procedures
    srslte::proc_t<ho_prep_proc_t> ho_prep_proc;

  private:
    bool
    send_ho_required(uint32_t target_eci_, srslte::plmn_id_t target_plmn_, srslte::unique_byte_buffer_t rrc_container);
    //! TS 36.413, Section 8.4.6 - eNB Status Transfer procedure

    // args
    s1ap*           s1ap_ptr;
    srslte::log_ref s1ap_log;

    // state
    bool                                release_requested = false;
    srslte::timer_handler::unique_timer ts1_reloc_prep;    ///< TS1_{RELOCprep} - max time for HO preparation
    srslte::timer_handler::unique_timer ts1_reloc_overall; ///< TS1_{RELOCOverall}
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

  // timers
  srslte::timer_handler* timers = nullptr;

  // procedures
  class s1_setup_proc_t
  {
  public:
    struct s1setupresult {
      bool success = false;
      enum class cause_t { timeout, failure } cause;
    };

    explicit s1_setup_proc_t(s1ap* s1ap_) : s1ap_ptr(s1ap_) {}
    srslte::proc_outcome_t init();
    srslte::proc_outcome_t step() { return srslte::proc_outcome_t::yield; }
    srslte::proc_outcome_t react(const s1setupresult& event);
    void                   then(const srslte::proc_state_t& result) const;
    const char*            name() const { return "MME Connection"; }

  private:
    srslte::proc_outcome_t start_mme_connection();

    s1ap* s1ap_ptr = nullptr;
  };

  ue*         find_s1apmsg_user(uint32_t enb_id, uint32_t mme_id);
  std::string get_cause(const asn1::s1ap::cause_c& c);

  srslte::proc_t<s1_setup_proc_t> s1setup_proc;
};

} // namespace srsenb

#endif // SRSENB_S1AP_H
