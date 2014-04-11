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


#ifndef udpsource_
#define udpsource_


#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "lte/io/format.h"

/* Low-level API */
typedef struct {
	int sockfd;
	struct sockaddr_in servaddr;
	data_type_t type;
}udpsource_t;

int udpsource_init(udpsource_t *q, char *address, int port, data_type_t type);
void udpsource_free(udpsource_t *q);

int udpsource_read(udpsource_t *q, void *buffer, int nsamples);


/* High-level API */
typedef struct {
	udpsource_t obj;
	struct udpsource_init {
		char *address;
		int port;
		int data_type;
	} init;
	struct udpsource_ctrl_in {
		int nsamples;				// Number of samples to read
	} ctrl_in;
	void* output;
	int out_len;
}udpsource_hl;

int udpsource_initialize(udpsource_hl* h);
int udpsource_work(	udpsource_hl* hl);
int udpsource_stop(udpsource_hl* h);

#endif
