/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

#include "srslte/config.h"

typedef enum {NETSOURCE_UDP, NETSOURCE_TCP} netsource_type_t; 

/* Low-level API */
typedef struct SRSLTE_API {
  int sockfd;
  int connfd; 
  struct sockaddr_in servaddr;
  netsource_type_t type; 
  struct sockaddr_in cliaddr;  
}netsource_t;

SRSLTE_API int netsource_init(netsource_t *q, 
                              char *address, 
                              int port, 
                              netsource_type_t type);

SRSLTE_API void netsource_free(netsource_t *q);

SRSLTE_API int netsource_set_nonblocking(netsource_t *q); 

SRSLTE_API int netsource_read(netsource_t *q, 
                              void *buffer, 
                              int nof_bytes);

SRSLTE_API int netsource_set_timeout(netsource_t *q, 
                                     uint32_t microseconds); 


/* High-level API */
typedef struct SRSLTE_API {
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

SRSLTE_API int netsource_initialize(netsource_hl* h);
SRSLTE_API int netsource_work(  netsource_hl* hl);
SRSLTE_API int netsource_stop(netsource_hl* h);

#endif // UDPSOURCE_
