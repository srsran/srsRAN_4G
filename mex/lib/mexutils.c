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

#include <complex.h>
#include "liblte/phy/common/phy_common.h"
#include "liblte/mex/mexutils.h"
#include "liblte/phy/utils/vector.h"


bool mexutils_isScalar(const mxArray *ptr) {
  return mxGetM(ptr) == 1 && mxGetN(ptr) == 1;
}

int mexutils_read_uint32_struct(const mxArray *ptr, const char *field_name, uint32_t *value) 
{
  mxArray *p; 
  p = mxGetField(ptr, 0, field_name);
  if (!p) {
    mexPrintf("Error field %s not found\n", field_name);
    return -1;
  } 
  *value = (uint32_t) mxGetScalar(p);
  return 0;
}

int mexutils_read_cell(const mxArray *ptr, lte_cell_t *cell) {
  if (mexutils_read_uint32_struct(ptr, "NCellID", &cell->id)) {
    return -1;
  }
  if (mexutils_read_uint32_struct(ptr, "CellRefP", &cell->nof_ports)) {
    return -1;
  }
  if (mexutils_read_uint32_struct(ptr, "NDLRB", &cell->nof_prb)) {
    return -1;
  }
  // TODO
  cell->cp = CPNORM;
  cell->phich_length = PHICH_NORM;
  cell->phich_resources = R_1_6; 
  return 0;
}

int mexutils_read_cf(const mxArray *ptr, cf_t **buffer) {
  int numelems = mxGetNumberOfElements(ptr);
  cf_t *tmp = vec_malloc(numelems * sizeof(cf_t));
  if (tmp) {
    double *inr=mxGetPr(ptr);
    double *ini=mxGetPi(ptr);
    for (int i=0;i<numelems;i++) {
      __real__ tmp[i] = (float) inr[i]; 
      if (ini) {
        __imag__ tmp[i] = (float) ini[i];
      }
    }    
    *buffer = tmp; 
    return numelems;
  } else {
    return -1;
  }
}

int mexutils_read_f(const mxArray *ptr, float **buffer) {
  int numelems = mxGetNumberOfElements(ptr);
  float *tmp = vec_malloc(numelems * sizeof(float));
  if (tmp) {
    double *inr=mxGetPr(ptr);
    for (int i=0;i<numelems;i++) {
      tmp[i] = (float) inr[i];       
    }    
    *buffer = tmp; 
    return numelems;
  } else {
    return -1;
  }
}

int mexutils_write_cf(cf_t *buffer, mxArray **ptr, uint32_t nr, uint32_t nc) {
  *ptr = mxCreateDoubleMatrix(nr, nc, mxCOMPLEX); 
  if (*ptr) {
    double *outr = mxGetPr(*ptr);
    double *outi = mxGetPi(*ptr);
    for (int i=0;i<nr*nc;i++) {
      outr[i] = (double) crealf(buffer[i]);
      outi[i] = (double) cimagf(buffer[i]);
    }
    return nc*nr;
  } else {
    return -1;
  }
}

int mexutils_write_f(float *buffer, mxArray **ptr, uint32_t nr, uint32_t nc) {
  *ptr = mxCreateDoubleMatrix(nr, nc, mxREAL); 
  if (*ptr) {
    double *outr = mxGetPr(*ptr);
    for (int i=0;i<nr*nc;i++) {
      outr[i] = (double) buffer[i];
    }
    return nc*nr;
  } else {
    return -1;
  }
}

