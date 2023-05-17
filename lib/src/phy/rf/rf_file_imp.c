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

#include "rf_file_imp.h"
#include "rf_file_imp_trx.h"
#include "rf_helper.h"
#include <errno.h>
#include <math.h>
#include <srsran/phy/common/phy_common.h>
#include <srsran/phy/common/timestamp.h>
#include <srsran/phy/utils/vector.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  // Common attributes
  char*            devname;
  srsran_rf_info_t info;
  uint32_t         nof_channels;

  // RF State
  uint32_t srate; // radio rate configured by upper layers
  uint32_t base_srate;
  uint32_t decim_factor; // decimation factor between base_srate used on transport on radio's rate
  double   rx_gain;
  uint32_t tx_freq_mhz[SRSRAN_MAX_CHANNELS];
  uint32_t rx_freq_mhz[SRSRAN_MAX_CHANNELS];
  bool     tx_off;
  char     id[RF_PARAM_LEN];

  // FILEs
  rf_file_tx_t transmitter[SRSRAN_MAX_CHANNELS];
  rf_file_rx_t receiver[SRSRAN_MAX_CHANNELS];
  bool         close_files;

  // Various sample buffers
  cf_t* buffer_decimation[SRSRAN_MAX_CHANNELS];
  cf_t* buffer_tx;

  // Rx timestamp
  uint64_t next_rx_ts;

  pthread_mutex_t tx_config_mutex;
  pthread_mutex_t rx_config_mutex;
  pthread_mutex_t decim_mutex;
  pthread_mutex_t rx_gain_mutex;
} rf_file_handler_t;

/*
 * Static methods
 */

static void update_rates(rf_file_handler_t* handler, double srate);

void rf_file_info(char* id, const char* format, ...)
{
#if VERBOSE
  struct timeval t;
  gettimeofday(&t, NULL);
  va_list args;
  va_start(args, format);
  printf("[%s@%02ld.%06ld] ", id ? id : "file", t.tv_sec % 10, t.tv_usec);
  vprintf(format, args);
  va_end(args);
#else  /* VERBOSE */
  // Do nothing
#endif /* VERBOSE */
}

void rf_file_error(char* id, const char* format, ...)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}

static inline int update_ts(void* h, uint64_t* ts, int nsamples, const char* dir)
{
  int ret = SRSRAN_ERROR;

  if (h && nsamples > 0) {
    rf_file_handler_t* handler = (rf_file_handler_t*)h;

    (*ts) += nsamples;

    srsran_timestamp_t _ts = {};
    srsran_timestamp_init_uint64(&_ts, *ts, handler->base_srate);
    rf_file_info(
        handler->id, "    -> next %s time after %d samples: %d + %.3f\n", dir, nsamples, _ts.full_secs, _ts.frac_secs);

    ret = SRSRAN_SUCCESS;
  }

  return ret;
}

int rf_file_handle_error(char* id, const char* text)
{
  // Not implemented
  return SRSRAN_ERROR;
}

/*
 * Public methods
 */

const char* rf_file_devname(void* h)
{
  return DEVNAME_FILE;
}

int rf_file_start_rx_stream(void* h, bool now)
{
  return SRSRAN_SUCCESS;
}

int rf_file_stop_rx_stream(void* h)
{
  return SRSRAN_SUCCESS;
}

void rf_file_flush_buffer(void* h)
{
  printf("%s\n", __FUNCTION__);
}

bool rf_file_has_rssi(void* h)
{
  return false;
}

float rf_file_get_rssi(void* h)
{
  return 0.0;
}

void rf_file_suppress_stdout(void* h)
{
  // do nothing
}

void rf_file_register_error_handler(void* h, srsran_rf_error_handler_t error_handler, void* arg)
{
  // do nothing
}

int rf_file_open(char* args, void** h)
{
  return rf_file_open_multi(args, h, 1);
}

