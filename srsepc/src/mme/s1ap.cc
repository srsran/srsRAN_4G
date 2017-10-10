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
#include "mme/s1ap.h"

namespace srsepc{

s1ap::s1ap()
{
}

s1ap::~s1ap()
{
}

int
s1ap::init(s1ap_args_t s1ap_args)
{
  m_mme_code    = s1ap_args.mme_code ;
  m_mme_group   = s1ap_args.mme_group;
  m_tac         = s1ap_args.tac;
  m_mcc         = s1ap_args.mcc;        
  m_mnc         = s1ap_args.mnc;        
  m_mme_bindx_addr = s1ap_args.mme_bindx_addr;
  m_mme_name = std::string("SRS MME");

  m_s1mme = enb_listen();
  return 0;
}

int
s1ap::enb_listen()
{
  /*This function sets up the SCTP socket for eNBs to connect to*/
  int sock_fd, err;
  struct sockaddr_in s1mme_addr;//TODO make this a configurable class memeber.

  sock_fd = socket (AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  if (sock_fd == -1){
    std::cout << "[S1APP] Could not create SCTP socket" <<std::endl; //TODO fix logging
    return -1;
  }

  //S1-MME bind
  bzero(&s1mme_addr, sizeof(s1mme_addr));
  s1mme_addr.sin_family = AF_INET;
  s1mme_addr.sin_addr.s_addr = htonl(INADDR_ANY); //TODO this should use the bindx information
  s1mme_addr.sin_port = htons(S1MME_PORT);
  err = bind(sock_fd, (struct sockaddr*) &s1mme_addr, sizeof (s1mme_addr));
  if (err != 0){
    std::cout << "Error binding SCTP socket" << std::endl;
  }

  //Listen for connections
  err = listen(sock_fd,SOMAXCONN);
  if (err != 0){
    std::cout << "Error in SCTP socket listen" << std::endl;
  }

  return sock_fd;
}

int
s1ap::get_s1_mme()
{
  return m_s1mme;
}

}//namespace srsepc
