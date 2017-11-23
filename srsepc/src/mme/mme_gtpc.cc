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

#include <iostream>
#include "srslte/asn1/gtpc.h"
#include "mme/mme_gtpc.h"

namespace srsepc{

mme_gtpc*          mme_gtpc::m_instance = NULL;
boost::mutex  mme_gtpc_instance_mutex;

mme_gtpc::mme_gtpc()
{
}

mme_gtpc::~mme_gtpc()
{
}

mme_gtpc*
mme_gtpc::get_instance(void)
{
  boost::mutex::scoped_lock lock(mme_gtpc_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new mme_gtpc();
  }
  return(m_instance);
}

void
mme_gtpc::cleanup(void)
{
  boost::mutex::scoped_lock lock(mme_gtpc_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}


void
mme_gtpc::init()
{
  m_mme_gtpc_ip = inet_addr("127.0.0.1");//FIXME At the moment, the GTP-C messages are not sent over the wire. So this parameter is not used.
  m_spgw = spgw::get_instance();
}

void
mme_gtpc::send_create_session_request(uint64_t imsi, struct create_session_response *cs_resp)
{
  struct srslte::gtpc_pdu cs_req_pdu;
  struct srslte::gtpc_create_session_request *cs_req = &cs_req_pdu.choice.create_session_request;

  //Setup GTP-C Header. FIXME: Length, sequence and other fields need to be added.
  cs_req_pdu.header.piggyback = false;
  cs_req_pdu.header.teid_present = true;
  cs_req_pdu.header.teid = 0; //Send create session request to the butler TEID
  cs_req_pdu.header.type = srslte::GTPC_MSG_TYPE_CREATE_SESSION_REQUEST;

  //Setup GTP-C Create Session Request IEs
  // Control TEID allocated \\
  cs_req->sender_f_teid.tied = get_new_ctrl_teid();
  cs_req->sender_f_teid.ipv4 = m_mme_gtpc_ip;
  // APN \\
  memcpy(cs_req->apn, "internet", sizeof("internet"));
  // RAT Type \\
  cs_req->rat_type = GTPC_RAT_TYPE::EUTRAN;

  //Save RX Control TEID
  //create_rx_control_teid(cs_req->sender_f_teid);

  //spgw->handle_create_session_request(&cs_req, cs_resp);
  return;
}
} //namespace srsepc