int rf_file_open_multi(char* args, void** h, uint32_t nof_channels)
{
  int ret = SRSRAN_ERROR;

  FILE* rx_files[SRSRAN_MAX_CHANNELS] = {NULL};
  FILE* tx_files[SRSRAN_MAX_CHANNELS] = {NULL};

  if (h && nof_channels <= SRSRAN_MAX_CHANNELS) {
    uint32_t base_srate = FILE_BASERATE_DEFAULT_HZ;

    // parse args
    if (args && strlen(args)) {
      // base_srate
      parse_uint32(args, "base_srate", -1, &base_srate);
    } else {
      fprintf(stderr, "[file] Error: RF device args are required for file-based no-RF module\n");
      goto clean_exit;
    }

    for (int i = 0; i < nof_channels; i++) {
      // rx_file
      char rx_file[RF_PARAM_LEN] = {};
      parse_string(args, "rx_file", i, rx_file);

      // tx_file
      char tx_file[RF_PARAM_LEN] = {};
      parse_string(args, "tx_file", i, tx_file);

      // initialize transmitter
      if (strlen(tx_file) != 0) {
        tx_files[i] = fopen(tx_file, "wb");
        if (tx_files[i] == NULL) {
          fprintf(stderr, "[file] Error: opening tx_file%d: %s; %s\n", i, tx_file, strerror(errno));
          goto clean_exit;
        }
      }

      // initialize receiver
      if (strlen(rx_file) != 0) {
        rx_files[i] = fopen(rx_file, "rb");
        if (rx_files[i] == NULL) {
          fprintf(stderr, "[file] Error: opening rx_file%d: %s; %s\n", i, rx_file, strerror(errno));
          goto clean_exit;
        }
      }
    }

    // defer further initialization to open_file method
    ret = rf_file_open_file(h, rx_files, tx_files, nof_channels, base_srate);
    if (ret != SRSRAN_SUCCESS) {
      goto clean_exit;
    }

    // add flag to close all files when closing device
    rf_file_handler_t* handler = (rf_file_handler_t*)(*h);
    handler->close_files       = true;
    return ret;
  }

clean_exit:
  for (int i = 0; i < nof_channels; i++) {
    if (rx_files[i] != NULL) {
      fclose(rx_files[i]);
    }
    if (tx_files[i] != NULL) {
      fclose(tx_files[i]);
    }
  }
  return ret;
}

