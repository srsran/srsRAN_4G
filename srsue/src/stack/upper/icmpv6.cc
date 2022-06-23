/**
 * @file icmpv6.cc
 * @author WingPig99 (5786734+WingPig99@users.noreply.github.com)
 * @brief
 * @version 0.1
 * @date 2021-12-17
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "srsue/hdr/stack/upper/icmpv6.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/standard_streams.h"
#include "srsran/config.h"
#include <arpa/inet.h>
#include <errno.h>
#include <linux/in6.h>
#include <net/if.h>
#include <netinet/icmp6.h>
#include <netinet/ip6.h>
#include <stdlib.h>
#include <unistd.h>

namespace srsue {
icmpv6::icmpv6()
{
  if (init() == SRSRAN_ERROR) {
    srsran::console("[UE ICMPv6] failed to create socket %s (L-%d)\n", strerror(errno), __LINE__);
  }
}

icmpv6::~icmpv6()
{
  if (sock_fd > 0) {
    close(sock_fd);
  }
}
int icmpv6::init()
{
  // create a socket
  sock_fd = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
  if (sock_fd == -1) {
    return SRSRAN_ERROR;
  }
  int option = 1;
  if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
    return SRSRAN_ERROR;
  }
  if (setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &option, sizeof(option)) == -1) {
    return SRSRAN_ERROR;
  }
  if (setsockopt(sock_fd, SOL_SOCKET, SO_DONTROUTE, &option, sizeof(option)) == -1) {
    return SRSRAN_ERROR;
  }
  if (setsockopt(sock_fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &ttl, sizeof(ttl)) == -1) {
    return SRSRAN_ERROR;
  }
  uint8_t flowTable[3] = {0x00, 0x00, 0x00};
  if (setsockopt(sock_fd, IPPROTO_IPV6, IPV6_AUTOFLOWLABEL, &flowTable, sizeof(flowTable)) == -1) {
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

/**
 * @brief Send router solicitaion
 *
 * @return int
 */
int icmpv6::send_router_solicitation(std::string itf_name)
{
  // lcoal address
  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", itf_name.c_str());
  if (setsockopt(sock_fd, SOL_SOCKET, SO_BINDTODEVICE, (void*)&ifr, strnlen(ifr.ifr_name, IFNAMSIZ)) < 0) {
    srsran::console("[UE ICMPv6] failed to send router solicitation: %s (L-%d)\n", strerror(errno), __LINE__);
    return SRSRAN_ERROR;
  }
  // compose ICMPv6 packet
  srsran::unique_byte_buffer_t rs = gen_router_solicitation();

  // send to all router address
  allrouter.sin6_family   = AF_INET6;
  allrouter.sin6_port     = 0;
  allrouter.sin6_scope_id = if_nametoindex(tun_dev_name);
  allrouter.sin6_flowinfo = htonl(0);
  if (inet_pton(AF_INET6, allrouter_str, &(allrouter.sin6_addr)) <= 0) {
    srsran::console("[UE ICMPv6] failed to send router solicitation: %s (L-%d)\n", strerror(errno), __LINE__);
    return SRSRAN_ERROR;
  }
  // send it
  ssize_t rst = sendto(sock_fd, rs->msg, rs->N_bytes, 0, (struct sockaddr*)&allrouter, sizeof(allrouter));
  if (rst <= -1) {
    srsran::console("[UE ICMPv6] Faild send router solicitation: %ld %s\n", rst, strerror(errno));
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

/**
 * @brief
 *
 * @param buf
 * @param len
 * @param prefix
 * @param prefix_len
 * @return int
 */
int icmpv6::parse_router_advertisement(const void* buf, size_t len, struct in6_addr* prefix, uint8_t* prefix_len)
{
  struct nd_router_advert ra;
  memcpy(&ra, buf, std::min(sizeof(struct nd_router_advert), len));
  if (ra.nd_ra_hdr.icmp6_type != ND_ROUTER_ADVERT) {
    return -1;
  }
  struct nd_opt_prefix_info opt;
  memcpy(&opt, (uint8_t*)buf + sizeof(struct nd_router_advert), sizeof(struct nd_opt_prefix_info));
  memcpy(prefix, &(opt.nd_opt_pi_prefix), sizeof(struct in6_addr));
  memcpy(prefix_len, &(opt.nd_opt_pi_prefix_len), sizeof(uint8_t));
  char astring[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, prefix, astring, INET6_ADDRSTRLEN);
  return 0;
}

/**
 * @brief recevive router advertisement
 *
 * @param prefix
 * @param prefix_len
 * @return int
 */
int icmpv6::recev_router_advertisement(struct in6_addr* prefix, uint8_t* prefix_len)
{
  // set read timeout
  struct timeval tv;
  tv.tv_sec  = 3;
  tv.tv_usec = 0;
  setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
  uint8_t* buf;
  buf         = (uint8_t*)malloc(sizeof(uint8_t) * 1024);
  ssize_t rst = recv(sock_fd, buf, 1024, 0);
  if (rst < 0) {
    printf("Read route advertisement failed: %s\n", strerror(errno));
    return -1;
  }
  struct nd_router_advert ra;
  memcpy(&ra, buf, sizeof(struct nd_router_advert));
  if (ra.nd_ra_hdr.icmp6_type == ND_ROUTER_ADVERT) {
    // log.info("received router advertisement\n");
  } else {
    return -1;
  }
  struct nd_opt_prefix_info opt;
  memcpy(&opt, buf + sizeof(struct nd_router_advert), sizeof(struct nd_opt_prefix_info));
  memcpy(prefix, &(opt.nd_opt_pi_prefix), sizeof(struct in6_addr));
  memcpy(prefix_len, &(opt.nd_opt_pi_prefix_len), sizeof(uint8_t));
  char astring[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, prefix, astring, INET6_ADDRSTRLEN);
  // log.info("Router advertisement Prefix: %s Len: %d\n", astring, *prefix_len);
  return 0;
}

/**
 * @brief gen router solicitation packet
 *
 * @return srsran::unique_byte_buffer_t
 */
srsran::unique_byte_buffer_t icmpv6::gen_router_solicitation()
{
  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();

  // compose ICMPv6 packet
  struct nd_router_solicit rs;
  bzero(&rs, sizeof(rs));
  rs.nd_rs_type     = ND_ROUTER_SOLICIT;
  rs.nd_rs_code     = 0;
  rs.nd_rs_cksum    = 0;
  rs.nd_rs_reserved = 0;

  memcpy(pdu->msg, (void*)&rs, sizeof(rs));
  pdu->N_bytes = sizeof(rs);

  return pdu;
}
} // namespace srsue