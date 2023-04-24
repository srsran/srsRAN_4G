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

#include "rf_file_imp_trx.h"
#include <errno.h>
#include <inttypes.h>
#include <srsran/config.h>
#include <srsran/phy/utils/vector.h>
#include <stdlib.h>
#include <string.h>

int rf_file_tx_open(rf_file_tx_t* q, rf_file_opts_t opts)
{
  int ret = SRSRAN_ERROR;

  if (q) {
    // Zero object
    memset(q, 0, sizeof(rf_file_tx_t));

    // Copy id
    strncpy(q->id, opts.id, FILE_ID_STRLEN - 1);
    q->id[FILE_ID_STRLEN - 1] = '\0';

    // Assign file
    q->file = opts.file;

    // Configure formats
    q->sample_format = opts.sample_format;
    q->frequency_mhz = opts.frequency_mhz;

    q->temp_buffer_convert = srsran_vec_malloc(FILE_MAX_BUFFER_SIZE);
    if (!q->temp_buffer_convert) {
      fprintf(stderr, "Error: allocating tx buffer\n");
      goto clean_exit;
    }

    if (pthread_mutex_init(&q->mutex, NULL)) {
      fprintf(stderr, "Error: creating mutex\n");
      goto clean_exit;
    }

    q->zeros = srsran_vec_malloc(FILE_MAX_BUFFER_SIZE);
    if (!q->zeros) {
      fprintf(stderr, "Error: allocating zeros\n");
      goto clean_exit;
    }
    memset(q->zeros, 0, FILE_MAX_BUFFER_SIZE);

    q->running = true;

    ret = SRSRAN_SUCCESS;
  }

clean_exit:
  return ret;
}

static int _rf_file_tx_baseband(rf_file_tx_t* q, cf_t* buffer, uint32_t nsamples)
{
  int n = SRSRAN_ERROR;

  // convert samples if necessary
  void*    buf       = (buffer) ? buffer : q->zeros;
  uint32_t sample_sz = sizeof(cf_t);

  if (q->sample_format == FILERF_TYPE_SC16) {
    buf       = q->temp_buffer_convert;
    sample_sz = 2 * sizeof(short);
    srsran_vec_convert_fi((float*)buffer, INT16_MAX, (short*)q->temp_buffer_convert, 2 * nsamples);
  }

  size_t ret = fwrite(buf, (size_t)sample_sz, (size_t)nsamples, q->file);
  if (ret < (size_t)nsamples) {
    rf_file_error(q->id,
                  "[file] Error: transmitter expected %d bytes and sent %zd. %s.\n",
                  NSAMPLES2NBYTES(nsamples),
                  ret,
                  strerror(errno));
    n = SRSRAN_ERROR;
    goto clean_exit;
  }

  // Increment sample counter
  q->nsamples += nsamples;
  n = nsamples;

clean_exit:
  return n;
}

int rf_file_tx_align(rf_file_tx_t* q, uint64_t ts)
{
  pthread_mutex_lock(&q->mutex);

  int64_t nsamples = (int64_t)ts - (int64_t)q->nsamples;

  if (nsamples > 0) {
    rf_file_info(q->id, " - Detected Tx gap of %d samples.\n", nsamples);
    _rf_file_tx_baseband(q, q->zeros, (uint32_t)nsamples);
  }

  pthread_mutex_unlock(&q->mutex);

  return (int)nsamples;
}

int rf_file_tx_baseband(rf_file_tx_t* q, cf_t* buffer, uint32_t nsamples)
{
  int n;

  pthread_mutex_lock(&q->mutex);

  if (q->sample_offset > 0) {
    _rf_file_tx_baseband(q, q->zeros, (uint32_t)q->sample_offset);
    q->sample_offset = 0;
  } else if (q->sample_offset < 0) {
    n = SRSRAN_MIN(-q->sample_offset, nsamples);
    buffer += n;
    nsamples -= n;
    q->sample_offset += n;
    if (nsamples == 0) {
      return n;
    }
  }

  n = _rf_file_tx_baseband(q, buffer, nsamples);

  pthread_mutex_unlock(&q->mutex);

  return n;
}

int rf_file_tx_get_nsamples(rf_file_tx_t* q)
{
  pthread_mutex_lock(&q->mutex);
  int ret = q->nsamples;
  pthread_mutex_unlock(&q->mutex);
  return ret;
}

int rf_file_tx_zeros(rf_file_tx_t* q, uint32_t nsamples)
{
  pthread_mutex_lock(&q->mutex);

  rf_file_info(q->id, " - Tx %d Zeros.\n", nsamples);
  _rf_file_tx_baseband(q, q->zeros, (uint32_t)nsamples);

  pthread_mutex_unlock(&q->mutex);

  return (int)nsamples;
}

bool rf_file_tx_match_freq(rf_file_tx_t* q, uint32_t freq_hz)
{
  bool ret = false;
  if (q) {
    ret = (q->frequency_mhz == 0 || q->frequency_mhz == freq_hz);
  }
  return ret;
}

void rf_file_tx_close(rf_file_tx_t* q)
{
  rf_file_info(q->id, "Closing ...\n");
  pthread_mutex_lock(&q->mutex);
  q->running = false;
  pthread_mutex_unlock(&q->mutex);

  pthread_mutex_destroy(&q->mutex);

  if (q->zeros) {
    free(q->zeros);
  }

  if (q->temp_buffer_convert) {
    free(q->temp_buffer_convert);
  }

  // not touching q->file as we don't know if we need to close it ourselves
}
