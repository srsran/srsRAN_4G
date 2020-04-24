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

#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "rf_helper.h"
#include "rf_soapy_imp.h"
#include "srslte/srslte.h"

#include <SoapySDR/Device.h>
#include <SoapySDR/Formats.h>
#include <SoapySDR/Logger.h>
#include <SoapySDR/Time.h>
#include <SoapySDR/Version.h>
#include <Types.h>

#define HAVE_ASYNC_THREAD 0

#define STOP_STREAM_BEFORE_RATE_CHANGE 0
#define USE_TX_MTU 0
#define SET_RF_BW 0

#define PRINT_RX_STATS 0
#define PRINT_TX_STATS 0

typedef struct {
  char*            devname;
  SoapySDRKwargs   args;
  SoapySDRDevice*  device;
  SoapySDRRange*   ranges;
  SoapySDRStream*  rxStream;
  SoapySDRStream*  txStream;
  bool             tx_stream_active;
  bool             rx_stream_active;
  srslte_rf_info_t info;
  double           tx_rate;
  size_t           rx_mtu, tx_mtu;
  size_t           num_rx_channels;
  size_t           num_tx_channels;

  srslte_rf_error_handler_t soapy_error_handler;
  void*                     soapy_error_handler_arg;

  bool      async_thread_running;
  pthread_t async_thread;

  uint32_t num_time_errors;
  uint32_t num_lates;
  uint32_t num_overflows;
  uint32_t num_underflows;
  uint32_t num_other_errors;
  uint32_t num_stream_curruption;
} rf_soapy_handler_t;

cf_t zero_mem[64 * 1024];

static void log_overflow(rf_soapy_handler_t* h)
{
  if (h->soapy_error_handler) {
    srslte_rf_error_t error;
    bzero(&error, sizeof(srslte_rf_error_t));
    error.type = SRSLTE_RF_ERROR_OVERFLOW;
    h->soapy_error_handler(h->soapy_error_handler_arg, error);
  } else {
    h->num_overflows++;
  }
}

static void log_late(rf_soapy_handler_t* h, bool is_rx)
{
  if (h->soapy_error_handler) {
    srslte_rf_error_t error;
    bzero(&error, sizeof(srslte_rf_error_t));
    error.opt  = is_rx ? 1 : 0;
    error.type = SRSLTE_RF_ERROR_LATE;
    h->soapy_error_handler(h->soapy_error_handler_arg, error);
  } else {
    h->num_lates++;
  }
}

static void log_underflow(rf_soapy_handler_t* h)
{
  if (h->soapy_error_handler) {
    srslte_rf_error_t error;
    bzero(&error, sizeof(srslte_rf_error_t));
    error.type = SRSLTE_RF_ERROR_UNDERFLOW;
    h->soapy_error_handler(h->soapy_error_handler_arg, error);
  } else {
    h->num_underflows++;
  }
}

#if HAVE_ASYNC_THREAD
static void* async_thread(void* h)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;

  while (handler->async_thread_running) {
    int        ret       = 0;
    size_t     chanMask  = 0;
    int        flags     = 0;
    const long timeoutUs = 400000; // arbitrarily chosen
    long long  timeNs;

    ret = SoapySDRDevice_readStreamStatus(handler->device, handler->txStream, &chanMask, &flags, &timeNs, timeoutUs);
    if (ret == SOAPY_SDR_TIME_ERROR) {
      // this is a late
      log_late(handler, false);
    } else if (ret == SOAPY_SDR_UNDERFLOW) {
      log_underflow(handler);
    } else if (ret == SOAPY_SDR_OVERFLOW) {
      log_overflow(handler);
    } else if (ret == SOAPY_SDR_TIMEOUT) {
      // this is a timeout of the readStreamStatus call, ignoring it ..
    } else if (ret == SOAPY_SDR_NOT_SUPPORTED) {
      // stopping async thread
      ERROR("Receiving async metadata not supported by device. Exiting thread.\n");
      handler->async_thread_running = false;
    } else {
      ERROR("Error while receiving aync metadata: %s (%d), flags=%d, channel=%zu, timeNs=%lld\n",
            SoapySDR_errToStr(ret),
            ret,
            flags,
            chanMask,
            timeNs);
      handler->async_thread_running = false;
    }
  }
  return NULL;
}
#endif

int soapy_error(void* h)
{
  return 0;
}

void rf_soapy_get_freq_range(void* h)
{
  // not supported
}

void rf_soapy_suppress_handler(const char* x)
{
  // not supported
}

