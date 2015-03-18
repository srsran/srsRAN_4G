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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>
#include <errno.h>
#include <fcntl.h>


#include "srslte/phy/io/netsink.h"

int netsink_init(netsink_t *q, char *address, int port, netsink_type_t type) {
  bzero(q, sizeof(netsink_t));

  q->sockfd=socket(AF_INET, type==NETSINK_TCP?SOCK_STREAM:SOCK_DGRAM,0);  
  if (q->sockfd < 0) {
    perror("socket");
    return -1; 
  }

  q->servaddr.sin_family = AF_INET;
  q->servaddr.sin_addr.s_addr=inet_addr(address);
  q->servaddr.sin_port=htons(port);
  q->connected = false; 
  q->type = type; 
  
  return 0;
}

void netsink_free(netsink_t *q) {
  if (q->sockfd) {
    close(q->sockfd);
  }
  bzero(q, sizeof(netsink_t));
}

int netsink_set_nonblocking(netsink_t *q) {
  if (fcntl(q->sockfd, F_SETFL, O_NONBLOCK)) {
    perror("fcntl");
    return -1; 
  }
  return 0; 
}

int netsink_write(netsink_t *q, void *buffer, int nof_bytes) {
  if (!q->connected) {
    if (connect(q->sockfd,&q->servaddr,sizeof(q->servaddr)) < 0) {
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
        q->sockfd=socket(AF_INET, q->type==NETSINK_TCP?SOCK_STREAM:SOCK_DGRAM,0);  
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



int netsink_initialize(netsink_hl* h) {
  return netsink_init(&h->obj, h->init.address, h->init.port, NETSINK_UDP);
}

int netsink_work(netsink_hl* h) {
  if (netsink_write(&h->obj, h->input, h->in_len)<0) {
    return -1;
  }
  return 0;
}

int netsink_stop(netsink_hl* h) {
  netsink_free(&h->obj);
  return 0;
}
