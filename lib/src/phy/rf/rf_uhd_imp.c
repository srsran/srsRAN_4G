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
#include <uhd.h>
#include <uhd/types/sensors.h>
#include <unistd.h>

#include "rf_helper.h"
#include "srslte/srslte.h"
#include "uhd_c_api.h"

#include "rf_uhd_imp.h"

#define HAVE_ASYNC_THREAD 1

#if UHD_VERSION < 3130000
#define UHD_SUPPORTS_COMMAND_TIME
#endif /* UHD_VERSION < 3140000 */

typedef struct {
  char*                  devname;
  uhd_usrp_handle        usrp;
  uhd_rx_streamer_handle rx_stream;
  uhd_tx_streamer_handle tx_stream;

  uhd_rx_metadata_handle rx_md, rx_md_first;
  uhd_tx_metadata_handle tx_md;

  srslte_rf_info_t info;
  size_t           rx_nof_samples;
  size_t           tx_nof_samples;
  double           tx_rate;
  bool             dynamic_rate;
  bool             has_rssi;
  uint32_t         nof_rx_channels;
  int              nof_tx_channels;

  srslte_rf_error_handler_t uhd_error_handler;
  void*                     uhd_error_handler_arg;

  float current_master_clock;

  bool      async_thread_running;
  pthread_t async_thread;

  pthread_mutex_t tx_mutex;
} rf_uhd_handler_t;

void suppress_handler(const char* x)
{
  // do nothing
}

static cf_t zero_mem[64 * 1024];

static void log_overflow(rf_uhd_handler_t* h)
{
  if (h->uhd_error_handler) {
    srslte_rf_error_t error;
    bzero(&error, sizeof(srslte_rf_error_t));
    error.type = SRSLTE_RF_ERROR_OVERFLOW;
    h->uhd_error_handler(h->uhd_error_handler_arg, error);
  }
}

static void log_late(rf_uhd_handler_t* h, bool is_rx)
{
  if (h->uhd_error_handler) {
    srslte_rf_error_t error;
    bzero(&error, sizeof(srslte_rf_error_t));
    error.opt  = is_rx ? 1 : 0;
    error.type = SRSLTE_RF_ERROR_LATE;
    h->uhd_error_handler(h->uhd_error_handler_arg, error);
  }
}

#if HAVE_ASYNC_THREAD
static void log_underflow(rf_uhd_handler_t* h)
{
  if (h->uhd_error_handler) {
    srslte_rf_error_t error;
    bzero(&error, sizeof(srslte_rf_error_t));
    error.type = SRSLTE_RF_ERROR_UNDERFLOW;
    h->uhd_error_handler(h->uhd_error_handler_arg, error);
  }
}
#endif

static void log_rx_error(rf_uhd_handler_t* h)
{
  if (h->uhd_error_handler) {
    char error_string[512];
    uhd_usrp_last_error(h->usrp, error_string, 512);
    ERROR("USRP reported the following error: %s\n", error_string);

    srslte_rf_error_t error;
    bzero(&error, sizeof(srslte_rf_error_t));
    error.type = SRSLTE_RF_ERROR_RX;
    h->uhd_error_handler(h->uhd_error_handler_arg, error);
  }
}

#if HAVE_ASYNC_THREAD
static void* async_thread(void* h)
{
  rf_uhd_handler_t*         handler = (rf_uhd_handler_t*)h;
  uhd_async_metadata_handle md;
  uhd_async_metadata_make(&md);
  while (handler->async_thread_running) {
    bool      valid;
    uhd_error err = uhd_tx_streamer_recv_async_msg(handler->tx_stream, &md, 0.5, &valid);
    if (err == UHD_ERROR_NONE) {
      if (valid) {
        uhd_async_metadata_event_code_t event_code;
        uhd_async_metadata_event_code(md, &event_code);
        if (event_code == UHD_ASYNC_METADATA_EVENT_CODE_UNDERFLOW ||
            event_code == UHD_ASYNC_METADATA_EVENT_CODE_UNDERFLOW_IN_PACKET) {
          log_underflow(handler);
        } else if (event_code == UHD_ASYNC_METADATA_EVENT_CODE_TIME_ERROR) {
          log_late(handler, false);
        }
      }
    } else {
      ERROR("Error while receiving aync metadata: 0x%x\n", err);
      return NULL;
    }
  }
  uhd_async_metadata_free(&md);
  return NULL;
}
#endif

static inline void uhd_free(rf_uhd_handler_t* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;

  // NULL handler, return
  if (!handler) {
    return;
  }

  uhd_tx_metadata_free(&handler->tx_md);
  uhd_rx_metadata_free(&handler->rx_md_first);
  uhd_rx_metadata_free(&handler->rx_md);

#if HAVE_ASYNC_THREAD
  if (handler->async_thread_running) {
    handler->async_thread_running = false;
    pthread_join(handler->async_thread, NULL);
  }
#endif

  uhd_tx_streamer_free(&handler->tx_stream);
  uhd_rx_streamer_free(&handler->rx_stream);
  uhd_usrp_free(&handler->usrp);

  free(handler);
}