void rf_soapy_msg_handler(const char* msg)
{
  // not supported
}

void rf_soapy_suppress_stdout(void* h)
{
  // not supported
}

void rf_soapy_register_error_handler(void* h, srslte_rf_error_handler_t new_handler, void* arg)
{
  rf_soapy_handler_t* handler      = (rf_soapy_handler_t*)h;
  handler->soapy_error_handler     = new_handler;
  handler->soapy_error_handler_arg = arg;
}

const char* rf_soapy_devname(void* h)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;
  return handler->devname;
}

static bool rf_soapy_rx_wait_lo_locked(rf_soapy_handler_t* handler)
{
  char* ret = SoapySDRDevice_readChannelSensor(handler->device, SOAPY_SDR_RX, 0, "lo_locked");
  if (ret != NULL) {
    return (strcmp(ret, "true") == 0 ? true : false);
  }
  return true;
}

void rf_soapy_calibrate_tx(void* h)
{
  rf_soapy_handler_t* handler   = (rf_soapy_handler_t*)h;
  double              actual_bw = SoapySDRDevice_getBandwidth(handler->device, SOAPY_SDR_TX, 0);
  char                str_buf[25];
  snprintf(str_buf, sizeof(str_buf), "%f", actual_bw);
  str_buf[24] = 0;
  SoapySDRDevice_writeSetting(handler->device, "CALIBRATE_TX", str_buf);
}

int rf_soapy_start_rx_stream(void* h, bool now)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;
  if (handler->rx_stream_active == false) {
    if (SoapySDRDevice_activateStream(handler->device, handler->rxStream, 0, 0, 0) != 0) {
      printf("Error starting Rx streaming.\n");
      return SRSLTE_ERROR;
    }

    handler->rx_stream_active = true;
  }
  return SRSLTE_SUCCESS;
}

int rf_soapy_start_tx_stream(void* h)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;
  if (handler->tx_stream_active == false) {
    if (SoapySDRDevice_activateStream(handler->device, handler->txStream, 0, 0, 0) != 0) {
      printf("Error starting Tx streaming.\n");
      return SRSLTE_ERROR;
    }
    handler->tx_stream_active = true;
  }
  return SRSLTE_SUCCESS;
}

int rf_soapy_stop_rx_stream(void* h)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;
  if (SoapySDRDevice_deactivateStream(handler->device, handler->rxStream, 0, 0) != 0) {
    printf("Error deactivating Rx streaming.\n");
    return SRSLTE_ERROR;
  }

  handler->rx_stream_active = false;
  return SRSLTE_SUCCESS;
}

int rf_soapy_stop_tx_stream(void* h)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;
  if (SoapySDRDevice_deactivateStream(handler->device, handler->txStream, 0, 0) != 0) {
    printf("Error deactivating Tx streaming.\n");
    return SRSLTE_ERROR;
  }

  handler->tx_stream_active = false;
  return SRSLTE_SUCCESS;
}

void rf_soapy_flush_buffer(void* h)
{
  int   n;
  cf_t  tmp1[1024];
  cf_t  tmp2[1024];
  void* data[2] = {tmp1, tmp2};
  do {
    n = rf_soapy_recv_with_time_multi(h, data, 1024, 0, NULL, NULL);
  } while (n > 0);
}

bool rf_soapy_has_rssi(void* h)
{
  // TODO: implement rf_soapy_has_rssi()
  return false;
}

float rf_soapy_get_rssi(void* h)
{
  printf("TODO: implement rf_soapy_get_rssi()\n");
  return 0.0;
}

