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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>


#include "srslte/io/filesink.h"

int srslte_filesink_init(srslte_filesink_t *q, char *filename, srslte_datatype_t type) {
  bzero(q, sizeof(srslte_filesink_t));
  q->f = fopen(filename, "w");
  if (!q->f) {
    perror("fopen");
    return -1;
  }
  q->type = type;
  return 0;
}

void srslte_filesink_free(srslte_filesink_t *q) {
  if (q->f) {
    fclose(q->f);
  }
  bzero(q, sizeof(srslte_filesink_t));
}

int srslte_filesink_write(srslte_filesink_t *q, void *buffer, int nsamples) {
  int i;
  float *fbuf = (float*) buffer;
  _Complex float *cbuf = (_Complex float*) buffer;
  _Complex short *sbuf = (_Complex short*) buffer;
  int size;

  switch(q->type) {
  case SRSLTE_FLOAT:
    for (i=0;i<nsamples;i++) {
      fprintf(q->f,"%g\n",fbuf[i]);
    }
    break;
  case SRSLTE_COMPLEX_FLOAT:
    for (i=0;i<nsamples;i++) {
      if (__imag__ cbuf[i] >= 0)
        fprintf(q->f,"%g+%gi\n",__real__ cbuf[i],__imag__ cbuf[i]);
      else
        fprintf(q->f,"%g-%gi\n",__real__ cbuf[i],fabsf(__imag__ cbuf[i]));
    }
    break;
  case SRSLTE_COMPLEX_SHORT:
    for (i=0;i<nsamples;i++) {
      if (__imag__ sbuf[i] >= 0)
        fprintf(q->f,"%hd+%hdi\n",__real__ sbuf[i],__imag__ sbuf[i]);
      else
        fprintf(q->f,"%hd-%hdi\n",__real__ sbuf[i],(short) abs(__imag__ sbuf[i]));
    }
    break;
  case SRSLTE_FLOAT_BIN:
  case SRSLTE_COMPLEX_FLOAT_BIN:
  case SRSLTE_COMPLEX_SHORT_BIN:
    if (q->type == SRSLTE_FLOAT_BIN) {
      size = sizeof(float);
    } else if (q->type == SRSLTE_COMPLEX_FLOAT_BIN) {
      size = sizeof(_Complex float);
    } else if (q->type == SRSLTE_COMPLEX_SHORT_BIN) {
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



int srslte_filesink_initialize(srslte_filesink_hl* h) {
  return srslte_filesink_init(&h->obj, h->init.file_name, h->init.data_type);
}

int srslte_filesink_work(srslte_filesink_hl* h) {
  if (srslte_filesink_write(&h->obj, h->input, h->in_len)<0) {
    return -1;
  }
  return 0;
}

int srslte_filesink_stop(srslte_filesink_hl* h) {
  srslte_filesink_free(&h->obj);
  return 0;
}