int rf_file_open_file(void** h, FILE** rx_files, FILE** tx_files, uint32_t nof_channels, uint32_t base_srate)
{
  int ret = SRSRAN_ERROR;

  if (h) {
    *h = NULL;

    rf_file_handler_t* handler = (rf_file_handler_t*)malloc(sizeof(rf_file_handler_t));
    if (!handler) {
      fprintf(stderr, "malloc: %s\n", strerror(errno));
      return SRSRAN_ERROR;
    }
    memset(handler, 0, sizeof(rf_file_handler_t));
    *h                        = handler;
    handler->base_srate       = base_srate;
    handler->info.max_rx_gain = FILE_MAX_GAIN_DB;
    handler->info.min_rx_gain = FILE_MIN_GAIN_DB;
    handler->info.max_tx_gain = FILE_MAX_GAIN_DB;
    handler->info.min_tx_gain = FILE_MIN_GAIN_DB;
    handler->nof_channels     = nof_channels;
    strcpy(handler->id, "file\0");

    rf_file_opts_t rx_opts = {};
    rf_file_opts_t tx_opts = {};
    tx_opts.id             = handler->id;
    rx_opts.id             = handler->id;

    if (pthread_mutex_init(&handler->tx_config_mutex, NULL)) {
      fprintf(stderr, "Mutex init: %s\n", strerror(errno));
    }
    if (pthread_mutex_init(&handler->rx_config_mutex, NULL)) {
      fprintf(stderr, "Mutex init: %s\n", strerror(errno));
    }
    if (pthread_mutex_init(&handler->decim_mutex, NULL)) {
      fprintf(stderr, "Mutex init: %s\n", strerror(errno));
    }
    if (pthread_mutex_init(&handler->rx_gain_mutex, NULL)) {
      fprintf(stderr, "Mutex init: %s\n", strerror(errno));
    }

    pthread_mutex_lock(&handler->rx_gain_mutex);
    handler->rx_gain = 0.0;
    pthread_mutex_unlock(&handler->rx_gain_mutex);

    // id
    // TODO: set some meaningful ID in handler->id

    // rx_format, tx_format
    // TODO: add other formats
    rx_opts.sample_format = FILERF_TYPE_FC32;
    tx_opts.sample_format = FILERF_TYPE_FC32;

    update_rates(handler, 1.92e6);

    // Create channels
    for (int i = 0; i < handler->nof_channels; i++) {
      if (rx_files != NULL && rx_files[i] != NULL) {
        rx_opts.file = rx_files[i];
        if (rf_file_rx_open(&handler->receiver[i], rx_opts) != SRSRAN_SUCCESS) {
          fprintf(stderr, "[file] Error: opening receiver\n");
          goto clean_exit;
        }
      } else {
        // no rx_files provided
        fprintf(stdout, "[file] %s rx channel %d not specified. Disabling receiver.\n", handler->id, i);
      }
      if (tx_files != NULL && tx_files[i] != NULL) {
        tx_opts.file = tx_files[i];
        if (rf_file_tx_open(&handler->transmitter[i], tx_opts) != SRSRAN_SUCCESS) {
          fprintf(stderr, "[file] Error: opening transmitter\n");
          goto clean_exit;
        }
      } else {
        // no tx_files provided
        fprintf(stdout, "[file] %s tx channel %d not specified. Disabling transmitter.\n", handler->id, i);
        handler->tx_off = true;
      }

      if (!handler->transmitter[i].running && !handler->receiver[i].running) {
        fprintf(stderr, "[file] Error: Neither tx nor rx specificed for channel %d.\n", i);
        goto clean_exit;
      }
    }

    // Create decimation and overflow buffer
    for (uint32_t i = 0; i < handler->nof_channels; i++) {
      handler->buffer_decimation[i] = srsran_vec_malloc(FILE_MAX_BUFFER_SIZE);
      if (!handler->buffer_decimation[i]) {
        fprintf(stderr, "Error: allocating decimation buffer\n");
        goto clean_exit;
      }
    }

    handler->buffer_tx = srsran_vec_malloc(FILE_MAX_BUFFER_SIZE);
    if (!handler->buffer_tx) {
      fprintf(stderr, "Error: allocating tx buffer\n");
      goto clean_exit;
    }

    ret = SRSRAN_SUCCESS;

  clean_exit:
    if (ret) {
      rf_file_close(handler);
    }
  }
  return ret;
}

int rf_file_close(void* h)
{
  rf_file_handler_t* handler = (rf_file_handler_t*)h;

  rf_file_info(handler->id, "Closing ...\n");

  // close receiver+transmitter and release related resources (except for the file handles)
  for (int i = 0; i < handler->nof_channels; i++) {
    rf_file_tx_close(&handler->transmitter[i]);
    rf_file_rx_close(&handler->receiver[i]);
  }

  // release other resources
  for (uint32_t i = 0; i < handler->nof_channels; i++) {
    if (handler->buffer_decimation[i]) {
      free(handler->buffer_decimation[i]);
    }
  }

  if (handler->buffer_tx) {
    free(handler->buffer_tx);
  }

  pthread_mutex_destroy(&handler->tx_config_mutex);
  pthread_mutex_destroy(&handler->rx_config_mutex);
  pthread_mutex_destroy(&handler->decim_mutex);
  pthread_mutex_destroy(&handler->rx_gain_mutex);

  // now close the files if we opened them ourselves
  if (handler->close_files) {
    for (int i = 0; i < handler->nof_channels; i++) {
      if (handler->receiver[i].file != NULL) {
        fclose(handler->receiver[i].file);
      }
      if (handler->transmitter[i].file != NULL) {
        fclose(handler->transmitter[i].file);
      }
    }
  }

  // Free all
  free(handler);

  return SRSRAN_SUCCESS;
}

void update_rates(rf_file_handler_t* handler, double srate)
{
  pthread_mutex_lock(&handler->decim_mutex);
  if (handler) {
    // Decimation must be full integer
    if (((uint64_t)handler->base_srate % (uint64_t)srate) == 0) {
      handler->srate        = (uint32_t)srate;
      handler->decim_factor = handler->base_srate / handler->srate;
    } else {
      fprintf(stderr,
              "Error: couldn't update sample rate. %.2f is not divisible by %.2f\n",
              srate / 1e6,
              handler->base_srate / 1e6);
    }
    printf("Current sample rate is %.2f MHz with a base rate of %.2f MHz (x%d decimation)\n",
           handler->srate / 1e6,
           handler->base_srate / 1e6,
           handler->decim_factor);
  }
  pthread_mutex_unlock(&handler->decim_mutex);
}