static inline int uhd_alloc(rf_uhd_handler_t* handler, char* args)
{
  uhd_error error;

  error = uhd_usrp_make(&handler->usrp, args);
  if (error) {
    ERROR("Error opening UHD: code %d\n", error);
    return SRSLTE_ERROR;
  }

  error = uhd_rx_streamer_make(&handler->rx_stream);
  if (error) {
    ERROR("Error making RX stream: %d\n", error);
    return SRSLTE_ERROR;
  }

  error = uhd_tx_streamer_make(&handler->tx_stream);
  if (error) {
    ERROR("Error making TX stream: %d\n", error);
    return SRSLTE_ERROR;
  }

  // Make metadata objects for RX/TX
  error = uhd_rx_metadata_make(&handler->rx_md);
  if (error) {
    ERROR("Error making RX Metadata: %d\n", error);
    return SRSLTE_ERROR;
  }

  error = uhd_rx_metadata_make(&handler->rx_md_first);
  if (error) {
    ERROR("Error making RX Metadata first: %d\n", error);
    return SRSLTE_ERROR;
  }

  error = uhd_tx_metadata_make(&handler->tx_md, false, 0, 0, false, false);
  if (error) {
    ERROR("Error making TX Metadata: %d\n", error);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

void rf_uhd_suppress_stdout(void* h)
{
  rf_uhd_register_msg_handler_c(suppress_handler);
}

void rf_uhd_register_error_handler(void* h, srslte_rf_error_handler_t new_handler, void* arg)
{
  rf_uhd_handler_t* handler      = (rf_uhd_handler_t*)h;
  handler->uhd_error_handler     = new_handler;
  handler->uhd_error_handler_arg = arg;
}

static bool find_string(uhd_string_vector_handle h, const char* str)
{
  char   buff[128];
  size_t n;
  uhd_string_vector_size(h, &n);
  for (int i = 0; i < n; i++) {
    uhd_string_vector_at(h, i, buff, 128);
    if (strstr(buff, str)) {
      return true;
    }
  }
  return false;
}

/**
 * Set the USRP time to the current GPS time (if sensor is found)
 *
 * The GPS time is read and the USRP time is set to the next full second during the next PPS.
 * It appears, however, that "uhd_usrp_set_time_next_pps()" which seems to be the correct function
 * to use, doesn't work. The C API call "uhd_usrp_set_time_unknown_pps()" works well.
 * @param handler Pointer to RF handler
 * @return Any error returned by UHD
 */
static uhd_error set_time_to_gps_time(rf_uhd_handler_t* handler)
{
  const char sensor_name[] = "gps_time";

  uhd_string_vector_handle sensors;
  uhd_string_vector_make(&sensors);

  uhd_error error = uhd_usrp_get_mboard_sensor_names(handler->usrp, 0, &sensors);

#if PRINT_SENSOR_NAMES
  size_t sensors_len;
  uhd_string_vector_size(sensors, &sensors_len);
  for (int i = 0; i < sensors_len; i++) {
    char buff[256];
    uhd_string_vector_at(sensors, i, buff, 128);
    printf("sensor %s present\n", buff);
  }
#endif

  if (error == UHD_ERROR_NONE && find_string(sensors, sensor_name)) {
    uhd_sensor_value_handle value_h;
    uhd_sensor_value_make_from_string(&value_h, "w", "t", "f");
    error = uhd_usrp_get_mboard_sensor(handler->usrp, sensor_name, 0, &value_h);

    int full_secs = 0;
    if (error == UHD_ERROR_NONE) {
      uhd_sensor_value_data_type_t sensor_dtype;
      uhd_sensor_value_data_type(value_h, &sensor_dtype);
      full_secs = uhd_sensor_value_to_int(value_h, &full_secs);
      // printf("GPS full_secs=%d\n", full_secs);

      double frac_secs;
      uhd_sensor_value_data_type(value_h, &sensor_dtype);
      full_secs = uhd_sensor_value_to_realnum(value_h, &frac_secs);
      // printf("GPS frac_secs=%f\n", frac_secs);

      full_secs = frac_secs;
      printf("Setting USRP time to %ds\n", full_secs);
      error = uhd_usrp_set_time_unknown_pps(handler->usrp, full_secs + 1, 0.0);
      if (error != UHD_ERROR_NONE) {
        char err_msg[256];
        uhd_usrp_last_error(handler->usrp, err_msg, 256);
        fprintf(stderr, "Error code %d: %s\n", error, err_msg);
      }

      // sleep a second to make sure values are set correctly
      sleep(1);
    }
    uhd_sensor_value_free(&value_h);
  }
  if (error != UHD_ERROR_NONE) {
    fprintf(stderr, "USRP has no sensor \"%s\", or reading failed. UHD error: %i\n", sensor_name, error);
  }

  uhd_string_vector_free(&sensors);

  return error;
}

// timeout in ms
static uhd_error
wait_sensor_locked(rf_uhd_handler_t* handler, char* sensor_name, bool is_mboard, int timeout, bool* is_locked)
{
  uhd_error error;
  *is_locked = false;

  uhd_sensor_value_handle  value_h;
  uhd_string_vector_handle sensors;

  uhd_string_vector_make(&sensors);
  uhd_sensor_value_make_from_bool(&value_h, "", true, "True", "False");
  if (is_mboard) {
    // motherboard sensor
    error = uhd_usrp_get_mboard_sensor_names(handler->usrp, 0, &sensors);
  } else {
    // daughterboard sensor
    error = uhd_usrp_get_rx_sensor_names(handler->usrp, 0, &sensors);
  }

  if (error == UHD_ERROR_NONE && find_string(sensors, sensor_name)) {
    do {
      if (is_mboard) {
        error = uhd_usrp_get_mboard_sensor(handler->usrp, sensor_name, 0, &value_h);
      } else {
        error = uhd_usrp_get_rx_sensor(handler->usrp, sensor_name, 0, &value_h);
      }
      if (error != UHD_ERROR_NONE) {
        break;
      }
      uhd_sensor_value_to_bool(value_h, is_locked);
      usleep(1000); // 1ms
      timeout -= 1; // 1ms
    } while (*is_locked == false && timeout > 0);
  }
  if (error != UHD_ERROR_NONE) {
    fprintf(stderr,
            "%s has no sensor \"%s\", or reading failed. UHD error: %i\n",
            is_mboard ? "Motherboard" : "Daugherboard",
            sensor_name,
            error);
    // board doesn't have this sensor, sleep for timeout
    *is_locked = true;
    usleep(timeout * 1000);
  }

  uhd_string_vector_free(&sensors);
  uhd_sensor_value_free(&value_h);

  return error;
}

const char* rf_uhd_devname(void* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  return handler->devname;
}

bool rf_uhd_rx_wait_lo_locked(void* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;

  // wait for clock source to lock
  char* sensor_name = "lo_locked";
  bool  is_locked   = false;

  // blocks until sensor is blocked
  uhd_error error = wait_sensor_locked(handler, sensor_name, false, 300, &is_locked);

  if (!is_locked || error != UHD_ERROR_NONE) {
    fprintf(stderr,
            "Could not lock reference clock source. Sensor: %s=%s, UHD error: %i\n",
            sensor_name,
            is_locked ? "true" : "false",
            error);
  }

  return is_locked;
}

int rf_uhd_start_rx_stream(void* h, bool now)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;

  uhd_stream_cmd_t stream_cmd = {.stream_mode = UHD_STREAM_MODE_START_CONTINUOUS, .stream_now = now};
  if (!now) {
    uhd_usrp_get_time_now(handler->usrp, 0, &stream_cmd.time_spec_full_secs, &stream_cmd.time_spec_frac_secs);
    stream_cmd.time_spec_frac_secs += 0.5;
    if (stream_cmd.time_spec_frac_secs > 1) {
      stream_cmd.time_spec_frac_secs -= 1;
      stream_cmd.time_spec_full_secs += 1;
    }
  }
  uhd_rx_streamer_issue_stream_cmd(handler->rx_stream, &stream_cmd);
  return 0;
}

int rf_uhd_stop_rx_stream(void* h)
{
  rf_uhd_handler_t* handler    = (rf_uhd_handler_t*)h;
  uhd_stream_cmd_t  stream_cmd = {.stream_mode = UHD_STREAM_MODE_STOP_CONTINUOUS, .stream_now = true};
  uhd_rx_streamer_issue_stream_cmd(handler->rx_stream, &stream_cmd);
  return 0;
}

void rf_uhd_flush_buffer(void* h)
{
  int   n;
  void* data[SRSLTE_MAX_CHANNELS] = {};
  for (int i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
    data[i] = zero_mem;
  }
  do {
    n = rf_uhd_recv_with_time_multi(h, data, 1024, 0, NULL, NULL);
  } while (n > 0);
}

bool rf_uhd_has_rssi(void* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  return handler->has_rssi;
}

bool get_has_rssi(void* h)
{
  rf_uhd_handler_t*        handler = (rf_uhd_handler_t*)h;
  uhd_string_vector_handle rx_sensors;
  uhd_string_vector_make(&rx_sensors);
  uhd_usrp_get_rx_sensor_names(handler->usrp, 0, &rx_sensors);
  bool ret = find_string(rx_sensors, "rssi");
  uhd_string_vector_free(&rx_sensors);
  return ret;
}

float rf_uhd_get_rssi(void* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  if (handler->has_rssi) {
    double val_out;

    uhd_sensor_value_handle rssi_value;
    uhd_sensor_value_make_from_realnum(&rssi_value, "rssi", 0, "dBm", "%f");
    uhd_usrp_get_rx_sensor(handler->usrp, "rssi", 0, &rssi_value);
    uhd_sensor_value_to_realnum(rssi_value, &val_out);
    uhd_sensor_value_free(&rssi_value);

    return val_out;
  } else {
    return 0.0;
  }
}

int rf_uhd_open(char* args, void** h)
{
  return rf_uhd_open_multi(args, h, 1);
}

int rf_uhd_open_multi(char* args, void** h, uint32_t nof_channels)
{
  uhd_error error;

  if (h) {
    *h = NULL;

    if (nof_channels > SRSLTE_MAX_CHANNELS) {
      fprintf(stderr,
              "Error opening UHD: maximum number of channels exceeded (%d>%d)\n",
              nof_channels,
              SRSLTE_MAX_CHANNELS);
      return SRSLTE_ERROR;
    }

    rf_uhd_handler_t* handler = (rf_uhd_handler_t*)malloc(sizeof(rf_uhd_handler_t));
    if (!handler) {
      perror("malloc");
      return SRSLTE_ERROR;
    }

    bzero(handler, sizeof(rf_uhd_handler_t));
    *h = handler;

    // Disable fast-path (U/L/O) messages
    setenv("UHD_LOG_FASTPATH_DISABLE", "1", 0);

    /* Set priority to UHD threads */
    uhd_set_thread_priority(uhd_default_thread_priority, true);

    /* Find available devices */
    uhd_string_vector_handle devices_str;
    uhd_string_vector_make(&devices_str);
    uhd_usrp_find("", &devices_str);

    char args2[512];

    handler->dynamic_rate = true;

    // Allow NULL parameter
    if (args == NULL) {
      args = "";
    }
    handler->devname = NULL;

    pthread_mutex_init(&handler->tx_mutex, NULL);

    // Initialize handler
    handler->uhd_error_handler = NULL;

    srslte_vec_cf_zero(zero_mem, 64 * 1024);

    // Check external clock argument
    enum { DEFAULT, EXTERNAL, GPSDO } clock_src;
    if (strstr(args, "clock=external")) {
      REMOVE_SUBSTRING_WITHCOMAS(args, "clock=external");
      clock_src = EXTERNAL;
    } else if (strstr(args, "clock=gpsdo")) {
      printf("Using GPSDO clock\n");
      REMOVE_SUBSTRING_WITHCOMAS(args, "clock=gpsdo");
      clock_src = GPSDO;
    } else {
      clock_src = DEFAULT;
    }

#if HAVE_ASYNC_THREAD
    bool start_async_thread = true;
    if (strstr(args, "silent")) {
      REMOVE_SUBSTRING_WITHCOMAS(args, "silent");
      start_async_thread = false;
    }
#endif

    // Set over the wire format
    char* otw_format = "sc16";
    if (strstr(args, "otw_format=sc12")) {
      REMOVE_SUBSTRING_WITHCOMAS(args, "otw_format=sc12");
      otw_format = "sc12";
    } else if (strstr(args, "otw_format=sc16")) {
      REMOVE_SUBSTRING_WITHCOMAS(args, "otw_format=sc16");
      /* Do nothing */
    } else if (strstr(args, "otw_format=")) {
      ERROR("Wrong over the wire format. Valid formats: sc12, sc16\n");
      return SRSLTE_ERROR;
    }

    // Set transmitter subdevice spec string
    const char tx_subdev_arg[]   = "tx_subdev_spec=";
    char       tx_subdev_str[64] = {0};
    char*      tx_subdev_ptr     = strstr(args, tx_subdev_arg);
    if (tx_subdev_ptr) {
      copy_subdev_string(tx_subdev_str, tx_subdev_ptr + strlen(tx_subdev_arg));
    }

    // Set receiver subdevice spec string
    const char rx_subdev_arg[]   = "rx_subdev_spec=";
    char       rx_subdev_str[64] = {0};
    char*      rx_subdev_ptr     = strstr(args, rx_subdev_arg);
    if (rx_subdev_ptr) {
      copy_subdev_string(rx_subdev_str, rx_subdev_ptr + strlen(rx_subdev_arg));
    }

    if (tx_subdev_ptr) {
      remove_substring(args, tx_subdev_arg);
      remove_substring(args, tx_subdev_str);
    }

    if (rx_subdev_ptr) {
      remove_substring(args, rx_subdev_arg);
      remove_substring(args, rx_subdev_str);
    }

    /* If device type or name not given in args, choose a B200 */
    if (args[0] == '\0') {
      if (find_string(devices_str, "type=b200") && !strstr(args, "recv_frame_size")) {
        // If B200 is available, use it
        args                          = "type=b200,master_clock_rate=23.04e6";
        handler->current_master_clock = 23040000;
        handler->devname              = DEVNAME_B200;
      } else if (find_string(devices_str, "type=x300")) {
        // Else if X300 is available, set master clock rate now (can't be changed later)
        args                          = "type=x300,master_clock_rate=184.32e6";
        handler->current_master_clock = 184320000;
        handler->dynamic_rate         = false;
        handler->devname              = DEVNAME_X300;
      } else if (find_string(devices_str, "type=e3x0")) {
        // Else if E3X0 is available, set master clock rate now (can't be changed later)
        args                  = "type=e3x0,master_clock_rate=30.72e6";
        handler->dynamic_rate = false;
        handler->devname      = DEVNAME_E3X0;
      } else if (find_string(devices_str, "type=n3xx")) {
        args                          = "type=n3xx,master_clock_rate=122.88e6";
        handler->current_master_clock = 122880000;
        handler->dynamic_rate         = false;
        handler->devname              = DEVNAME_N300;
        srslte_use_standard_symbol_size(true);
      }
    } else {
      // If args is set and x300 type is specified, make sure master_clock_rate is defined
      if (strstr(args, "type=x300")) {
        sprintf(args2, "%s,master_clock_rate=184.32e6", args);
        args                          = args2;
        handler->current_master_clock = 184320000;
        handler->dynamic_rate         = false;
        handler->devname              = DEVNAME_X300;
      } else if (strstr(args, "type=n3xx")) {
        sprintf(args2, "%s,master_clock_rate=122.88e6", args);
        args                          = args2;
        handler->current_master_clock = 122880000;
        handler->dynamic_rate         = false;
        handler->devname              = DEVNAME_N300;
        srslte_use_standard_symbol_size(true);
      } else if (strstr(args, "type=e3x0")) {
        snprintf(args2, sizeof(args2), "%s,master_clock_rate=30.72e6", args);
        args             = args2;
        handler->devname = DEVNAME_E3X0;
      } else {
        snprintf(args2, sizeof(args2), "%s,master_clock_rate=23.04e6", args);
        args                          = args2;
        handler->current_master_clock = 23040000;
        handler->devname              = DEVNAME_B200;
      }
    }

    uhd_string_vector_free(&devices_str);

    /* Create UHD handler */
    printf("Opening USRP channels=%d, args: %s\n", nof_channels, args);
    if (uhd_alloc(handler, args)) {
      uhd_free(handler);
      return SRSLTE_ERROR;
    }

    /* Set transmitter subdev spec if specified */
    if (strlen(tx_subdev_str)) {
      uhd_subdev_spec_handle subdev_spec_handle = {0};

      printf("Setting tx_subdev_spec to '%s'\n", tx_subdev_str);

      uhd_subdev_spec_make(&subdev_spec_handle, tx_subdev_str);
      uhd_usrp_set_tx_subdev_spec(handler->usrp, subdev_spec_handle, 0);
      uhd_subdev_spec_free(&subdev_spec_handle);
    }

    /* Set receiver subdev spec if specified */
    if (strlen(rx_subdev_str)) {
      uhd_subdev_spec_handle subdev_spec_handle = {0};

      printf("Setting rx_subdev_spec to '%s'\n", rx_subdev_str);

      uhd_subdev_spec_make(&subdev_spec_handle, rx_subdev_str);
      uhd_usrp_set_rx_subdev_spec(handler->usrp, subdev_spec_handle, 0);
      uhd_subdev_spec_free(&subdev_spec_handle);
    }

    if (!handler->devname) {
      char dev_str[1024];
      uhd_usrp_get_mboard_name(handler->usrp, 0, dev_str, 1024);
      if (strstr(dev_str, "B2") || strstr(dev_str, "B2")) {
        handler->devname = DEVNAME_B200;
      } else if (strstr(dev_str, "X3") || strstr(dev_str, "X3")) {
        handler->devname = DEVNAME_X300;
      } else if (strstr(dev_str, "n3xx")) {
        handler->devname = DEVNAME_N300;
      }
    }
    if (!handler->devname) {
      handler->devname = "uhd_unknown";
    }

    bool  is_locked   = false;
    char* sensor_name = "";

    // Set external clock reference
    if (clock_src == EXTERNAL) {
      uhd_usrp_set_clock_source(handler->usrp, "external", 0);
      uhd_usrp_set_time_source(handler->usrp, "external", 0);
      sensor_name = "ref_locked";
    } else if (clock_src == GPSDO) {
      uhd_usrp_set_clock_source(handler->usrp, "gpsdo", 0);
      uhd_usrp_set_time_source(handler->usrp, "gpsdo", 0);
      set_time_to_gps_time(handler);
      sensor_name = "gps_locked";
    }
    // wait until external reference / GPS is locked
    if (clock_src != DEFAULT) {
      // blocks until clock source is locked
      error = wait_sensor_locked(handler, sensor_name, true, 300, &is_locked);

      if (!is_locked || error != UHD_ERROR_NONE) {
        fprintf(stderr,
                "Could not lock reference clock source. Sensor: %s=%s, UHD error: %i\n",
                sensor_name,
                is_locked ? "true" : "false",
                error);
      }
    }

    handler->has_rssi = get_has_rssi(handler);

    size_t channel[SRSLTE_MAX_CHANNELS] = {};
    for (int i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
      channel[i] = i;
    }
    uhd_stream_args_t stream_args = {
        .cpu_format   = "fc32",
        .otw_format   = otw_format,
        .args         = "",
        .channel_list = channel,
        .n_channels   = nof_channels,
    };

    handler->nof_rx_channels = nof_channels;
    handler->nof_tx_channels = nof_channels;

    /* Set default rate to avoid decimation warnings and warn about USB2 low throughput */
    double default_srate = (double)srslte_sampling_freq_hz(SRSLTE_MAX_PRB); // Consider standard rates
    for (int i = 0; i < nof_channels; i++) {
      uhd_usrp_set_rx_rate(handler->usrp, default_srate, i);
      uhd_usrp_set_tx_rate(handler->usrp, default_srate, i);
    }

    if (nof_channels > 1 && clock_src != GPSDO) {
      uhd_usrp_set_time_unknown_pps(handler->usrp, 0, 0.0);
    }

    /* Initialize rx and tx stremers */
    error = uhd_usrp_get_rx_stream(handler->usrp, &stream_args, handler->rx_stream);
    if (error) {
      uhd_free(handler);
      ERROR("Error opening RX stream: %d\n", error);
      return SRSLTE_ERROR;
    }

    error = uhd_usrp_get_tx_stream(handler->usrp, &stream_args, handler->tx_stream);
    if (error) {
      uhd_free(handler);
      ERROR("Error opening TX stream: %d\n", error);
      return SRSLTE_ERROR;
    }

    uhd_rx_streamer_max_num_samps(handler->rx_stream, &handler->rx_nof_samples);
    uhd_tx_streamer_max_num_samps(handler->tx_stream, &handler->tx_nof_samples);

    uhd_meta_range_handle rx_gain_range = NULL;
    uhd_meta_range_make(&rx_gain_range);
    uhd_usrp_get_rx_gain_range(handler->usrp, "", 0, rx_gain_range);
    uhd_meta_range_start(rx_gain_range, &handler->info.min_rx_gain);
    uhd_meta_range_stop(rx_gain_range, &handler->info.max_rx_gain);
    uhd_meta_range_free(&rx_gain_range);

    uhd_meta_range_handle tx_gain_range = NULL;
    uhd_meta_range_make(&tx_gain_range);
    uhd_usrp_get_tx_gain_range(handler->usrp, "", 0, tx_gain_range);
    uhd_meta_range_start(tx_gain_range, &handler->info.min_tx_gain);
    uhd_meta_range_stop(tx_gain_range, &handler->info.max_tx_gain);
    uhd_meta_range_free(&tx_gain_range);

    // Set starting gain to half maximum in case of using AGC
    rf_uhd_set_rx_gain(handler, handler->info.max_rx_gain * 0.7);

#if HAVE_ASYNC_THREAD
    if (start_async_thread) {
      // Start low priority thread to receive async commands
      handler->async_thread_running = true;
      if (pthread_create(&handler->async_thread, NULL, async_thread, handler)) {
        uhd_free(handler);
        ERROR("pthread_create\n");
        return SRSLTE_ERROR;
      }
    }
#endif

    /* Restore priorities  */
    uhd_set_thread_priority(0, false);

    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int rf_uhd_close(void* h)
{
  rf_uhd_stop_rx_stream(h);

  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;

  /// Free all made UHD objects
  uhd_free(handler);

  /** Something else to close the USRP?? */
  return SRSLTE_SUCCESS;
}

void rf_uhd_set_master_clock_rate(void* h, double rate)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  if (handler->dynamic_rate && handler->current_master_clock != rate) {
    uhd_usrp_set_master_clock_rate(handler->usrp, rate, 0);
    handler->current_master_clock = rate;
  }
}

double rf_uhd_set_rx_srate(void* h, double freq)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;

  // Set master clock rate
  if (fmod(handler->current_master_clock, freq)) {
    rf_uhd_set_master_clock_rate(handler, freq);
  }

  if (handler->nof_rx_channels > 1) {
#ifdef UHD_SUPPORTS_COMMAND_TIME
    time_t full;
    double frac;
    uhd_usrp_get_time_now(handler->usrp, 0, &full, &frac);
    frac += 0.100;
    if (frac >= 1.0) {
      full++;
      frac -= 1.0;
    };
    uhd_usrp_set_command_time(handler->usrp, full, frac, 0);
#endif /* UHD_SUPPORTS_COMMAND_TIME */
    for (int i = 0; i < handler->nof_rx_channels; i++) {
      uhd_usrp_set_rx_rate(handler->usrp, freq, i);
    }
#ifdef UHD_SUPPORTS_COMMAND_TIME
    usleep(100000);
#endif /* UHD_SUPPORTS_COMMAND_TIME */
  } else {
    uhd_usrp_set_rx_rate(handler->usrp, freq, 0);
  }
  return freq;
}

double rf_uhd_set_tx_srate(void* h, double freq)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;

  // Set master clock rate
  if (fmod(handler->current_master_clock, freq)) {
    rf_uhd_set_master_clock_rate(handler, 4 * freq);
  }

  if (handler->nof_tx_channels > 1) {
#ifdef UHD_SUPPORTS_COMMAND_TIME
    time_t full;
    double frac;
    uhd_usrp_get_time_now(handler->usrp, 0, &full, &frac);
    frac += 0.100;
    if (frac >= 1.0) {
      full++;
      frac -= 1.0;
    };
    uhd_usrp_set_command_time(handler->usrp, full, frac, 0);
#endif /* UHD_SUPPORTS_COMMAND_TIME */
    for (int i = 0; i < handler->nof_tx_channels; i++) {
      uhd_usrp_set_tx_rate(handler->usrp, freq, i);
    }
#ifdef UHD_SUPPORTS_COMMAND_TIME
    usleep(100000);
#endif /* UHD_SUPPORTS_COMMAND_TIME */
  } else {
    uhd_usrp_set_tx_rate(handler->usrp, freq, 0);
  }
  handler->tx_rate = freq;
  return freq;
}

