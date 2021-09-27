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

#ifndef SRSRAN_RF_FILE_IMP_TRX_H
#define SRSRAN_RF_FILE_IMP_TRX_H

#include "srsran/config.h"
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Definitions */
#define VERBOSE (0)
#define NSAMPLES2NBYTES(X) (((uint32_t)(X)) * sizeof(cf_t))
#define NBYTES2NSAMPLES(X) ((X) / sizeof(cf_t))
#define FILE_MAX_BUFFER_SIZE (NSAMPLES2NBYTES(3072000)) // 10 subframes at 20 MHz
#define FILE_TIMEOUT_MS (1000)
#define FILE_BASERATE_DEFAULT_HZ (23040000)
#define FILE_ID_STRLEN 16
#define FILE_MAX_GAIN_DB (30.0f)
#define FILE_MIN_GAIN_DB (0.0f)

typedef enum { FILERF_TYPE_FC32 = 0, FILERF_TYPE_SC16 } rf_file_format_t;

typedef struct {
  char             id[FILE_ID_STRLEN];
  rf_file_format_t sample_format;
  FILE*            file;
  uint64_t         nsamples;
  bool             running;
  pthread_mutex_t  mutex;
  cf_t*            zeros;
  void*            temp_buffer_convert;
  uint32_t         frequency_hz_mhz;
} rf_file_tx_t;

typedef struct {
  char             id[FILE_ID_STRLEN];
  rf_file_format_t sample_format;
  FILE*            file;
  uint64_t         nsamples;
  bool             running;
  pthread_t        thread;
  pthread_mutex_t  mutex;
  cf_t*            temp_buffer;
  void*            temp_buffer_convert;
  uint32_t         frequency_mhz;
} rf_file_rx_t;

typedef struct {
  const char*      id;
  rf_file_format_t sample_format;
  FILE*            file;
  uint32_t         frequency_mhz;
} rf_file_opts_t;

/*
 * Common functions
 */
SRSRAN_API int rf_file_handle_error(char* id, const char* text);

/*
 * Receiver functions
 */
SRSRAN_API int rf_file_rx_open(rf_file_rx_t* q, rf_file_opts_t opts);

SRSRAN_API int rf_file_rx_baseband(rf_file_rx_t* q, cf_t* buffer, uint32_t nsamples);

SRSRAN_API bool rf_file_rx_match_freq(rf_file_rx_t* q, uint32_t freq_hz);

SRSRAN_API void rf_file_rx_close(rf_file_rx_t* q);

#endif // SRSRAN_RF_FILE_IMP_TRX_H
