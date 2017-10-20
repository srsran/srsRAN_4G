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
#include <boost/thread/mutex.hpp>
#include "mme/mme.h"

namespace srsepc{

mme*          mme::m_instance = NULL;
boost::mutex  mme_instance_mutex;

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
  boost::mutex::scoped_lock lock(mme_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new mme();
  }
  return(m_instance);
}

void
mme::cleanup(void)
{
  boost::mutex::scoped_lock lock(mme_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}

int
mme::init(all_args_t* args)
{
  /*Init loggers*/
  if (!args->log_args.filename.compare("stdout")) {
    m_logger = &m_logger_stdout;
  } else {
    m_logger_file.init(args->log_args.filename);
    m_logger_file.log("\n---  Software Radio Systems MME log ---\n\n");
    m_logger = &m_logger_file;
  }

  m_s1ap_log.init("S1AP", m_logger);
  m_s1ap_log.set_level(srslte::LOG_LEVEL_DEBUG);
  m_s1ap_log.set_hex_limit(32);
  if(m_s1ap.init(args->s1ap_args, &m_s1ap_log)){
    m_s1ap_log.error("Error initializing MME S1APP\n");
    exit(-1);
  }
  m_s1ap_log.info("Initialized S1-MME\n");
  m_s1ap_log.console("Initialized S1-MME\n");
  return 0;
}

void
mme::stop()
{
  if(m_running)
  {
    m_running = false;
    thread_cancel();
    wait_thread_finish();
  }
  m_s1ap.stop();
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
  int s1mme = m_s1ap.get_s1_mme();
  while(m_running)
  {
    m_s1ap_log.debug("Waiting for SCTP Msg\n");
    pdu->reset();
    rd_sz = sctp_recvmsg(s1mme, pdu->msg, sz,(struct sockaddr*) &enb_addr, &fromlen, &sri, &msg_flags);
    if (rd_sz == -1 && errno != EAGAIN){
      m_s1ap_log.error("Error reading from SCTP socket: %s", strerror(errno));
    }
    else if (rd_sz == -1 && errno == EAGAIN){
      m_s1ap_log.debug("Socket timeout reached");
    }
    else{
      pdu->N_bytes = rd_sz;
      m_s1ap_log.info("Received S1AP msg. Size: %d\n", pdu->N_bytes);
      m_s1ap.handle_s1ap_rx_pdu(pdu,&sri);
    }
  }
  return;
}

} //namespace srsepc
