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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>


#include "liblte/phy/io/filesink.h"

int filesink_init(filesink_t *q, char *filename, data_type_t type) {
  bzero(q, sizeof(filesink_t));
  q->f = fopen(filename, "w");
  if (!q->f) {
    perror("fopen");
    return -1;
  }
  q->type = type;
  return 0;
}

void filesink_free(filesink_t *q) {
  if (q->f) {
    fclose(q->f);
  }
  bzero(q, sizeof(filesink_t));
}

int filesink_write(filesink_t *q, void *buffer, int nsamples) {
  int i;
  float *fbuf = (float*) buffer;
  _Complex float *cbuf = (_Complex float*) buffer;
  _Complex short *sbuf = (_Complex short*) buffer;
  int size;

  switch(q->type) {
  case FLOAT:
    for (i=0;i<nsamples;i++) {
      fprintf(q->f,"%g\n",fbuf[i]);
    }
    break;
  case COMPLEX_FLOAT:
    for (i=0;i<nsamples;i++) {
      if (__imag__ cbuf[i] >= 0)
        fprintf(q->f,"%g+%gi\n",__real__ cbuf[i],__imag__ cbuf[i]);
      else
        fprintf(q->f,"%g-%gi\n",__real__ cbuf[i],fabsf(__imag__ cbuf[i]));
    }
    break;
  case COMPLEX_SHORT:
    for (i=0;i<nsamples;i++) {
      if (__imag__ sbuf[i] >= 0)
        fprintf(q->f,"%hd+%hdi\n",__real__ sbuf[i],__imag__ sbuf[i]);
      else
        fprintf(q->f,"%hd-%hdi\n",__real__ sbuf[i],(short) abs(__imag__ sbuf[i]));
    }
    break;
  case FLOAT_BIN:
  case COMPLEX_FLOAT_BIN:
  case COMPLEX_SHORT_BIN:
    if (q->type == FLOAT_BIN) {
      size = sizeof(float);
    } else if (q->type == COMPLEX_FLOAT_BIN) {
      size = sizeof(_Complex float);
    } else if (q->type == COMPLEX_SHORT_BIN) {
      size = sizeof(_Complex short);
    }
    return fwrite(buffer, size, nsamples, q->f);
    break;
  default:
    i = -1;
    break;
  }
  return i;
}



int filesink_initialize(filesink_hl* h) {
  return filesink_init(&h->obj, h->init.file_name, h->init.data_type);
}

int filesink_work(filesink_hl* h) {
  if (filesink_write(&h->obj, h->input, h->in_len)<0) {
    return -1;
  }
  return 0;
}

int filesink_stop(filesink_hl* h) {
  filesink_free(&h->obj);
  return 0;
}