int rf_soapy_open_multi(char* args, void** h, uint32_t num_requested_channels)
{
  size_t          length;
  SoapySDRKwargs* soapy_args = SoapySDRDevice_enumerate(NULL, &length);

  if (length == 0) {
    printf("No Soapy devices found.\n");
    SoapySDRKwargsList_clear(soapy_args, length);
    return SRSLTE_ERROR;
  }
  char* devname = DEVNAME_NONE;
  for (size_t i = 0; i < length; i++) {
    printf("Soapy has found device #%d: ", (int)i);
    for (size_t j = 0; j < soapy_args[i].size; j++) {
      printf("%s=%s, ", soapy_args[i].keys[j], soapy_args[i].vals[j]);
      if (!strcmp(soapy_args[i].keys[j], "name") && !strcmp(soapy_args[i].vals[j], "LimeSDR-USB")) {
        devname = DEVNAME_LIME;
      } else if (!strcmp(soapy_args[i].keys[j], "name") && !strcmp(soapy_args[i].vals[j], "LimeSDR Mini")) {
        devname = DEVNAME_LIME_MINI;
      }
    }
    printf("\n");
  }

  // Select Soapy device by id
  int dev_id = 0;
  if (args != NULL) {
    const char dev_arg[] = "id=";
    char*      dev_ptr   = strstr(args, dev_arg);
    if (dev_ptr) {
      char dev_str[64] = {0};
      copy_subdev_string(dev_str, dev_ptr + strnlen(dev_arg, 64));
      dev_id = strtol(dev_str, NULL, 0);
      if (dev_id < 0 || dev_id > 10) {
        ERROR("Failed to set device. Using 0 as default.\n");
        dev_id = 0;
      }
      remove_substring(args, dev_arg);
      remove_substring(args, dev_str);
    }
  }

  // select last device if dev_id exceeds available devices
  dev_id = SRSLTE_MIN(dev_id, length - 1);
  printf("Selecting Soapy device: %d\n", dev_id);

  SoapySDRDevice* sdr = SoapySDRDevice_make(&(soapy_args[dev_id]));
  if (sdr == NULL) {
    printf("Failed to create Soapy object\n");
    return SRSLTE_ERROR;
  }
  SoapySDRKwargsList_clear(soapy_args, length);

  // create handler
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)malloc(sizeof(rf_soapy_handler_t));
  bzero(handler, sizeof(rf_soapy_handler_t));
  *h                        = handler;
  handler->device           = sdr;
  handler->tx_stream_active = false;
  handler->rx_stream_active = false;
  handler->devname          = devname;

  // create stream args from device args
  SoapySDRKwargs stream_args = {};
#if SOAPY_SDR_API_VERSION >= 0x00060000
  stream_args = SoapySDRKwargs_fromString(args);
