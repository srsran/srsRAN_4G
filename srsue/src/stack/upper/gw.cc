/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
#include "srsran/common/network_utils.h"
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/upper/ipv6.h"
#include "srsue/hdr/stack/upper/icmpv6.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <linux/ip.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace srsue {

gw::gw(srslog::basic_logger& logger_) : thread("GW"), logger(logger_), tft_matcher(logger) {}

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

  mbsfn_sock_addr.sin_family = AF_INET;
  if (inet_pton(mbsfn_sock_addr.sin_family, "127.0.0.1", &mbsfn_sock_addr.sin_addr.s_addr) != 1) {
    perror("inet_pton");
    return false;
  }

  return SRSRAN_SUCCESS;
}

gw::~gw()
{
  if (tun_fd_inet > 0) {
    close(tun_fd_inet);
  }
  if (tun_fd_ims > 0) {
    close(tun_fd_ims);
  }
}

void gw::start_pcap(srsran::gw_pcap* pcap_)
{
  pcap = pcap_;
}

void gw::stop()
{
  if (run_enable) {
    run_enable = false;
    if (if_up_inet || if_up_ims) {
      if_up_inet = false;
      if_up_ims  = false;
      if (running) {
        thread_cancel();
      }

      // Wait thread to exit gracefully otherwise might leave a mutex locked
      int cnt = 0;
      while (running && cnt < 100) {
        usleep(10000);
        cnt++;
      }
      wait_thread_finish();

      current_ip_addr_inet = 0;
      current_ip_addr_ims  = 0;
    }
    // TODO: tear down TUN device?
  }
  if (mbsfn_sock_fd) {
    close(mbsfn_sock_fd);
  }
}

void gw::get_metrics(gw_metrics_t& m, const uint32_t nof_tti)
{
  std::lock_guard<std::mutex> lock(gw_mutex);

  std::chrono::duration<double> secs = std::chrono::high_resolution_clock::now() - metrics_tp;

  double dl_tput_mbps_real_time = (dl_tput_bytes * 8 / (double)1e6) / secs.count();
  double ul_tput_mbps_real_time = (ul_tput_bytes * 8 / (double)1e6) / secs.count();
  double ims_dl_tput_mbps_real_time = (ims_dl_tput_bytes * 8 / (double)1e6) / secs.count();
  double ims_ul_tput_mbps_real_time = (ims_ul_tput_bytes * 8 / (double)1e6) / secs.count();

  // Use the provided TTI counter to compute rate for metrics interface
  m.dl_tput_mbps = (nof_tti > 0) ? ((dl_tput_bytes * 8 / (double)1e6) / (nof_tti / 1000.0)) : 0.0;
  m.ul_tput_mbps = (nof_tti > 0) ? ((ul_tput_bytes * 8 / (double)1e6) / (nof_tti / 1000.0)) : 0.0;
  m.ims_dl_tput_mbps = (nof_tti > 0) ? ((ims_dl_tput_bytes * 8 / (double)1e6) / (nof_tti / 1000.0)) : 0.0;
  m.ims_ul_tput_mbps = (nof_tti > 0) ? ((ims_ul_tput_bytes * 8 / (double)1e6) / (nof_tti / 1000.0)) : 0.0;

  logger.debug("gw_rx_rate_mbps=%4.2f (real=%4.2f), gw_tx_rate_mbps=%4.2f (real=%4.2f)\n"
               "ims_gw_rx_rate_mbps=%4.2f (real=%4.2f), ims_gw_tx_rate_mbps=%4.2f (real=%4.2f)",
               m.dl_tput_mbps,
               dl_tput_mbps_real_time,
               m.ul_tput_mbps,
               ul_tput_mbps_real_time,
               m.ims_dl_tput_mbps,
               ims_dl_tput_mbps_real_time,
               m.ims_ul_tput_mbps,
               ims_ul_tput_mbps_real_time);

  // reset counters and store time
  metrics_tp    = std::chrono::high_resolution_clock::now();
  dl_tput_bytes = 0;
  ul_tput_bytes = 0;
  ims_dl_tput_bytes = 0;
  ims_ul_tput_bytes = 0;
}

