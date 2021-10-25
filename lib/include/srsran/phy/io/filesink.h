/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
