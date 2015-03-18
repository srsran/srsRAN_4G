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


#ifndef NETSINK_
#define NETSINK_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "srslte/config.h"

typedef enum {NETSINK_UDP, NETSINK_TCP} netsink_type_t; 

/* Low-level API */
typedef struct LIBLTE_API {
  int sockfd;
  bool connected;
  netsink_type_t type; 
  struct sockaddr_in servaddr;
}netsink_t;

LIBLTE_API int netsink_init(netsink_t *q, 
                            char *address, 
                            int port, 
                            netsink_type_t type);

LIBLTE_API void netsink_free(netsink_t *q);

LIBLTE_API int netsink_write(netsink_t *q, 
                             void *buffer, 
                             int nof_bytes);

LIBLTE_API int netsink_set_nonblocking(netsink_t *q); 


/* High-level API */
typedef struct LIBLTE_API {
  netsink_t obj;
  struct netsink_init {
    char *address;
    int port;
    int block_length;
    int data_type;
  } init;
  void* input;
  int in_len;
}netsink_hl;

LIBLTE_API int netsink_initialize(netsink_hl* h);
LIBLTE_API int netsink_work(  netsink_hl* hl);
LIBLTE_API int netsink_stop(netsink_hl* h);

#endif // UDPSINK_