#endif

  // Setup Rx streamer
  size_t num_available_channels = SoapySDRDevice_getNumChannels(handler->device, SOAPY_SDR_RX);
  if ((num_available_channels > 0) && (num_requested_channels > 0)) {
    handler->num_rx_channels = SRSLTE_MIN(num_available_channels, num_requested_channels);
    size_t rx_channels[handler->num_rx_channels];
    for (int i = 0; i < handler->num_rx_channels; i++) {
      rx_channels[i] = i;
    }
    printf("Setting up Rx stream with %zd channel(s)\n", handler->num_rx_channels);
#if SOAPY_SDR_API_VERSION < 0x00080000
    if (SoapySDRDevice_setupStream(handler->device,
                                   &handler->rxStream,
                                   SOAPY_SDR_RX,
                                   SOAPY_SDR_CF32,
                                   rx_channels,
                                   handler->num_rx_channels,
                                   &stream_args) != 0) {
#else
    handler->rxStream = SoapySDRDevice_setupStream(
        handler->device, SOAPY_SDR_RX, SOAPY_SDR_CF32, rx_channels, handler->num_rx_channels, &stream_args);
    if (handler->rxStream == NULL) {
#endif
      printf("Rx setupStream fail: %s\n", SoapySDRDevice_lastError());
      return SRSLTE_ERROR;
    }
    handler->rx_mtu = SoapySDRDevice_getStreamMTU(handler->device, handler->rxStream);
  }

  // Setup Tx streamer
  num_available_channels = SoapySDRDevice_getNumChannels(handler->device, SOAPY_SDR_TX);
  if ((num_available_channels > 0) && (num_requested_channels > 0)) {
    handler->num_tx_channels = SRSLTE_MIN(num_available_channels, num_requested_channels);
    size_t tx_channels[handler->num_tx_channels];
    for (int i = 0; i < handler->num_tx_channels; i++) {
      tx_channels[i] = i;
    }
    printf("Setting up Tx stream with %zd channel(s)\n", handler->num_tx_channels);
#if SOAPY_SDR_API_VERSION < 0x00080000
    if (SoapySDRDevice_setupStream(handler->device,
                                   &handler->txStream,
                                   SOAPY_SDR_TX,
                                   SOAPY_SDR_CF32,
                                   tx_channels,
                                   handler->num_tx_channels,
                                   &stream_args) != 0) {
#else
    handler->txStream = SoapySDRDevice_setupStream(
        handler->device, SOAPY_SDR_TX, SOAPY_SDR_CF32, tx_channels, handler->num_tx_channels, &stream_args);
    if (handler->txStream == NULL) {
#endif
      printf("Tx setupStream fail: %s\n", SoapySDRDevice_lastError());
      return SRSLTE_ERROR;
    }
    handler->tx_mtu = SoapySDRDevice_getStreamMTU(handler->device, handler->txStream);
  }

  // init rx/tx rate to lowest LTE rate to avoid decimation warnings
  rf_soapy_set_rx_srate(handler, 1.92e6);
  rf_soapy_set_tx_srate(handler, 1.92e6);

  // list device sensors
  size_t list_length;
  char** list;
  list = SoapySDRDevice_listSensors(handler->device, &list_length);
  printf("Available device sensors: \n");
  for (int i = 0; i < list_length; i++) {
    printf(" - %s\n", list[i]);
  }

  // list channel sensors
  for (uint32_t i = 0; i < handler->num_rx_channels; ++i) {
    list = SoapySDRDevice_listChannelSensors(handler->device, SOAPY_SDR_RX, i, &list_length);
    printf("Available sensors for Rx channel %d: \n", i);
    for (int j = 0; j < list_length; j++) {
      printf(" - %s\n", list[j]);
    }
  }

  // Set static radio info
  SoapySDRRange tx_range    = SoapySDRDevice_getGainRange(handler->device, SOAPY_SDR_TX, 0);
  SoapySDRRange rx_range    = SoapySDRDevice_getGainRange(handler->device, SOAPY_SDR_RX, 0);
  handler->info.min_tx_gain = tx_range.minimum;
  handler->info.max_tx_gain = tx_range.maximum;
  handler->info.min_rx_gain = rx_range.minimum;
  handler->info.max_rx_gain = rx_range.maximum;

  // Check device arguments
  if (args) {
    // config file
    const char config_arg[]   = "config=";
    char       config_str[64] = {0};
    char*      config_ptr     = strstr(args, config_arg);
    if (config_ptr) {
      copy_subdev_string(config_str, config_ptr + strlen(config_arg));
      printf("Loading config file %s\n", config_str);
      SoapySDRDevice_writeSetting(handler->device, "LOAD_CONFIG", config_str);
      remove_substring(args, config_arg);
      remove_substring(args, config_str);
    }

    // rx antenna
    const char rx_ant_arg[]   = "rxant=";
    char       rx_ant_str[64] = {0};
    char*      rx_ant_ptr     = strstr(args, rx_ant_arg);
    if (rx_ant_ptr) {
      copy_subdev_string(rx_ant_str, rx_ant_ptr + strlen(rx_ant_arg));
      printf("Setting Rx antenna to %s\n", rx_ant_str);
      if (SoapySDRDevice_setAntenna(handler->device, SOAPY_SDR_RX, 0, rx_ant_str) != 0) {
        ERROR("Failed to set Rx antenna.\n");
      }
      remove_substring(args, rx_ant_arg);
      remove_substring(args, rx_ant_str);
    }

    // tx antenna
    const char tx_ant_arg[]   = "txant=";
    char       tx_ant_str[64] = {0};
    char*      tx_ant_ptr     = strstr(args, tx_ant_arg);
    if (tx_ant_ptr) {
      copy_subdev_string(tx_ant_str, tx_ant_ptr + strlen(tx_ant_arg));
      printf("Setting Tx antenna to %s\n", tx_ant_str);
      if (SoapySDRDevice_setAntenna(handler->device, SOAPY_SDR_TX, 0, tx_ant_str) != 0) {
        ERROR("Failed to set Tx antenna.\n");
      }
      remove_substring(args, tx_ant_arg);
      remove_substring(args, tx_ant_str);
    }

    // log level
    const char loglevel_arg[]   = "loglevel=";
    char       loglevel_str[64] = {0};
    char*      loglevel_ptr     = strstr(args, loglevel_arg);
    if (loglevel_ptr) {
      copy_subdev_string(loglevel_str, loglevel_ptr + strlen(loglevel_arg));
      if (strcmp(loglevel_str, "error") == 0) {
        SoapySDR_setLogLevel(SOAPY_SDR_ERROR);
      }
      remove_substring(args, loglevel_arg);
      remove_substring(args, loglevel_str);
    }
  }

#if HAVE_ASYNC_THREAD
  bool start_async_thread = true;
  if (args) {
    if (strstr(args, "silent")) {
      REMOVE_SUBSTRING_WITHCOMAS(args, "silent");
      start_async_thread = false;
    }
  }
#endif

  // receive one subframe to allow for transceiver calibration
  if (strstr(devname, "lime")) {
    // set default tx gain and leave some time to calibrate tx
    rf_soapy_set_tx_gain(handler, 45);
    rf_soapy_set_rx_gain(handler, 35);

    cf_t  dummy_buffer[1920];
    cf_t* dummy_buffer_array[SRSLTE_MAX_PORTS];
    for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
      dummy_buffer_array[i] = dummy_buffer;
    }
    rf_soapy_start_rx_stream(handler, true);
    rf_soapy_recv_with_time_multi(handler, (void**)dummy_buffer_array, 1920, false, NULL, NULL);
    rf_soapy_stop_rx_stream(handler);

    usleep(10000);
  }

  // list gains and AGC mode
  bool has_agc = SoapySDRDevice_hasGainMode(handler->device, SOAPY_SDR_RX, 0);
  list         = SoapySDRDevice_listGains(handler->device, SOAPY_SDR_RX, 0, &list_length);
  printf("State of gain elements for Rx channel 0 (AGC %s):\n", has_agc ? "supported" : "not supported");
  for (int i = 0; i < list_length; i++) {
    printf(" - %s: %.2f dB\n", list[i], SoapySDRDevice_getGainElement(handler->device, SOAPY_SDR_RX, 0, list[i]));
  }

  has_agc = SoapySDRDevice_hasGainMode(handler->device, SOAPY_SDR_TX, 0);
  list    = SoapySDRDevice_listGains(handler->device, SOAPY_SDR_TX, 0, &list_length);
  printf("State of gain elements for Tx channel 0 (AGC %s):\n", has_agc ? "supported" : "not supported");
  for (int i = 0; i < list_length; i++) {
    printf(" - %s: %.2f dB\n", list[i], SoapySDRDevice_getGainElement(handler->device, SOAPY_SDR_TX, 0, list[i]));
  }

  // print actual antenna configuration
  char* ant = SoapySDRDevice_getAntenna(handler->device, SOAPY_SDR_RX, 0);
  printf("Rx antenna set to %s\n", ant);

  ant = SoapySDRDevice_getAntenna(handler->device, SOAPY_SDR_TX, 0);
  printf("Tx antenna set to %s\n", ant);

#if HAVE_ASYNC_THREAD
  if (start_async_thread) {
    // Start low priority thread to receive async commands
    handler->async_thread_running = true;
    if (pthread_create(&handler->async_thread, NULL, async_thread, handler)) {
      perror("pthread_create");
      return -1;
    }
  }
#endif

  return SRSLTE_SUCCESS;
}