double rf_file_set_rx_srate(void* h, double srate)
{
  double ret = 0.0;
  if (h) {
    rf_file_handler_t* handler = (rf_file_handler_t*)h;
    update_rates(handler, srate);
    ret = handler->srate;
  }
  return ret;
}

int rf_file_set_rx_gain(void* h, double gain)
{
  double ret = 0.0;
  if (h) {
    rf_file_handler_t* handler = (rf_file_handler_t*)h;
    handler->rx_gain           = gain;
    ret                        = gain;
  }
  return ret;
}

int rf_file_set_rx_gain_ch(void* h, uint32_t ch, double gain)
{
  return rf_file_set_rx_gain(h, gain);
}

int rf_file_set_tx_gain(void* h, double gain)
{
  return 0.0;
}

int rf_file_set_tx_gain_ch(void* h, uint32_t ch, double gain)
{
  return rf_file_set_tx_gain(h, gain);
}

double rf_file_get_rx_gain(void* h)
{
  double ret = 0.0;
  if (h) {
    rf_file_handler_t* handler = (rf_file_handler_t*)h;
    ret                        = handler->rx_gain;
  }
  return ret;
}

double rf_file_get_tx_gain(void* h)
{
  return 0.0;
}

srsran_rf_info_t* rf_file_get_info(void* h)
{
  srsran_rf_info_t* info = NULL;
  if (h) {
    rf_file_handler_t* handler = (rf_file_handler_t*)h;
    info                       = &handler->info;
  }
  return info;
}

double rf_file_set_rx_freq(void* h, uint32_t ch, double freq)
{
  double ret = NAN;
  if (h) {
    rf_file_handler_t* handler = (rf_file_handler_t*)h;
    pthread_mutex_lock(&handler->rx_config_mutex);
    if (ch < handler->nof_channels && isnormal(freq) && freq > 0.0) {
      handler->rx_freq_mhz[ch] = (uint32_t)(freq / 1e6);
      ret                      = freq;
    }
    pthread_mutex_unlock(&handler->rx_config_mutex);
  }
  return ret;
}

double rf_file_set_tx_srate(void* h, double srate)
{
  double ret = 0.0;
  if (h) {
    rf_file_handler_t* handler = (rf_file_handler_t*)h;
    update_rates(handler, srate);
    ret = srate;
  }
  return ret;
}

double rf_file_set_tx_freq(void* h, uint32_t ch, double freq)
{
  double ret = NAN;
  if (h) {
    rf_file_handler_t* handler = (rf_file_handler_t*)h;
    pthread_mutex_lock(&handler->tx_config_mutex);
    if (ch < handler->nof_channels && isnormal(freq) && freq > 0.0) {
      handler->tx_freq_mhz[ch] = (uint32_t)(freq / 1e6);
      ret                      = freq;
    }
    pthread_mutex_unlock(&handler->tx_config_mutex);
  }
  return ret;
}

void rf_file_get_time(void* h, time_t* secs, double* frac_secs)
{
  if (h) {
    if (secs) {
      *secs = 0;
    }

    if (frac_secs) {
      *frac_secs = 0;
    }
  }
}

int rf_file_recv_with_time(void* h, void* data, uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs)
{
  return rf_file_recv_with_time_multi(h, &data, nsamples, blocking, secs, frac_secs);
}

