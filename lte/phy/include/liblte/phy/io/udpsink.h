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


#ifndef UDPSINK_
#define UDPSINK_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>

#include "liblte/config.h"
#include "liblte/phy/io/format.h"

/* Low-level API */
typedef struct LIBLTE_API {
  int sockfd;
  struct sockaddr_in servaddr;
  data_type_t type;
}udpsink_t;

LIBLTE_API int udpsink_init(udpsink_t *q, char *address, int port, data_type_t type);
LIBLTE_API void udpsink_free(udpsink_t *q);

LIBLTE_API int udpsink_write(udpsink_t *q, void *buffer, int nsamples);


/* High-level API */
typedef struct LIBLTE_API {
  udpsink_t obj;
  struct udpsink_init {
    char *address;
    int port;
    int block_length;
    int data_type;
  } init;
  void* input;
  int in_len;
}udpsink_hl;

LIBLTE_API int udpsink_initialize(udpsink_hl* h);
LIBLTE_API int udpsink_work(  udpsink_hl* hl);
LIBLTE_API int udpsink_stop(udpsink_hl* h);

#endif // UDPSINK_
