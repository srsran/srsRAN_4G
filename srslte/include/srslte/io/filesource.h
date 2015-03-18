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


#ifndef FILESOURCE_
#define FILESOURCE_

#include <stdint.h>
#include <stdlib.h>

#include "srslte/config.h"
#include "srslte/io/format.h"

/* Low-level API */
typedef struct SRSLTE_API {
  FILE *f;
  data_type_t type;
}filesource_t;

SRSLTE_API int filesource_init(filesource_t *q, char *filename, data_type_t type);
SRSLTE_API void filesource_free(filesource_t *q);

SRSLTE_API void filesource_seek(filesource_t *q, int pos);
SRSLTE_API int filesource_read(filesource_t *q, void *buffer, int nsamples);


/* High-level API */
typedef struct SRSLTE_API {
  filesource_t obj;
  struct filesource_init {
    char *file_name;
    int block_length;
    int data_type;
  } init;
  struct filesource_ctrl_in {
    int nsamples;        // Number of samples to read
  } ctrl_in;
  void* output;
  int out_len;
}filesource_hl;

SRSLTE_API int filesource_initialize(filesource_hl* h);
SRSLTE_API int filesource_work(  filesource_hl* hl);
SRSLTE_API int filesource_stop(filesource_hl* h);

#endif // FILESOURCE_