int rf_file_recv_with_time_multi(void*    h,
                                 void**   data,
                                 uint32_t nsamples,
                                 bool     blocking,
                                 time_t*  secs,
                                 double*  frac_secs)
{
  int ret = SRSRAN_ERROR;

  if (h) {
    rf_file_handler_t* handler = (rf_file_handler_t*)h;

    // Map ports to data buffers according to the selected frequencies
    pthread_mutex_lock(&handler->rx_config_mutex);
    bool  mapped[SRSRAN_MAX_CHANNELS]  = {}; // Mapped mask, set to true when the physical channel is used
    cf_t* buffers[SRSRAN_MAX_CHANNELS] = {}; // Buffer pointers, NULL if unmatched

    // For each logical channel...
    for (uint32_t logical = 0; logical < handler->nof_channels; logical++) {
      bool unmatched = true;

      // For each physical channel...
      for (uint32_t physical = 0; physical < handler->nof_channels; physical++) {
        // Consider a match if the physical channel is NOT mapped and the frequency match
        if (!mapped[physical] && rf_file_rx_match_freq(&handler->receiver[physical], handler->rx_freq_mhz[logical])) {
          // Not mapped and matched frequency with receiver
          buffers[physical] = (cf_t*)data[logical];
          mapped[physical]  = true;
          unmatched         = false;
          break;
        }
      }

      // If no matching frequency found; set data to zeros
      if (unmatched) {
        srsran_vec_zero(data[logical], nsamples);
      }
    }
    pthread_mutex_unlock(&handler->rx_config_mutex);

    // Protect the access to decim_factor since is a shared variable
    pthread_mutex_lock(&handler->decim_mutex);
    uint32_t decim_factor = handler->decim_factor;
    pthread_mutex_unlock(&handler->decim_mutex);

    uint32_t nbytes            = NSAMPLES2NBYTES(nsamples * decim_factor);
    uint32_t nsamples_baserate = nsamples * decim_factor;

    rf_file_info(handler->id, "Rx %d samples (%d B)\n", nsamples, nbytes);

    // set timestamp for this reception
    if (secs != NULL && frac_secs != NULL) {
      srsran_timestamp_t ts = {};
      srsran_timestamp_init_uint64(&ts, handler->next_rx_ts, handler->base_srate);
      *secs      = ts.full_secs;
      *frac_secs = ts.frac_secs;
    }

    // return if receiver is turned off
    if (!handler->receiver[0].running) {
      update_ts(handler, &handler->next_rx_ts, nsamples_baserate, "rx");
      return nsamples;
    }

    // Check available buffer size
    if (nbytes > FILE_MAX_BUFFER_SIZE) {
      fprintf(stderr,
              "[file] Error: Trying to receive %d B but buffer is only %zu B at channel %d.\n",
              nbytes,
              FILE_MAX_BUFFER_SIZE,
              0);
      goto clean_exit;
    }

    // receive samples
    srsran_timestamp_t ts_tx = {}, ts_rx = {};
    srsran_timestamp_init_uint64(&ts_tx, handler->transmitter[0].nsamples, handler->base_srate);
    srsran_timestamp_init_uint64(&ts_rx, handler->next_rx_ts, handler->base_srate);
    rf_file_info(handler->id, " - next rx time: %d + %.3f\n", ts_rx.full_secs, ts_rx.frac_secs);
    rf_file_info(handler->id, " - next tx time: %d + %.3f\n", ts_tx.full_secs, ts_tx.frac_secs);

    // check for tx gap if we're also transmitting on this radio
    for (int i = 0; i < handler->nof_channels; i++) {
      if (handler->transmitter[i].running) {
        rf_file_tx_align(&handler->transmitter[i], handler->next_rx_ts + nsamples_baserate);
      }
    }

    // copy from rx buffer as many samples as requested into provided buffer
    bool    completed                  = false;
    int32_t count[SRSRAN_MAX_CHANNELS] = {};
    while (!completed) {
      uint32_t completed_count = 0;

      // Iterate channels
      for (uint32_t i = 0; i < handler->nof_channels; i++) {
        cf_t* ptr = (decim_factor != 1 || buffers[i] == NULL) ? handler->buffer_decimation[i] : buffers[i];

        // Completed condition
        if (count[i] < nsamples_baserate && handler->receiver[i].running) {
          // Keep receiving
          int32_t n = rf_file_rx_baseband(&handler->receiver[i], &ptr[count[i]], nsamples_baserate - count[i]);
          if (n > 0) {
            // No error
            count[i] += n;
          } else {
            if (n != SRSRAN_ERROR_RX_EOF) {
              // Other error, exit
              fprintf(stderr, "Error: receiving data.\n");
            }
            ret = n;
            goto clean_exit;
          }
        } else {
          // Completed, count it
          completed_count++;
        }
      }

      // Check if all channels are completed
      completed = (completed_count == handler->nof_channels);
    }
    rf_file_info(handler->id, " - read %d samples.\n", NBYTES2NSAMPLES(nbytes));

    // decimate if needed
    if (decim_factor != 1) {
      for (uint32_t c = 0; c < handler->nof_channels; c++) {
        // skip if buffer is not available
        if (buffers[c]) {
          cf_t* dst = buffers[c];
          cf_t* ptr = handler->buffer_decimation[c];

          for (uint32_t i = 0, n = 0; i < nsamples; i++) {
            // Averaging decimation
            cf_t avg = 0.0f;
            for (int j = 0; j < decim_factor; j++, n++) {
              avg += ptr[n];
            }
            dst[i] = avg; // divide by decim_factor later via scale
          }

          rf_file_info(handler->id,
                       "  - re-adjust bytes due to %dx decimation %d --> %d samples)\n",
                       decim_factor,
                       nsamples_baserate,
                       nsamples);
        }
      }
    }

    // Set gain
    pthread_mutex_lock(&handler->rx_gain_mutex);
    float scale = srsran_convert_dB_to_amplitude(handler->rx_gain);
    pthread_mutex_unlock(&handler->rx_gain_mutex);
    // scale shall also incorporate decim_factor
    scale = scale / decim_factor;
    for (uint32_t c = 0; c < handler->nof_channels; c++) {
      if (buffers[c]) {
        srsran_vec_sc_prod_cfc(buffers[c], scale, buffers[c], nsamples);
      }
    }

    // update rx time
    update_ts(handler, &handler->next_rx_ts, nsamples_baserate, "rx");
  }

  ret = nsamples;

clean_exit:

  return ret;
}

