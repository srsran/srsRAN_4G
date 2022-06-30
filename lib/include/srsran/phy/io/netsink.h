/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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

#ifndef SRSRAN_NETSINK_H
#define SRSRAN_NETSINK_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "srsran/config.h"

typedef enum { SRSRAN_NETSINK_UDP, SRSRAN_NETSINK_TCP } srsran_netsink_type_t;

/* Low-level API */
typedef struct SRSRAN_API {
  int                   sockfd;
  bool                  connected;
  srsran_netsink_type_t type;
  struct sockaddr_in    servaddr;
} srsran_netsink_t;

SRSRAN_API int srsran_netsink_init(srsran_netsink_t* q, const char* address, uint16_t port, srsran_netsink_type_t type);

SRSRAN_API void srsran_netsink_free(srsran_netsink_t* q);

SRSRAN_API int srsran_netsink_write(srsran_netsink_t* q, void* buffer, int nof_bytes);

SRSRAN_API int srsran_netsink_set_nonblocking(srsran_netsink_t* q);

#endif // SRSRAN_NETSINK_H
