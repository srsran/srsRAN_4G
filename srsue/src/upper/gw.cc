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
#include <linux/ipv6.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

namespace srsue {

gw::gw()
  :if_up(false)
{
  current_ip_addr = 0;
  default_netmask = true;
  tundevname = "";
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

void gw::set_tundevname(const std::string & devname)
{
  tundevname = devname;
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

    if (!if_up) {
      gw_log->warning("TUN/TAP not up - dropping gw RX message\n");
    } else {
      int n = write(tun_fd, pdu->msg, pdu->N_bytes); 
      if(n > 0 && (pdu->N_bytes != (uint32_t) n) ) {
        gw_log->warning("DL TUN/TAP write failure\n");
      }
    }
  }
  pool->deallocate(pdu);
}

/*******************************************************************************
  NAS interface
*******************************************************************************/
srslte::error_t gw::setup_if_addr(uint8_t pdn_type, uint32_t ip_addr, uint8_t *ipv6_if_addr, char *err_str)
{
  srslte::error_t err;
  if(pdn_type == LIBLTE_MME_PDN_TYPE_IPV4 || pdn_type == LIBLTE_MME_PDN_TYPE_IPV4V6 ){
    err = setup_if_addr4(ip_addr, err_str);
    if(err!= srslte::ERROR_NONE){
      return err;
    }
  }
  if(pdn_type == LIBLTE_MME_PDN_TYPE_IPV6 || pdn_type == LIBLTE_MME_PDN_TYPE_IPV4V6 ){
    err = setup_if_addr6(ipv6_if_addr, err_str);
    if(err!= srslte::ERROR_NONE){
      return err;
    }
  }
  
  // Setup a thread to receive packets from the TUN device
  start(GW_THREAD_PRIO);
  return srslte::ERROR_NONE;
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
  uint32 idx = 0;
  int32  N_bytes = 0;

  srslte::byte_buffer_t *pdu = pool_allocate_blocking;
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
    if (N_bytes > 0) {
      struct iphdr *ip_pkt = (struct iphdr*)pdu->msg;
      struct ipv6hdr *ip6_pkt = (struct ipv6hdr*)pdu->msg;
      uint16_t pkt_len = 0;
      pdu->N_bytes = idx + N_bytes;
      if (ip_pkt->version == 4 || ip_pkt->version == 6) {
        if (ip_pkt->version == 4){
          pkt_len = ntohs(ip_pkt->tot_len);
        } else if (ip_pkt->version == 6){
          pkt_len = ntohs(ip6_pkt->payload_len)+40;
        } else {
          gw_log->error_hex(pdu->msg, pdu->N_bytes, "Unsupported IP version. Dropping packet.\n");
          continue;
        }
        gw_log->debug("IPv%d packet total length: %d Bytes\n", ip_pkt->version, pkt_len);
        // Check if entire packet was received
        if (pkt_len == pdu->N_bytes) {
          gw_log->info_hex(pdu->msg, pdu->N_bytes, "TX PDU");

          while(run_enable && !pdcp->is_lcid_enabled(cfg.lcid) && attach_wait < ATTACH_WAIT_TOUT) {
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
          if (pdcp->is_lcid_enabled(cfg.lcid)) {
            pdu->set_timestamp();
            ul_tput_bytes += pdu->N_bytes;
            pdcp->write_sdu(cfg.lcid, pdu, false);
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
          gw_log->debug("Entire packet not read from socket. Total Length %d, N_Bytes %d.\n", ip_pkt->tot_len, pdu->N_bytes);
        }
      } else {
        gw_log->error("IP Version not handled. Version %d\n", ip_pkt->version);
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

/**************************/
/* TUN Interface Helpers  */
/**************************/
srslte::error_t gw::init_if(char *err_str)
{
  if (if_up) {
    return (srslte::ERROR_ALREADY_STARTED);
  }

  // Construct the TUN device
  tun_fd = open("/dev/net/tun", O_RDWR);
  gw_log->info("TUN file descriptor = %d\n", tun_fd);
  if (0 > tun_fd) {
    err_str = strerror(errno);
    gw_log->debug("Failed to open TUN device: %s\n", err_str);
    return (srslte::ERROR_CANT_START);
  }

  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  strncpy(ifr.ifr_ifrn.ifrn_name, tundevname.c_str(), std::min(tundevname.length(), (size_t)(IFNAMSIZ-1)));
  ifr.ifr_ifrn.ifrn_name[IFNAMSIZ-1] = 0;
  if (0 > ioctl(tun_fd, TUNSETIFF, &ifr)) {
    err_str = strerror(errno);
    gw_log->debug("Failed to set TUN device name: %s\n", err_str);
    close(tun_fd);
    return (srslte::ERROR_CANT_START);
  }

  // Bring up the interface
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (0 > ioctl(sock, SIOCGIFFLAGS, &ifr)) {
    err_str = strerror(errno);
    gw_log->debug("Failed to bring up socket: %s\n", err_str);
    close(tun_fd);
    return (srslte::ERROR_CANT_START);
  }
  ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
  if (0 > ioctl(sock, SIOCSIFFLAGS, &ifr)) {
    err_str = strerror(errno);
    gw_log->debug("Failed to set socket flags: %s\n", err_str);
    close(tun_fd);
    return (srslte::ERROR_CANT_START);
  }

  // Delete link-local IPv6 address.
  struct in6_addr in6p;
  char addr_str[INET6_ADDRSTRLEN];
  if(find_ipv6_addr(&in6p)){
    gw_log->debug("Found link-local IPv6 address: %s\n",inet_ntop(AF_INET6, &in6p, addr_str,INET6_ADDRSTRLEN) );
    del_ipv6_addr(&in6p);
  } else {
    gw_log->warning("Could not find link-local IPv6 address.\n");
  }
  if_up = true;

  return(srslte::ERROR_NONE);
}

srslte::error_t gw::setup_if_addr4(uint32_t ip_addr, char *err_str)
{
  if (ip_addr != current_ip_addr) {
    if (!if_up) {
      if (init_if(err_str)) {
        gw_log->error("init_if failed\n");
        return (srslte::ERROR_CANT_START);
      }
    }

    // Setup the IP address
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = htonl(ip_addr);
    if (0 > ioctl(sock, SIOCSIFADDR, &ifr)) {
      err_str = strerror(errno);
      gw_log->debug("Failed to set socket address: %s\n", err_str);
      close(tun_fd);
      return (srslte::ERROR_CANT_START);
    }
    ifr.ifr_netmask.sa_family = AF_INET;
    const char *mask = "255.255.255.0";
    if (!default_netmask) {
      mask = netmask.c_str();
    }
    ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr = inet_addr(mask);
    if (0 > ioctl(sock, SIOCSIFNETMASK, &ifr)) {
      err_str = strerror(errno);
      gw_log->debug("Failed to set socket netmask: %s\n", err_str);
      close(tun_fd);
      return (srslte::ERROR_CANT_START);
    }
    current_ip_addr = ip_addr;
  }
  return(srslte::ERROR_NONE);
}

srslte::error_t gw::setup_if_addr6(uint8_t *ipv6_if_id, char *err_str)
{
  struct sockaddr_in6 sai;
  struct in6_ifreq ifr6;
  bool match = true;

  for (int i=0; i<8; i++){
    if(ipv6_if_id[i] != current_if_id[i]){
      match = false;
      break;
    }
  }

  if (!match) {
    if (!if_up) {
      if( init_if(err_str) ) {
        gw_log->error("init_if failed\n");
        return(srslte::ERROR_CANT_START);
      }
    }

    // Setup the IP address
    sock = socket(AF_INET6, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET6;

    if(inet_pton(AF_INET6, "fe80::", (void *)&sai.sin6_addr) <= 0) {
      gw_log->error("Bad address\n");
      return srslte::ERROR_CANT_START;
    }

    memcpy(&sai.sin6_addr.s6_addr[8], ipv6_if_id, 8);
    if (ioctl(sock, SIOGIFINDEX, &ifr) < 0) {
      perror("SIOGIFINDEX");
      return srslte::ERROR_CANT_START;
    }
    ifr6.ifr6_ifindex = ifr.ifr_ifindex;
    ifr6.ifr6_prefixlen = 64;
    memcpy((char *) &ifr6.ifr6_addr, (char *) &sai.sin6_addr,
      sizeof(struct in6_addr));

    if (ioctl(sock, SIOCSIFADDR, &ifr6) < 0) {
      err_str = strerror(errno);
      gw_log->error("Could not set IPv6 Link local address. Error %s\n", err_str);
      return srslte::ERROR_CANT_START;
    }

    for (int i=0; i<8; i++){
      current_if_id[i] = ipv6_if_id[i];
    }
  }

  return(srslte::ERROR_NONE);
}

bool gw::find_ipv6_addr(struct in6_addr *in6_out)
{
  int n, rtattrlen, fd = -1;
  unsigned int if_index;
  struct rtattr *rta, *rtatp;
  struct nlmsghdr *nlmp;
  struct ifaddrmsg *rtmp;
  struct in6_addr *in6p;
  char buf[2048];
  struct {
    struct nlmsghdr n;
    struct ifaddrmsg r;
    char buf[1024];
  } req;

  gw_log->debug("Trying to obtain IPv6 addr of %s interface\n", tundevname.c_str());
  
  //Get Interface Index
  if_index = if_nametoindex(tundevname.c_str());
  if(if_index == 0){
    gw_log->error("Could not find interface index\n");
    goto err_out;
  }

  // Open NETLINK socket
  fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
  if (fd < 0) {
    gw_log->error("Error openning NETLINK socket -- %s\n", strerror(errno));
    goto err_out;
  }

  // We use RTM_GETADDR to get the ip address from the kernel
  memset(&req, 0, sizeof(req));
  req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
  req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_MATCH;
  req.n.nlmsg_type = RTM_GETADDR;

  // AF_INET6 is used to signify the kernel to fetch only ipv6 entires. 
  req.r.ifa_family = AF_INET6;
  
  // Fill up all the attributes for the rtnetlink header. 
  // The lenght is important. 16 signifies we are requesting IPv6 addresses
  rta = (struct rtattr *)(((char *)&req) + NLMSG_ALIGN(req.n.nlmsg_len));
  rta->rta_len = RTA_LENGTH(16);

  // Time to send and recv the message from kernel 
  n = send(fd, &req, req.n.nlmsg_len, 0);
  if (n < 0) {
    gw_log->error("Error sending NETLINK message to kernel -- %s", strerror(errno));
    goto err_out;
  }

  n = recv(fd, buf, sizeof(buf), 0);
  if (n < 0) {
    gw_log->error("Error receiving from NETLINK socket\n");
    goto err_out;
  }

  if (n == 0) {
    gw_log->error("Nothing received from NETLINK Socket\n");
    goto err_out;
  }

  // Parse the reply
  for (nlmp = (struct nlmsghdr *)buf; NLMSG_OK (nlmp, n); nlmp = NLMSG_NEXT (nlmp, n)){
    
    //Chack NL message type
    if (nlmp->nlmsg_type == NLMSG_DONE){ 
      gw_log->error("Reach end of NETLINK message without finding IPv6 address.\n");
      goto err_out;
    }
    if (nlmp->nlmsg_type == NLMSG_ERROR) {
      gw_log->error("NLMSG_ERROR in NETLINK reply\n");
      goto err_out;
    }
    gw_log->debug("NETLINK message type %d\n", nlmp->nlmsg_type);

    //Get IFA message
    rtmp = (struct ifaddrmsg *)NLMSG_DATA(nlmp);
    rtatp = (struct rtattr *)IFA_RTA(rtmp);
    rtattrlen = IFA_PAYLOAD(nlmp);
    for (; RTA_OK(rtatp, rtattrlen); rtatp = RTA_NEXT(rtatp, rtattrlen)) {
      // We are looking IFA_ADDRESS rt_attribute type.
      // For more info on the different types see man(7) rtnetlink.
      if (rtatp->rta_type == IFA_ADDRESS) {
        in6p = (struct in6_addr *)RTA_DATA(rtatp);
        if (if_index == rtmp->ifa_index) {
          for (int i = 0; i < 16; i++) {
            in6_out->s6_addr[i] = in6p->s6_addr[i];
          }
          goto out;
        }
      }
    }
  }

err_out:
  if (fd > 0) {
    close(fd);
  }
  return false;
out:
  close(fd);
  return true;
}

void gw::del_ipv6_addr(struct in6_addr *in6p)
{
  int status, fd =-1;
  unsigned int if_index;
  struct {
    struct nlmsghdr n;
    struct ifaddrmsg ifa;
    char buf[1024];
  } req;

  //Get Interface Index
  if_index = if_nametoindex(tundevname.c_str());
  if(if_index == 0){
    gw_log->error("Could not find interface index\n");
    goto out;
  }

  // Open netlink socket
  fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
  if (fd < 0) {
    gw_log->error("Error openning NETLINK socket -- %s\n", strerror(errno));
    goto out;
  }
  
  // We use RTM_DELADDR to delete the ip address from the interface
  memset(&req, 0, sizeof(req));
  req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
  req.n.nlmsg_type = RTM_DELADDR;
  req.n.nlmsg_flags = NLM_F_REQUEST;

  req.ifa.ifa_family = AF_INET6;
  req.ifa.ifa_prefixlen = 64;
  req.ifa.ifa_index = if_index;  // set the tun_srsue index
  req.ifa.ifa_scope = 0;
  
  //Add RT atribute
  struct rtattr *rta;
  rta = (struct rtattr *)(((char *)&req.n) + NLMSG_ALIGN(req.n.nlmsg_len));
  rta->rta_type = IFA_LOCAL;
  rta->rta_len = RTA_LENGTH(16);
  memcpy(RTA_DATA(rta), in6p, 16);
  req.n.nlmsg_len = NLMSG_ALIGN(req.n.nlmsg_len) + rta->rta_len;

  status = send(fd, &req, req.n.nlmsg_len, 0);
  if (status < 0) {
    gw_log->error("Error sending NETLINK message\n");
    goto out;
  }

out:
  if (fd<0){
    close(fd);
  }
  return;
}
} // namespace srsue
