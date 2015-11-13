/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 *  File:         netsource.h
 *
 *  Description:  Network socket source.
 *                Supports reading binary data from a TCP or UDP socket.
 *
 *  Reference:
 *****************************************************************************/

#ifndef NETSOURCE_
#define NETSOURCE_

#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "srslte/config.h"

typedef enum {
  SRSLTE_NETSOURCE_UDP, 
  SRSLTE_NETSOURCE_TCP  
} srslte_netsource_type_t; 

/* Low-level API */
typedef struct SRSLTE_API {
  int sockfd;
  int connfd; 
  struct sockaddr_in servaddr;
  srslte_netsource_type_t type; 
  struct sockaddr_in cliaddr;  
}srslte_netsource_t;

SRSLTE_API int srslte_netsource_init(srslte_netsource_t *q, 
                                     char *address, 
                                     int port, 
                                     srslte_netsource_type_t type);

SRSLTE_API void srslte_netsource_free(srslte_netsource_t *q);

SRSLTE_API int srslte_netsource_set_nonblocking(srslte_netsource_t *q); 

SRSLTE_API int srslte_netsource_read(srslte_netsource_t *q, 
                                     void *buffer, 
                                     int nof_bytes);

SRSLTE_API int srslte_netsource_set_timeout(srslte_netsource_t *q, 
                                            uint32_t microseconds); 

#endif // UDPSOURCE_
