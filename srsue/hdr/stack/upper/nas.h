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

#ifndef SRSUE_NAS_H
#define SRSUE_NAS_H

#include "srslte/asn1/liblte_mme.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/nas_pcap.h"
#include "srslte/common/security.h"
#include "srslte/common/stack_procedure.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srsue/hdr/stack/upper/nas_common.h"
#include "srsue/hdr/stack/upper/nas_metrics.h"

using srslte::byte_buffer_t;

namespace srsue {

class nas : public nas_interface_rrc, public nas_interface_ue
{
public:
  nas(srslte::log* log_);
  void init(usim_interface_nas* usim_, rrc_interface_nas* rrc_, gw_interface_nas* gw_, const nas_args_t& args_);
  void stop();
  void run_tti(uint32_t tti) final;

  void        get_metrics(nas_metrics_t* m);
  emm_state_t get_state();

  // RRC interface
  void     left_rrc_connected();
  void     paging(srslte::s_tmsi_t* ue_identity);
  void     set_barring(barring_t barring);
  void     write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  uint32_t get_k_enb_count();
  bool     is_attached();
  bool     get_k_asme(uint8_t* k_asme_, uint32_t n);
  uint32_t get_ipv4_addr();
  bool     get_ipv6_addr(uint8_t* ipv6_addr);

  // UE interface
  void start_attach_request(srslte::proc_state_t* result) final;
  bool detach_request(const bool switch_off) final;

  void plmn_search_completed(rrc_interface_nas::found_plmn_t found_plmns[rrc_interface_nas::MAX_FOUND_PLMNS],
                             int                             nof_plmns) final;
  bool start_connection_request(srslte::establishment_cause_t establish_cause,
                                srslte::unique_byte_buffer_t  ded_info_nas);
  bool connection_request_completed(bool outcome) final;

  // PCAP
  void start_pcap(srslte::nas_pcap *pcap_);

private:
  srslte::byte_buffer_pool* pool    = nullptr;
  srslte::log*              nas_log = nullptr;
  rrc_interface_nas*        rrc     = nullptr;
  usim_interface_nas*       usim    = nullptr;
  gw_interface_nas*         gw      = nullptr;

  nas_args_t cfg = {};

  emm_state_t state = EMM_STATE_DEREGISTERED;

  nas_interface_rrc::barring_t current_barring = BARRING_NONE;

  bool                 plmn_is_selected = false;
  srslte::plmn_id_t    current_plmn;
  srslte::plmn_id_t    home_plmn;

  std::vector<srslte::plmn_id_t> known_plmns;

  LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT emm_info;

  // Security context
  struct nas_sec_ctxt{
    uint8_t  ksi;
    uint8_t  k_asme[32];
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t k_enb_count;
    srslte::CIPHERING_ALGORITHM_ID_ENUM  cipher_algo;
    srslte::INTEGRITY_ALGORITHM_ID_ENUM  integ_algo;
    LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT guti;
  };

  typedef enum { DEFAULT_EPS_BEARER = 0, DEDICATED_EPS_BEARER } eps_bearer_type_t;

  typedef struct {
    eps_bearer_type_t type;
    uint8_t           eps_bearer_id;
    uint8_t           linked_eps_bearer_id;
  } eps_bearer_t;

  typedef std::map<uint8_t, eps_bearer_t>  eps_bearer_map_t;
  typedef std::pair<uint8_t, eps_bearer_t> eps_bearer_map_pair_t;
  eps_bearer_map_t                         eps_bearer;

  bool         have_guti    = false;
  bool         have_ctxt    = false;
  nas_sec_ctxt ctxt         = {};
  bool         auth_request = false;

  uint32_t ip_addr       = 0;
  uint8_t  ipv6_if_id[8] = {};

  uint8_t chap_id = 0;

  uint8_t transaction_id = 0;

  // Security
  bool    eia_caps[8]   = {};
  bool    eea_caps[8]   = {};
  uint8_t k_nas_enc[32] = {};
  uint8_t k_nas_int[32] = {};

  // PCAP
  srslte::nas_pcap* pcap = nullptr;

  bool running = false;

  void integrity_generate(uint8_t *key_128,
                          uint32_t count,
                          uint8_t direction,
                          uint8_t *msg,
                          uint32_t msg_len,
                          uint8_t *mac);
  bool integrity_check(srslte::byte_buffer_t* pdu);
  void cipher_encrypt(srslte::byte_buffer_t* pdu);
  void cipher_decrypt(srslte::byte_buffer_t* pdu);
  void set_k_enb_count(uint32_t count);

  bool check_cap_replay(LIBLTE_MME_UE_SECURITY_CAPABILITIES_STRUCT *caps);

  void select_plmn();