/*******************************************************************************
  PDCP interface
*******************************************************************************/
void gw::write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "RX PDU. Stack latency: %ld us", pdu->get_latency_us().count());
  {
    std::unique_lock<std::mutex> lock(gw_mutex);
    if (lcid == 3) {
      dl_tput_bytes += pdu->N_bytes;
    } else if (lcid == 4) {
      ims_dl_tput_bytes += pdu->N_bytes;
    }
  }
  if (!if_up_inet && !if_up_ims) {
    if (run_enable) {
      logger.warning("TUN/TAP not up - dropping gw RX message");
    }
  } else if (pdu->N_bytes < 20) {
    // Packet not large enough to hold IPv4 Header
    logger.warning("Packet to small to hold IPv4 header. Dropping packet with %d B", pdu->N_bytes);
  } else {
    if (pcap != nullptr) {
      pcap->write_dl_pdu(lcid, pdu->msg, pdu->N_bytes);
    }

    // Only handle IPv4 and IPv6 packets
    struct iphdr* ip_pkt = (struct iphdr*)pdu->msg;
    if (ip_pkt->version == 4 || ip_pkt->version == 6) {
      int     n     = 0;
      int     n_    = 0;
      uint8_t tried = 0;

      // check this packet whether is router advertisement
      if (ip_pkt->version == 6) {
        struct ipv6hdr* ipv6_pkt = (struct ipv6hdr*)pdu->msg;
        // ICMP and Router Advertisement
        if (ipv6_pkt->nexthdr == 0x3a && *(pdu->msg + sizeof(ipv6hdr)) == 0x86) {
          // prase packet struct
          in6_addr prefix;
          uint8_t  prefix_len;
          if (icmpv6::parse_router_advertisement(
                  pdu->msg + sizeof(ipv6hdr), ipv6_pkt->payload_len, &prefix, &prefix_len) == 0) {
            update_ipv6_prefix(lcid, &prefix, &prefix_len);
          }
          return;
        }
      }

      if (lcid == 4) {
      write_ims_pdu:
        if (n >= 0) {
          n_ = write(tun_fd_ims, pdu->msg + n, pdu->N_bytes - n);
        } else {
          n_ = write(tun_fd_ims, pdu->msg, pdu->N_bytes);
        }
        if (errno == EAGAIN && tried < 3) {
          // buffer is no sufficent, need write again
          usleep(100);
          tried++;
          if (n_ >= 0) {
            n += n_;
          }
          goto write_ims_pdu;
        }
        n += n_;
      } else {
      write_normal_pdu:
        if (n >= 0) {
          n_ = write(tun_fd_inet, pdu->msg + n, pdu->N_bytes - n);
        } else {
          n_ = write(tun_fd_inet, pdu->msg, pdu->N_bytes);
        }
        if (errno == EAGAIN && tried < 3) {
          usleep(100);
          tried++;
          n += n_;
          goto write_normal_pdu;
        }
        n += n_;
      }
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
    {
      std::unique_lock<std::mutex> lock(gw_mutex);
      dl_tput_bytes += pdu->N_bytes;
    }

    if (pcap != nullptr) {
      pcap->write_dl_pdu_mch(lcid, pdu->msg, pdu->N_bytes);
    }

    // Hack to drop initial 2 bytes
    pdu->msg += 2;
    pdu->N_bytes -= 2;
    struct in_addr dst_addr;
    memcpy(&dst_addr.s_addr, &pdu->msg[16], 4);

    if (!if_up_inet) {
      if (run_enable) {
        logger.warning("TUN/TAP not up - dropping gw RX message");
      }
    } else {
      int n = 0;
      if (lcid == 4) {
        // FIX: as same as Line-202
        n = write(tun_fd_ims, pdu->msg, pdu->N_bytes);
      } else if (lcid == 3) {
        n = write(tun_fd_inet, pdu->msg, pdu->N_bytes);
      }
      if (n > 0 && (pdu->N_bytes != (uint32_t)n)) {
        logger.warning("DL TUN/TAP write failure");
      }
    }
  }
}

