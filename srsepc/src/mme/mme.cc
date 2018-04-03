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

#include <iostream> //TODO Remove
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>
#include "srsepc/hdr/mme/mme.h"

namespace srsepc{

mme*          mme::m_instance = NULL;
pthread_mutex_t mme_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

mme::mme():
  m_running(false)
{
  m_pool = srslte::byte_buffer_pool::get_instance();     
  return;
}

mme::~mme()
{
  return;
}

mme*
mme::get_instance(void)
{
  pthread_mutex_lock(&mme_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new mme();
  }
  pthread_mutex_unlock(&mme_instance_mutex);
  return(m_instance);
}

void
mme::cleanup(void)
{
  pthread_mutex_lock(&mme_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&mme_instance_mutex);
}

int
mme::init(mme_args_t* args, srslte::log_filter *s1ap_log, srslte::log_filter *mme_gtpc_log, hss_interface_s1ap * hss_)
{

  /*Init logger*/
  m_s1ap_log = s1ap_log;
  m_mme_gtpc_log = mme_gtpc_log;
  /*Init S1AP*/
  m_s1ap = s1ap::get_instance();
  if(m_s1ap->init(args->s1ap_args, s1ap_log, hss_)){
    m_s1ap_log->error("Error initializing MME S1APP\n");
    exit(-1);
  }

  /*Init GTP-C*/
  m_mme_gtpc = mme_gtpc::get_instance();
  if(!m_mme_gtpc->init(m_mme_gtpc_log))
  {
    m_s1ap_log->console("Error initializing GTP-C\n");
    exit(-1);
  }

  /*Log successful initialization*/
  m_s1ap_log->info("MME Initialized. MCC: %d, MNC: %d\n",args->s1ap_args.mcc, args->s1ap_args.mnc);
  m_s1ap_log->console("MME Initialized. \n");
  return 0;
}

void
mme::stop()
{
  if(m_running)
  {
    m_s1ap->stop();
    m_s1ap->cleanup();
    m_running = false;
    thread_cancel();
    wait_thread_finish();
  }
  return;
}

void
mme::run_thread()
{
  srslte::byte_buffer_t *pdu = m_pool->allocate();
  uint32_t sz = SRSLTE_MAX_BUFFER_SIZE_BYTES - SRSLTE_BUFFER_HEADER_OFFSET;

  struct sockaddr_in enb_addr;
  struct sctp_sndrcvinfo sri;
  socklen_t fromlen = sizeof(enb_addr);
  bzero(&enb_addr, sizeof(enb_addr));
  int rd_sz;
  int msg_flags=0;

  //Mark the thread as running
  m_running=true;

  //Get S1-MME socket
  int s1mme = m_s1ap->get_s1_mme();
  while(m_running)
  {
    m_s1ap_log->debug("Waiting for SCTP Msg\n");
    pdu->reset();
    rd_sz = sctp_recvmsg(s1mme, pdu->msg, sz,(struct sockaddr*) &enb_addr, &fromlen, &sri, &msg_flags);
    if (rd_sz == -1 && errno != EAGAIN){
      m_s1ap_log->error("Error reading from SCTP socket: %s", strerror(errno));
    }
    else if (rd_sz == -1 && errno == EAGAIN){
      m_s1ap_log->debug("Socket timeout reached");
    }
    else{
      if(msg_flags & MSG_NOTIFICATION)
      {
        //Received notification
        union sctp_notification *notification = (union sctp_notification*)pdu->msg;
        m_s1ap_log->debug("SCTP Notification %d\n", notification->sn_header.sn_type);
        if (notification->sn_header.sn_type == SCTP_SHUTDOWN_EVENT)
        {
          m_s1ap_log->info("SCTP Association Shutdown. Association: %d\n",sri.sinfo_assoc_id);
          m_s1ap_log->console("SCTP Association Shutdown. Association: %d\n",sri.sinfo_assoc_id);
          m_s1ap->delete_enb_ctx(sri.sinfo_assoc_id);
        }
      }
      else
      { 
        //Received data
        pdu->N_bytes = rd_sz;
        m_s1ap_log->info("Received S1AP msg. Size: %d\n", pdu->N_bytes);
        m_s1ap->handle_s1ap_rx_pdu(pdu,&sri);
      }
    }
  }
  return;
}

} //namespace srsepc
