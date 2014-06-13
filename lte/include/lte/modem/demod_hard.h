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


#ifndef DEMOD_HARD_
#define DEMOD_HARD_

#include <complex.h>
#include <stdint.h>

#include "lte/config.h"
#include "modem_table.h"

typedef _Complex float cf_t;

typedef struct LIBLTE_API {
	enum modem_std table; /* In this implementation, mapping table is hard-coded */
}demod_hard_t;


LIBLTE_API void demod_hard_init(demod_hard_t* q);
LIBLTE_API void demod_hard_table_set(demod_hard_t* q, enum modem_std table);
LIBLTE_API int demod_hard_demodulate(demod_hard_t* q, cf_t* symbols, char *bits, int nsymbols);



/* High-level API */
typedef struct LIBLTE_API {
	demod_hard_t obj;
	struct demod_hard_init {
		enum modem_std std;		// Symbol mapping standard (see modem_table.h)
	} init;

	cf_t* input;
	int in_len;

	char* output;
	int out_len;
}demod_hard_hl;

LIBLTE_API int demod_hard_initialize(demod_hard_hl* hl);
LIBLTE_API int demod_hard_work(demod_hard_hl* hl);
LIBLTE_API int demod_hard_stop(demod_hard_hl* hl);


#endif // DEMOD_HARD_
