/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/enb_interfaces.h"

#include "s1ap_metrics.h"
#include "srslte/asn1/liblte_s1ap.h"
#include "srslte/common/network_utils.h"
#include "srslte/common/stack_procedure.h"

namespace srsenb {

typedef struct {
  uint32_t       rnti;
  uint32_t       eNB_UE_S1AP_ID;
  uint32_t       MME_UE_S1AP_ID;
  bool           release_requested;
  uint16_t       stream_id;
  struct timeval init_timestamp;
} ue_ctxt_t;

class s1ap : public s1ap_interface_rrc
{
public:
  static const uint32_t ts1_reloc_prep_timeout_ms    = 10000;
  static const uint32_t ts1_reloc_overall_timeout_ms = 10000;

  s1ap();
  bool init(s1ap_args_t                       args_,
            rrc_interface_s1ap*               rrc_,
            srslte::log*                      s1ap_log_,
            srslte::timer_handler*            timers_,
            srsenb::stack_interface_s1ap_lte* stack_);
  void stop();
  void get_metrics(s1ap_metrics_t& m);

  // RRC interface
  void initial_ue(uint16_t rnti, LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM cause, srslte::unique_byte_buffer_t pdu);
  void initial_ue(uint16_t                                 rnti,
                  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM cause,
                  srslte::unique_byte_buffer_t             pdu,
                  uint32_t                                 m_tmsi,
                  uint8_t                                  mmec);
  void write_pdu(uint16_t rnti, srslte::unique_byte_buffer_t pdu);
  bool user_exists(uint16_t rnti);
  bool user_release(uint16_t rnti, LIBLTE_S1AP_CAUSERADIONETWORK_ENUM cause_radio);
  void ue_ctxt_setup_complete(uint16_t rnti, LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT* res);
  void ue_erab_setup_complete(uint16_t rnti, LIBLTE_S1AP_MESSAGE_E_RABSETUPRESPONSE_STRUCT* res);
  bool is_mme_connected();
  // void ue_capabilities(uint16_t rnti, LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *caps);

  // Stack interface
  bool
  handle_mme_rx_msg(srslte::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags);

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
  srslte::log*                      s1ap_log = nullptr;
  srslte::byte_buffer_pool*         pool     = nullptr;
  srsenb::stack_interface_s1ap_lte* stack    = nullptr;

  srslte::socket_handler_t            s1ap_socket;
  struct sockaddr_in                  mme_addr            = {}; // MME address
  bool                                mme_connected       = false;
  bool                                running             = false;
  uint32_t                            next_eNB_UE_S1AP_ID = 1; // Next ENB-side UE identifier
  uint16_t                            next_ue_stream_id   = 1; // Next UE SCTP stream identifier
  srslte::timer_handler::unique_timer mme_connect_timer, s1setup_timeout;

  // Protocol IEs sent with every UL S1AP message
  LIBLTE_S1AP_TAI_STRUCT        tai;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT eutran_cgi;

  LIBLTE_S1AP_MESSAGE_S1SETUPRESPONSE_STRUCT s1setupresponse;

  void build_tai_cgi();
  bool connect_mme();
  bool setup_s1();
  bool sctp_send_s1ap_pdu(LIBLTE_S1AP_S1AP_PDU_STRUCT* tx_pdu, uint32_t rnti, const char* procedure_name);

  bool handle_s1ap_rx_pdu(srslte::byte_buffer_t* pdu);
  bool handle_initiatingmessage(LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT* msg);
  bool handle_successfuloutcome(LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT* msg);
  bool handle_unsuccessfuloutcome(LIBLTE_S1AP_UNSUCCESSFULOUTCOME_STRUCT* msg);
  bool handle_paging(LIBLTE_S1AP_MESSAGE_PAGING_STRUCT* msg);

  bool handle_s1setupresponse(LIBLTE_S1AP_MESSAGE_S1SETUPRESPONSE_STRUCT* msg);

  bool handle_dlnastransport(LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT* msg);
  bool handle_initialctxtsetuprequest(LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPREQUEST_STRUCT* msg);
  bool handle_uectxtreleasecommand(LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMMAND_STRUCT* msg);
  bool handle_s1setupfailure(LIBLTE_S1AP_MESSAGE_S1SETUPFAILURE_STRUCT* msg);
  bool handle_erabsetuprequest(LIBLTE_S1AP_MESSAGE_E_RABSETUPREQUEST_STRUCT* msg);
  bool handle_uecontextmodifyrequest(LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONREQUEST_STRUCT* msg);

