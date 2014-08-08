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


#ifndef CEXPTAB_
#define CEXPTAB_

#include <complex.h>
#include <stdint.h>
#include "liblte/config.h"

typedef _Complex float cf_t;

typedef struct LIBLTE_API {
  uint32_t size;
  cf_t *tab;
}cexptab_t;

LIBLTE_API int cexptab_init(cexptab_t *nco, 
                            uint32_t size);

LIBLTE_API void cexptab_free(cexptab_t *nco);

LIBLTE_API void cexptab_gen(cexptab_t *nco, 
                            cf_t *x, 
                            float freq, 
                            uint32_t len);

LIBLTE_API void cexptab_gen_direct(cf_t *x, 
                                   float freq, 
                                   uint32_t len);

#endif // CEXPTAB_
