/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <errno.h>

#include "liblte/phy/io/udpsource.h"

int udpsource_init(udpsource_t *q, char *address, int port) {
  bzero(q, sizeof(udpsource_t));

  q->sockfd=socket(AF_INET,SOCK_DGRAM,0);

  if (q->sockfd < 0) {
    perror("socket");
    return -1; 
  }
  
  q->servaddr.sin_family = AF_INET;
  q->servaddr.sin_addr.s_addr=inet_addr(address);
  q->servaddr.sin_port=htons(port);
  
  if (bind(q->sockfd,(struct sockaddr *)&q->servaddr,sizeof(struct sockaddr_in))) {
    perror("bind");
    return -1; 
  }

  return 0;
}

void udpsource_free(udpsource_t *q) {
  if (q->sockfd) {
    close(q->sockfd);
  }
  bzero(q, sizeof(udpsource_t));
}

int udpsource_read(udpsource_t *q, void *buffer, int nbytes) {
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
}

int udpsource_set_nonblocking(udpsource_t *q) {
  if (fcntl(q->sockfd, F_SETFL, O_NONBLOCK)) {
    perror("fcntl");
    return -1; 
  }
  return 0; 
}

int udpsource_set_timeout(udpsource_t *q, uint32_t microseconds) {
  struct timeval t; 
  t.tv_sec = 0; 
  t.tv_usec = microseconds; 
  if (setsockopt(q->sockfd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(struct timeval))) {
    perror("setsockopt");
    return -1; 
  }
  return 0; 
}

int udpsource_initialize(udpsource_hl* h) {
  return udpsource_init(&h->obj, h->init.address, h->init.port);
}

int udpsource_work(udpsource_hl* h) {
  h->out_len = udpsource_read(&h->obj, h->output, h->ctrl_in.nsamples);
  if (h->out_len < 0) {
    return -1;
  }
  return 0;
}

int udpsource_stop(udpsource_hl* h) {
  udpsource_free(&h->obj);
  return 0;
}
