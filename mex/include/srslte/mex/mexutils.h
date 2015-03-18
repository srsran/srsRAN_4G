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


#ifndef MEXUTILS_
#define MEXUTILS_


#include <stdbool.h>

#ifdef UNDEF_BOOL
#undef bool
#endif

#include "mex.h"

#include "srslte/config.h"

typedef _Complex float cf_t; 

SRSLTE_API bool mexutils_isScalar(const mxArray *ptr);

SRSLTE_API int mexutils_read_cell(const mxArray *ptr, 
                                  lte_cell_t *cell);

SRSLTE_API char *mexutils_get_char_struct(const mxArray *ptr, 
                                          const char *field_name); 

SRSLTE_API int mexutils_read_uint32_struct(const mxArray *ptr, 
                                           const char *field_name, 
                                           uint32_t *value); 

SRSLTE_API int mexutils_read_float_struct(const mxArray *ptr, 
                                          const char *field_name, 
                                          float *value); 

SRSLTE_API int mexutils_write_f(float *buffer, 
                                mxArray **ptr, 
                                uint32_t nr, 
                                uint32_t nc);

SRSLTE_API int mexutils_write_cf(cf_t *buffer, 
                                 mxArray **ptr, 
                                 uint32_t nr, 
                                 uint32_t nc);

SRSLTE_API int mexutils_write_uint8(uint8_t *buffer, 
                                    mxArray **ptr, 
                                    uint32_t nr, 
                                    uint32_t nc);

SRSLTE_API int mexutils_write_int(int *buffer, 
                                  mxArray **ptr, 
                                  uint32_t nr, 
                                  uint32_t nc);

SRSLTE_API int mexutils_read_uint8(const mxArray *ptr, 
                                   uint8_t **buffer);

SRSLTE_API int mexutils_read_f(const mxArray *ptr, 
                               float **buffer);

SRSLTE_API int mexutils_read_cf(const mxArray *ptr, 
                                cf_t **buffer);
  
#endif
