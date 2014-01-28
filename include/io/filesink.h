/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>.
 * This file is part of OSLD-lib (http://https://github.com/ismagom/osld-lib)
 *
 * OSLD-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSLD-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OSLD-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILESINK_
#define FILESINK_

#include <stdint.h>
#include <stdlib.h>

#include "io/format.h"

/* Low-level API */
typedef struct {
	FILE *f;
	file_data_type_t type;
}filesink_t;

int filesink_init(filesink_t *q, char *filename, file_data_type_t type);
void filesink_close(filesink_t *q);

int filesink_write(filesink_t *q, void *buffer, int nsamples);


/* High-level API */
typedef struct {
	filesink_t obj;
	struct filesink_init {
		char *file_name;
		int block_length;
		int data_type;
	} init;
	void* input;
	int in_len;
}filesink_hl;

int filesink_initialize(filesink_hl* h);
int filesink_work(	filesink_hl* hl);
int filesink_stop(filesink_hl* h);

#endif
