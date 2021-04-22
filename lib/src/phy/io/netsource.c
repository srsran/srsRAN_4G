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

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "srsran/phy/io/netsource.h"

int srsran_netsource_init(srsran_netsource_t* q, const char* address, uint16_t port, srsran_netsource_type_t type)
{
  bzero(q, sizeof(srsran_netsource_t));

  q->sockfd = socket(AF_INET, type == SRSRAN_NETSOURCE_TCP ? SOCK_STREAM : SOCK_DGRAM, 0);

  if (q->sockfd < 0) {
    perror("socket");
    return -1;
  }

  // Make sockets reusable
  int enable = 1;
#if defined(SO_REUSEADDR)
  if (setsockopt(q->sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");
#endif
#if defined(SO_REUSEPORT)
  if (setsockopt(q->sockfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEPORT) failed");
#endif
  q->type = type;

  q->servaddr.sin_family      = AF_INET;
  q->servaddr.sin_addr.s_addr = inet_addr(address);
  q->servaddr.sin_port        = htons(port);

  if (bind(q->sockfd, (struct sockaddr*)&q->servaddr, sizeof(struct sockaddr_in))) {
    perror("bind");
    return -1;
  }
  q->connfd = 0;

  return 0;
}

void srsran_netsource_free(srsran_netsource_t* q)
{
  if (q->sockfd) {
    close(q->sockfd);
  }
  bzero(q, sizeof(srsran_netsource_t));
}

int srsran_netsource_read(srsran_netsource_t* q, void* buffer, int nbytes)
{
  if (q->type == SRSRAN_NETSOURCE_UDP) {
    int n = recv(q->sockfd, buffer, nbytes, 0);

    if (n == -1) {
      if (errno == EAGAIN) {
        return 0;
      } else {
        return -1;
      }
    } else {
      return n;
    }
  } else {
    if (q->connfd == 0) {
      printf("Waiting for TCP connection\n");
      listen(q->sockfd, 1);
      socklen_t clilen = sizeof(q->cliaddr);
      q->connfd        = accept(q->sockfd, (struct sockaddr*)&q->cliaddr, &clilen);
      if (q->connfd < 0) {
        perror("accept");
        return -1;
      }
    }
    int n = read(q->connfd, buffer, nbytes);
    if (n == 0) {
      printf("Connection closed\n");
      close(q->connfd);
      q->connfd = 0;
      return 0;
    }
    if (n == -1) {
      perror("read");
    }
    return n;
  }
}

int srsran_netsource_write(srsran_netsource_t* q, void* buffer, int nbytes)
{
  // Loop until all bytes are sent
  char* ptr = (char*)buffer;
  while (nbytes > 0) {
    ssize_t i = send(q->connfd, ptr, nbytes, 0);
    if (i < 1) {
      perror("Error calling send()\n");
      return SRSRAN_ERROR;
    }
    ptr += i;
    nbytes -= i;
  }
  return SRSRAN_SUCCESS;
}

int srsran_netsource_set_nonblocking(srsran_netsource_t* q)
{
  if (fcntl(q->sockfd, F_SETFL, O_NONBLOCK)) {
    perror("fcntl");
    return -1;
  }
  return 0;
}

int srsran_netsource_set_timeout(srsran_netsource_t* q, uint32_t microseconds)
{
  struct timeval t;
  t.tv_sec  = 0;
  t.tv_usec = microseconds;
  if (setsockopt(q->sockfd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(struct timeval))) {
    perror("setsockopt");
    return -1;
  }
  return 0;
}
