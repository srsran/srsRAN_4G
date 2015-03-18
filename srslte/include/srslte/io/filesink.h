/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

#include "srslte/config.h"
#include "srslte/io/format.h"

/* Low-level API */
typedef struct SRSLTE_API {
  FILE *f;
  srslte_datatype_t type;
} srslte_filesink_t;

SRSLTE_API int srslte_filesink_init(srslte_filesink_t *q, 
                                    char *filename, 
                                    srslte_datatype_t type);

SRSLTE_API void srslte_filesink_free(srslte_filesink_t *q);

SRSLTE_API int srslte_filesink_write(srslte_filesink_t *q, 
                                     void *buffer, 
                                     int nsamples);


/* High-level API */
typedef struct SRSLTE_API {
  srslte_filesink_t obj;
  struct srslte_filesink_init {
    char *file_name;
    int block_length;
    int data_type;
  } init;
  void* input;
  int in_len;
}srslte_filesink_hl;

SRSLTE_API int srslte_filesink_initialize(srslte_filesink_hl* h);
SRSLTE_API int srslte_filesink_work(  srslte_filesink_hl* hl);
SRSLTE_API int srslte_filesink_stop(srslte_filesink_hl* h);

#endif // FILESINK_
