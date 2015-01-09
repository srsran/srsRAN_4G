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


#ifndef NETSOURCE_
#define NETSOURCE_

#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "liblte/config.h"

typedef enum {NETSOURCE_UDP, NETSOURCE_TCP} netsource_type_t; 

/* Low-level API */
typedef struct LIBLTE_API {
  int sockfd;
  int connfd; 
  struct sockaddr_in servaddr;
  netsource_type_t type; 
  struct sockaddr_in cliaddr;  
}netsource_t;

LIBLTE_API int netsource_init(netsource_t *q, 
                              char *address, 
                              int port, 
                              netsource_type_t type);

LIBLTE_API void netsource_free(netsource_t *q);

LIBLTE_API int netsource_set_nonblocking(netsource_t *q); 

LIBLTE_API int netsource_read(netsource_t *q, 
                              void *buffer, 
                              int nof_bytes);

LIBLTE_API int netsource_set_timeout(netsource_t *q, 
                                     uint32_t microseconds); 


/* High-level API */
typedef struct LIBLTE_API {
  netsource_t obj;
  struct netsource_init {
    char *address;
    int port;
    int data_type;
  } init;
  struct netsource_ctrl_in {
    int nsamples;        // Number of samples to read
  } ctrl_in;
  void* output;
  int out_len;
}netsource_hl;

LIBLTE_API int netsource_initialize(netsource_hl* h);
LIBLTE_API int netsource_work(  netsource_hl* hl);
LIBLTE_API int netsource_stop(netsource_hl* h);

#endif // UDPSOURCE_
