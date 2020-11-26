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