int rf_soapy_open(char* args, void** h)
{
  return rf_soapy_open_multi(args, h, 1);
}

int rf_soapy_close(void* h)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;

#if HAVE_ASYNC_THREAD
  if (handler->async_thread_running) {
    handler->async_thread_running = false;
    pthread_join(handler->async_thread, NULL);
  }
#endif

  if (handler->tx_stream_active) {
    rf_soapy_stop_tx_stream(handler);
    SoapySDRDevice_closeStream(handler->device, handler->txStream);
  }

  if (handler->rx_stream_active) {
    rf_soapy_stop_rx_stream(handler);
    SoapySDRDevice_closeStream(handler->device, handler->rxStream);
  }

  SoapySDRDevice_unmake(handler->device);

  // print statistics
  if (handler->num_lates)
    printf("#lates=%d\n", handler->num_lates);
  if (handler->num_overflows)
    printf("#overflows=%d\n", handler->num_overflows);
  if (handler->num_underflows)
    printf("#underflows=%d\n", handler->num_underflows);
  if (handler->num_time_errors)
    printf("#time_errors=%d\n", handler->num_time_errors);
  if (handler->num_other_errors)
    printf("#other_errors=%d\n", handler->num_other_errors);

  free(handler);

  return SRSLTE_SUCCESS;
}

double rf_soapy_set_rx_srate(void* h, double rate)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;

#if STOP_STREAM_BEFORE_RATE_CHANGE
  // Restart streaming, as the Lime seems to have problems reconfiguring the sample rate during streaming
  bool rx_stream_active = handler->rx_stream_active;
  if (rx_stream_active) {
    rf_soapy_stop_rx_stream(handler);
  }
