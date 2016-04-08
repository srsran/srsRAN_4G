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

#include <string.h>
#include <complex.h>
#include "srslte/mex/mexutils.h"
#include "srslte/utils/vector.h"
#include "srslte/common/phy_common.h"


bool mexutils_isScalar(const mxArray *ptr) {
  return mxGetM(ptr) == 1 && mxGetN(ptr) == 1;
}

bool mexutils_isCell(const mxArray *ptr) {
  return mxIsCell(ptr);
}

int mexutils_getLength(const mxArray *ptr) {
  const mwSize *dims;
  dims = mxGetDimensions(ptr);
  return dims[0];
}

mxArray* mexutils_getCellArray(const mxArray *ptr, int idx) {
  return mxGetCell(ptr, idx);
}

char *mexutils_get_char_struct(const mxArray *ptr, const char *field_name) {
  mxArray *p; 
  p = mxGetField(ptr, 0, field_name);
  if (!p) {
    return "";
  } 
  
  if (mxIsCell(p)) {
    return mxArrayToString(mxGetCell(p,0));
  } else {
    return mxArrayToString(p);    
  }  
}

int mexutils_read_uint32_struct(const mxArray *ptr, const char *field_name, uint32_t *value) 
{
  mxArray *p; 
  p = mxGetField(ptr, 0, field_name);
  if (!p) {
    return -1;
  } 
  *value = (uint32_t) mxGetScalar(p);
  return 0;
}

int mexutils_read_float_struct(const mxArray *ptr, const char *field_name, float *value) 
{
  mxArray *p; 
  p = mxGetField(ptr, 0, field_name);
  if (!p) {
    return -1;
  } 
  *value = (float) mxGetScalar(p);
  return 0;
}

int mexutils_read_cell(const mxArray *ptr, srslte_cell_t *cell) {
  if (mexutils_read_uint32_struct(ptr, "NCellID", &cell->id)) {
    return -1;
  }
  if (mexutils_read_uint32_struct(ptr, "CellRefP", &cell->nof_ports)) {
    return -1;
  }
  if (mexutils_read_uint32_struct(ptr, "NDLRB", &cell->nof_prb)) {
    return -1;
  }
  if (!strcmp(mexutils_get_char_struct(ptr, "CyclicPrefix"), "Extended")) {
    cell->cp = SRSLTE_CP_EXT;
  } else {
    cell->cp = SRSLTE_CP_NORM;
  }  
  if (!strcmp(mexutils_get_char_struct(ptr, "PHICHDuration"), "Extended")) {
    cell->phich_length = SRSLTE_PHICH_EXT;
  } else {
    cell->phich_length = SRSLTE_PHICH_NORM;
  }
  if (!strcmp(mexutils_get_char_struct(ptr, "Ng"), "Sixth")) {
    cell->phich_resources = SRSLTE_PHICH_R_1_6; 
  } else if (!strcmp(mexutils_get_char_struct(ptr, "Ng"), "Half")) {
    cell->phich_resources = SRSLTE_PHICH_R_1_2; 
  } else if (!strcmp(mexutils_get_char_struct(ptr, "Ng"), "Two")) {
    cell->phich_resources = SRSLTE_PHICH_R_2; 
  } else {
    cell->phich_resources = SRSLTE_PHICH_R_1; 
  }
  
  return 0;
}

int mexutils_read_cf(const mxArray *ptr, cf_t **buffer) {
  int numelems = mxGetNumberOfElements(ptr);
  cf_t *tmp = srslte_vec_malloc(numelems * sizeof(cf_t));
  if (tmp) {
    double *inr=mxGetPr(ptr);
    double *ini=mxGetPi(ptr);
    for (int i=0;i<numelems;i++) {
      __real__ tmp[i] = (float) inr[i]; 
      if (ini) {
        __imag__ tmp[i] = (float) ini[i];
      } else {
        __imag__ tmp[i] = 0; 
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
  float *tmp = srslte_vec_malloc(numelems * sizeof(float));
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

int mexutils_read_uint8(const mxArray *ptr, uint8_t **buffer) {
  int numelems = mxGetNumberOfElements(ptr);
  uint8_t *tmp = srslte_vec_malloc(numelems * sizeof(uint8_t));
  if (tmp) {
    double *inr=mxGetPr(ptr);
    for (int i=0;i<numelems;i++) {
      tmp[i] = (uint8_t) inr[i];       
    }    
    *buffer = tmp; 
    return numelems;
  } else {
    return -1;
  }
}


int mexutils_read_uint64(const mxArray *ptr, uint64_t **buffer) {
  int numelems = mxGetNumberOfElements(ptr);
  uint64_t *tmp = srslte_vec_malloc(numelems * sizeof(uint64_t));
  if (tmp) {
    uint64_t *inr=(uint64_t*) mxGetPr(ptr);
    for (int i=0;i<numelems;i++) {
      tmp[i] = (uint64_t) inr[i];       
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

int mexutils_write_s(short *buffer, mxArray **ptr, uint32_t nr, uint32_t nc) {
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

int mexutils_write_uint8(uint8_t *buffer, mxArray **ptr, uint32_t nr, uint32_t nc) {
  *ptr = mxCreateNumericMatrix(nr, nc, mxUINT8_CLASS, mxREAL); 
  if (*ptr) {
    uint8_t *outr = (uint8_t*) mxGetPr(*ptr);
    memcpy(outr, buffer, nr*nc*sizeof(uint8_t));
    return nc*nr;
  } else {
    return -1;
  }
}

int mexutils_write_int(int *buffer, mxArray **ptr, uint32_t nr, uint32_t nc) {
  *ptr = mxCreateNumericMatrix(nr, nc, mxINT32_CLASS, mxREAL); 
  if (*ptr) {
    int *outr = (int*) mxGetPr(*ptr);
    memcpy(outr, buffer, nr*nc*sizeof(int));
    return nc*nr;
  } else {
    return -1;
  }
}