double rf_uhd_set_rx_gain(void* h, double gain)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  for (int i = 0; i < handler->nof_rx_channels; i++) {
    uhd_usrp_set_rx_gain(handler->usrp, gain, i, "");
  }
  return gain;
}

double rf_uhd_set_tx_gain(void* h, double gain)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  for (int i = 0; i < handler->nof_tx_channels; i++) {
    uhd_usrp_set_tx_gain(handler->usrp, gain, i, "");
  }
  return gain;
}

double rf_uhd_get_rx_gain(void* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  double            gain;
  uhd_usrp_get_rx_gain(handler->usrp, 0, "", &gain);
  return gain;
}

double rf_uhd_get_tx_gain(void* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  double            gain;
  uhd_usrp_get_tx_gain(handler->usrp, 0, "", &gain);
  return gain;
}

srslte_rf_info_t* rf_uhd_get_info(void* h)
{
  srslte_rf_info_t* info = NULL;
  if (h) {
    rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
    info                      = &handler->info;
  }
  return info;
}

double rf_uhd_set_rx_freq(void* h, uint32_t ch, double freq)
{
  uhd_tune_request_t tune_request = {
      .target_freq     = freq,
      .rf_freq_policy  = UHD_TUNE_REQUEST_POLICY_AUTO,
      .dsp_freq_policy = UHD_TUNE_REQUEST_POLICY_AUTO,
  };
  uhd_tune_result_t tune_result;
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  if (ch < handler->nof_rx_channels) {
    uhd_usrp_set_rx_freq(handler->usrp, &tune_request, ch, &tune_result);
  } else {
    for (int i = 0; i < handler->nof_rx_channels; i++) {
      uhd_usrp_set_rx_freq(handler->usrp, &tune_request, i, &tune_result);
    }
  }
  rf_uhd_rx_wait_lo_locked(handler);
  return freq;
}

