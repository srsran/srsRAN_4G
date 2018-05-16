/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include "srsue/hdr/upper/gw.h"

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <sys/socket.h>


namespace srsue {

gw::gw()
  :if_up(false)
{
  current_ip_addr = 0;
  default_netmask = true;
}

void gw::init(pdcp_interface_gw *pdcp_, nas_interface_gw *nas_, srslte::log *gw_log_, srslte::srslte_gw_config_t cfg_)
{
  pool    = srslte::byte_buffer_pool::get_instance();
  pdcp    = pdcp_;
  nas     = nas_;
  gw_log  = gw_log_;
  cfg     = cfg_;
  run_enable = true;

  gettimeofday(&metrics_time[1], NULL);
  dl_tput_bytes = 0;
  ul_tput_bytes = 0;
  // MBSFN
  mbsfn_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (mbsfn_sock_fd < 0) {
    gw_log->error("Failed to create MBSFN sink socket\n");
  }
  if (fcntl(mbsfn_sock_fd, F_SETFL, O_NONBLOCK)) {
    gw_log->error("Failed to set non-blocking MBSFN sink socket\n");
  }

  mbsfn_sock_addr.sin_family      = AF_INET;
  mbsfn_sock_addr.sin_addr.s_addr =inet_addr("127.0.0.1");

  bzero(mbsfn_ports, SRSLTE_N_MCH_LCIDS*sizeof(uint32_t));
}

void gw::stop()
{
  if(run_enable)
  {
    run_enable = false;
    if(if_up)
    {
      close(tun_fd);
      
      // Wait thread to exit gracefully otherwise might leave a mutex locked
      int cnt=0;
      while(running && cnt<100) {
        usleep(10000);
        cnt++;
      }
      if (running) {
        thread_cancel();
      }
      wait_thread_finish();

      current_ip_addr = 0;
    }
    // TODO: tear down TUN device?
  }
  if (mbsfn_sock_fd) {
    close(mbsfn_sock_fd);
  }
}

void gw::get_metrics(gw_metrics_t &m)
{
  gettimeofday(&metrics_time[2], NULL);
  get_time_interval(metrics_time);
  double secs = (double) metrics_time[0].tv_sec+metrics_time[0].tv_usec*1e-6;
  
  m.dl_tput_mbps = (dl_tput_bytes*8/(double)1e6)/secs;
  m.ul_tput_mbps = (ul_tput_bytes*8/(double)1e6)/secs;
  gw_log->info("RX throughput: %4.6f Mbps. TX throughput: %4.6f Mbps.\n",
               m.dl_tput_mbps, m.ul_tput_mbps);

  memcpy(&metrics_time[1], &metrics_time[2], sizeof(struct timeval));
  dl_tput_bytes = 0;
  ul_tput_bytes = 0;
}

void gw::set_netmask(std::string netmask)
{
  default_netmask = false;
  this->netmask = netmask;
}


/*******************************************************************************
  PDCP interface
*******************************************************************************/
void gw::write_pdu(uint32_t lcid, srslte::byte_buffer_t *pdu)
{
  gw_log->info_hex(pdu->msg, pdu->N_bytes, "RX PDU. Stack latency: %ld us\n", pdu->get_latency_us());
  dl_tput_bytes += pdu->N_bytes;
  if(!if_up)
  {
    gw_log->warning("TUN/TAP not up - dropping gw RX message\n");
  }else{
    int n = write(tun_fd, pdu->msg, pdu->N_bytes); 
    if(n > 0 && (pdu->N_bytes != (uint32_t)n))
    {
      gw_log->warning("DL TUN/TAP write failure. Wanted to write %d B but only wrote %d B.\n", pdu->N_bytes, n);
    } 
  }
  pool->deallocate(pdu);
}

void gw::write_pdu_mch(uint32_t lcid, srslte::byte_buffer_t *pdu)
{
  if(pdu->N_bytes>2)
  {
    gw_log->info_hex(pdu->msg, pdu->N_bytes, "RX MCH PDU (%d B). Stack latency: %ld us\n", pdu->N_bytes, pdu->get_latency_us());
    dl_tput_bytes += pdu->N_bytes;

    //Hack to drop initial 2 bytes
    pdu->msg +=2;
    pdu->N_bytes-=2;
    struct in_addr dst_addr;
    memcpy(&dst_addr.s_addr, &pdu->msg[16],4);

    if(!if_up)
    {
      gw_log->warning("TUN/TAP not up - dropping gw RX message\n");
    }else{
      int n = write(tun_fd, pdu->msg, pdu->N_bytes); 
      if(n > 0 && (pdu->N_bytes != (uint32_t)n))
      {
        gw_log->warning("DL TUN/TAP write failure\n");
      }
    }
    /*
    // Strip IP/UDP header
    pdu->msg += 28;
    pdu->N_bytes -= 28;

    if(mbsfn_sock_fd) {
      if(lcid > 0 && lcid < SRSLTE_N_MCH_LCIDS) {
        mbsfn_sock_addr.sin_port = htons(mbsfn_ports[lcid]);
        if(sendto(mbsfn_sock_fd, pdu->msg, pdu->N_bytes, MSG_EOR, (struct sockaddr*)&mbsfn_sock_addr, sizeof(struct sockaddr_in))<0) {
          gw_log->error("Failed to send MCH PDU to port %d\n", mbsfn_ports[lcid]);
        }
      }
    }*/
  }
  pool->deallocate(pdu);
}

/*******************************************************************************
  NAS interface
*******************************************************************************/
srslte::error_t gw::setup_if_addr(uint32_t ip_addr, char *err_str)
{
  if (ip_addr != current_ip_addr) {
    if(!if_up)
    {
      if(init_if(err_str))
      {
        gw_log->error("init_if failed\n");
        return(srslte::ERROR_CANT_START);
      }
    }

    // Setup the IP address
    sock                                                   = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family                                 = AF_INET;
    ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = htonl(ip_addr);
    if(0 > ioctl(sock, SIOCSIFADDR, &ifr))
    {
      err_str = strerror(errno);
      gw_log->debug("Failed to set socket address: %s\n", err_str);
      close(tun_fd);
      return(srslte::ERROR_CANT_START);
    }
    ifr.ifr_netmask.sa_family                                 = AF_INET;
    const char *mask = "255.255.255.0";
    if (!default_netmask) {
      mask = netmask.c_str();
    }
    ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr = inet_addr(mask);
    if(0 > ioctl(sock, SIOCSIFNETMASK, &ifr))
    {
      err_str = strerror(errno);
      gw_log->debug("Failed to set socket netmask: %s\n", err_str);
      close(tun_fd);
      return(srslte::ERROR_CANT_START);
    }

    current_ip_addr = ip_addr;

    // Setup a thread to receive packets from the TUN device
    start(GW_THREAD_PRIO);
  }

  return(srslte::ERROR_NONE);
}

srslte::error_t gw::init_if(char *err_str)
{
  if(if_up)
  {
    return(srslte::ERROR_ALREADY_STARTED);
  }

  char dev[IFNAMSIZ] = "tun_srsue";

  // Construct the TUN device
  tun_fd = open("/dev/net/tun", O_RDWR);
  gw_log->info("TUN file descriptor = %d\n", tun_fd);
  if(0 > tun_fd)
  {
      err_str = strerror(errno);
      gw_log->debug("Failed to open TUN device: %s\n", err_str);
      return(srslte::ERROR_CANT_START);
  }
  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  strncpy(ifr.ifr_ifrn.ifrn_name, dev, IFNAMSIZ-1);
  ifr.ifr_ifrn.ifrn_name[IFNAMSIZ-1] = 0;
  if(0 > ioctl(tun_fd, TUNSETIFF, &ifr))
  {
      err_str = strerror(errno);
      gw_log->debug("Failed to set TUN device name: %s\n", err_str);
      close(tun_fd);
      return(srslte::ERROR_CANT_START);
  }

  // Bring up the interface
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(0 > ioctl(sock, SIOCGIFFLAGS, &ifr))
  {
      err_str = strerror(errno);
      gw_log->debug("Failed to bring up socket: %s\n", err_str);
      close(tun_fd);
      return(srslte::ERROR_CANT_START);
  }
  ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
  if(0 > ioctl(sock, SIOCSIFFLAGS, &ifr))
  {
      err_str = strerror(errno);
      gw_log->debug("Failed to set socket flags: %s\n", err_str);
      close(tun_fd);
      return(srslte::ERROR_CANT_START);
  }

  if_up = true;

  return(srslte::ERROR_NONE);
}


/*******************************************************************************
  RRC interface
*******************************************************************************/
void gw::add_mch_port(uint32_t lcid, uint32_t port)
{
  if(lcid > 0 && lcid < SRSLTE_N_MCH_LCIDS) {
    mbsfn_ports[lcid] = port;
  }
}



/********************/
/*    GW Receive    */
/********************/
void gw::run_thread()
{
  struct iphdr   *ip_pkt;
  uint32          idx = 0;
  int32           N_bytes;
  srslte::byte_buffer_t *pdu = pool_allocate;
  if (!pdu) {
    gw_log->error("Fatal Error: Couldn't allocate PDU in run_thread().\n");
    return;
  }

  const static uint32_t ATTACH_WAIT_TOUT = 40; // 4 sec
  uint32_t attach_wait = 0;

  gw_log->info("GW IP packet receiver thread run_enable\n");

  running = true;
  while(run_enable)
  {
    if (SRSLTE_MAX_BUFFER_SIZE_BYTES-SRSLTE_BUFFER_HEADER_OFFSET > idx) {
      N_bytes = read(tun_fd, &pdu->msg[idx], SRSLTE_MAX_BUFFER_SIZE_BYTES-SRSLTE_BUFFER_HEADER_OFFSET - idx);
    } else {
      gw_log->error("GW pdu buffer full - gw receive thread exiting.\n");
      gw_log->console("GW pdu buffer full - gw receive thread exiting.\n");
      break;
    }
    gw_log->debug("Read %d bytes from TUN fd=%d, idx=%d\n", N_bytes, tun_fd, idx);
    if(N_bytes > 0)
    {
      pdu->N_bytes = idx + N_bytes;
      ip_pkt       = (struct iphdr*)pdu->msg;

      // Warning: Accept only IPv4 packets
      if (ip_pkt->version == 4) {
        // Check if entire packet was received
        if(ntohs(ip_pkt->tot_len) == pdu->N_bytes)
        {
          gw_log->info_hex(pdu->msg, pdu->N_bytes, "TX PDU");

          while(run_enable && !pdcp->is_drb_enabled(cfg.lcid) && attach_wait < ATTACH_WAIT_TOUT) {
            if (!attach_wait) {
              gw_log->info("LCID=%d not active, requesting NAS attach (%d/%d)\n", cfg.lcid, attach_wait, ATTACH_WAIT_TOUT);
              if (!nas->attach_request()) {
                gw_log->warning("Could not re-establish the connection\n");
              }
            }
            usleep(100000);
            attach_wait++;
          }

          attach_wait = 0;

          if (!run_enable) {
            break;
          }

          // Send PDU directly to PDCP
          if (pdcp->is_drb_enabled(cfg.lcid)) {
            pdu->set_timestamp();
            ul_tput_bytes += pdu->N_bytes;
            pdcp->write_sdu(cfg.lcid, pdu);

            do {
              pdu = pool_allocate;
              if (!pdu) {
                gw_log->error("Fatal Error: Couldn't allocate PDU in run_thread().\n");
                usleep(100000);
              }
            } while(!pdu);
            idx = 0;
          }
        }else{
          idx += N_bytes;
        }
      }
    }else{
      gw_log->error("Failed to read from TUN interface - gw receive thread exiting.\n");
      gw_log->console("Failed to read from TUN interface - gw receive thread exiting.\n");
      break;
    }
  }
  running = false;
  gw_log->info("GW IP receiver thread exiting.\n");
}

} // namespace srsue
