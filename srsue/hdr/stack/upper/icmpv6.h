/**
 * @file icmpv6.h
 * @author WingPig99 (5786734+WingPig99@users.noreply.github.com)
 * @brief
 * @version 0.1
 * @date 2021-12-17
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef SRSUE_ICMPv6_H
#define SRSUE_ICMPv6_H

#include "netinet/in.h"
#include "srsran/common/byte_buffer.h"
#include "srsran/common/standard_streams.h"
#include "sys/socket.h"
namespace srsue {

// Router Solicitation Message Format
// Ref: rfc4861[https://datatracker.ietf.org/doc/html/rfc4861], section 4.1
struct router_solicitation {
  uint8_t  type;
  uint8_t  code;
  uint16_t checksum;
  uint32_t reserved;
};

class icmpv6
{
private:
  int                 sock_fd;
  struct sockaddr_in6 allrouter;
  int                 ttl           = 255;
  const char*         allrouter_str = "ff02::2";
  int                 recev_router_advertisement(struct in6_addr* prefix, uint8_t* prefix_len);

public:
  icmpv6(/* args */);
  ~icmpv6();
  int                          init();
  int                          send_router_solicitation(std::string itf_name);
  srsran::unique_byte_buffer_t gen_router_solicitation();
  static int parse_router_advertisement(const void* buf, size_t len, struct in6_addr* prefix, uint8_t* prefix_len);
  void       destory();
};
} // namespace srsue

#endif