double rf_uhd_set_tx_freq(void* h, uint32_t ch, double freq)
{
  uhd_tune_request_t tune_request = {
      .target_freq     = freq,
      .rf_freq_policy  = UHD_TUNE_REQUEST_POLICY_AUTO,
      .dsp_freq_policy = UHD_TUNE_REQUEST_POLICY_AUTO,
  };
  uhd_tune_result_t tune_result;
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  if (ch < handler->nof_tx_channels) {
    uhd_usrp_set_tx_freq(handler->usrp, &tune_request, ch, &tune_result);
  } else {
    for (int i = 0; i < handler->nof_tx_channels; i++) {
      uhd_usrp_set_tx_freq(handler->usrp, &tune_request, i, &tune_result);
    }
  }
  return freq;
}

void rf_uhd_get_time(void* h, time_t* secs, double* frac_secs)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  uhd_usrp_get_time_now(handler->usrp, 0, secs, frac_secs);
}

void rf_uhd_sync_pps(void* h)
{
  if (h) {
    rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
    uhd_usrp_set_time_unknown_pps(handler->usrp, 0, 0);
  }
}

int rf_uhd_recv_with_time(void* h, void* data, uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs)
{
  return rf_uhd_recv_with_time_multi(h, &data, nsamples, blocking, secs, frac_secs);
}

