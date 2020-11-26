/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSLTE_FILESINK_H
#define SRSLTE_FILESINK_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "srslte/config.h"
#include "srslte/phy/io/format.h"

/* Low-level API */
typedef struct SRSLTE_API {
  FILE*             f;
  srslte_datatype_t type;
} srslte_filesink_t;

SRSLTE_API int srslte_filesink_init(srslte_filesink_t* q, char* filename, srslte_datatype_t type);

SRSLTE_API void srslte_filesink_free(srslte_filesink_t* q);

SRSLTE_API int srslte_filesink_write(srslte_filesink_t* q, void* buffer, int nsamples);

SRSLTE_API int srslte_filesink_write_multi(srslte_filesink_t* q, void** buffer, int nsamples, int nchannels);

#endif // SRSLTE_FILESINK_H