/*******************************************************************************
  NAS interface
*******************************************************************************/
int gw::setup_if_addr(uint32_t                eps_bearer_id,
                      srsran::srsran_apn_type apn_type,
                      uint8_t                 pdn_type,
                      uint32_t                ip_addr,
                      uint8_t*                ipv6_if_addr,
                      char*                   err_str)
{
  int err;

  // Make sure the worker thread is terminated before spawning a new one.
  if (running) {
    run_enable = false;
    thread_cancel();
    wait_thread_finish();
  }

  if (pdn_type == LIBLTE_MME_PDN_TYPE_IPV4 || pdn_type == LIBLTE_MME_PDN_TYPE_IPV4V6) {
    err = setup_if_addr4(ip_addr, apn_type, err_str);
    if (err != SRSRAN_SUCCESS) {
      srsran::console("failed to set addr4 %s\n", err_str);
      return err;
    }
  }
  if (pdn_type == LIBLTE_MME_PDN_TYPE_IPV6 || pdn_type == LIBLTE_MME_PDN_TYPE_IPV4V6) {
    err = setup_if_addr6(ipv6_if_addr, apn_type, err_str);
    if (err != SRSRAN_SUCCESS) {
      srsran::console("failed to set addr6 %s\n", err_str);
      return err;
    }
  }

  // set bearer id
  if (apn_type == srsran::srsran_apn_type::IMS) {
    default_eps_bearer_id_ims = static_cast<int>(eps_bearer_id);
  } else {
    default_eps_bearer_id_inet = static_cast<int>(eps_bearer_id);
  }

  // Setup a thread to receive packets from the TUN device
  run_enable = true;
  start(GW_THREAD_PRIO);

  if (pdn_type == LIBLTE_MME_PDN_TYPE_IPV6 || pdn_type == LIBLTE_MME_PDN_TYPE_IPV4V6) {
    send_router_solicitation(apn_type);
  }

  return SRSRAN_SUCCESS;
}

int gw::deactivate_eps_bearer(const uint32_t eps_bearer_id)
{
  std::lock_guard<std::mutex> lock(gw_mutex);

  // only deactivation of default bearer
  if (eps_bearer_id == static_cast<uint32_t>(default_eps_bearer_id_inet)) {
    logger.debug("Deactivating EPS bearer %d", eps_bearer_id);
    default_eps_bearer_id_inet = NOT_ASSIGNED;
    return SRSRAN_SUCCESS;
  } else if (eps_bearer_id == static_cast<uint32_t>(default_eps_bearer_id_ims)) {
    logger.debug("Deactivating EPS bearer %d", eps_bearer_id);
    default_eps_bearer_id_ims = NOT_ASSIGNED;
    return SRSRAN_SUCCESS;
  } else {
    // delete TFT template (if any) for this bearer
    tft_matcher.delete_tft_for_eps_bearer(eps_bearer_id);
    return SRSRAN_SUCCESS;
  }
}

bool gw::is_running()
{
  return running;
}