int rf_uhd_recv_with_time_multi(void*    h,
                                void*    data[SRSLTE_MAX_PORTS],
                                uint32_t nsamples,
                                bool     blocking,
                                time_t*  secs,
                                double*  frac_secs)
{
  rf_uhd_handler_t*       handler           = (rf_uhd_handler_t*)h;
  uhd_rx_metadata_handle* md                = &handler->rx_md_first;
  size_t                  rxd_samples       = 0;
  size_t                  rxd_samples_total = 0;
  int                     trials            = 0;
  if (blocking) {
    while (rxd_samples_total < nsamples && trials < 100) {
      void* buffs_ptr[SRSLTE_MAX_CHANNELS] = {};
      for (int i = 0; i < handler->nof_rx_channels; i++) {
        cf_t* data_c = (cf_t*)data[i];
        buffs_ptr[i] = &data_c[rxd_samples_total];
      }

      size_t num_samps_left = nsamples - rxd_samples_total;
      size_t num_rx_samples = (num_samps_left > handler->rx_nof_samples) ? handler->rx_nof_samples : num_samps_left;

      rxd_samples = 0;
      uhd_error error =
          uhd_rx_streamer_recv(handler->rx_stream, buffs_ptr, num_rx_samples, md, 1.0, false, &rxd_samples);
      if (error) {
        ERROR("Error receiving from UHD: %d\n", error);
        log_rx_error(handler);
        return -1;
      }

      uhd_rx_metadata_error_code_t error_code = 0;
      uhd_rx_metadata_error_code(*md, &error_code);

      md = &handler->rx_md;
      rxd_samples_total += rxd_samples;
      trials++;

      if (error_code == UHD_RX_METADATA_ERROR_CODE_OVERFLOW) {
        log_overflow(handler);
      } else if (error_code == UHD_RX_METADATA_ERROR_CODE_LATE_COMMAND) {
        log_late(handler, true);
      } else if (error_code == UHD_RX_METADATA_ERROR_CODE_TIMEOUT) {
        ERROR("Error timed out while receiving samples from UHD.\n");
        return -1;
      } else if (error_code != UHD_RX_METADATA_ERROR_CODE_NONE) {
        ERROR("Error code 0x%x was returned during streaming. Aborting.\n", error_code);
        INFO("Error code 0x%x was returned during streaming. Aborting.\n", error_code);
      }
    }
  } else {
    uhd_error error   = uhd_rx_streamer_recv(handler->rx_stream, data, nsamples, md, 0.0, false, &rxd_samples);
    rxd_samples_total = rxd_samples;
    if (error) {
      ERROR("Error receiving from UHD: %d\n", error);
      log_rx_error(handler);
      return -1;
    }
  }
  if (secs && frac_secs) {
    uhd_rx_metadata_time_spec(handler->rx_md_first, secs, frac_secs);
  }
  return rxd_samples_total;
}

