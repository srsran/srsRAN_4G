/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "srslte/phy/io/netsink.h"

int srslte_netsink_init(srslte_netsink_t* q, const char* address, uint16_t port, srslte_netsink_type_t type)
{
  bzero(q, sizeof(srslte_netsink_t));

  q->sockfd = socket(AF_INET, type == SRSLTE_NETSINK_TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
  if (q->sockfd < 0) {
    perror("socket");
    return -1;
  }

  int enable = 1;
#if defined(SO_REUSEADDR)
  if (setsockopt(q->sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");
#endif
#if defined(SO_REUSEPORT)
  if (setsockopt(q->sockfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEPORT) failed");
#endif

  q->servaddr.sin_family      = AF_INET;
  q->servaddr.sin_addr.s_addr = inet_addr(address);
  q->servaddr.sin_port        = htons(port);
  q->connected                = false;
  q->type                     = type;

  return 0;
}

void srslte_netsink_free(srslte_netsink_t* q)
{
  if (q->sockfd) {
    close(q->sockfd);
  }
  bzero(q, sizeof(srslte_netsink_t));
}

int srslte_netsink_set_nonblocking(srslte_netsink_t* q)
{
  if (fcntl(q->sockfd, F_SETFL, O_NONBLOCK)) {
    perror("fcntl");
    return -1;
  }
  return 0;
}

int srslte_netsink_write(srslte_netsink_t* q, void* buffer, int nof_bytes)
{
  if (!q->connected) {
    if (connect(q->sockfd, &q->servaddr, sizeof(q->servaddr)) < 0) {
      if (errno == ECONNREFUSED || errno == EINPROGRESS) {
        return 0;
      } else {
        perror("connect");
        exit(-1);
        return -1;
      }
    } else {
      q->connected = true;
    }
  }
  int n = 0;
  if (q->connected) {
    n = write(q->sockfd, buffer, nof_bytes);
    if (n < 0) {
      if (errno == ECONNRESET) {
        close(q->sockfd);
        q->sockfd = socket(AF_INET, q->type == SRSLTE_NETSINK_TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
        if (q->sockfd < 0) {
          perror("socket");
          return -1;
        }
        q->connected = false;
        return 0;
      }
    }
  }
  return n;
}
