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
#include <algorithm>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <netinet/udp.h>
#include <linux/ip.h>
#include "srsepc/hdr/mbms-gw/mbms-gw.h"
#include "srslte/upper/gtpu.h"

namespace srsepc{

mbms_gw*          mbms_gw::m_instance = NULL;
pthread_mutex_t mbms_gw_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

const uint16_t MBMS_GW_BUFFER_SIZE = 2500;

mbms_gw::mbms_gw():
  m_running(false),
  m_sgi_mb_up(false)
{
  return;
}

mbms_gw::~mbms_gw()
{
  return;
}

mbms_gw*
mbms_gw::get_instance(void)
{
  pthread_mutex_lock(&mbms_gw_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new mbms_gw();
  }
  pthread_mutex_unlock(&mbms_gw_instance_mutex);
  return(m_instance);
}

void
mbms_gw::cleanup(void)
{
  pthread_mutex_lock(&mbms_gw_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&mbms_gw_instance_mutex);
}

int
mbms_gw::init(mbms_gw_args_t* args, srslte::log_filter *mbms_gw_log)
{
  srslte::error_t err;
  m_pool = srslte::byte_buffer_pool::get_instance();

  //Init log
  m_mbms_gw_log = mbms_gw_log;

  err = init_sgi_mb_if(args);
  if (err != srslte::ERROR_NONE)
  {
    m_mbms_gw_log->console("Error initializing SGi-MB.\n");
    m_mbms_gw_log->error("Error initializing SGi-MB.\n");
    return -1;
  }
  err = init_m1_u(args);
  if (err != srslte::ERROR_NONE)
  {
    m_mbms_gw_log->console("Error initializing SGi-MB.\n");
    m_mbms_gw_log->error("Error initializing SGi-MB.\n");
    return -1;
  }
  m_mbms_gw_log->info("MBMS GW Initiated\n");
  m_mbms_gw_log->console("MBMS GW Initiated\n");
  return 0;
}

void
mbms_gw::stop()
{
  if(m_running)
  {
    if(m_sgi_mb_up)
    {
      close(m_sgi_mb_if);
      m_mbms_gw_log->info("Closed SGi-MB interface\n");
    }
    m_running = false;
    thread_cancel();
    wait_thread_finish();
  }
  return;
}

srslte::error_t
mbms_gw::init_sgi_mb_if(mbms_gw_args_t *args)
{
  char dev[IFNAMSIZ] = "sgi_mb";
  struct ifreq ifr;

  if(m_sgi_mb_up)
  {
    return(srslte::ERROR_ALREADY_STARTED);
  }


  // Construct the TUN device
  m_sgi_mb_if = open("/dev/net/tun", O_RDWR);
  m_mbms_gw_log->info("TUN file descriptor = %d\n", m_sgi_mb_if);
  if(m_sgi_mb_if < 0)
  {
      m_mbms_gw_log->error("Failed to open TUN device: %s\n", strerror(errno));
      return(srslte::ERROR_CANT_START);
  }

  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  strncpy(ifr.ifr_ifrn.ifrn_name, dev, IFNAMSIZ-1);
  ifr.ifr_ifrn.ifrn_name[IFNAMSIZ-1]='\0';

  if(ioctl(m_sgi_mb_if, TUNSETIFF, &ifr) < 0)
  {
      m_mbms_gw_log->error("Failed to set TUN device name: %s\n", strerror(errno));
      close(m_sgi_mb_if);
      return(srslte::ERROR_CANT_START);
  }

  // Bring up the interface
  int sgi_mb_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sgi_mb_sock<0)
  {
    m_mbms_gw_log->error("Failed to bring up socket: %s\n", strerror(errno));
    close(m_sgi_mb_if);
    return(srslte::ERROR_CANT_START);
  }

  if(ioctl(sgi_mb_sock, SIOCGIFFLAGS, &ifr) < 0)
  {
      m_mbms_gw_log->error("Failed to bring up interface: %s\n", strerror(errno));
      close(m_sgi_mb_if);
      close(sgi_mb_sock);
      return(srslte::ERROR_CANT_START);
  }

  ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
  if(ioctl(sgi_mb_sock, SIOCSIFFLAGS, &ifr) < 0)
  {
      m_mbms_gw_log->error("Failed to set socket flags: %s\n", strerror(errno));
      close(sgi_mb_sock);
      close(m_sgi_mb_if);
      return(srslte::ERROR_CANT_START);
  }

  //Set IP of the interface
  struct sockaddr_in *addr = (struct sockaddr_in*)&ifr.ifr_addr;
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr(args->sgi_mb_if_addr.c_str());
  addr->sin_port = 0;

