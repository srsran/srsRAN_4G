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
#ifndef SRSEPC_S1AP_H
#define SRSEPC_S1AP_H

#include "mme_gtpc.h"
#include "nas.h"
#include "s1ap_ctx_mngmt_proc.h"
#include "s1ap_mngmt_proc.h"
#include "s1ap_nas_transport.h"
#include "s1ap_paging.h"
#include "srsepc/hdr/hss/hss.h"
#include "srslte/asn1/gtpc.h"
#include "srslte/asn1/liblte_mme.h"
#include "srslte/asn1/s1ap_asn1.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/s1ap_pcap.h"
#include "srslte/interfaces/epc_interfaces.h"
#include <arpa/inet.h>
#include <map>
#include <netinet/sctp.h>
#include <set>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace srsepc {

const uint16_t S1MME_PORT = 36412;

using s1ap_pdu_t = asn1::s1ap::s1ap_pdu_c;

class s1ap : public s1ap_interface_nas, public s1ap_interface_gtpc, public s1ap_interface_mme
{
public:
  static s1ap* get_instance();
  static void  cleanup();

  int  enb_listen();
  int  init(s1ap_args_t s1ap_args, srslte::log_filter* s1ap_log, srslte::log_filter* nas_log);
  void stop();

  int get_s1_mme();

  void delete_enb_ctx(int32_t assoc_id);

  bool s1ap_tx_pdu(const s1ap_pdu_t& pdu, struct sctp_sndrcvinfo* enb_sri);
  void handle_s1ap_rx_pdu(srslte::byte_buffer_t* pdu, struct sctp_sndrcvinfo* enb_sri);
  void handle_initiating_message(const asn1::s1ap::init_msg_s& msg, struct sctp_sndrcvinfo* enb_sri);
  void handle_successful_outcome(const asn1::s1ap::successful_outcome_s& msg);

  void activate_eps_bearer(uint64_t imsi, uint8_t ebi);

  void print_enb_ctx_info(const std::string& prefix, const enb_ctx_t& enb_ctx);

  uint32_t   get_plmn();
  uint16_t   get_tac();
  uint32_t   get_next_mme_ue_s1ap_id();
  enb_ctx_t* find_enb_ctx(uint16_t enb_id);
  void       add_new_enb_ctx(const enb_ctx_t& enb_ctx, const struct sctp_sndrcvinfo* enb_sri);
  void       get_enb_ctx(uint16_t sctp_stream);

  bool add_nas_ctx_to_imsi_map(nas* nas_ctx);
  bool add_nas_ctx_to_mme_ue_s1ap_id_map(nas* nas_ctx);
  bool add_ue_to_enb_set(int32_t enb_assoc, uint32_t mme_ue_s1ap_id);

  virtual nas* find_nas_ctx_from_imsi(uint64_t imsi);
  nas*         find_nas_ctx_from_mme_ue_s1ap_id(uint32_t mme_ue_s1ap_id);

  bool         release_ue_ecm_ctx(uint32_t mme_ue_s1ap_id);
  void         release_ues_ecm_ctx_in_enb(int32_t enb_assoc);
  virtual bool delete_ue_ctx(uint64_t imsi);

  uint32_t         allocate_m_tmsi(uint64_t imsi);
  virtual uint64_t find_imsi_from_m_tmsi(uint32_t m_tmsi);

  s1ap_args_t         m_s1ap_args;
  srslte::log_filter* m_s1ap_log;
  srslte::log_filter* m_nas_log;

  s1ap_mngmt_proc*     m_s1ap_mngmt_proc;
  s1ap_nas_transport*  m_s1ap_nas_transport;
  s1ap_ctx_mngmt_proc* m_s1ap_ctx_mngmt_proc;
  s1ap_paging*         m_s1ap_paging;

  std::map<uint32_t, uint64_t>   m_tmsi_to_imsi;
  std::map<uint16_t, enb_ctx_t*> m_active_enbs;

  // Interfaces
  virtual bool send_initial_context_setup_request(uint64_t imsi, uint16_t erab_to_setup);
  virtual bool send_ue_context_release_command(uint32_t mme_ue_s1ap_id);
  virtual bool send_downlink_nas_transport(uint32_t               enb_ue_s1ap_id,
                                           uint32_t               mme_ue_s1ap_id,
                                           srslte::byte_buffer_t* nas_msg,
                                           struct sctp_sndrcvinfo enb_sri);
  virtual bool send_paging(uint64_t imsi, uint16_t erab_to_setup);

  virtual bool expire_nas_timer(enum nas_timer_type type, uint64_t imsi);

private:
  s1ap();
  virtual ~s1ap();

  static s1ap* m_instance;

  uint32_t                  m_plmn;
  srslte::byte_buffer_pool* m_pool;

  hss_interface_nas*                     m_hss;
  int                                    m_s1mme;
  std::map<int32_t, uint16_t>            m_sctp_to_enb_id;
  std::map<int32_t, std::set<uint32_t> > m_enb_assoc_to_ue_ids;

  std::map<uint64_t, nas*> m_imsi_to_nas_ctx;
  std::map<uint32_t, nas*> m_mme_ue_s1ap_id_to_nas_ctx;

  uint32_t m_next_mme_ue_s1ap_id;
  uint32_t m_next_m_tmsi;

  // GTP-C Interface
  mme_gtpc* m_mme_gtpc;

  // PCAP
  bool              m_pcap_enable;
  srslte::s1ap_pcap m_pcap;
};

inline uint32_t s1ap::get_plmn()
{
  return m_plmn;
}

inline uint16_t s1ap::get_tac()
{
  return m_s1ap_args.tac;
}

} // namespace srsepc
#endif // SRSEPC_S1AP_H
