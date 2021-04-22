/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/upper/ipv6.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <linux/ip.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace srsue {

gw::gw() : thread("GW"), logger(srslog::fetch_basic_logger("GW", false)), tft_matcher(logger) {}

int gw::init(const gw_args_t& args_, stack_interface_gw* stack_)
{
  stack      = stack_;
  args       = args_;
  run_enable = true;

  logger.set_level(srslog::str_to_basic_level(args.log.gw_level));
  logger.set_hex_dump_max_size(args.log.gw_hex_limit);

  metrics_tp = std::chrono::high_resolution_clock::now();

  // MBSFN
  mbsfn_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (mbsfn_sock_fd < 0) {
    logger.error("Failed to create MBSFN sink socket");
    return SRSRAN_ERROR;
  }
  if (fcntl(mbsfn_sock_fd, F_SETFL, O_NONBLOCK)) {
    logger.error("Failed to set non-blocking MBSFN sink socket");
    return SRSRAN_ERROR;
  }

  mbsfn_sock_addr.sin_family      = AF_INET;
  mbsfn_sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  return SRSRAN_SUCCESS;
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

void gw::get_metrics(gw_metrics_t& m, const uint32_t nof_tti)
{
  std::chrono::duration<double> secs = std::chrono::high_resolution_clock::now() - metrics_tp;

  double dl_tput_mbps_real_time = (dl_tput_bytes * 8 / (double)1e6) / secs.count();
  double ul_tput_mbps_real_time = (ul_tput_bytes * 8 / (double)1e6) / secs.count();

  // Use the provided TTI counter to compute rate for metrics interface
  m.dl_tput_mbps = (nof_tti > 0) ? ((dl_tput_bytes * 8 / (double)1e6) / (nof_tti / 1000.0)) : 0.0;
  m.ul_tput_mbps = (nof_tti > 0) ? ((ul_tput_bytes * 8 / (double)1e6) / (nof_tti / 1000.0)) : 0.0;

  logger.info("gw_rx_rate_mbps=%4.2f (real=%4.2f), gw_tx_rate_mbps=%4.2f (real=%4.2f)",
              m.dl_tput_mbps,
              dl_tput_mbps_real_time,
              m.ul_tput_mbps,
              ul_tput_mbps_real_time);

  // reset counters and store time
  metrics_tp    = std::chrono::high_resolution_clock::now();
  dl_tput_bytes = 0;
  ul_tput_bytes = 0;
}

/*******************************************************************************
  PDCP interface
*******************************************************************************/
void gw::write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "RX PDU. Stack latency: %ld us", pdu->get_latency_us().count());
  dl_tput_bytes += pdu->N_bytes;
  if (!if_up) {
    logger.warning("TUN/TAP not up - dropping gw RX message");
  } else if (pdu->N_bytes < 20) {
    // Packet not large enough to hold IPv4 Header
    logger.warning("Packet to small to hold IPv4 header. Dropping packet with %d B", pdu->N_bytes);
  } else {
    // Only handle IPv4 and IPv6 packets
    struct iphdr* ip_pkt = (struct iphdr*)pdu->msg;
    if (ip_pkt->version == 4 || ip_pkt->version == 6) {
      int n = write(tun_fd, pdu->msg, pdu->N_bytes);
      if (n > 0 && (pdu->N_bytes != (uint32_t)n)) {
        logger.warning("DL TUN/TAP write failure. Wanted to write %d B but only wrote %d B.", pdu->N_bytes, n);
      }
    } else {
      logger.error("Unsupported IP version. Dropping packet with %d B", pdu->N_bytes);
    }
  }
}

void gw::write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  if (pdu->N_bytes > 2) {
    logger.info(pdu->msg,
                pdu->N_bytes,
                "RX MCH PDU (%d B). Stack latency: %ld us",
                pdu->N_bytes,
                pdu->get_latency_us().count());
    dl_tput_bytes += pdu->N_bytes;

    // Hack to drop initial 2 bytes
    pdu->msg += 2;
    pdu->N_bytes -= 2;
    struct in_addr dst_addr;
    memcpy(&dst_addr.s_addr, &pdu->msg[16], 4);

    if (!if_up) {
      logger.warning("TUN/TAP not up - dropping gw RX message");
    } else {
      int n = write(tun_fd, pdu->msg, pdu->N_bytes);
      if (n > 0 && (pdu->N_bytes != (uint32_t)n)) {
        logger.warning("DL TUN/TAP write failure");
      }
    }
  }
}

