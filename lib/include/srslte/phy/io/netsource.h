/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSLTE_NETSOURCE_H
#define SRSLTE_NETSOURCE_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "srslte/config.h"

typedef enum { SRSLTE_NETSOURCE_UDP, SRSLTE_NETSOURCE_TCP } srslte_netsource_type_t;

/* Low-level API */
typedef struct SRSLTE_API {
  int                     sockfd;
  int                     connfd;
  struct sockaddr_in      servaddr;
  srslte_netsource_type_t type;
  struct sockaddr_in      cliaddr;
} srslte_netsource_t;

SRSLTE_API int
srslte_netsource_init(srslte_netsource_t* q, const char* address, uint16_t port, srslte_netsource_type_t type);

SRSLTE_API void srslte_netsource_free(srslte_netsource_t* q);

SRSLTE_API int srslte_netsource_set_nonblocking(srslte_netsource_t* q);

SRSLTE_API int srslte_netsource_read(srslte_netsource_t* q, void* buffer, int nof_bytes);

SRSLTE_API int srslte_netsource_write(srslte_netsource_t* q, void* buffer, int nbytes);

SRSLTE_API int srslte_netsource_set_timeout(srslte_netsource_t* q, uint32_t microseconds);

#endif // SRSLTE_NETSOURCE_H