int gw::apply_traffic_flow_template(const uint8_t& erab_id, const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft)
{
  return tft_matcher.apply_traffic_flow_template(erab_id, tft);
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
  int32_t eps_bearer_id_ = 0;

  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  const static uint32_t REGISTER_WAIT_TOUT = 40, SERVICE_WAIT_TOUT = 40; // 4 sec
  uint32_t              register_wait = 0, service_wait = 0;
  int32_t               tun_fd_ = 0;

  logger.info("GW IP packet receiver thread run_enable");

  running = true;
  while (run_enable) {
    // Read packet from TUN
    if (SRSRAN_MAX_BUFFER_SIZE_BYTES - SRSRAN_BUFFER_HEADER_OFFSET > idx) {
      // N_bytes = read(tun_fd_inet, &pdu->msg[idx], SRSRAN_MAX_BUFFER_SIZE_BYTES - SRSRAN_BUFFER_HEADER_OFFSET - idx);
      if (if_up_inet) {
        tun_fd_ = tun_fd_inet;
        N_bytes = read(tun_fd_, &pdu->msg[idx], SRSRAN_MAX_BUFFER_SIZE_BYTES - SRSRAN_BUFFER_HEADER_OFFSET - idx);
        eps_bearer_id_ = default_eps_bearer_id_inet;
      }
      // if IMS set up and there is no data read from Internet, try to read something from IMS
      if (if_up_ims && (N_bytes == -1 && errno == EAGAIN)) {
        tun_fd_ = tun_fd_ims;
        N_bytes = read(tun_fd_, &pdu->msg[idx], SRSRAN_MAX_BUFFER_SIZE_BYTES - SRSRAN_BUFFER_HEADER_OFFSET - idx);
        eps_bearer_id_ = default_eps_bearer_id_ims;
      }
    } else {
      logger.error("GW pdu buffer full - gw receive thread exiting.");
      srsran::console("GW pdu buffer full - gw receive thread exiting.\n");
      break;
    }

    if (N_bytes == -1 && errno == EAGAIN) {
      usleep(1000 * 100);
      continue;
    }

    if (N_bytes <= 0) {
      logger.error("Failed to read from TUN interface - gw receive thread exiting.");
      srsran::console("Failed to read from TUN interface - gw receive thread exiting.\n");
      break;
    } else {
      logger.debug("Read %d bytes from TUN fd=%d, idx=%d", N_bytes, tun_fd_, idx);
      srsran::console("Read %04d bytes from TUN fd=%d, idx=%d, bearer_id=%d\n", N_bytes, tun_fd_, idx, eps_bearer_id_);
    }

    {
      std::unique_lock<std::mutex> lock(gw_mutex);
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

        // Make sure UE is attached and has default EPS bearer activated
        while (run_enable && eps_bearer_id_ == NOT_ASSIGNED && register_wait < REGISTER_WAIT_TOUT) {
          if (!register_wait) {
            logger.info("UE is not attached, waiting for NAS attach (%d/%d)", register_wait, REGISTER_WAIT_TOUT);
          }
          lock.unlock();
          std::this_thread::sleep_for(std::chrono::microseconds(100));
          lock.lock();
          register_wait++;
        }
        register_wait = 0;

        // If we are still not attached by this stage, drop packet
        if (run_enable && eps_bearer_id_ == NOT_ASSIGNED) {
          continue;
        }

        if (!run_enable) {
          break;
        }

        // Beyond this point we should have a activated default EPS bearer
        srsran_assert(eps_bearer_id_ != NOT_ASSIGNED, "Default EPS bearer not activated");

        uint8_t eps_bearer_id__ = eps_bearer_id_;
        tft_matcher.check_tft_filter_match(pdu, eps_bearer_id__);

        // Wait for service request if necessary
        while (run_enable && !stack->has_active_radio_bearer(eps_bearer_id_) && service_wait < SERVICE_WAIT_TOUT) {
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

        if (pcap != nullptr) {
          pcap->write_ul_pdu(eps_bearer_id_, pdu->msg, pdu->N_bytes);
        }

        // Send PDU directly to PDCP
        pdu->set_timestamp();
        ul_tput_bytes += pdu->N_bytes;
        stack->write_sdu(eps_bearer_id_, std::move(pdu));
        do {
          pdu = srsran::make_byte_buffer();
          if (!pdu) {
            logger.error("Fatal Error: Couldn't allocate PDU in run_thread().");
            usleep(100000);
          }
        } while (!pdu);
        idx = 0;
      } else {
        idx += N_bytes;
        logger.debug("Entire packet not read from socket. Total Length %d, N_Bytes %d.", ip_pkt->tot_len, pdu->N_bytes);
      }
    } // end of holdering gw_mutex
  }
  running = false;
  logger.info("GW IP receiver thread exiting.");
}