  // Parsers
  void parse_attach_accept(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_attach_reject(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_authentication_request(uint32_t lcid, srslte::unique_byte_buffer_t pdu, const uint8_t sec_hdr_type);
  void parse_authentication_reject(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_identity_request(srslte::unique_byte_buffer_t pdu, const uint8_t sec_hdr_type);
  void parse_security_mode_command(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_service_reject(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_esm_information_request(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_emm_information(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_detach_request(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_emm_status(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_activate_dedicated_eps_bearer_context_request(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_deactivate_eps_bearer_context_request(srslte::unique_byte_buffer_t pdu);
  void parse_activate_test_mode(uint32_t lcid, srslte::unique_byte_buffer_t pdu, const uint8_t sec_hdr_type);
  void parse_modify_eps_bearer_context_request(srslte::unique_byte_buffer_t pdu);

  // Packet generators
  void gen_attach_request(srslte::byte_buffer_t* msg);
  void gen_service_request(srslte::byte_buffer_t* msg);

  // Senders
  void send_attach_complete(const uint8_t& transaction_id, const uint8_t& eps_bearer_id);
  void send_identity_response(uint8 id_type, const uint8_t sec_hdr_type);
  void send_service_request();
  void send_esm_information_response(const uint8 proc_transaction_id);
  void send_authentication_response(const uint8_t* res, const size_t res_len, const uint8_t sec_hdr_type);
  void send_authentication_failure(const uint8_t cause, const uint8_t* auth_fail_param);
  void gen_pdn_connectivity_request(LIBLTE_BYTE_MSG_STRUCT *msg);
  void send_security_mode_reject(uint8_t cause);
  void send_detach_request(bool switch_off);
  void send_detach_accept();
  void send_activate_dedicated_eps_bearer_context_accept(const uint8_t& proc_transaction_id,
                                                         const uint8_t& eps_bearer_id);
  void send_deactivate_eps_bearer_context_accept(const uint8_t& proc_transaction_id, const uint8_t& eps_bearer_id);
  void send_modify_eps_bearer_context_accept(const uint8_t& proc_transaction_id, const uint8_t& eps_bearer_id);
  void send_activate_test_mode_complete(const uint8_t sec_hdr_type);

  // Other internal helpers
  void enter_emm_deregistered();

  // security context persistence file
  bool read_ctxt_file(nas_sec_ctxt *ctxt);
  bool write_ctxt_file(nas_sec_ctxt ctxt);

  // ctxt file helpers
  std::string hex_to_string(uint8_t *hex, int size);
  bool        string_to_hex(std::string hex_str, uint8_t *hex, uint32_t len);
  std::string emm_info_str(LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT *info);

  template <class T>
  bool readvar(std::istream &file, const char *key, T *var)
  {
    std::string line;
    size_t len = strlen(key);
    std::getline(file, line);
    if(line.substr(0,len).compare(key)) {
      return false;
    }
    *var = (T)atoi(line.substr(len).c_str());
    return true;
  }

  bool readvar(std::istream &file, const char *key, uint8_t *var, int varlen)
  {
    std::string line;
    size_t len = strlen(key);
    std::getline(file, line);
    if(line.substr(0,len).compare(key)) {
      return false;
    }
    std::string tmp = line.substr(len);
    if(!string_to_hex(tmp, var, varlen)) {
      return false;
    }
    return true;
  }

  std::vector<uint8_t> split_string(const std::string input)
  {
    std::vector<uint8_t> list;
    std::stringstream    ss(input);
    while (ss.good()) {
      std::string substr;
      getline(ss, substr, ',');
      if (not substr.empty()) {
        list.push_back(atoi(substr.c_str()));
      }
    }
    return list;
  }

  class rrc_connect_proc : public srslte::proc_impl_t
  {
  public:
    struct connection_request_completed_t {
      bool outcome;
    };

    srslte::proc_outcome_t init(nas* nas_ptr_, srslte::establishment_cause_t cause_, srslte::unique_byte_buffer_t pdu);
    srslte::proc_outcome_t step() final;
    static const char*     name() { return "RRC Connect"; }

  private:
    nas* nas_ptr;
    enum class state_t { conn_req, wait_attach } state;
    uint32_t wait_timeout;
  };
  class plmn_search_proc : public srslte::proc_impl_t
  {
  public:
    struct plmn_search_complete_t {
      rrc_interface_nas::found_plmn_t found_plmns[rrc_interface_nas::MAX_FOUND_PLMNS];
      int                             nof_plmns;
      plmn_search_complete_t(rrc_interface_nas::found_plmn_t* plmns_, int nof_plmns_) : nof_plmns(nof_plmns_)
      {
        if (nof_plmns > 0) {
          std::copy(&plmns_[0], &plmns_[nof_plmns], found_plmns);
        }
      }
    };

    srslte::proc_outcome_t init(nas* nas_ptr_);
    srslte::proc_outcome_t step() final;
    srslte::proc_outcome_t trigger_event(const plmn_search_complete_t& t);
    static const char*     name() { return "PLMN Search"; }

  private:
    nas* nas_ptr;
    enum class state_t { plmn_search, rrc_connect } state;
  };
  srslte::callback_list_t                     callbacks;
  srslte::proc_t<plmn_search_proc>            plmn_searcher;
  srslte::proc_t<rrc_connect_proc>            rrc_connector;
  srslte::proc_t<srslte::query_proc_t<bool> > conn_req_proc;
};

} // namespace srsue


#endif // SRSUE_NAS_H
