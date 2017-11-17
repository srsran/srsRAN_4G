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
#include <boost/thread/mutex.hpp>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include "spgw/spgw.h"

namespace srsepc{

spgw*          spgw::m_instance = NULL;
boost::mutex  spgw_instance_mutex;

spgw::spgw():
  m_running(false)
{
  m_pool = srslte::byte_buffer_pool::get_instance();     
  return;
}

spgw::~spgw()
{
  return;
}

spgw*
spgw::get_instance(void)
{
  boost::mutex::scoped_lock lock(spgw_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new spgw();
  }
  return(m_instance);
}

void
spgw::cleanup(void)
{
  boost::mutex::scoped_lock lock(spgw_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}

int
spgw::init(spgw_args_t* args, srslte::log_filter *spgw_log)
{
  //Init log
  m_spgw_log = spgw_log;
    
  //Init Si interface
  init_sgi_if();
  m_spgw_log->info("SP-GW Initialized.\n");
  m_spgw_log->console("SP-GW Initialized.\n");
  return 0;
}

void
spgw::stop()
{
  if(m_running)
  {
    m_running = false;
    thread_cancel();
    wait_thread_finish();
  }
  return;
}

void
spgw::run_thread()
{
  //Mark the thread as running
  m_running=true;
  while (m_running)
  {
    sleep(1);
  }
  return;
}

srslte::error_t
spgw::init_sgi_if()
{
  char dev[IFNAMSIZ] = "srs_spgw_sgi";
  struct ifreq ifr;

  // Construct the TUN device
  m_sgi_if = open("/dev/net/tun", O_RDWR);
  m_spgw_log->info("TUN file descriptor = %d\n", m_sgi_if);
  if(m_sgi_if < 0)
  {
      m_spgw_log->debug("Failed to open TUN device: %s\n", strerror(errno));
      return(srslte::ERROR_CANT_START);
  }
  
  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI; 
  strncpy(ifr.ifr_ifrn.ifrn_name, dev, IFNAMSIZ);
  if(ioctl(m_sgi_if, TUNSETIFF, &ifr) < 0)
  {
      m_spgw_log->debug("Failed to set TUN device name: %s\n", strerror(errno));
      close(m_sgi_if);
      return(srslte::ERROR_CANT_START);
  }

  // Bring up the interface
  m_sgi_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(ioctl(m_sgi_sock, SIOCGIFFLAGS, &ifr) < 0)
  {
      m_spgw_log->debug("Failed to bring up socket: %s\n", strerror(errno));
      close(m_sgi_if);
      return(srslte::ERROR_CANT_START);
  }
  ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
  if(ioctl(m_sgi_sock, SIOCSIFFLAGS, &ifr) < 0)
  {
      m_spgw_log->debug("Failed to set socket flags: %s\n", strerror(errno));
      close(m_sgi_if);
      return(srslte::ERROR_CANT_START);
  }
  
  //if_up = true;
  
  return(srslte::ERROR_NONE);

}

} //namespace srsepc