int rf_uhd_send_timed(void*  h,
                      void*  data,
                      int    nsamples,
                      time_t secs,
                      double frac_secs,
                      bool   has_time_spec,
                      bool   blocking,
                      bool   is_start_of_burst,
                      bool   is_end_of_burst)
{
  // Maximum number of channels to NULL
  void* _data[SRSLTE_MAX_CHANNELS] = {NULL};

  // Set only first channel
  _data[0] = data;

  return rf_uhd_send_timed_multi(
      h, _data, nsamples, secs, frac_secs, has_time_spec, blocking, is_start_of_burst, is_end_of_burst);
}

int rf_uhd_send_timed_multi(void*  h,
                            void** data,
                            int    nsamples,
                            time_t secs,
                            double frac_secs,
                            bool   has_time_spec,
                            bool   blocking,
                            bool   is_start_of_burst,
                            bool   is_end_of_burst)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;

  pthread_mutex_lock(&handler->tx_mutex);
  int ret = -1;

  /* Resets the USRP time TODO: this might cause problems for burst transmissions */
  if (!has_time_spec && is_start_of_burst && handler->nof_tx_channels > 1) {
    uhd_usrp_set_time_now(handler->usrp, 0, 0, 0);
    uhd_tx_metadata_set_time_spec(&handler->tx_md, 0, 0.1);
  }

  size_t txd_samples;
  int    trials = 0;
  if (blocking) {
    if (has_time_spec) {
      uhd_tx_metadata_set_time_spec(&handler->tx_md, secs, frac_secs);
    }
    int   n = 0;
    cf_t* data_c[SRSLTE_MAX_CHANNELS] = {};
    for (int i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
      data_c[i] = data[i] ? data[i] : zero_mem;
    }
    do {
      size_t tx_samples = handler->tx_nof_samples;

      // First packet is start of burst if so defined, others are never
      if (n == 0) {
        uhd_tx_metadata_set_start(&handler->tx_md, is_start_of_burst);
      } else {
        uhd_tx_metadata_set_start(&handler->tx_md, false);
      }

      // middle packets are never end of burst, last one as defined
      if (nsamples - n > tx_samples) {
        uhd_tx_metadata_set_end(&handler->tx_md, false);
      } else {
        tx_samples = nsamples - n;
        uhd_tx_metadata_set_end(&handler->tx_md, is_end_of_burst);
      }

      const void* buffs_ptr[SRSLTE_MAX_CHANNELS] = {};
      for (int i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
        void* buff   = (void*)&data_c[i][n];
        buffs_ptr[i] = buff;
      }
      uhd_error error =
          uhd_tx_streamer_send(handler->tx_stream, buffs_ptr, tx_samples, &handler->tx_md, 1.0, &txd_samples);
      if (error) {
        ERROR("Error sending to UHD: %d\n", error);
        goto unlock;
      }
      // Increase time spec
      uhd_tx_metadata_add_time_spec(&handler->tx_md, txd_samples / handler->tx_rate);
      n += txd_samples;
      trials++;
    } while (n < nsamples && trials < 100);

    ret = nsamples;

  } else {

    const void* buffs_ptr[SRSLTE_MAX_CHANNELS] = {};
    for (int i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
      buffs_ptr[i] = data[i];
    }
    uhd_tx_metadata_set_has_time_spec(&handler->tx_md, is_start_of_burst);
    uhd_tx_metadata_set_start(&handler->tx_md, is_start_of_burst);
    uhd_tx_metadata_set_end(&handler->tx_md, is_end_of_burst);
    uhd_error error = uhd_tx_streamer_send(handler->tx_stream, buffs_ptr, nsamples, &handler->tx_md, 0.0, &txd_samples);
    if (error) {
      ERROR("Error sending to UHD: %d\n", error);
      goto unlock;
    }

    ret = txd_samples;
  }
unlock:
  pthread_mutex_unlock(&handler->tx_mutex);
  return ret;
}