#endif // STOP_STREAM_BEFORE_RATE_CHANGE

  for (uint32_t i = 0; i < handler->num_rx_channels; i++) {
    if (SoapySDRDevice_setSampleRate(handler->device, SOAPY_SDR_RX, i, rate) != 0) {
      printf("setSampleRate Rx fail: %s\n", SoapySDRDevice_lastError());
      return SRSLTE_ERROR;
    }

#if SET_RF_BW
    // Set bandwidth close to current rate
    size_t         bw_length;
    SoapySDRRange* bw_range = SoapySDRDevice_getBandwidthRange(handler->device, SOAPY_SDR_RX, 0, &bw_length);
    for (int k = 0; k < bw_length; ++k) {
      double bw = rate * 0.75;
      bw        = SRSLTE_MIN(bw, bw_range[k].maximum);
      bw        = SRSLTE_MAX(bw, bw_range[k].minimum);
      if (SoapySDRDevice_setBandwidth(handler->device, SOAPY_SDR_RX, i, bw) != 0) {
        printf("setBandwidth fail: %s\n", SoapySDRDevice_lastError());
        return SRSLTE_ERROR;
      }
      printf("Set Rx bandwidth to %.2f MHz\n", SoapySDRDevice_getBandwidth(handler->device, SOAPY_SDR_RX, i) / 1e6);
    }
#endif // SET_RF_BW
  }

#if STOP_STREAM_BEFORE_RATE_CHANGE
  if (rx_stream_active) {
    rf_soapy_start_rx_stream(handler, true);
  }
#endif // STOP_STREAM_BEFORE_RATE_CHANGE

  // retrun sample rate of first channel
  return SoapySDRDevice_getSampleRate(handler->device, SOAPY_SDR_RX, 0);
}

double rf_soapy_set_tx_srate(void* h, double rate)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;

#if STOP_STREAM_BEFORE_RATE_CHANGE
  // stop/start streaming during rate reconfiguration
  bool rx_stream_active = handler->rx_stream_active;
  if (handler->rx_stream_active) {
    rf_soapy_stop_rx_stream(handler);
  }
#endif // STOP_STREAM_BEFORE_RATE_CHANGE

  for (uint32_t i = 0; i < handler->num_tx_channels; i++) {
    if (SoapySDRDevice_setSampleRate(handler->device, SOAPY_SDR_TX, i, rate) != 0) {
      printf("setSampleRate Tx fail: %s\n", SoapySDRDevice_lastError());
      return SRSLTE_ERROR;
    }

#if SET_RF_BW
    size_t         bw_length;
    SoapySDRRange* bw_range = SoapySDRDevice_getBandwidthRange(handler->device, SOAPY_SDR_TX, i, &bw_length);
    for (int k = 0; k < bw_length; ++k) {
      // try to set the BW a bit narrower than sampling rate to prevent aliasing but make sure to stay within device
      // boundaries
      double bw = rate * 0.75;
      bw        = SRSLTE_MAX(bw, bw_range[k].minimum);
      bw        = SRSLTE_MIN(bw, bw_range[k].maximum);
      if (SoapySDRDevice_setBandwidth(handler->device, SOAPY_SDR_TX, i, bw) != 0) {
        printf("setBandwidth fail: %s\n", SoapySDRDevice_lastError());
        return SRSLTE_ERROR;
      }
      printf("Set Tx bandwidth to %.2f MHz\n", SoapySDRDevice_getBandwidth(handler->device, SOAPY_SDR_TX, i) / 1e6);
    }
#endif // SET_RF_BW
  }

#if STOP_STREAM_BEFORE_RATE_CHANGE
  if (rx_stream_active) {
    rf_soapy_start_rx_stream(handler, true);
  }
#endif // STOP_STREAM_BEFORE_RATE_CHANGE

  handler->tx_rate = SoapySDRDevice_getSampleRate(handler->device, SOAPY_SDR_TX, 0);

  return handler->tx_rate;
}

double rf_soapy_set_rx_gain(void* h, double gain)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;

  for (uint32_t i = 0; i < handler->num_rx_channels; i++) {
    if (SoapySDRDevice_setGain(handler->device, SOAPY_SDR_RX, i, gain) != 0) {
      printf("setGain fail: %s\n", SoapySDRDevice_lastError());
      return SRSLTE_ERROR;
    }
  }
  return rf_soapy_get_rx_gain(h);
}

double rf_soapy_set_tx_gain(void* h, double gain)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;
  for (uint32_t i = 0; i < handler->num_tx_channels; i++) {
    if (SoapySDRDevice_setGain(handler->device, SOAPY_SDR_TX, i, gain) != 0) {
      printf("setGain fail: %s\n", SoapySDRDevice_lastError());
      return SRSLTE_ERROR;
    }
  }
  return rf_soapy_get_tx_gain(h);
}

// Return gain of first channel
double rf_soapy_get_rx_gain(void* h)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;
  return SoapySDRDevice_getGain(handler->device, SOAPY_SDR_RX, 0);
}

