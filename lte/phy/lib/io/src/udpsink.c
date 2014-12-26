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


#include "liblte/phy/io/udpsink.h"

int udpsink_init(udpsink_t *q, char *address, int port) {
  bzero(q, sizeof(udpsink_t));

  q->sockfd=socket(AF_INET,SOCK_DGRAM,0);
  
  if (q->sockfd < 0) {
    perror("socket");
    return -1; 
  }

  q->servaddr.sin_family = AF_INET;
  q->servaddr.sin_addr.s_addr=inet_addr(address);
  q->servaddr.sin_port=htons(port);

  return 0;
}

void udpsink_free(udpsink_t *q) {
  if (q->sockfd) {
    close(q->sockfd);
  }
  bzero(q, sizeof(udpsink_t));
}

int udpsink_write(udpsink_t *q, void *buffer, int nof_bytes) {
  return sendto(q->sockfd, buffer, nof_bytes, 0,
        &q->servaddr, sizeof(struct sockaddr_in));  
}



int udpsink_initialize(udpsink_hl* h) {
  return udpsink_init(&h->obj, h->init.address, h->init.port);
}

int udpsink_work(udpsink_hl* h) {
  if (udpsink_write(&h->obj, h->input, h->in_len)<0) {
    return -1;
  }
  return 0;
}

int udpsink_stop(udpsink_hl* h) {
  udpsink_free(&h->obj);
  return 0;
}