/*******************************************************************************
  NAS interface
*******************************************************************************/
int gw::setup_if_addr(uint32_t eps_bearer_id,
                      uint32_t lcid,
                      uint8_t  pdn_type,
                      uint32_t ip_addr,
                      uint8_t* ipv6_if_addr,
                      char*    err_str)
{
  int err;
  if (pdn_type == LIBLTE_MME_PDN_TYPE_IPV4 || pdn_type == LIBLTE_MME_PDN_TYPE_IPV4V6) {
    err = setup_if_addr4(ip_addr, err_str);
    if (err != SRSRAN_SUCCESS) {
      return err;
    }
  }
  if (pdn_type == LIBLTE_MME_PDN_TYPE_IPV6 || pdn_type == LIBLTE_MME_PDN_TYPE_IPV4V6) {
    err = setup_if_addr6(ipv6_if_addr, err_str);
    if (err != SRSRAN_SUCCESS) {
      return err;
    }
  }

  eps_lcid[eps_bearer_id] = lcid;
  default_lcid            = lcid;
  tft_matcher.set_default_lcid(lcid);

  // Setup a thread to receive packets from the TUN device
  start(GW_THREAD_PRIO);
  return SRSRAN_SUCCESS;
}


bool gw::is_running()
{
  return running;
}