// Return gain of first channel
double rf_soapy_get_tx_gain(void* h)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;
  return SoapySDRDevice_getGain(handler->device, SOAPY_SDR_TX, 0);
}

srslte_rf_info_t* rf_soapy_get_info(void* h)
{
  srslte_rf_info_t* info = NULL;
  if (h) {
    rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;
    info                        = &handler->info;
  }
  return info;
}

double rf_soapy_set_rx_freq(void* h, uint32_t ch, double freq)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;

  for (uint32_t i = 0; i < handler->num_rx_channels; i++) {
    if (SoapySDRDevice_setFrequency(handler->device, SOAPY_SDR_RX, i, freq, NULL) != 0) {
      printf("setFrequency fail: %s\n", SoapySDRDevice_lastError());
      return SRSLTE_ERROR;
    }
  }

  // wait until LO is locked
  rf_soapy_rx_wait_lo_locked(handler);

  // Return actual frequency for channel 0
  return SoapySDRDevice_getFrequency(handler->device, SOAPY_SDR_RX, 0);
}

double rf_soapy_set_tx_freq(void* h, uint32_t ch, double freq)
{
  rf_soapy_handler_t* handler = (rf_soapy_handler_t*)h;
  for (uint32_t i = 0; i < handler->num_tx_channels; i++) {
    if (SoapySDRDevice_setFrequency(handler->device, SOAPY_SDR_TX, i, freq, NULL) != 0) {
      printf("setFrequency fail: %s\n", SoapySDRDevice_lastError());
      return SRSLTE_ERROR;
    }
  }
  return SoapySDRDevice_getFrequency(handler->device, SOAPY_SDR_TX, 0);
}

void rf_soapy_get_time(void* h, time_t* secs, double* frac_secs)
{
  printf("Todo: implement rf_soapy_get_time()\n");
}

// TODO: add multi-channel support
int rf_soapy_recv_with_time_multi(void*    h,
                                  void*    data[SRSLTE_MAX_PORTS],
                                  uint32_t nsamples,
                                  bool     blocking,
                                  time_t*  secs,
                                  double*  frac_secs)
{
  rf_soapy_handler_t* handler   = (rf_soapy_handler_t*)h;
  int                 flags     = 0;      // flags set by receive operation
  const long          timeoutUs = 400000; // arbitrarily chosen

  int       trials = 0;
  int       ret    = 0;
  long long timeNs; // timestamp for receive buffer
  int       n = 0;

#if PRINT_RX_STATS
  printf("rx: nsamples=%d rx_mtu=%zd\n", nsamples, handler->rx_mtu);
#endif

  do {
    size_t rx_samples = SRSLTE_MIN(nsamples - n, handler->rx_mtu);
#if PRINT_RX_STATS
    printf(" - rx_samples=%zd\n", rx_samples);
#endif

    void* buffs_ptr[SRSLTE_MAX_PORTS] = {};
    for (int i = 0; i < handler->num_rx_channels; i++) {
      cf_t* data_c = (cf_t*)data[i];
      buffs_ptr[i] = &data_c[n];
    }

    ret = SoapySDRDevice_readStream(
        handler->device, handler->rxStream, buffs_ptr, rx_samples, &flags, &timeNs, timeoutUs);
    if (ret == SOAPY_SDR_OVERFLOW || (ret > 0 && (flags & SOAPY_SDR_END_ABRUPT) != 0)) {
      log_overflow(handler);
      continue;
    } else if (ret == SOAPY_SDR_TIMEOUT) {
      log_late(handler, true);
      continue;
    } else if (ret < 0) {
      // unspecific error
      printf("SoapySDRDevice_readStream returned %d: %s\n", ret, SoapySDR_errToStr(ret));
      handler->num_other_errors++;
    }

    // update rx time only for first segment
    if (secs != NULL && frac_secs != NULL && n == 0) {
      *secs      = floor(timeNs / 1e9);
      *frac_secs = (timeNs % 1000000000) / 1e9;
      // printf("rx_time: secs=%lld, frac_secs=%lf timeNs=%llu\n", *secs, *frac_secs, timeNs);
    }

#if PRINT_RX_STATS
    printf(" - rx: %d/%zd\n", ret, rx_samples);
#endif

    n += ret;
    trials++;
  } while (n < nsamples && trials < 100);

  return n;
}

int rf_soapy_recv_with_time(void* h, void* data, uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs)
{
  return rf_soapy_recv_with_time_multi(h, &data, nsamples, blocking, secs, frac_secs);
}

