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

#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>
#include <unistd.h>

#include "mme/s1ap.h"

namespace srsepc{

s1ap::s1ap():
  m_s1mme(-1)
{
}

s1ap::~s1ap()
{
}

int
s1ap::init(s1ap_args_t s1ap_args, srslte::log *s1ap_log)
{
  m_mme_code    = s1ap_args.mme_code ;
  m_mme_group   = s1ap_args.mme_group;
  m_tac         = s1ap_args.tac;
  m_mcc         = s1ap_args.mcc;        
  m_mnc         = s1ap_args.mnc;        
  m_mme_bind_addr = s1ap_args.mme_bind_addr;
  m_mme_name = std::string("SRS MME");

  m_log_h = s1ap_log;

  m_s1mme = enb_listen();
  return 0;
}

void
s1ap::stop()
{
  if (m_s1mme != -1){
    close(m_s1mme);
  }
  return;
}

int
s1ap::get_s1_mme()
{
  return m_s1mme;
}

int
s1ap::enb_listen()
{
  /*This function sets up the SCTP socket for eNBs to connect to*/
  int sock_fd, err;
  struct sockaddr_in s1mme_addr;//TODO make this a configurable class memeber.
  struct sctp_event_subscribe evnts;

  m_log_h->info("Initializing S1-MME ...");
  sock_fd = socket (AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  if (sock_fd == -1){
    std::cout << "[S1APP] Could not create SCTP socket" <<std::endl; //TODO fix logging
    return -1;
  }

  //Set timeout
  struct timeval timeout;      
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  if (setsockopt (sock_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
    std::cout << "Set socket timeout failed" << std::endl;
    return -1; 
  }

  bzero (&evnts, sizeof (evnts)) ;
  evnts.sctp_data_io_event = 1;
  if(setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof (evnts))){
    std::cout << "Subscribing to sctp_data_io_events failed" << std::cout;
    return -1;
  }

  //S1-MME bind
  bzero(&s1mme_addr, sizeof(s1mme_addr));
  s1mme_addr.sin_family = AF_INET; 
  inet_pton(AF_INET, m_mme_bind_addr.c_str(), &(s1mme_addr.sin_addr) );
  //s1mme_addr.sin_addr.s_addr = htonl(INADDR_ANY); //TODO this should use the bindx information
  s1mme_addr.sin_port = htons(S1MME_PORT);
  err = bind(sock_fd, (struct sockaddr*) &s1mme_addr, sizeof (s1mme_addr));
  if (err != 0){
    std::cout << "Error binding SCTP socket" << std::endl;
    return -1;
  }

  //Listen for connections
  err = listen(sock_fd,SOMAXCONN);
  if (err != 0){
    std::cout << "Error in SCTP socket listen" << std::endl;
    return -1;
  }

  return sock_fd;
}


}//namespace srsepc
