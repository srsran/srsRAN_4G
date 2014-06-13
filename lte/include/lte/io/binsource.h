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


#ifndef BINSOURCE_
#define BINSOURCE_


#include <stdint.h>
#include "lte/config.h"

/* Low-level API */
typedef struct LIBLTE_API{
	unsigned int seed;
	uint32_t *seq_buff;
	int seq_buff_nwords;
	int seq_cache_nbits;
	int seq_cache_rp;
}binsource_t;

LIBLTE_API void binsource_init(binsource_t* q);
LIBLTE_API void binsource_free(binsource_t* q);
LIBLTE_API void binsource_seed_set(binsource_t* q, unsigned int seed);
LIBLTE_API void binsource_seed_time(binsource_t *q);
LIBLTE_API int binsource_cache_gen(binsource_t* q, int nbits);
LIBLTE_API void binsource_cache_cpy(binsource_t* q, char *bits, int nbits);
LIBLTE_API int binsource_generate(binsource_t* q, char *bits, int nbits);

/* High-level API */
typedef struct LIBLTE_API {
	binsource_t obj;
	struct binsource_init {
		int cache_seq_nbits; 	// If non-zero, generates random bits on init
		unsigned int seed;		// If non-zero, uses as random seed, otherwise local time is used.
	} init;
	struct binsource_ctrl_in {
		int nbits;				// Number of bits to generate
	} ctrl_in;
	char* output;
	int out_len;
}binsource_hl;

LIBLTE_API int binsource_initialize(binsource_hl* h);
LIBLTE_API int binsource_work(	binsource_hl* hl);
LIBLTE_API int binsource_stop(binsource_hl* hl);

#endif // BINSOURCE_
