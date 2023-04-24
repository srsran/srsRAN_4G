/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "srsran/phy/io/filesink.h"

int srsran_filesink_init(srsran_filesink_t* q, const char* filename, srsran_datatype_t type)
{
  bzero(q, sizeof(srsran_filesink_t));
  q->f = fopen(filename, "w");
  if (!q->f) {
    perror("fopen");
    return -1;
  }
  q->type = type;
  return 0;
}

void srsran_filesink_free(srsran_filesink_t* q)
{
  if (q->f) {
    fclose(q->f);
  }
  bzero(q, sizeof(srsran_filesink_t));
}

int srsran_filesink_write(srsran_filesink_t* q, void* buffer, int nsamples)
{
  int             i    = 0;
  float*          fbuf = (float*)buffer;
  _Complex float* cbuf = (_Complex float*)buffer;
  _Complex short* sbuf = (_Complex short*)buffer;
  int             size = 0;

  switch (q->type) {
    case SRSRAN_TEXT:
      fprintf(q->f, "%s", (char*)buffer);
      break;
    case SRSRAN_FLOAT:
      for (i = 0; i < nsamples; i++) {
        fprintf(q->f, "%g\n", fbuf[i]);
      }
      break;
    case SRSRAN_COMPLEX_FLOAT:
      for (i = 0; i < nsamples; i++) {
        if (__imag__ cbuf[i] >= 0)
          fprintf(q->f, "%g+%gi\n", __real__ cbuf[i], __imag__ cbuf[i]);
        else
          fprintf(q->f, "%g-%gi\n", __real__ cbuf[i], fabsf(__imag__ cbuf[i]));
      }
      break;
    case SRSRAN_COMPLEX_SHORT:
      for (i = 0; i < nsamples; i++) {
        if (__imag__ sbuf[i] >= 0)
          fprintf(q->f, "%hd+%hdi\n", __real__ sbuf[i], __imag__ sbuf[i]);
        else
          fprintf(q->f, "%hd-%hdi\n", __real__ sbuf[i], (short)abs(__imag__ sbuf[i]));
      }
      break;
    case SRSRAN_FLOAT_BIN:
    case SRSRAN_COMPLEX_FLOAT_BIN:
    case SRSRAN_COMPLEX_SHORT_BIN:
      if (q->type == SRSRAN_FLOAT_BIN) {
        size = sizeof(float);
      } else if (q->type == SRSRAN_COMPLEX_FLOAT_BIN) {
        size = sizeof(_Complex float);
      } else if (q->type == SRSRAN_COMPLEX_SHORT_BIN) {
        size = sizeof(_Complex short);
      }
      return fwrite(buffer, size, nsamples, q->f);
    default:
      i = -1;
      break;
  }
  return i;
}

int srsran_filesink_write_multi(srsran_filesink_t* q, void** buffer, int nsamples, int nchannels)
{
  int              i, j;
  float**          fbuf = (float**)buffer;
  _Complex float** cbuf = (_Complex float**)buffer;
  _Complex short** sbuf = (_Complex short**)buffer;
  int              size = 0;

  switch (q->type) {
    case SRSRAN_FLOAT:
      for (i = 0; i < nsamples; i++) {
        for (j = 0; j < nchannels; j++) {
          fprintf(q->f, "%g%c", fbuf[j][i], (j != (nchannels - 1)) ? '\t' : '\n');
        }
      }
      break;
    case SRSRAN_COMPLEX_FLOAT:
      for (i = 0; i < nsamples; i++) {
        for (j = 0; j < nchannels; j++) {
          fprintf(q->f, "%g%+gi%c", __real__ cbuf[j][i], __imag__ cbuf[j][i], (j != (nchannels - 1)) ? '\t' : '\n');
        }
      }
      break;
    case SRSRAN_COMPLEX_SHORT:
      for (i = 0; i < nsamples; i++) {
        for (j = 0; j < nchannels; j++) {
          fprintf(q->f, "%hd%+hdi%c", __real__ sbuf[j][i], __imag__ sbuf[j][i], (j != (nchannels - 1)) ? '\t' : '\n');
        }
      }
      break;
    case SRSRAN_FLOAT_BIN:
    case SRSRAN_COMPLEX_FLOAT_BIN:
    case SRSRAN_COMPLEX_SHORT_BIN:
      if (q->type == SRSRAN_FLOAT_BIN) {
        size = sizeof(float);
      } else if (q->type == SRSRAN_COMPLEX_FLOAT_BIN) {
        size = sizeof(_Complex float);
      } else if (q->type == SRSRAN_COMPLEX_SHORT_BIN) {
        size = sizeof(_Complex short);
      }
      if (nchannels > 1) {
        uint32_t count = 0;
        for (i = 0; i < nsamples; i++) {
          for (j = 0; j < nchannels; j++) {
            if (q->type == SRSRAN_FLOAT_BIN) {
              count += fwrite(&fbuf[j][i], size, 1, q->f);
            } else if (q->type == SRSRAN_COMPLEX_FLOAT_BIN) {
              count += fwrite(&cbuf[j][i], size, 1, q->f);
            } else if (q->type == SRSRAN_COMPLEX_SHORT_BIN) {
              count += fwrite(&sbuf[j][i], size, 1, q->f);
            }
          }
        }
        return count;
      } else {
        return fwrite(buffer[0], size, nsamples, q->f);
      }
      break;
    default:
      i = -1;
      break;
  }
  return i;
}