int gw::update_lcid(uint32_t eps_bearer_id, uint32_t new_lcid)
{
  auto it = eps_lcid.find(eps_bearer_id);
  if (it != eps_lcid.end()) {
    uint32_t old_lcid = eps_lcid[eps_bearer_id];
    logger.debug("Found EPS bearer %d. Update old lcid %d to new lcid %d", eps_bearer_id, old_lcid, new_lcid);
    eps_lcid[eps_bearer_id] = new_lcid;
    if (old_lcid == default_lcid) {
      logger.debug("Defaulting new lcid %d", new_lcid);
      default_lcid = new_lcid;
      tft_matcher.set_default_lcid(new_lcid);
    }
    // TODO: update need filters if not the default lcid
  } else {
    logger.error("Did not found EPS bearer %d for updating LCID.", eps_bearer_id);
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

int gw::apply_traffic_flow_template(const uint8_t&                                 erab_id,
                                    const uint8_t&                                 lcid,
                                    const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft)
{
  return tft_matcher.apply_traffic_flow_template(erab_id, lcid, tft);
}

void gw::set_test_loop_mode(const test_loop_mode_state_t mode, const uint32_t ip_pdu_delay_ms)
{
  logger.error("UE test loop mode not supported");
}

/*******************************************************************************
  RRC interface
*******************************************************************************/
void gw::add_mch_port(uint32_t lcid, uint32_t port)
{
  if (lcid > 0 && lcid < SRSRAN_N_MCH_LCIDS) {
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

  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  const static uint32_t REGISTER_WAIT_TOUT = 40, SERVICE_WAIT_TOUT = 40; // 4 sec
  uint32_t              register_wait = 0, service_wait = 0;

  logger.info("GW IP packet receiver thread run_enable");

  running = true;
  while (run_enable) {
    // Read packet from TUN
    if (SRSRAN_MAX_BUFFER_SIZE_BYTES - SRSRAN_BUFFER_HEADER_OFFSET > idx) {
      N_bytes = read(tun_fd, &pdu->msg[idx], SRSRAN_MAX_BUFFER_SIZE_BYTES - SRSRAN_BUFFER_HEADER_OFFSET - idx);
    } else {
      logger.error("GW pdu buffer full - gw receive thread exiting.");
      srsran::console("GW pdu buffer full - gw receive thread exiting.\n");
      break;
    }
    logger.debug("Read %d bytes from TUN fd=%d, idx=%d", N_bytes, tun_fd, idx);

    if (N_bytes <= 0) {
      logger.error("Failed to read from TUN interface - gw receive thread exiting.");
      srsran::console("Failed to read from TUN interface - gw receive thread exiting.\n");
      break;
    }

    // Check if IP version makes sense and get packtet length
    struct iphdr*   ip_pkt  = (struct iphdr*)pdu->msg;
    struct ipv6hdr* ip6_pkt = (struct ipv6hdr*)pdu->msg;
    uint16_t        pkt_len = 0;
    pdu->N_bytes            = idx + N_bytes;
    if (ip_pkt->version == 4) {
      pkt_len = ntohs(ip_pkt->tot_len);
    } else if (ip_pkt->version == 6) {
      pkt_len = ntohs(ip6_pkt->payload_len) + 40;
    } else {
      logger.error(pdu->msg, pdu->N_bytes, "Unsupported IP version. Dropping packet.");
      continue;
    }
    logger.debug("IPv%d packet total length: %d Bytes", int(ip_pkt->version), pkt_len);

    // Check if entire packet was received
    if (pkt_len == pdu->N_bytes) {
      logger.info(pdu->msg, pdu->N_bytes, "TX PDU");

      // Make sure UE is attached
      while (run_enable && !stack->is_registered() && register_wait < REGISTER_WAIT_TOUT) {
        if (!register_wait) {
          logger.info("UE is not attached, waiting for NAS attach (%d/%d)", register_wait, REGISTER_WAIT_TOUT);
        }
        usleep(100000);
        register_wait++;
      }
      register_wait = 0;

      // If we are still not attached by this stage, drop packet
      if (run_enable && !stack->is_registered()) {
        continue;
      }

      // Wait for service request if necessary
      while (run_enable && !stack->is_lcid_enabled(default_lcid) && service_wait < SERVICE_WAIT_TOUT) {
        if (!service_wait) {
          logger.info(
              "UE does not have service, waiting for NAS service request (%d/%d)", service_wait, SERVICE_WAIT_TOUT);
          stack->start_service_request();
        }
        usleep(100000);
        service_wait++;
      }
      service_wait = 0;

      // Quit before writing packet if necessary
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
          pdu = srsran::make_byte_buffer();
          if (!pdu) {
            logger.error("Fatal Error: Couldn't allocate PDU in run_thread().");
            usleep(100000);
          }
        } while (!pdu);
        idx = 0;
      }
    } else {
      idx += N_bytes;
      logger.debug("Entire packet not read from socket. Total Length %d, N_Bytes %d.", ip_pkt->tot_len, pdu->N_bytes);
    }
  }
  running = false;
  logger.info("GW IP receiver thread exiting.");
}



/**************************/
/* TUN Interface Helpers  */
/**************************/
int gw::init_if(char* err_str)
{
  if (if_up) {
    return SRSRAN_ERROR_ALREADY_STARTED;
  }

  // change into netns
  if (!args.netns.empty()) {
    std::string netns("/run/netns/");
    netns += args.netns;
    netns_fd = open(netns.c_str(), O_RDONLY);
    if (netns_fd == -1) {
      err_str = strerror(errno);
      logger.error("Failed to find netns %s (%s): %s", args.netns.c_str(), netns.c_str(), err_str);
      return SRSRAN_ERROR_CANT_START;
    }
    if (setns(netns_fd, CLONE_NEWNET) == -1) {
      err_str = strerror(errno);
      logger.error("Failed to change netns: %s", err_str);
      return SRSRAN_ERROR_CANT_START;
    }
  }

  // Construct the TUN device
  tun_fd = open("/dev/net/tun", O_RDWR);
  logger.info("TUN file descriptor = %d", tun_fd);
  if (0 > tun_fd) {
    err_str = strerror(errno);
    logger.error("Failed to open TUN device: %s", err_str);
    return SRSRAN_ERROR_CANT_START;
  }

  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  strncpy(
      ifr.ifr_ifrn.ifrn_name, args.tun_dev_name.c_str(), std::min(args.tun_dev_name.length(), (size_t)(IFNAMSIZ - 1)));
  ifr.ifr_ifrn.ifrn_name[IFNAMSIZ - 1] = 0;
  if (0 > ioctl(tun_fd, TUNSETIFF, &ifr)) {
    err_str = strerror(errno);
    logger.error("Failed to set TUN device name: %s", err_str);
    close(tun_fd);
    return SRSRAN_ERROR_CANT_START;
  }

  // Bring up the interface
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (0 > ioctl(sock, SIOCGIFFLAGS, &ifr)) {
    err_str = strerror(errno);
    logger.error("Failed to bring up socket: %s", err_str);
    close(tun_fd);
    return SRSRAN_ERROR_CANT_START;
  }
  ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
  if (0 > ioctl(sock, SIOCSIFFLAGS, &ifr)) {
    err_str = strerror(errno);
    logger.error("Failed to set socket flags: %s", err_str);
    close(tun_fd);
    return SRSRAN_ERROR_CANT_START;
  }

  // Delete link-local IPv6 address.
  struct in6_addr in6p;
  char            addr_str[INET6_ADDRSTRLEN];
  if (find_ipv6_addr(&in6p)) {
    logger.debug("Found link-local IPv6 address: %s", inet_ntop(AF_INET6, &in6p, addr_str, INET6_ADDRSTRLEN));
    del_ipv6_addr(&in6p);
  } else {
    logger.warning("Could not find link-local IPv6 address.");
  }
  if_up = true;

  return SRSRAN_SUCCESS;
}

int gw::setup_if_addr4(uint32_t ip_addr, char* err_str)
{
  if (ip_addr != current_ip_addr) {
    if (!if_up) {
      if (init_if(err_str)) {
        logger.error("init_if failed");
        return SRSRAN_ERROR_CANT_START;
      }
    }

    // Setup the IP address
    sock                                                  = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family                                = AF_INET;
    ((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr.s_addr = htonl(ip_addr);
    if (0 > ioctl(sock, SIOCSIFADDR, &ifr)) {
      err_str = strerror(errno);
      logger.debug("Failed to set socket address: %s", err_str);
      close(tun_fd);
      return SRSRAN_ERROR_CANT_START;
    }
    ifr.ifr_netmask.sa_family                                = AF_INET;
    ((struct sockaddr_in*)&ifr.ifr_netmask)->sin_addr.s_addr = inet_addr(args.tun_dev_netmask.c_str());
    if (0 > ioctl(sock, SIOCSIFNETMASK, &ifr)) {
      err_str = strerror(errno);
      logger.debug("Failed to set socket netmask: %s", err_str);
      close(tun_fd);
      return SRSRAN_ERROR_CANT_START;
    }
    current_ip_addr = ip_addr;
  }
  return SRSRAN_SUCCESS;
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
        logger.error("init_if failed");
        return SRSRAN_ERROR_CANT_START;
      }
    }

    // Setup the IP address
    sock                   = socket(AF_INET6, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET6;

    if (inet_pton(AF_INET6, "fe80::", (void*)&sai.sin6_addr) <= 0) {
      logger.error("Bad address");
      return SRSRAN_ERROR_CANT_START;
    }

    memcpy(&sai.sin6_addr.s6_addr[8], ipv6_if_id, 8);
    if (ioctl(sock, SIOGIFINDEX, &ifr) < 0) {
      perror("SIOGIFINDEX");
      return SRSRAN_ERROR_CANT_START;
    }
    ifr6.ifr6_ifindex   = ifr.ifr_ifindex;
    ifr6.ifr6_prefixlen = 64;
    memcpy((char*)&ifr6.ifr6_addr, (char*)&sai.sin6_addr, sizeof(struct in6_addr));

    if (ioctl(sock, SIOCSIFADDR, &ifr6) < 0) {
      err_str = strerror(errno);
      logger.error("Could not set IPv6 Link local address. Error %s", err_str);
      return SRSRAN_ERROR_CANT_START;
    }

    for (int i = 0; i < 8; i++) {
      current_if_id[i] = ipv6_if_id[i];
    }
  }

  return SRSRAN_SUCCESS;
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

  logger.debug("Trying to obtain IPv6 addr of %s interface", args.tun_dev_name.c_str());

  // Get Interface Index
  if_index = if_nametoindex(args.tun_dev_name.c_str());
  if (if_index == 0) {
    logger.error("Could not find interface index");
    goto err_out;
  }

  // Open NETLINK socket
  fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
  if (fd < 0) {
    logger.error("Error openning NETLINK socket -- %s", strerror(errno));
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
    logger.error("Error sending NETLINK message to kernel -- %s", strerror(errno));
    goto err_out;
  }

  n = recv(fd, buf, sizeof(buf), 0);
  if (n < 0) {
    logger.error("Error receiving from NETLINK socket");
    goto err_out;
  }

  if (n == 0) {
    logger.error("Nothing received from NETLINK Socket");
    goto err_out;
  }

  // Parse the reply
  for (nlmp = (struct nlmsghdr*)buf; NLMSG_OK(nlmp, n); nlmp = NLMSG_NEXT(nlmp, n)) {
    // Chack NL message type
    if (nlmp->nlmsg_type == NLMSG_DONE) {
      logger.error("Reach end of NETLINK message without finding IPv6 address.");
      goto err_out;
    }
    if (nlmp->nlmsg_type == NLMSG_ERROR) {
      logger.error("NLMSG_ERROR in NETLINK reply");
      goto err_out;
    }
    logger.debug("NETLINK message type %d", nlmp->nlmsg_type);

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
    logger.error("Could not find interface index");
    goto out;
  }

  // Open netlink socket
  fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
  if (fd < 0) {
    logger.error("Error openning NETLINK socket -- %s", strerror(errno));
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
    logger.error("Error sending NETLINK message");
    goto out;
  }

out:
  if (fd >= 0) {
    close(fd);
  }
}
} // namespace srsue
