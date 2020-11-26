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
 *  File:         filesource.h
 *
 *  Description:  File source.
 *                Supports reading floats, complex floats and complex shorts
 *                from file in text or binary formats.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_FILESOURCE_H
#define SRSLTE_FILESOURCE_H

#include <stdint.h>
#include <stdlib.h>

#include "srslte/config.h"
#include "srslte/phy/io/format.h"

/* Low-level API */
typedef struct SRSLTE_API {
  FILE*             f;
  srslte_datatype_t type;
} srslte_filesource_t;

SRSLTE_API int srslte_filesource_init(srslte_filesource_t* q, char* filename, srslte_datatype_t type);

SRSLTE_API void srslte_filesource_free(srslte_filesource_t* q);

SRSLTE_API void srslte_filesource_seek(srslte_filesource_t* q, int pos);

SRSLTE_API int srslte_filesource_read(srslte_filesource_t* q, void* buffer, int nsamples);

SRSLTE_API int srslte_filesource_read_multi(srslte_filesource_t* q, void** buffer, int nsamples, int nof_channels);

#endif // SRSLTE_FILESOURCE_H
