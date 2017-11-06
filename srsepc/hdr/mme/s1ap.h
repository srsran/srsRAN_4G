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
#include "mme/s1ap_common.h"
#include "mme/s1ap_mngmt_proc.h"
#include "mme/s1ap_nas_transport.h"
#include "hss/hss.h"

namespace srsepc{

const uint16_t S1MME_PORT = 36412;

class s1ap
{
public:
  s1ap();
  virtual ~s1ap();
  int enb_listen(); 
  int init(s1ap_args_t s1ap_args, srslte::log *s1ap_log);
  void stop();

  int get_s1_mme();

  bool handle_s1ap_rx_pdu(srslte::byte_buffer_t *pdu, struct sctp_sndrcvinfo *enb_sri);

  bool handle_initiating_message(LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *msg, struct sctp_sndrcvinfo *enb_sri);

  bool handle_s1_setup_request(LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT *msg, struct sctp_sndrcvinfo *enb_sri);

  bool send_s1_setup_failure(struct sctp_sndrcvinfo *enb_sri);
  
  bool send_s1_setup_response(struct sctp_sndrcvinfo *enb_sri);
 
  bool handle_initial_ue_message(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *init_ue, struct sctp_sndrcvinfo *enb_sri);
 
  bool handle_uplink_nas_transport(LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ul_xport, struct sctp_sndrcvinfo *enb_sri);

  void print_enb_ctx_info(const enb_ctx_t &enb_ctx);

private:

  s1ap_args_t                  m_s1ap_args;
  uint32_t                     m_plmn;
  srslte::byte_buffer_pool    *m_pool;
  srslte::log                 *m_s1ap_log;

  hss *m_hss;
  int m_s1mme;
  std::map<uint16_t,enb_ctx_t*> m_active_enbs;
  uint32_t m_next_mme_ue_s1ap_id;
 
  s1ap_mngmt_proc m_s1ap_mngmt_proc;
  s1ap_nas_transport m_s1ap_nas_transport;
};




} //namespace srsepc

#endif //S1AP_H