int rf_soapy_send_timed(void*  h,
                        void*  data,
                        int    nsamples,
                        time_t secs,
                        double frac_secs,
                        bool   has_time_spec,
                        bool   blocking,
                        bool   is_start_of_burst,
                        bool   is_end_of_burst)
{
  void* _data[SRSLTE_MAX_PORTS] = {data, zero_mem, zero_mem, zero_mem};
  return rf_soapy_send_timed_multi(
      h, _data, nsamples, secs, frac_secs, has_time_spec, blocking, is_start_of_burst, is_end_of_burst);
}

// Todo: Check correct handling of flags, use RF metrics API, fix timed transmissions
int rf_soapy_send_timed_multi(void*  h,
                              void*  data[SRSLTE_MAX_PORTS],
                              int    nsamples,
                              time_t secs,
                              double frac_secs,
                              bool   has_time_spec,
                              bool   blocking,
                              bool   is_start_of_burst,
                              bool   is_end_of_burst)
{
  rf_soapy_handler_t* handler   = (rf_soapy_handler_t*)h;
  int                 flags     = 0;
  const long          timeoutUs = 100000; // arbitrarily chosen
  long long           timeNs    = 0;
  int                 trials    = 0;
  int                 ret       = 0;
  int                 n         = 0;

#if PRINT_TX_STATS
  printf("tx: namples=%d, mtu=%zd\n", nsamples, handler->tx_mtu);
#endif

  if (!handler->tx_stream_active) {
    rf_soapy_start_tx_stream(h);
  }

  // Convert initial tx time
  if (has_time_spec) {
    timeNs = (long long)secs * 1000000000;
    timeNs = timeNs + (frac_secs * 1000000000);
  }

  do {
#if USE_TX_MTU
    size_t tx_samples = SRSLTE_MIN(nsamples - n, handler->tx_mtu);
#else
    size_t tx_samples = nsamples;
    if (tx_samples > nsamples - n) {
      tx_samples = nsamples - n;
    }
#endif

    // (re-)set stream flags
    flags = 0;
    if (is_start_of_burst && is_end_of_burst) {
      flags |= SOAPY_SDR_ONE_PACKET;
    }

    if (is_end_of_burst) {
      flags |= SOAPY_SDR_END_BURST;
    }

    // only set time flag for first tx
    if (has_time_spec && n == 0) {
      flags |= SOAPY_SDR_HAS_TIME;
    }

#if PRINT_TX_STATS
    printf(" - tx_samples=%zd at timeNs=%llu flags=%d\n", tx_samples, timeNs, flags);
#endif

    const void* buffs_ptr[SRSLTE_MAX_PORTS] = {};
    for (int i = 0; i < handler->num_tx_channels; i++) {
      cf_t* data_c = data[i] ? data[i] : zero_mem;
      buffs_ptr[i] = &data_c[n];
    }

    ret = SoapySDRDevice_writeStream(
        handler->device, handler->txStream, buffs_ptr, tx_samples, &flags, timeNs, timeoutUs);
    if (ret >= 0) {
      // Tx was ok
#if PRINT_TX_STATS
      printf(" - tx: %d/%zd\n", ret, tx_samples);
#endif
      // Advance tx time
      if (has_time_spec && ret < nsamples) {
        long long adv = SoapySDR_ticksToTimeNs(ret, handler->tx_rate);
#if PRINT_TX_STATS
        printf(" - tx: timeNs_old=%llu, adv=%llu, timeNs_new=%llu, tx_rate=%f\n",
               timeNs,
               adv,
               timeNs + adv,
               handler->tx_rate);
#endif
        timeNs += adv;
      }
      n += ret;
    } else if (ret < 0) {
      // An error has occured
      switch (ret) {
        case SOAPY_SDR_TIMEOUT:
          log_late(handler, false);
          printf("L");
          break;
        case SOAPY_SDR_STREAM_ERROR:
          handler->num_stream_curruption++;
          printf("E");
          break;
        case SOAPY_SDR_TIME_ERROR:
          handler->num_time_errors++;
          printf("T");
          break;
        case SOAPY_SDR_UNDERFLOW:
          log_underflow(handler);
          printf("U");
          break;
        default:
          ERROR("Error during writeStream\n");
          exit(-1);
          return SRSLTE_ERROR;
      }
    }
    trials++;
  } while (n < nsamples && trials < 100);

  if (n != nsamples) {
    ERROR("Couldn't write all samples after %d trials.\n", trials);
  }

  return n;
}
