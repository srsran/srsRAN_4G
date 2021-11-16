/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rf_file_imp_trx.h"
#include <srsran/phy/utils/vector.h>
#include <stdlib.h>
#include <string.h>

int rf_file_rx_open(rf_file_rx_t* q, rf_file_opts_t opts)
{
  int ret = SRSRAN_ERROR;

  if (q) {
    // Zero object
    memset(q, 0, sizeof(rf_file_rx_t));

    // Copy id
    strncpy(q->id, opts.id, FILE_ID_STRLEN - 1);
    q->id[FILE_ID_STRLEN - 1] = '\0';

    // Assign file
    q->file = opts.file;

    // Configure formats
    q->sample_format = opts.sample_format;
    q->frequency_mhz = opts.frequency_mhz;

    q->temp_buffer = srsran_vec_malloc(FILE_MAX_BUFFER_SIZE);
    if (!q->temp_buffer) {
      fprintf(stderr, "Error: allocating rx buffer\n");
      goto clean_exit;
    }

    q->temp_buffer_convert = srsran_vec_malloc(FILE_MAX_BUFFER_SIZE);
    if (!q->temp_buffer_convert) {
      fprintf(stderr, "Error: allocating rx buffer\n");
      goto clean_exit;
    }

    if (pthread_mutex_init(&q->mutex, NULL)) {
      fprintf(stderr, "Error: creating mutex\n");
      goto clean_exit;
    }

    q->running = true;

    ret = SRSRAN_SUCCESS;
  }

clean_exit:
  return ret;
}

int rf_file_rx_baseband(rf_file_rx_t* q, cf_t* buffer, uint32_t nsamples)
{
  uint32_t sample_sz = sizeof(cf_t);

  int ret = fread(buffer, sample_sz, nsamples, q->file);
  if (ret > 0) {
    return ret;
  } else {
    return SRSRAN_ERROR_RX_EOF;
  }
}

bool rf_file_rx_match_freq(rf_file_rx_t* q, uint32_t freq_hz)
{
  bool ret = false;
  if (q) {
    ret = (q->frequency_mhz == 0 || q->frequency_mhz == freq_hz);
  }
  return ret;
}

void rf_file_rx_close(rf_file_rx_t* q)
{
  rf_file_info(q->id, "Closing ...\n");
  q->running = false;

  if (q->temp_buffer) {
    free(q->temp_buffer);
  }

  if (q->temp_buffer_convert) {
    free(q->temp_buffer_convert);
  }

  q->file = NULL;
}
