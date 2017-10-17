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

mme::mme()
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
    m_logger_file.log("\n\n");
    m_logger = &m_logger_file;
  }

  m_s1ap_log.init("S1AP", m_logger);
  m_s1ap_log.set_level(srslte::LOG_LEVEL_DEBUG);
  m_s1ap_log.set_hex_limit(32);
  if(m_s1ap.init(args->s1ap_args, &m_s1ap_log)){
    std::cout << "Error initializing MME S1APP" << std::endl;
    exit(-1);
  }
  m_s1ap_log.console("Initialized MME\n");
  return 0;
}

void
mme::stop()
{
  m_s1ap.stop();
  return;
}

int
mme::get_s1_mme()
{
  return m_s1ap.get_s1_mme();
}

void
mme::main_loop()
{
  srslte::byte_buffer_t *pdu = m_pool->allocate();
  uint32_t sz = SRSLTE_MAX_BUFFER_SIZE_BYTES - SRSLTE_BUFFER_HEADER_OFFSET;

  struct sockaddr_in enb_addr;
  struct sctp_sndrcvinfo sri;
  socklen_t fromlen = sizeof(enb_addr);
  bzero(&enb_addr, sizeof(enb_addr));
  int rd_sz;
  int msg_flags=0;

  //Get S1-MME socket
  int s1mme = m_s1ap.get_s1_mme();
  while(true)
  {
    std::cout << "Waiting for SCTP Msg " << std::endl;
    pdu->reset();
    rd_sz = sctp_recvmsg(s1mme, pdu->msg, sz,(struct sockaddr*) &enb_addr, &fromlen, &sri, &msg_flags);
    if (rd_sz == -1 && errno != EAGAIN){
      std::cout<< "Error reading from SCTP socket" << std::endl;
      printf("Error: %s\n", strerror(errno));
    }
    else if (rd_sz == -1 && errno == EAGAIN){
      std::cout << "Timeout reached" << std::endl;
    }
    else{
      pdu->N_bytes = rd_sz;
      std::cout<< "Received SCTP msg." << std::endl;
      std::cout << "\tSize: " << pdu->N_bytes << std::endl;
      std::cout << "\tMsg: " << pdu->msg << std::endl;
      m_s1ap.handle_s1ap_rx_pdu(pdu,&sri);
      sctp_send(s1mme, "OK", 2, &sri, 0);


    }
  }
}

} //namespace srsepe<