int rf_file_send_timed(void*  h,
                       void*  data,
                       int    nsamples,
                       time_t secs,
                       double frac_secs,
                       bool   has_time_spec,
                       bool   blocking,
                       bool   is_start_of_burst,
                       bool   is_end_of_burst)
{
  void* _data[4] = {data, NULL, NULL, NULL};

  return rf_file_send_timed_multi(
      h, _data, nsamples, secs, frac_secs, has_time_spec, blocking, is_start_of_burst, is_end_of_burst);
}

int rf_file_send_timed_multi(void*  h,
                             void*  data[4],
                             int    nsamples,
                             time_t secs,
                             double frac_secs,
                             bool   has_time_spec,
                             bool   blocking,
                             bool   is_start_of_burst,
                             bool   is_end_of_burst)
{
  int ret = SRSRAN_ERROR;

  if (h && data && nsamples > 0) {
    rf_file_handler_t* handler = (rf_file_handler_t*)h;

    // Map ports to data buffers according to the selected frequencies
    pthread_mutex_lock(&handler->tx_config_mutex);
    bool  mapped[SRSRAN_MAX_CHANNELS]  = {}; // Mapped mask, set to true when the physical channel is used
    cf_t* buffers[SRSRAN_MAX_CHANNELS] = {}; // Buffer pointers, NULL if unmatched or zero transmission

    // For each logical channel...
    for (uint32_t logical = 0; logical < handler->nof_channels; logical++) {
      // For each physical channel...
      for (uint32_t physical = 0; physical < handler->nof_channels; physical++) {
        // Consider a match if the physical channel is NOT mapped and the frequency match
        if (!mapped[physical] &&
            rf_file_tx_match_freq(&handler->transmitter[physical], handler->tx_freq_mhz[logical])) {
          // Not mapped and matched frequency with receiver
          buffers[physical] = (cf_t*)data[logical];
          mapped[physical]  = true;
          break;
        }
      }
    }
    pthread_mutex_unlock(&handler->tx_config_mutex);

    // Protect the access to decim_factor since is a shared variable
    pthread_mutex_lock(&handler->decim_mutex);
    uint32_t decim_factor = handler->decim_factor;
    pthread_mutex_unlock(&handler->decim_mutex);

    uint32_t nbytes            = NSAMPLES2NBYTES(nsamples);
    uint32_t nsamples_baseband = nsamples * decim_factor;
    uint32_t nbytes_baseband   = NSAMPLES2NBYTES(nsamples_baseband);
    if (nbytes_baseband > FILE_MAX_BUFFER_SIZE) {
      fprintf(stderr, "Error: trying to transmit too many samples (%d > %zu).\n", nbytes, FILE_MAX_BUFFER_SIZE);
      goto clean_exit;
    }

    rf_file_info(handler->id, "Tx %d samples (%d B)\n", nsamples, nbytes);

    // return if transmitter is switched off
    if (handler->tx_off) {
      return SRSRAN_SUCCESS;
    }

    // check if this is a tx in the future
    if (has_time_spec) {
      rf_file_info(handler->id, "    - tx time: %d + %.3f\n", secs, frac_secs);

      srsran_timestamp_t ts = {};
      srsran_timestamp_init(&ts, secs, frac_secs);
      uint64_t tx_ts              = srsran_timestamp_uint64(&ts, handler->base_srate);
      int      num_tx_gap_samples = 0;

      for (int i = 0; i < handler->nof_channels; i++) {
        if (handler->transmitter[i].running) {
          num_tx_gap_samples = rf_file_tx_align(&handler->transmitter[i], tx_ts);
        }
      }

      if (num_tx_gap_samples < 0) {
        fprintf(stderr,
                "[file] Error: tx time is %.3f ms in the past (%" PRIu64 " < %" PRIu64 ")\n",
                -1000.0 * num_tx_gap_samples / handler->base_srate,
                tx_ts,
                handler->transmitter[0].nsamples);
        goto clean_exit;
      }
    }

    // Send base-band samples
    for (int i = 0; i < handler->nof_channels; i++) {
      if (buffers[i] != NULL) {
        // Select buffer pointer depending on interpolation
        cf_t* buf = (decim_factor != 1) ? handler->buffer_tx : buffers[i];

        // Interpolate if required
        if (decim_factor != 1) {
          rf_file_info(handler->id,
                       "  - re-adjust bytes due to %dx interpolation %d --> %d samples)\n",
                       decim_factor,
                       nsamples,
                       nsamples_baseband);

          int   n   = 0;
          cf_t* src = buffers[i];
          for (int k = 0; k < nsamples; k++) {
            // perform zero order hold
            for (int j = 0; j < decim_factor; j++, n++) {
              buf[n] = src[k];
            }
          }

          if (nsamples_baseband != n) {
            fprintf(stderr,
                    "Number of tx samples (%d) does not match with number of interpolated samples (%d)\n",
                    nsamples_baseband,
                    n);
            goto clean_exit;
          }
        }

        int n = rf_file_tx_baseband(&handler->transmitter[i], buf, nsamples_baseband);
        if (n == SRSRAN_ERROR) {
          goto clean_exit;
        }
      } else {
        int n = rf_file_tx_zeros(&handler->transmitter[i], nsamples_baseband);
        if (n == SRSRAN_ERROR) {
          goto clean_exit;
        }
      }
    }
  }

  ret = SRSRAN_SUCCESS;

clean_exit:

  return ret;
}

rf_dev_t srsran_rf_dev_file = {"file",
                               rf_file_devname,
                               rf_file_start_rx_stream,
                               rf_file_stop_rx_stream,
                               rf_file_flush_buffer,
                               rf_file_has_rssi,
                               rf_file_get_rssi,
                               rf_file_suppress_stdout,
                               rf_file_register_error_handler,
                               rf_file_open,
                               .srsran_rf_open_multi = rf_file_open_multi,
                               rf_file_close,
                               rf_file_set_rx_srate,
                               rf_file_set_rx_gain,
                               rf_file_set_rx_gain_ch,
                               rf_file_set_tx_gain,
                               rf_file_set_tx_gain_ch,
                               rf_file_get_rx_gain,
                               rf_file_get_tx_gain,
                               rf_file_get_info,
                               rf_file_set_rx_freq,
                               rf_file_set_tx_srate,
                               rf_file_set_tx_freq,
                               rf_file_get_time,
                               NULL,
                               rf_file_recv_with_time,
                               rf_file_recv_with_time_multi,
                               rf_file_send_timed,
                               .srsran_rf_send_timed_multi = rf_file_send_timed_multi};
