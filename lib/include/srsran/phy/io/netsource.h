/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

#ifndef SRSRAN_NETSOURCE_H
#define SRSRAN_NETSOURCE_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "srsran/config.h"

typedef enum { SRSRAN_NETSOURCE_UDP, SRSRAN_NETSOURCE_TCP } srsran_netsource_type_t;

/* Low-level API */
typedef struct SRSRAN_API {
  int                     sockfd;
  int                     connfd;
  struct sockaddr_in      servaddr;
  srsran_netsource_type_t type;
  struct sockaddr_in      cliaddr;
} srsran_netsource_t;

SRSRAN_API int
srsran_netsource_init(srsran_netsource_t* q, const char* address, uint16_t port, srsran_netsource_type_t type);

SRSRAN_API void srsran_netsource_free(srsran_netsource_t* q);

SRSRAN_API int srsran_netsource_set_nonblocking(srsran_netsource_t* q);

SRSRAN_API int srsran_netsource_read(srsran_netsource_t* q, void* buffer, int nof_bytes);

SRSRAN_API int srsran_netsource_write(srsran_netsource_t* q, void* buffer, int nbytes);

SRSRAN_API int srsran_netsource_set_timeout(srsran_netsource_t* q, uint32_t microseconds);

#endif // SRSRAN_NETSOURCE_H
