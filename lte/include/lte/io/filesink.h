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


#ifndef FILESINK_
#define FILESINK_

#include <stdint.h>
#include <stdlib.h>

#include "lte/config.h"
#include "lte/io/format.h"

/* Low-level API */
typedef struct LIBLTE_API {
	FILE *f;
	data_type_t type;
}filesink_t;

LIBLTE_API int filesink_init(filesink_t *q, char *filename, data_type_t type);
LIBLTE_API void filesink_free(filesink_t *q);

LIBLTE_API int filesink_write(filesink_t *q, void *buffer, int nsamples);


/* High-level API */
typedef struct LIBLTE_API {
	filesink_t obj;
	struct filesink_init {
		char *file_name;
		int block_length;
		int data_type;
	} init;
	void* input;
	int in_len;
}filesink_hl;

LIBLTE_API int filesink_initialize(filesink_hl* h);
LIBLTE_API int filesink_work(	filesink_hl* hl);
LIBLTE_API int filesink_stop(filesink_hl* h);

#endif // FILESINK_
