/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
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
#ifndef S1AP_H
#define S1AP_H

#include "srslte/asn1/gtpc.h"
#include "srslte/asn1/liblte_s1ap.h"
#include "srslte/asn1/liblte_mme.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"

#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>
#include <unistd.h>
#include <map>
#include <set>
#include "mme/s1ap_common.h"
#include "mme/s1ap_mngmt_proc.h"
#include "mme/s1ap_nas_transport.h"
#include "mme/s1ap_ctx_mngmt_proc.h"
#include "mme/mme_gtpc.h"
#include "hss/hss.h"

namespace srsepc{

const uint16_t S1MME_PORT = 36412;

class s1ap
{
public:

  static s1ap* get_instance();
  static void cleanup();

  int enb_listen();
  int init(s1ap_args_t s1ap_args, srslte::log_filter *s1ap_log);
  void stop();

  int get_s1_mme();

  void delete_enb_ctx(int32_t assoc_id);
  void delete_ues_in_enb(uint16_t enb_id);

  bool handle_s1ap_rx_pdu(srslte::byte_buffer_t *pdu, struct sctp_sndrcvinfo *enb_sri);
  bool handle_initiating_message(LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *msg, struct sctp_sndrcvinfo *enb_sri);
  bool handle_successful_outcome(LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT *msg);

  void activate_eps_bearer(uint32_t mme_s1ap_id, uint8_t ebi);

  void print_enb_ctx_info(const std::string &prefix, const enb_ctx_t &enb_ctx);

  uint32_t get_plmn();
  uint32_t get_next_mme_ue_s1ap_id();
  enb_ctx_t* find_enb_ctx(uint16_t enb_id);
  void add_new_enb_ctx(const enb_ctx_t &enb_ctx, const struct sctp_sndrcvinfo* enb_sri);
  ue_ctx_t* find_ue_ctx(uint32_t mme_ue_s1ap_id);
  void add_new_ue_ctx(const ue_ctx_t &ue_ctx);
  bool delete_ue_ctx(ue_ctx_t *ue_ctx);

  uint32_t allocate_m_tmsi(uint32_t mme_ue_s1ap_id);

  s1ap_args_t                    m_s1ap_args;
  srslte::log_filter            *m_s1ap_log;

  s1ap_mngmt_proc*               m_s1ap_mngmt_proc;
  s1ap_nas_transport*             m_s1ap_nas_transport;
  s1ap_ctx_mngmt_proc*             m_s1ap_ctx_mngmt_proc;

  std::map<uint32_t, uint32_t>                      m_tmsi_to_s1ap_id;

private:
  s1ap();
  virtual ~s1ap();

  static s1ap *m_instance;

  uint32_t                       m_plmn;
  srslte::byte_buffer_pool      *m_pool;

  hss *m_hss;
  int m_s1mme;
  std::map<uint16_t, enb_ctx_t*>                    m_active_enbs;
  std::map<int32_t, uint16_t>                       m_sctp_to_enb_id;
  std::map<uint32_t, ue_ctx_t*>                     m_active_ues;
  std::map<uint16_t,std::set<uint32_t> >            m_enb_id_to_ue_ids;
  uint32_t                                          m_next_mme_ue_s1ap_id;
  uint32_t                                          m_next_m_tmsi;

  //FIXME the GTP-C should be moved to the MME class, the the packaging of GTP-C messages is done.
  mme_gtpc *m_mme_gtpc;
};

inline uint32_t
s1ap::get_plmn()
{
  return m_plmn;
}


} //namespace srsepc

#endif //S1AP_H
