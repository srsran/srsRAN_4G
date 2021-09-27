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

#ifndef SRSRAN_RF_IMP_TRX_H
#define SRSRAN_RF_IMP_TRX_H

#include "rf_file_imp.h"
#include "rf_file_imp_trx.h"
#include "rf_helper.h"
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
  uint32_t tx_freq_mhz[SRSRAN_MAX_PORTS];
  uint32_t rx_freq_mhz[SRSRAN_MAX_PORTS];
  bool     tx_used;

  // FILEs
  rf_file_tx_t transmitter[SRSRAN_MAX_PORTS];
  rf_file_rx_t receiver[SRSRAN_MAX_PORTS];

  char id[PARAM_LEN_SHORT];

  // Various sample buffers
  cf_t* buffer_decimation[SRSRAN_MAX_PORTS];
  cf_t* buffer_tx;

  // Rx timestamp
  uint64_t next_rx_ts;

  pthread_mutex_t tx_config_mutex;
  pthread_mutex_t rx_config_mutex;
  pthread_mutex_t decim_mutex;
} rf_file_handler_t;

/*
 * Static methods
 */

static void update_rates(rf_file_handler_t* handler, double srate);

static inline int update_ts(void* h, uint64_t* ts, int nsamples, const char* dir)
{
  int ret = SRSRAN_ERROR;

  if (h && nsamples > 0) {
    rf_file_handler_t* handler = (rf_file_handler_t*)h;

    (*ts) += nsamples;

    srsran_timestamp_t _ts = {};
    srsran_timestamp_init_uint64(&_ts, *ts, handler->base_srate);

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
  perror("Cannot open file-based RF as regular device. Use rf_file_open_file() instead.");
  return SRSRAN_ERROR_INVALID_COMMAND;
}

int rf_file_open_file(void** h, FILE **rx_files, FILE **tx_files, uint32_t nof_channels, uint32_t base_srate)
{
  int ret = SRSRAN_ERROR;

  if (h) {
    *h = NULL;

    rf_file_handler_t* handler = (rf_file_handler_t*)malloc(sizeof(rf_file_handler_t));
    if (!handler) {
      perror("malloc");
      return SRSRAN_ERROR;
    }
    memset(handler, 0, sizeof(rf_file_handler_t));
    *h                        = handler;
    handler->base_srate       = FILE_BASERATE_DEFAULT_HZ; // Sample rate for 100 PRB cell
    handler->rx_gain          = 0.0;
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
      perror("Mutex init");
    }
    if (pthread_mutex_init(&handler->rx_config_mutex, NULL)) {
      perror("Mutex init");
    }
    if (pthread_mutex_init(&handler->decim_mutex, NULL)) {
      perror("Mutex init");
    }

    // base_srate
    handler->base_srate = base_srate;

    // id
    // TODO: set some meaningful ID in handler->id

    // rx_format, tx_format
    // TODO: add other formats
    rx_opts.sample_format = FILERF_TYPE_FC32;
    tx_opts.sample_format = FILERF_TYPE_FC32;

    update_rates(handler, 1.92e6);

    // Create channels
    for (int i = 0; i < handler->nof_channels; i++) {
      if (rx_files != NULL) {
        rx_opts.file = rx_files[i];
        if (rf_file_rx_open(&handler->receiver[i], rx_opts) != SRSRAN_SUCCESS) {
          fprintf(stderr, "[file] Error: opening receiver\n");
          goto clean_exit;
        }
      } else {
        fprintf(stdout, "[file] %s Rx file not specified. Disabling receiver.\n", handler->id);
      }
      if (tx_files != NULL) {
        tx_opts.file = tx_files[i];
        // TX is not implemented yet
        //        if(rf_file_tx_open(&handler->transmitter[i], tx_opts) != SRSRAN_SUCCESS) {
        //          fprintf(stderr, "[file] Error: opening transmitter\n");
        //          goto clean_exit;
        //        }
      } else {
        // no tx_files provided
        fprintf(stdout, "[file] %s Tx file not specified. Disabling transmitter.\n", handler->id);
      }

      if (!handler->transmitter[i].running && !handler->receiver[i].running) {
        fprintf(stderr, "[file] Error: Neither Tx file nor Rx file specified.\n");
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
  void* data_multi[SRSRAN_MAX_PORTS] = {NULL};
  data_multi[0] = data;
  return rf_file_recv_with_time_multi(h, data_multi, nsamples, blocking, secs, frac_secs);
}

int rf_file_recv_with_time_multi(void*    h,
                                 void*    data[SRSRAN_MAX_PORTS],
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
    cf_t* buffers[SRSRAN_MAX_PORTS] = {}; // Buffer pointers, NULL if unmatched
    for (uint32_t i = 0; i < handler->nof_channels; i++) {
      bool mapped = false;

      // Find first matching frequency
      for (uint32_t j = 0; j < handler->nof_channels && !mapped; j++) {
        // Traverse all channels, break if mapped
        if (buffers[j] == NULL && rf_file_rx_match_freq(&handler->receiver[j], handler->rx_freq_mhz[i])) {
          // Available buffer and matched frequency with receiver
          buffers[j] = (cf_t*)data[i];
          mapped     = true;
        }
      }

      // If no matching frequency found; set data to zeros
      if (!mapped && data[i]) {
        memset(data[i], 0, sizeof(cf_t) * nsamples);
      }
    }
    pthread_mutex_unlock(&handler->rx_config_mutex);

    // Protect the access to decim_factor since is a shared variable
    pthread_mutex_lock(&handler->decim_mutex);
    uint32_t decim_factor = handler->decim_factor;
    pthread_mutex_unlock(&handler->decim_mutex);

    uint32_t nbytes            = NSAMPLES2NBYTES(nsamples * decim_factor);
    uint32_t nsamples_baserate = nsamples * decim_factor;

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

    // copy from rx buffer as many samples as requested into provided buffer
    bool    completed               = false;
    int32_t count[SRSRAN_MAX_PORTS] = {};
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
            dst[i] = avg;
          }
        }
      }
    }

    // Set gain
    float scale = srsran_convert_dB_to_amplitude(handler->rx_gain);
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
  // Not implemented
  fprintf(stderr, "Error: rf_file_send_timed_multi not implemented.\n");
  return SRSRAN_ERROR;
}

#endif