  bool send_initialuemessage(uint16_t                                 rnti,
                             LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM cause,
                             srslte::unique_byte_buffer_t             pdu,
                             bool                                     has_tmsi,
                             uint32_t                                 m_tmsi = 0,
                             uint8_t                                  mmec   = 0);
  bool send_ulnastransport(uint16_t rnti, srslte::unique_byte_buffer_t pdu);
  bool send_uectxtreleaserequest(uint16_t rnti, LIBLTE_S1AP_CAUSE_STRUCT* cause);
  bool send_uectxtreleasecomplete(uint16_t rnti, uint32_t mme_ue_id, uint32_t enb_ue_id);
  bool send_initial_ctxt_setup_response(uint16_t rnti, LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT* res_);
  bool send_initial_ctxt_setup_failure(uint16_t rnti);
  bool send_erab_setup_response(uint16_t rnti, LIBLTE_S1AP_MESSAGE_E_RABSETUPRESPONSE_STRUCT* res_);
  // bool send_ue_capabilities(uint16_t rnti, LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *caps)
  bool send_uectxmodifyresp(uint16_t rnti);
  bool send_uectxmodifyfailure(uint16_t rnti, LIBLTE_S1AP_CAUSE_STRUCT* cause);
  // handover
  bool send_ho_required(uint16_t                     rnti,
                        uint32_t                     target_eci,
                        srslte::plmn_id_t            target_plmn,
                        srslte::unique_byte_buffer_t rrc_container) override;
  bool handle_hopreparationfailure(LIBLTE_S1AP_MESSAGE_HANDOVERPREPARATIONFAILURE_STRUCT* msg);
  bool handle_s1hocommand(LIBLTE_S1AP_MESSAGE_HANDOVERCOMMAND_STRUCT& msg);
  bool send_enb_status_transfer_proc(uint16_t rnti, std::vector<bearer_status_info>& bearer_status_list) override;

  bool        find_mme_ue_id(uint32_t mme_ue_id, uint16_t* rnti, uint32_t* enb_ue_id);
  std::string get_cause(const LIBLTE_S1AP_CAUSE_STRUCT* c);

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
      srslte::proc_outcome_t react(const LIBLTE_S1AP_MESSAGE_HANDOVERPREPARATIONFAILURE_STRUCT& msg);
      srslte::proc_outcome_t react(LIBLTE_S1AP_MESSAGE_HANDOVERCOMMAND_STRUCT& msg);
      void                   then(const srslte::proc_state_t& result);
      const char*            name() { return "HandoverPreparation"; }

    private:
      s1ap::ue* ue_ptr   = nullptr;
      s1ap*     s1ap_ptr = nullptr;

      uint32_t                     target_eci = 0;
      srslte::plmn_id_t            target_plmn;
      srslte::unique_byte_buffer_t rrc_container;
    };

    explicit ue(uint16_t rnti, s1ap* s1ap_ptr_);

    ue_ctxt_t&                      get_ctxt() { return ctxt; }
    srslte::proc_t<ho_prep_proc_t>& get_ho_prep_proc() { return ho_prep_proc; }

    bool send_enb_status_transfer_proc(std::vector<bearer_status_info>& bearer_status_list);

  private:
    bool
    send_ho_required(uint32_t target_eci_, srslte::plmn_id_t target_plmn_, srslte::unique_byte_buffer_t rrc_container);
    //! TS 36.413, Section 8.4.6 - eNB Status Transfer procedure

    s1ap*        s1ap_ptr;
    srslte::log* s1ap_log;
    ue_ctxt_t    ctxt = {};

    srslte::timer_handler::unique_timer ts1_reloc_prep;    ///< TS1_{RELOCprep} - max time for HO preparation
    srslte::timer_handler::unique_timer ts1_reloc_overall; ///< TS1_{RELOCOverall}

    // user procedures
    srslte::proc_t<ho_prep_proc_t> ho_prep_proc;
  };
  std::map<uint16_t, std::unique_ptr<ue> > users;
  std::map<uint32_t, uint16_t>             enbid_to_rnti_map;

  ue_ctxt_t* get_user_ctxt(uint16_t rnti);

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
  srslte::proc_t<s1_setup_proc_t> s1setup_proc;
};

} // namespace srsenb

#endif // SRSENB_S1AP_H
