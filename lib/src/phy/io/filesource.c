/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "srslte/phy/io/filesource.h"
#include "srslte/phy/utils/debug.h"

int srslte_filesource_init(srslte_filesource_t* q, char* filename, srslte_datatype_t type)
{
  bzero(q, sizeof(srslte_filesource_t));
  q->f = fopen(filename, "r");
  if (!q->f) {
    perror("fopen");
    return -1;
  }
  q->type = type;
  return 0;
}

void srslte_filesource_free(srslte_filesource_t* q)
{
  if (q->f) {
    fclose(q->f);
  }
  bzero(q, sizeof(srslte_filesource_t));
}

void srslte_filesource_seek(srslte_filesource_t* q, int pos)
{
  if (fseek(q->f, pos, SEEK_SET) != 0) {
    perror("srslte_filesource_seek");
  }
}

int read_complex_f(FILE* f, _Complex float* y)
{
  char           in_str[64];
  _Complex float x = 0;
  if (NULL == fgets(in_str, 64, f)) {
    return -1;
  } else {
    if (index(in_str, 'i') || index(in_str, 'j')) {
      sscanf(in_str, "%f%fi", &(__real__ x), &(__imag__ x));
    } else {
      __imag__ x = 0;
      sscanf(in_str, "%f", &(__real__ x));
    }
    *y = x;
    return 0;
  }
}

int srslte_filesource_read(srslte_filesource_t* q, void* buffer, int nsamples)
{
  int             i;
  float*          fbuf = (float*)buffer;
  _Complex float* cbuf = (_Complex float*)buffer;
  _Complex short* sbuf = (_Complex short*)buffer;
  int             size = 0;

  switch (q->type) {
    case SRSLTE_FLOAT:
      for (i = 0; i < nsamples; i++) {
        if (EOF == fscanf(q->f, "%g\n", &fbuf[i]))
          break;
      }
      break;
    case SRSLTE_COMPLEX_FLOAT:
      for (i = 0; i < nsamples; i++) {
        if (read_complex_f(q->f, &cbuf[i])) {
          break;
        }
      }
      break;
    case SRSLTE_COMPLEX_SHORT:
      for (i = 0; i < nsamples; i++) {
        if (EOF == fscanf(q->f, "%hd%hdi\n", &(__real__ sbuf[i]), &(__imag__ sbuf[i])))
          break;
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
      return fread(buffer, size, nsamples, q->f);
      break;
    default:
      i = -1;
      break;
  }
  return i;
}

int srslte_filesource_read_multi(srslte_filesource_t* q, void** buffer, int nsamples, int nof_channels)
{
  int              i, j, count = 0;
  _Complex float** cbuf = (_Complex float**)buffer;

  switch (q->type) {
    case SRSLTE_FLOAT:
    case SRSLTE_COMPLEX_FLOAT:
    case SRSLTE_COMPLEX_SHORT:
    case SRSLTE_FLOAT_BIN:
    case SRSLTE_COMPLEX_SHORT_BIN:
      ERROR("%s.%d:Read Mode not implemented\n", __FILE__, __LINE__);
      count = SRSLTE_ERROR;
      break;
    case SRSLTE_COMPLEX_FLOAT_BIN:
      for (i = 0; i < nsamples; i++) {
        for (j = 0; j < nof_channels; j++) {
          count += fread(&cbuf[j][i], sizeof(cf_t), (size_t)1, q->f);
        }
      }
      break;
    default:
      count = SRSLTE_ERROR;
      break;
  }
  return count;
}
