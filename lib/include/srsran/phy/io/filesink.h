/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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
 *  File:         filesink.h
 *
 *  Description:  File sink.
 *                Supports writing floats, complex floats and complex shorts
 *                to file in text or binary formats.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_FILESINK_H
#define SRSRAN_FILESINK_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "srsran/config.h"
#include "srsran/phy/io/format.h"

/* Low-level API */
typedef struct SRSRAN_API {
  FILE*             f;
  srsran_datatype_t type;
} srsran_filesink_t;

SRSRAN_API int srsran_filesink_init(srsran_filesink_t* q, const char* filename, srsran_datatype_t type);

SRSRAN_API void srsran_filesink_free(srsran_filesink_t* q);

SRSRAN_API int srsran_filesink_write(srsran_filesink_t* q, void* buffer, int nsamples);

SRSRAN_API int srsran_filesink_write_multi(srsran_filesink_t* q, void** buffer, int nsamples, int nchannels);

#endif // SRSRAN_FILESINK_H
