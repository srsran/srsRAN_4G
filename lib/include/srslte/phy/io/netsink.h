/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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
 *  File:         netsink.h
 *
 *  Description:  Network socket sink.
 *                Supports writing binary data to a TCP or UDP socket.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_NETSINK_H
#define SRSLTE_NETSINK_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "srslte/config.h"

typedef enum { SRSLTE_NETSINK_UDP, SRSLTE_NETSINK_TCP } srslte_netsink_type_t;

/* Low-level API */
typedef struct SRSLTE_API {
  int                   sockfd;
  bool                  connected;
  srslte_netsink_type_t type;
  struct sockaddr_in    servaddr;
} srslte_netsink_t;

SRSLTE_API int srslte_netsink_init(srslte_netsink_t* q, const char* address, uint16_t port, srslte_netsink_type_t type);

SRSLTE_API void srslte_netsink_free(srslte_netsink_t* q);

SRSLTE_API int srslte_netsink_write(srslte_netsink_t* q, void* buffer, int nof_bytes);

SRSLTE_API int srslte_netsink_set_nonblocking(srslte_netsink_t* q);

#endif // SRSLTE_NETSINK_H
