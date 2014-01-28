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

#ifndef FILESOURCE_
#define FILESOURCE_

#include <stdint.h>
#include <stdlib.h>

#include "io/format.h"

/* Low-level API */
typedef struct {
	FILE *f;
	file_data_type_t type;
}filesource_t;

int filesource_init(filesource_t *q, char *filename, file_data_type_t type);
void filesource_close(filesource_t *q);

int filesource_read(filesource_t *q, void *buffer, int nsamples);


/* High-level API */
typedef struct {
	filesource_t obj;
	struct filesource_init {
		char *file_name;
		int block_length;
		int data_type;
	} init;
	struct filesource_ctrl_in {
		int nsamples;				// Number of samples to read
	} ctrl_in;
	void* output;
	int* out_len;
}filesource_hl;

int filesource_initialize(filesource_hl* h);
int filesource_work(	filesource_hl* hl);
int filesource_stop(filesource_hl* h);

#endif
