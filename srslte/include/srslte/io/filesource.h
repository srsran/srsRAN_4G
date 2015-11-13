/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 *  File:         filesource.h
 *
 *  Description:  File source.
 *                Supports reading floats, complex floats and complex shorts
 *                from file in text or binary formats.
 *
 *  Reference:
 *****************************************************************************/

#ifndef FILESOURCE_
#define FILESOURCE_

#include <stdint.h>
#include <stdlib.h>

#include "srslte/config.h"
#include "srslte/io/format.h"

/* Low-level API */
typedef struct SRSLTE_API {
  FILE *f;
  srslte_datatype_t type;
} srslte_filesource_t;

SRSLTE_API int srslte_filesource_init(srslte_filesource_t *q, 
                                      char *filename, 
                                      srslte_datatype_t type);

SRSLTE_API void srslte_filesource_free(srslte_filesource_t *q);

SRSLTE_API void srslte_filesource_seek(srslte_filesource_t *q,
                                       int pos);

SRSLTE_API int srslte_filesource_read(srslte_filesource_t *q, 
                                      void *buffer, 
                                      int nsamples);

#endif // FILESOURCE_
