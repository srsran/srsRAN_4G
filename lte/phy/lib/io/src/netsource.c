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

#include "liblte/phy/io/netsource.h"

int netsource_init(netsource_t *q, char *address, int port, netsource_type_t type) {
  bzero(q, sizeof(netsource_t));

  q->sockfd=socket(AF_INET,type==NETSOURCE_TCP?SOCK_STREAM:SOCK_DGRAM,0);

  if (q->sockfd < 0) {
    perror("socket");
    return -1; 
  }
  q->type = type; 
  
  q->servaddr.sin_family = AF_INET;
  q->servaddr.sin_addr.s_addr=inet_addr(address);
  q->servaddr.sin_port=htons(port);
  
  if (bind(q->sockfd,(struct sockaddr *)&q->servaddr,sizeof(struct sockaddr_in))) {
    perror("bind");
    return -1; 
  }
  q->connfd = 0; 

  return 0;
}

void netsource_free(netsource_t *q) {
  if (q->sockfd) {
    close(q->sockfd);
  }
  bzero(q, sizeof(netsource_t));
}

int netsource_read(netsource_t *q, void *buffer, int nbytes) {
  if (q->type == NETSOURCE_UDP) {
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
      q->connfd = accept(q->sockfd, (struct sockaddr *)&q->cliaddr, &clilen);
      if (q->connfd < 0) {
          perror("accept");
          return -1;
      }
    }
    int n = read(q->connfd, buffer, nbytes);
    if (n == -1) {
      if (errno == ECONNRESET) {
        printf("Connection closed\n");
        close(q->connfd);
        q->connfd = 0; 
        return 0;
      } else {
        perror("read");
      }
    }
    return n; 
  }
}

int netsource_set_nonblocking(netsource_t *q) {
  if (fcntl(q->sockfd, F_SETFL, O_NONBLOCK)) {
    perror("fcntl");
    return -1; 
  }
  return 0; 
}

int netsource_set_timeout(netsource_t *q, uint32_t microseconds) {
  struct timeval t; 
  t.tv_sec = 0; 
  t.tv_usec = microseconds; 
  if (setsockopt(q->sockfd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(struct timeval))) {
    perror("setsockopt");
    return -1; 
  }
  return 0; 
}

int netsource_initialize(netsource_hl* h) {
  return netsource_init(&h->obj, h->init.address, h->init.port, NETSOURCE_UDP);
}

int netsource_work(netsource_hl* h) {
  h->out_len = netsource_read(&h->obj, h->output, h->ctrl_in.nsamples);
  if (h->out_len < 0) {
    return -1;
  }
  return 0;
}

int netsource_stop(netsource_hl* h) {
  netsource_free(&h->obj);
  return 0;
}