/**************************/
/* TUN Interface Helpers  */
/**************************/
int gw::init_if(srsran::srsran_apn_type srsran_apn_type, char* err_str)
{
  if (if_up_inet && srsran_apn_type == srsran::srsran_apn_type::Internet) {
    return SRSRAN_ERROR_ALREADY_STARTED;
  }
  if (if_up_ims && srsran_apn_type == srsran::srsran_apn_type::IMS) {
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
  if (srsran_apn_type == srsran::srsran_apn_type::Internet) {
    tun_fd_inet = open("/dev/net/tun", O_RDWR);
    logger.info("TUN file descriptor = %d", tun_fd_inet);
    if (0 > tun_fd_inet) {
      err_str = strerror(errno);
      logger.error("Failed to open TUN device: %s", err_str);
      return SRSRAN_ERROR_CANT_START;
    }
  } else if (srsran_apn_type == srsran::srsran_apn_type::IMS) {
    tun_fd_ims = open("/dev/net/tun", O_RDWR);
    logger.info("TUN file descriptor = %d", tun_fd_ims);
    if (0 > tun_fd_ims) {
      err_str = strerror(errno);
      logger.error("Failed to open TUN device: %s", err_str);
      return SRSRAN_ERROR_CANT_START;
    }
  }

  int32_t&      tun_fd_      = fetch_right_tun_fd(srsran_apn_type);
  struct ifreq& ifr_tmp      = fetch_right_ifr(srsran_apn_type);
  std::string   tun_dev_name = fetch_interface_name(srsran_apn_type);

  memset(&ifr_tmp, 0, sizeof(ifr_tmp));
  ifr_tmp.ifr_flags = IFF_TUN | IFF_NO_PI;
  strncpy(ifr_tmp.ifr_ifrn.ifrn_name, tun_dev_name.c_str(), std::min(tun_dev_name.length(), (size_t)(IFNAMSIZ - 1)));
  ifr_tmp.ifr_ifrn.ifrn_name[IFNAMSIZ - 1] = 0;
  if (0 > ioctl(tun_fd_, TUNSETIFF, &ifr_tmp)) {
    err_str = strerror(errno);
    logger.error("Failed to set TUN device name: %s", err_str);
    close(tun_fd_);
    return SRSRAN_ERROR_CANT_START;
  }

  // Bring up the interface
  sock_inet = socket(AF_INET, SOCK_DGRAM, 0);
  if (0 > ioctl(sock_inet, SIOCGIFFLAGS, &ifr_tmp)) {
    err_str = strerror(errno);
    logger.error("Failed to bring up socket: %s", err_str);
    close(tun_fd_);
    return SRSRAN_ERROR_CANT_START;
  }
  ifr_tmp.ifr_flags |= IFF_UP | IFF_RUNNING;
  if (0 > ioctl(sock_inet, SIOCSIFFLAGS, &ifr_tmp)) {
    err_str = strerror(errno);
    logger.error("Failed to set socket flags: %s", err_str);
    close(tun_fd_);
    return SRSRAN_ERROR_CANT_START;
  }

  // Delete link-local IPv6 address.
  struct in6_addr in6p;
  char            addr_str[INET6_ADDRSTRLEN];
  if (find_ipv6_addr(srsran_apn_type, &in6p)) {
    logger.debug("Found link-local IPv6 address: %s", inet_ntop(AF_INET6, &in6p, addr_str, INET6_ADDRSTRLEN));
    del_ipv6_addr(srsran_apn_type, &in6p);
  } else {
    logger.warning("Could not find link-local IPv6 address.");
  }

  // set tun_fd as non-blocking mode
  fcntl(tun_fd_, F_SETFL, O_NONBLOCK);
  if (srsran_apn_type == srsran::srsran_apn_type::IMS) {
    if_up_ims = true;
  } else {
    if_up_inet = true;
  }

  return SRSRAN_SUCCESS;
}

int gw::setup_if_addr4(uint32_t ip_addr, srsran::srsran_apn_type srsran_apn_type, char* err_str)
{
  int32_t& sock = sock_inet;
  if (srsran_apn_type == srsran::srsran_apn_type::IMS) {
    sock = sock_ims;
  }

  if ((srsran_apn_type == srsran::srsran_apn_type::Internet && ip_addr != current_ip_addr_inet) ||
      (srsran_apn_type == srsran::srsran_apn_type::IMS && ip_addr != current_ip_addr_ims)) {
    if ((srsran_apn_type == srsran::srsran_apn_type::Internet && !if_up_inet) ||
        (srsran_apn_type == srsran::srsran_apn_type::IMS && !if_up_ims)) {
      if (init_if(srsran_apn_type, err_str)) {
        srsran::console("init_if failed\n");
        return SRSRAN_ERROR_CANT_START;
      }
    }

    if (sock > 0) {
      close(sock);
    }

    struct ifreq& ifr_tmp = fetch_right_ifr(srsran_apn_type);
    int32_t&      tun_fd_ = fetch_right_tun_fd(srsran_apn_type);

    // Setup the IP address
    sock                                                      = socket(AF_INET, SOCK_DGRAM, 0);
    ifr_tmp.ifr_addr.sa_family                                = AF_INET;
    ((struct sockaddr_in*)&ifr_tmp.ifr_addr)->sin_addr.s_addr = htonl(ip_addr);
    if (0 > ioctl(sock, SIOCSIFADDR, &ifr_tmp)) {
      err_str = strerror(errno);
      logger.debug("Failed to set socket address: %s", err_str);
      close(tun_fd_);
      return SRSRAN_ERROR_CANT_START;
    }
    ifr_tmp.ifr_netmask.sa_family = AF_INET;
    if (inet_pton(ifr_tmp.ifr_netmask.sa_family,
                  args.tun_dev_netmask.c_str(),
                  &((struct sockaddr_in*)&ifr_tmp.ifr_netmask)->sin_addr.s_addr) != 1) {
      logger.error("Invalid tun_dev_netmask: %s", args.tun_dev_netmask.c_str());
      srsran::console("Invalid tun_dev_netmask: %s\n", args.tun_dev_netmask.c_str());
      perror("inet_pton");
      return SRSRAN_ERROR_CANT_START;
    }
    if (0 > ioctl(sock, SIOCSIFNETMASK, &ifr_tmp)) {
      err_str = strerror(errno);
      logger.debug("Failed to set socket netmask: %s", err_str);
      close(tun_fd_);
      return SRSRAN_ERROR_CANT_START;
    }
    if (srsran_apn_type == srsran::srsran_apn_type::Internet) {
      current_ip_addr_inet = ip_addr;
    } else if (srsran_apn_type == srsran::srsran_apn_type::IMS) {
      current_ip_addr_ims = ip_addr;
    }
  }
  return SRSRAN_SUCCESS;
}

int gw::setup_if_addr6(uint8_t* ipv6_if_id, srsran::srsran_apn_type srsran_apn_type, char* err_str)
{
  struct sockaddr_in6 sai;
  struct in6_ifreq    ifr6;
  bool                match = true;
  int32_t&            sock  = sock_inet;

  if (srsran_apn_type == srsran::srsran_apn_type::IMS) {
    for (int i = 0; i < 8; i++) {
      if (ipv6_if_id[i] != current_if_id_ims[i]) {
        match = false;
        break;
      }
    }
    sock = sock_ims;
  } else if (srsran_apn_type == srsran::srsran_apn_type::Internet) {
    for (int i = 0; i < 8; i++) {
      if (ipv6_if_id[i] != current_if_id_inet[i]) {
        match = false;
        break;
      }
    }
    sock = sock_inet;
  }

  if (!match) {
    if ((srsran_apn_type == srsran::srsran_apn_type::IMS && !if_up_ims) ||
        (srsran_apn_type == srsran::srsran_apn_type::Internet && !if_up_inet)) {
      if (init_if(srsran_apn_type, err_str)) {
        logger.error("init_if failed");
        return SRSRAN_ERROR_CANT_START;
      }
    }

    if (sock > 0) {
      close(sock);
    }

    // Setup the IP address
    struct ifreq& ifr_tmp      = fetch_right_ifr(srsran_apn_type);
    sock                   = socket(AF_INET6, SOCK_DGRAM, 0);
    ifr_tmp.ifr_addr.sa_family = AF_INET6;

    if (inet_pton(AF_INET6, "fe80::", (void*)&sai.sin6_addr) <= 0) {
      logger.error("Bad address");
      return SRSRAN_ERROR_CANT_START;
    }

    memcpy(&sai.sin6_addr.s6_addr[8], ipv6_if_id, 8);
    if (ioctl(sock, SIOGIFINDEX, &ifr_tmp) < 0) {
      perror("SIOGIFINDEX");
      return SRSRAN_ERROR_CANT_START;
    }
    ifr6.ifr6_ifindex   = ifr_tmp.ifr_ifindex;
    ifr6.ifr6_prefixlen = 64;
    memcpy((char*)&ifr6.ifr6_addr, (char*)&sai.sin6_addr, sizeof(struct in6_addr));

    if (ioctl(sock, SIOCSIFADDR, &ifr6) < 0) {
      err_str = strerror(errno);
      logger.error("Could not set IPv6 Link local address. Error %s", err_str);
      return SRSRAN_ERROR_CANT_START;
    }

    if (srsran_apn_type == srsran::srsran_apn_type::IMS) {
      for (int i = 0; i < 8; i++) {
        current_if_id_ims[i] = ipv6_if_id[i];
      }
    } else if (srsran_apn_type == srsran::srsran_apn_type::Internet) {
      for (int i = 0; i < 8; i++) {
        current_if_id_inet[i] = ipv6_if_id[i];
      }
    }
  }

  return SRSRAN_SUCCESS;
}

bool gw::find_ipv6_addr(srsran::srsran_apn_type srsran_apn_type, struct in6_addr* in6_out)
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
  std::string tun_dev_name = fetch_interface_name(srsran_apn_type);

  logger.debug("Trying to obtain IPv6 addr of %s interface", tun_dev_name.c_str());

  // Get Interface Index
  if_index = if_nametoindex(tun_dev_name.c_str());
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

void gw::del_ipv6_addr(srsran::srsran_apn_type srsran_apn_type, struct in6_addr* in6p)
{
  int          status, fd = -1;
  unsigned int if_index;
  struct {
    struct nlmsghdr  n;
    struct ifaddrmsg ifa;
    char             buf[1024];
  } req;
  std::string tun_dev_name = fetch_interface_name(srsran_apn_type);

  // Get Interface Index
  if_index = if_nametoindex(tun_dev_name.c_str());
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

struct ifreq& gw::fetch_right_ifr(srsran::srsran_apn_type srsran_apn_type)
{
  if (srsran_apn_type == srsran::srsran_apn_type::IMS) {
    logger.debug("Requesting ifr for ims");
    return ifr_ims;
  }
  logger.debug("Requesting ifr for other");
  return ifr_inet;
}

std::string gw::fetch_interface_name(srsran::srsran_apn_type srsran_apn_type)
{
  if (srsran_apn_type == srsran::srsran_apn_type::IMS) {
    logger.debug("Requesting tun_dev_name for ims");
    return args.tun_dev_name_ims;
  }
  logger.debug("Requesting tun_dev_name for other");
  return args.tun_dev_name;
}

int32_t& gw::fetch_right_tun_fd(srsran::srsran_apn_type srsran_apn_type)
{
  if (srsran_apn_type == srsran::srsran_apn_type::IMS) {
    logger.debug("Requesting ifr for ims");
    return tun_fd_ims;
  }
  logger.debug("Requesting ifr for other");
  return tun_fd_inet;
}

void gw::setup_route(uint32_t pcscf_addr, srsran::srsran_apn_type srsran_apn_type)
{
  struct rtentry rt;
  int            if_index;
  int            fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    srsran::console("[UE GW] socket() failed: %s (L-%d)\n", strerror(errno), __LINE__);
    return;
  }

  memset(&rt, 0, sizeof(rt));

  // via
  std::string dev_name = fetch_interface_name(srsran_apn_type);
  if_index             = if_nametoindex(dev_name.c_str());
  if (if_index == 0) {
    srsran::console("[UE GW]if_nametoindex() failed: interface %s\n", dev_name);
    return;
  }
  rt.rt_dev = (char*)dev_name.c_str();

  // destination
  struct sockaddr_in* addr = (struct sockaddr_in*)&rt.rt_dst;
  addr->sin_family         = AF_INET;
  addr->sin_addr.s_addr    = htonl(pcscf_addr);
  // mask
  addr                  = (struct sockaddr_in*)&rt.rt_genmask;
  addr->sin_family      = AF_INET;
  addr->sin_addr.s_addr = inet_addr("255.255.255.255");

  // update the route
  rt.rt_flags  = RTF_UP | RTF_HOST;
  rt.rt_metric = 1;
  int rc       = ioctl(fd, SIOCADDRT, &rt);
  if (rc == -1) {
    logger.debug("Setup IP route failed: %s\n", strerror(errno));
  }
  close(fd);
}

void gw::setup_route_v6(uint8_t pcscf_addr[16], srsran::srsran_apn_type srsran_apn_type)
{
  struct in6_rtmsg rt_msg;
  int              if_index;

  int fd = socket(AF_INET6, SOCK_DGRAM, 0);
  if (fd < 0) {
    srsran::console("[UE GW] socket() failed: %s (L-%d)\n", strerror(errno), __LINE__);
    return;
  }

  memset(&rt_msg, 0, sizeof(rt_msg));

  // Via
  std::string dev_name = fetch_interface_name(srsran_apn_type);
  if_index             = if_nametoindex(dev_name.c_str());
  if (if_index == 0) {
    srsran::console("[UE GW]if_nametoindex() failed: interface %s\n", dev_name);
    return;
  }
  rt_msg.rtmsg_ifindex = if_index;

  // destination
  struct in6_addr addrv6 = srsran::bytes_to_ipv6(pcscf_addr);
  memcpy((void*)&(rt_msg.rtmsg_dst), (void*)&addrv6, sizeof(struct in6_addr));
  rt_msg.rtmsg_dst_len = 128;

  // update the route
  rt_msg.rtmsg_flags  = RTF_UP | RTF_HOST;
  rt_msg.rtmsg_metric = 1;
  int rc              = ioctl(fd, SIOCADDRT, &rt_msg);
  if (rc == -1) {
    srsran::console("Setup IPv6 route failed: %s\n", strerror(errno));
    logger.error("Setup IPv6 route failed: %s\n", strerror(errno));
  } else {
    logger.info("Setup IPv6 route successed");
  }
  close(fd);
}
void gw::send_router_solicitation(srsran::srsran_apn_type srsran_apn_type)
{
  srsue::icmpv6 icmp;
  icmp.send_router_solicitation(fetch_interface_name(srsran_apn_type));
}

int gw::update_ipv6_prefix(int lcid, struct in6_addr* prefix, uint8_t* prefix_len)
{
  uint8_t* current_if_id_ = nullptr;
  ifreq&   ifr_           = fetch_right_ifr(srsran::srsran_apn_type::Internet);
  int32_t& sock           = sock_inet;

  if (lcid == 4) {
    current_if_id_ = current_if_id_ims;
    ifr_           = fetch_right_ifr(srsran::srsran_apn_type::IMS);
    sock           = sock_ims;
  } else {
    current_if_id_ = current_if_id_inet;
    ifr_           = fetch_right_ifr(srsran::srsran_apn_type::Internet);
    sock           = sock_inet;
  }

  // combine prefix with IPv6 interface id
  char astring[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, prefix, astring, INET6_ADDRSTRLEN);
  uint8_t combined_IPv6_address[16];
  for (int i = 0; i < 8; i++) {
    combined_IPv6_address[8 + i] = *(current_if_id_ + i);
  }
  uint8_t* prefix_ = (uint8_t*)prefix;
  for (int i = 0; i < (*prefix_len / 8); i++) {
    combined_IPv6_address[i] = *(prefix_ + i);
  }
  uint8_t bits = *prefix_len % 8;
  if (bits != 0) {
    uint8_t pos                = *prefix_len / 8 + 1;
    combined_IPv6_address[pos] = *(prefix_ + pos);
    combined_IPv6_address[pos] = combined_IPv6_address[pos] | ((0xff >> bits) & combined_IPv6_address[pos]);
  }
  inet_ntop(AF_INET6, &combined_IPv6_address, astring, INET6_ADDRSTRLEN);

  // Setup the IPv6 address
  sock                    = socket(AF_INET6, SOCK_DGRAM, 0);
  ifr_.ifr_addr.sa_family = AF_INET6;
  if (ioctl(sock, SIOGIFINDEX, &ifr_) < 0) {
    perror("SIOGIFINDEX");
    return SRSRAN_ERROR_CANT_START;
  }
  struct in6_ifreq ifr6;
  ifr6.ifr6_ifindex   = ifr_.ifr_ifindex;
  ifr6.ifr6_prefixlen = 64;
  memcpy((char*)&ifr6.ifr6_addr, (char*)combined_IPv6_address, sizeof(struct in6_addr));
  if (ioctl(sock, SIOCSIFADDR, &ifr6) < 0) {
    return SRSRAN_ERROR_CANT_START;
  }
  return SRSRAN_SUCCESS;
}

} // namespace srsue
