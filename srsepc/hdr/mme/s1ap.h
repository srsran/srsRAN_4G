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
#include "srslte/common/common.h"
#include "srslte/common/log.h"

#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>
#include <unistd.h>

#include "mme/s1ap_mngmt_proc.h"

namespace srsepc{

const uint16_t S1MME_PORT = 36412;

typedef struct{
  uint8_t       mme_code;
  uint16_t      mme_group;
  uint16_t      tac;        // 16-bit tac
  uint16_t      mcc;        // BCD-coded with 0xF filler
  uint16_t      mnc;        // BCD-coded with 0xF filler
  std::string   mme_bind_addr;
  std::string   mme_name;
} s1ap_args_t;

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

private:
  uint8_t       m_mme_code;
  uint16_t      m_mme_group;
  uint16_t      m_tac;        // 16-bit tac
  uint16_t      m_mcc;        // BCD-coded with 0xF filler
  uint16_t      m_mnc;        // BCD-coded with 0xF filler
  uint32_t      m_plmn;

  std::string   m_mme_bind_addr;
  std::string   m_mme_name;

  srslte::log   *m_s1ap_log;

  int m_s1mme;
  s1ap_mngmt_proc m_s1ap_mngmt_proc;
};




} //namespace srsepc

#endif //S1AP_H
