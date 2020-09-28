/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include "srsue/hdr/stack/upper/gw.h"
#include "srslte/upper/ipv6.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <linux/ip.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace srsue {

gw::gw() : thread("GW"), pool(srslte::byte_buffer_pool::get_instance()), tft_matcher(&log) {}

int gw::init(const gw_args_t& args_, srslte::logger* logger_, stack_interface_gw* stack_)
{
  stack      = stack_;
  logger     = logger_;
  args       = args_;
  run_enable = true;

  log.init("GW  ", logger);
  log.set_level(args.log.gw_level);
  log.set_hex_limit(args.log.gw_hex_limit);

  gettimeofday(&metrics_time[1], NULL);

  // MBSFN
  mbsfn_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (mbsfn_sock_fd < 0) {
    log.error("Failed to create MBSFN sink socket\n");
    return SRSLTE_ERROR;
  }
  if (fcntl(mbsfn_sock_fd, F_SETFL, O_NONBLOCK)) {
    log.error("Failed to set non-blocking MBSFN sink socket\n");
    return SRSLTE_ERROR;
  }

  mbsfn_sock_addr.sin_family      = AF_INET;
  mbsfn_sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  return SRSLTE_SUCCESS;
}

void gw::stop()
{
  if (run_enable) {
    run_enable = false;
    if (if_up) {
      close(tun_fd);

      // Wait thread to exit gracefully otherwise might leave a mutex locked
      int cnt = 0;
      while (running && cnt < 100) {
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

void gw::get_metrics(gw_metrics_t& m)
{
  gettimeofday(&metrics_time[2], NULL);
  get_time_interval(metrics_time);
  double secs = (double)metrics_time[0].tv_sec + metrics_time[0].tv_usec * 1e-6;

  m.dl_tput_mbps = (dl_tput_bytes * 8 / (double)1e6) / secs;
  m.ul_tput_mbps = (ul_tput_bytes * 8 / (double)1e6) / secs;
  log.info("RX throughput: %4.6f Mbps. TX throughput: %4.6f Mbps.\n", m.dl_tput_mbps, m.ul_tput_mbps);

  memcpy(&metrics_time[1], &metrics_time[2], sizeof(struct timeval));
  dl_tput_bytes = 0;
  ul_tput_bytes = 0;
}

/*******************************************************************************
  PDCP interface
*******************************************************************************/
void gw::write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  log.info_hex(pdu->msg, pdu->N_bytes, "RX PDU. Stack latency: %ld us\n", pdu->get_latency_us());
  dl_tput_bytes += pdu->N_bytes;
  if (!if_up) {
    log.warning("TUN/TAP not up - dropping gw RX message\n");
  } else if (pdu->N_bytes < 20) {
    // Packet not large enough to hold IPv4 Header
    log.warning("Packet to small to hold IPv4 header. Dropping packet with %d B\n", pdu->N_bytes);
  } else {
    // Only handle IPv4 and IPv6 packets
    struct iphdr* ip_pkt = (struct iphdr*)pdu->msg;
    if (ip_pkt->version == 4 || ip_pkt->version == 6) {
      int n = write(tun_fd, pdu->msg, pdu->N_bytes);
      if (n > 0 && (pdu->N_bytes != (uint32_t)n)) {
        log.warning("DL TUN/TAP write failure. Wanted to write %d B but only wrote %d B.\n", pdu->N_bytes, n);
      }
    } else {
      log.error("Unsupported IP version. Dropping packet with %d B\n", pdu->N_bytes);
    }
  }
}

void gw::write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  if (pdu->N_bytes > 2) {
    log.info_hex(
        pdu->msg, pdu->N_bytes, "RX MCH PDU (%d B). Stack latency: %ld us\n", pdu->N_bytes, pdu->get_latency_us());
    dl_tput_bytes += pdu->N_bytes;

    // Hack to drop initial 2 bytes
    pdu->msg += 2;
    pdu->N_bytes -= 2;
    struct in_addr dst_addr;
    memcpy(&dst_addr.s_addr, &pdu->msg[16], 4);

    if (!if_up) {
      log.warning("TUN/TAP not up - dropping gw RX message\n");
    } else {
      int n = write(tun_fd, pdu->msg, pdu->N_bytes);
      if (n > 0 && (pdu->N_bytes != (uint32_t)n)) {
        log.warning("DL TUN/TAP write failure\n");
      }
    }
  }
}

/*******************************************************************************
  NAS interface
*******************************************************************************/
int gw::setup_if_addr(uint32_t lcid, uint8_t pdn_type, uint32_t ip_addr, uint8_t* ipv6_if_addr, char* err_str)
{
  int err;
  if (pdn_type == LIBLTE_MME_PDN_TYPE_IPV4 || pdn_type == LIBLTE_MME_PDN_TYPE_IPV4V6) {
    err = setup_if_addr4(ip_addr, err_str);
    if (err != SRSLTE_SUCCESS) {
      return err;
    }
  }
  if (pdn_type == LIBLTE_MME_PDN_TYPE_IPV6 || pdn_type == LIBLTE_MME_PDN_TYPE_IPV4V6) {
    err = setup_if_addr6(ipv6_if_addr, err_str);
    if (err != SRSLTE_SUCCESS) {
      return err;
    }
  }

  default_lcid = lcid;
  tft_matcher.set_default_lcid(lcid);

  // Setup a thread to receive packets from the TUN device
  start(GW_THREAD_PRIO);
  return SRSLTE_SUCCESS;
}

int gw::apply_traffic_flow_template(const uint8_t&                                 erab_id,
                                    const uint8_t&                                 lcid,
                                    const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft)
{
  return tft_matcher.apply_traffic_flow_template(erab_id, lcid, tft);
}

void gw::set_test_loop_mode(const test_loop_mode_state_t mode, const uint32_t ip_pdu_delay_ms)
{
  log.error("UE test loop mode not supported\n");
}

/*******************************************************************************
  RRC interface
*******************************************************************************/
void gw::add_mch_port(uint32_t lcid, uint32_t port)
{
  if (lcid > 0 && lcid < SRSLTE_N_MCH_LCIDS) {
    mbsfn_ports[lcid] = port;
  }
}

/********************/
/*    GW Receive    */
/********************/
void gw::run_thread()
{
  uint32 idx     = 0;
  int32  N_bytes = 0;

  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool, true);
  if (!pdu) {
    log.error("Fatal Error: Couldn't allocate PDU in run_thread().\n");
    return;
  }

  const static uint32_t ATTACH_WAIT_TOUT = 40; // 4 sec
  uint32_t              attach_wait      = 0;

  log.info("GW IP packet receiver thread run_enable\n");

  running = true;
  while (run_enable) {
    if (SRSLTE_MAX_BUFFER_SIZE_BYTES - SRSLTE_BUFFER_HEADER_OFFSET > idx) {
      N_bytes = read(tun_fd, &pdu->msg[idx], SRSLTE_MAX_BUFFER_SIZE_BYTES - SRSLTE_BUFFER_HEADER_OFFSET - idx);
    } else {
      log.error("GW pdu buffer full - gw receive thread exiting.\n");
      srslte::console("GW pdu buffer full - gw receive thread exiting.\n");
      break;
    }
    log.debug("Read %d bytes from TUN fd=%d, idx=%d\n", N_bytes, tun_fd, idx);
    if (N_bytes > 0) {
      struct iphdr*   ip_pkt  = (struct iphdr*)pdu->msg;
      struct ipv6hdr* ip6_pkt = (struct ipv6hdr*)pdu->msg;
      uint16_t        pkt_len = 0;
      pdu->N_bytes            = idx + N_bytes;
      if (ip_pkt->version == 4 || ip_pkt->version == 6) {
        if (ip_pkt->version == 4) {
          pkt_len = ntohs(ip_pkt->tot_len);
        } else if (ip_pkt->version == 6) {
          pkt_len = ntohs(ip6_pkt->payload_len) + 40;
        } else {
          log.error_hex(pdu->msg, pdu->N_bytes, "Unsupported IP version. Dropping packet.\n");
          continue;
        }
        log.debug("IPv%d packet total length: %d Bytes\n", ip_pkt->version, pkt_len);
        // Check if entire packet was received
        if (pkt_len == pdu->N_bytes) {
          log.info_hex(pdu->msg, pdu->N_bytes, "TX PDU");

          while (run_enable && !stack->is_lcid_enabled(default_lcid) && attach_wait < ATTACH_WAIT_TOUT) {
            if (!attach_wait) {
              log.info(
                  "LCID=%d not active, requesting NAS attach (%d/%d)\n", default_lcid, attach_wait, ATTACH_WAIT_TOUT);
              if (not stack->switch_on()) {
                log.warning("Could not re-establish the connection\n");
              }
            }
            usleep(100000);
            attach_wait++;
          }

          attach_wait = 0;

          if (!run_enable) {
            break;
          }

          uint8_t lcid = tft_matcher.check_tft_filter_match(pdu);
          // Send PDU directly to PDCP
          if (stack->is_lcid_enabled(lcid)) {
            pdu->set_timestamp();
            ul_tput_bytes += pdu->N_bytes;
            stack->write_sdu(lcid, std::move(pdu));
            do {
              pdu = srslte::allocate_unique_buffer(*pool);
              if (!pdu) {
                log.error("Fatal Error: Couldn't allocate PDU in run_thread().\n");
                usleep(100000);
              }
            } while (!pdu);
            idx = 0;
          }
        } else {
          idx += N_bytes;
          log.debug(
              "Entire packet not read from socket. Total Length %d, N_Bytes %d.\n", ip_pkt->tot_len, pdu->N_bytes);
        }
      } else {
        log.error("IP Version not handled. Version %d\n", ip_pkt->version);
      }
    } else {
      log.error("Failed to read from TUN interface - gw receive thread exiting.\n");
      srslte::console("Failed to read from TUN interface - gw receive thread exiting.\n");
      break;
    }
  }
  running = false;
  log.info("GW IP receiver thread exiting.\n");
}

/**************************/
/* TUN Interface Helpers  */
/**************************/
int gw::init_if(char* err_str)
{
  if (if_up) {
    return SRSLTE_ERROR_ALREADY_STARTED;
  }

  // change into netns
  if (!args.netns.empty()) {
    std::string netns("/run/netns/");
    netns += args.netns;
    netns_fd = open(netns.c_str(), O_RDONLY);
    if (netns_fd == -1) {
      err_str = strerror(errno);
      log.error("Failed to find netns %s (%s): %s\n", args.netns.c_str(), netns.c_str(), err_str);
      return SRSLTE_ERROR_CANT_START;
    }
    if (setns(netns_fd, CLONE_NEWNET) == -1) {
      err_str = strerror(errno);
      log.error("Failed to change netns: %s\n", err_str);
      return SRSLTE_ERROR_CANT_START;
    }
  }

  // Construct the TUN device
  tun_fd = open("/dev/net/tun", O_RDWR);
  log.info("TUN file descriptor = %d\n", tun_fd);
  if (0 > tun_fd) {
    err_str = strerror(errno);
    log.error("Failed to open TUN device: %s\n", err_str);
    return SRSLTE_ERROR_CANT_START;
  }

  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  strncpy(
      ifr.ifr_ifrn.ifrn_name, args.tun_dev_name.c_str(), std::min(args.tun_dev_name.length(), (size_t)(IFNAMSIZ - 1)));
  ifr.ifr_ifrn.ifrn_name[IFNAMSIZ - 1] = 0;
  if (0 > ioctl(tun_fd, TUNSETIFF, &ifr)) {
    err_str = strerror(errno);
    log.error("Failed to set TUN device name: %s\n", err_str);
    close(tun_fd);
    return SRSLTE_ERROR_CANT_START;
  }

  // Bring up the interface
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (0 > ioctl(sock, SIOCGIFFLAGS, &ifr)) {
    err_str = strerror(errno);
    log.error("Failed to bring up socket: %s\n", err_str);
    close(tun_fd);
    return SRSLTE_ERROR_CANT_START;
  }
  ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
  if (0 > ioctl(sock, SIOCSIFFLAGS, &ifr)) {
    err_str = strerror(errno);
    log.error("Failed to set socket flags: %s\n", err_str);
    close(tun_fd);
    return SRSLTE_ERROR_CANT_START;
  }

  // Delete link-local IPv6 address.
  struct in6_addr in6p;
  char            addr_str[INET6_ADDRSTRLEN];
  if (find_ipv6_addr(&in6p)) {
    log.debug("Found link-local IPv6 address: %s\n", inet_ntop(AF_INET6, &in6p, addr_str, INET6_ADDRSTRLEN));
    del_ipv6_addr(&in6p);
  } else {
    log.warning("Could not find link-local IPv6 address.\n");
  }
  if_up = true;

  return SRSLTE_SUCCESS;
}

int gw::setup_if_addr4(uint32_t ip_addr, char* err_str)
{
  if (ip_addr != current_ip_addr) {
    if (!if_up) {
      if (init_if(err_str)) {
        log.error("init_if failed\n");
        return SRSLTE_ERROR_CANT_START;
      }
    }

    // Setup the IP address
    sock                                                  = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family                                = AF_INET;
    ((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr.s_addr = htonl(ip_addr);
    if (0 > ioctl(sock, SIOCSIFADDR, &ifr)) {
      err_str = strerror(errno);
      log.debug("Failed to set socket address: %s\n", err_str);
      close(tun_fd);
      return SRSLTE_ERROR_CANT_START;
    }
    ifr.ifr_netmask.sa_family                                = AF_INET;
    ((struct sockaddr_in*)&ifr.ifr_netmask)->sin_addr.s_addr = inet_addr(args.tun_dev_netmask.c_str());
    if (0 > ioctl(sock, SIOCSIFNETMASK, &ifr)) {
      err_str = strerror(errno);
      log.debug("Failed to set socket netmask: %s\n", err_str);
      close(tun_fd);
      return SRSLTE_ERROR_CANT_START;
    }
    current_ip_addr = ip_addr;
  }
  return SRSLTE_SUCCESS;
}

int gw::setup_if_addr6(uint8_t* ipv6_if_id, char* err_str)
{
  struct sockaddr_in6 sai;
  struct in6_ifreq    ifr6;
  bool                match = true;

  for (int i = 0; i < 8; i++) {
    if (ipv6_if_id[i] != current_if_id[i]) {
      match = false;
      break;
    }
  }

  if (!match) {
    if (!if_up) {
      if (init_if(err_str)) {
        log.error("init_if failed\n");
        return SRSLTE_ERROR_CANT_START;
      }
    }

    // Setup the IP address
    sock                   = socket(AF_INET6, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET6;

    if (inet_pton(AF_INET6, "fe80::", (void*)&sai.sin6_addr) <= 0) {
      log.error("Bad address\n");
      return SRSLTE_ERROR_CANT_START;
    }

    memcpy(&sai.sin6_addr.s6_addr[8], ipv6_if_id, 8);
    if (ioctl(sock, SIOGIFINDEX, &ifr) < 0) {
      perror("SIOGIFINDEX");
      return SRSLTE_ERROR_CANT_START;
    }
    ifr6.ifr6_ifindex   = ifr.ifr_ifindex;
    ifr6.ifr6_prefixlen = 64;
    memcpy((char*)&ifr6.ifr6_addr, (char*)&sai.sin6_addr, sizeof(struct in6_addr));

    if (ioctl(sock, SIOCSIFADDR, &ifr6) < 0) {
      err_str = strerror(errno);
      log.error("Could not set IPv6 Link local address. Error %s\n", err_str);
      return SRSLTE_ERROR_CANT_START;
    }

    for (int i = 0; i < 8; i++) {
      current_if_id[i] = ipv6_if_id[i];
    }
  }

  return SRSLTE_SUCCESS;
}

bool gw::find_ipv6_addr(struct in6_addr* in6_out)
{
  int               n, rtattrlen, fd = -1;
  unsigned int      if_index;
  struct rtattr *   rta, *rtatp;
  struct nlmsghdr*  nlmp;
  struct ifaddrmsg* rtmp;
  struct in6_addr*  in6p;
  char              buf[2048];
  struct {
    struct nlmsghdr  n;
    struct ifaddrmsg r;
    char             buf[1024];
  } req;

  log.debug("Trying to obtain IPv6 addr of %s interface\n", args.tun_dev_name.c_str());

  // Get Interface Index
  if_index = if_nametoindex(args.tun_dev_name.c_str());
  if (if_index == 0) {
    log.error("Could not find interface index\n");
    goto err_out;
  }

  // Open NETLINK socket
  fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
  if (fd < 0) {
    log.error("Error openning NETLINK socket -- %s\n", strerror(errno));
    goto err_out;
  }

  // We use RTM_GETADDR to get the ip address from the kernel
  memset(&req, 0, sizeof(req));
  req.n.nlmsg_len   = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
  req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_MATCH;
  req.n.nlmsg_type  = RTM_GETADDR;

  // AF_INET6 is used to signify the kernel to fetch only ipv6 entires.
  req.r.ifa_family = AF_INET6;

  // Fill up all the attributes for the rtnetlink header.
  // The length is important. 16 signifies we are requesting IPv6 addresses
  rta          = (struct rtattr*)(((char*)&req) + NLMSG_ALIGN(req.n.nlmsg_len));
  rta->rta_len = RTA_LENGTH(16);

  // Time to send and recv the message from kernel
  n = send(fd, &req, req.n.nlmsg_len, 0);
  if (n < 0) {
    log.error("Error sending NETLINK message to kernel -- %s", strerror(errno));
    goto err_out;
  }

  n = recv(fd, buf, sizeof(buf), 0);
  if (n < 0) {
    log.error("Error receiving from NETLINK socket\n");
    goto err_out;
  }

  if (n == 0) {
    log.error("Nothing received from NETLINK Socket\n");
    goto err_out;
  }

  // Parse the reply
  for (nlmp = (struct nlmsghdr*)buf; NLMSG_OK(nlmp, n); nlmp = NLMSG_NEXT(nlmp, n)) {

    // Chack NL message type
    if (nlmp->nlmsg_type == NLMSG_DONE) {
      log.error("Reach end of NETLINK message without finding IPv6 address.\n");
      goto err_out;
    }
    if (nlmp->nlmsg_type == NLMSG_ERROR) {
      log.error("NLMSG_ERROR in NETLINK reply\n");
      goto err_out;
    }
    log.debug("NETLINK message type %d\n", nlmp->nlmsg_type);

    // Get IFA message
    rtmp      = (struct ifaddrmsg*)NLMSG_DATA(nlmp);
    rtatp     = (struct rtattr*)IFA_RTA(rtmp);
    rtattrlen = IFA_PAYLOAD(nlmp);
    for (; RTA_OK(rtatp, rtattrlen); rtatp = RTA_NEXT(rtatp, rtattrlen)) {
      // We are looking IFA_ADDRESS rt_attribute type.
      // For more info on the different types see man(7) rtnetlink.
      if (rtatp->rta_type == IFA_ADDRESS) {
        in6p = (struct in6_addr*)RTA_DATA(rtatp);
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

void gw::del_ipv6_addr(struct in6_addr* in6p)
{
  int          status, fd = -1;
  unsigned int if_index;
  struct {
    struct nlmsghdr  n;
    struct ifaddrmsg ifa;
    char             buf[1024];
  } req;

  // Get Interface Index
  if_index = if_nametoindex(args.tun_dev_name.c_str());
  if (if_index == 0) {
    log.error("Could not find interface index\n");
    goto out;
  }

  // Open netlink socket
  fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
  if (fd < 0) {
    log.error("Error openning NETLINK socket -- %s\n", strerror(errno));
    goto out;
  }

  // We use RTM_DELADDR to delete the ip address from the interface
  memset(&req, 0, sizeof(req));
  req.n.nlmsg_len   = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
  req.n.nlmsg_type  = RTM_DELADDR;
  req.n.nlmsg_flags = NLM_F_REQUEST;

  req.ifa.ifa_family    = AF_INET6;
  req.ifa.ifa_prefixlen = 64;
  req.ifa.ifa_index     = if_index; // set the tun_srsue index
  req.ifa.ifa_scope     = 0;

  // Add RT atribute
  struct rtattr* rta;
  rta           = (struct rtattr*)(((char*)&req.n) + NLMSG_ALIGN(req.n.nlmsg_len));
  rta->rta_type = IFA_LOCAL;
  rta->rta_len  = RTA_LENGTH(16);
  memcpy(RTA_DATA(rta), in6p, 16);
  req.n.nlmsg_len = NLMSG_ALIGN(req.n.nlmsg_len) + rta->rta_len;

  status = send(fd, &req, req.n.nlmsg_len, 0);
  if (status < 0) {
    log.error("Error sending NETLINK message\n");
    goto out;
  }

out:
  if (fd >= 0) {
    close(fd);
  }
}
} // namespace srsue