  if (ioctl(sgi_mb_sock, SIOCSIFADDR, &ifr) < 0) {
    m_mbms_gw_log->error("Failed to set TUN interface IP. Address: %s, Error: %s\n", args->sgi_mb_if_addr.c_str(), strerror(errno));
    close(m_sgi_mb_if);
    close(sgi_mb_sock);
    return srslte::ERROR_CANT_START;
  }

  ifr.ifr_netmask.sa_family                                 = AF_INET;
  ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr = inet_addr(args->sgi_mb_if_mask.c_str());
  if (ioctl(sgi_mb_sock, SIOCSIFNETMASK, &ifr) < 0) {
    m_mbms_gw_log->error("Failed to set TUN interface Netmask. Error: %s\n", strerror(errno));
    close(m_sgi_mb_if);
    close(sgi_mb_sock);
    return srslte::ERROR_CANT_START;
  }

  m_sgi_mb_up = true;
  close(sgi_mb_sock);
  return(srslte::ERROR_NONE);
}

srslte::error_t
mbms_gw::init_m1_u(mbms_gw_args_t *args)
{
  int addrlen;
  struct sockaddr_in addr;
  m_m1u = socket(AF_INET, SOCK_DGRAM, 0);
  if(m_m1u<0)
  {
    m_mbms_gw_log->error("Failed to open socket: %s\n", strerror(errno));
    return srslte::ERROR_CANT_START;
  }
  m_m1u_up = true;

  /* set no loopback */
  char loopch = 0;
  if(setsockopt(m_m1u, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&loopch, sizeof(char))<0){
    m_mbms_gw_log->error("Failed to disable loopback: %s\n", strerror(errno));
    return srslte::ERROR_CANT_START;
  } else {
    m_mbms_gw_log->debug("Loopback disabled\n");
  }

  /* Set local interface for outbound multicast packets*/
  /* The IP must be associated with a local multicast capable interface */
  struct in_addr local_if;
  local_if.s_addr = inet_addr("127.0.1.200");
  if(setsockopt(m_m1u, IPPROTO_IP, IP_MULTICAST_IF, (char*)&local_if, sizeof(struct in_addr))<0){
    perror("Error setting multicast interface.\n");
  } else {
    printf("Multicast interface specified.\n");
  }

  bzero(&m_m1u_multi_addr,sizeof(m_m1u_multi_addr));
  m_m1u_multi_addr.sin_family = AF_INET;
  m_m1u_multi_addr.sin_port = htons(GTPU_RX_PORT+1);
  m_m1u_multi_addr.sin_addr.s_addr = inet_addr(args->m1u_multi_addr.c_str());
  m_mbms_gw_log->info("Initialized M1-U\n");

  return srslte::ERROR_NONE;
}

void
mbms_gw::run_thread()
{
  //Mark the thread as running
  m_running=true;
  srslte::byte_buffer_t *msg;
  msg = m_pool->allocate();


  uint8_t seq = 0;
  while(m_running)
  {
    msg->reset();
    int n;
    do{
      n = read(m_sgi_mb_if, msg->msg, SRSLTE_MAX_BUFFER_SIZE_BYTES);
    }while(n == -1 && errno == EAGAIN);

    if(n<0){
      m_mbms_gw_log->error("Error reading from TUN interface. Error: %s\n", strerror(errno));
    }
    else{
      msg->N_bytes = n;
      handle_sgi_md_pdu(msg);
    }
  }
  m_pool->deallocate(msg);
  return;
}

void
mbms_gw::handle_sgi_md_pdu(srslte::byte_buffer_t *msg)
{
  uint8_t version;
  srslte::gtpu_header_t header;

  //Setup GTP-U header
  header.flags        = 0x30;
  header.message_type = 0xFF;
  header.length       = msg->N_bytes;
  header.teid         = 0xAAAA; //FIXME Harcoded TEID for now

  //Sanity Check IP packet
  if(msg->N_bytes < 20)
  {
    m_mbms_gw_log->error("IPv4 min len: %d, drop msg len %d\n", 20, msg->N_bytes);
    return;
  }

  //IP Headers
  struct iphdr *iph = (struct iphdr *) msg->msg;
  if(iph->version != 4)
  {
    m_mbms_gw_log->warning("IPv6 not supported yet.\n");
    return;
  }

  //Write GTP-U header into packet
  if(!srslte::gtpu_write_header(&header, msg, m_mbms_gw_log))
  {
    m_mbms_gw_log->console("Error writing GTP-U header on PDU\n");
  }

  int n = sendto(m_m1u, msg->msg, msg->N_bytes, 0,
                 (sockaddr *) &m_m1u_multi_addr, sizeof(struct sockaddr));
  if(n<0){
    m_mbms_gw_log->console("Error writting to M1-U socket.\n");
  }
  else{
    m_mbms_gw_log->debug("Sent %d Bytes\n", msg->N_bytes);
  }
}

} //namespace srsepc
