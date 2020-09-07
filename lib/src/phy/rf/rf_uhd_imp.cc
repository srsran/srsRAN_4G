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

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <uhd.h>
#include <uhd/types/sensors.h>
#include <uhd/usrp/multi_usrp.hpp>

#include "rf_helper.h"
#include "srslte/srslte.h"

#include "rf_uhd_generic.h"
#include "rf_uhd_imp.h"

#ifdef UHD_ENABLE_RFNOC
#include "rf_uhd_rfnoc.h"
#endif // UHD_ENABLE_RFNOC

#define HAVE_ASYNC_THREAD 1

/**
 * Transmitter finite state machine. This FSM manages the burst state of the transmitter. If the driver detects an
 * Underflow, Overflow or Late, the FSM will make sure the burst is ended and started at the next transmission. The FSM
 * has the following states:
 * - BURST: A burst has started
 * - END_OF_BURST: An underflow, overflow or late has been detected, the next transmission shall be aborted and an end
 *                 of burst will be send in the next transmission;
 * - START_BURST: The next transmission will be flagged as start of burst.
 *
 *   +-------+ L/O/U detected +--------------+   EoB Sent   +-------------+
 *   | Burst |--------------->| End-of-burst |------------->| Start burst |<---  Initial state
 *   +-------+   |            +--------------+         ^    +-------------+
 *      ^        |                                     |           |
 *      |        |               Burst ACK             |           |
 *      |        +-------------------------------------+           |
 *      |                                                          |
 *      |               Start of burst is transmitted              |
 *      +----------------------------------------------------------+
 */
typedef enum {
  RF_UHD_IMP_TX_STATE_START_BURST = 0,
  RF_UHD_IMP_TX_STATE_BURST,
  RF_UHD_IMP_TX_STATE_END_OF_BURST,
} rf_uhd_imp_underflow_state_t;

/**
 * List of devices that do NOT support dynamic master-clock-rate
 */
const std::set<std::string> RH_UHD_IMP_FIX_MASTER_CLOCK_RATE_DEVICE_LIST = {"x300", "n3xx", "e3x0"};

/**
 * List of devices that do NOT support stream stop/start after a time out
 */
const std::set<std::string> RF_UHD_IMP_PROHIBITED_STREAM_REMAKE = {DEVNAME_X300,
                                                                   DEVNAME_N300,
                                                                   DEVNAME_E3X0,
                                                                   DEVNAME_B200};

/**
 * List of devices that do NOT require/support to restart streaming after rate changes/timeouts
 */
const std::set<std::string> RF_UHD_IMP_PROHIBITED_STOP_START = {DEVNAME_B200};

/**
 * Defines a delay used between the current USRP time and the start of the transmission. This value needs to be high
 * enough for being distributed to all the devices before the time expires and short enough to be as seamless as
 * possible.
 */
static const double RF_UHD_IMP_STREAM_DELAY_S = 0.1;

/**
 * Defines a delay used between the current USRP time and a timed command.
 */
static const double RF_UHD_IMP_TIMED_COMMAND_DELAY_S = 0.1;

/**
 * Send and receive timeout in seconds
 */
static const double RF_UHD_IMP_TRX_TIMEOUT_S = 0.5;

/**
 * Receive asynchronous message receiver timeout
 */
static const double RF_UHD_IMP_ASYNCH_MSG_TIMEOUT_S = 0.0;

/**
 * Asynchronous message receiver sleep time
 */
static const std::chrono::milliseconds RF_UHD_IMP_ASYNCH_MSG_SLEEP_MS = std::chrono::milliseconds(100);

/**
 * Maximum of Rx Trials
 */
static const uint32_t RF_UHD_IMP_MAX_RX_TRIALS = 100;

struct rf_uhd_handler_t {
  std::string                            devname;
  std::shared_ptr<rf_uhd_safe_interface> uhd = nullptr;

  srslte_rf_info_t info;
  size_t           rx_nof_samples      = 0;
  size_t           tx_nof_samples      = 0;
  double           tx_rate             = 1.92e6;
  double           rx_rate             = 1.92e6;
  bool             dynamic_master_rate = true;
  uint32_t         nof_rx_channels     = 0;
  uint32_t         nof_tx_channels     = 0;

  srslte_rf_error_handler_t    uhd_error_handler     = nullptr;
  void*                        uhd_error_handler_arg = nullptr;
  rf_uhd_imp_underflow_state_t tx_state              = RF_UHD_IMP_TX_STATE_START_BURST;

  double current_master_clock = 0.0;

  bool rx_stream_enabled = false;

  std::mutex tx_mutex;
  std::mutex rx_mutex;

#if HAVE_ASYNC_THREAD
  // Asynchronous transmission message thread
  bool                    async_thread_running = false;
  std::thread             async_thread;
  std::mutex              async_mutex;
  std::condition_variable async_cvar;
#endif /* HAVE_ASYNC_THREAD */
};

#if UHD_VERSION < 31100
static void (*handler)(const char*);

void translate_handler(uhd::msg::type_t type, const std::string& msg)
{
  if (handler)
    handler(msg.c_str());
}
#endif

void rf_uhd_register_msg_handler_c(void (*new_handler)(const char*))
{
#if UHD_VERSION < 31100
  handler = new_handler;
  uhd::msg::register_handler(translate_handler);
#endif
}

void suppress_handler(const char* x)
{
  // do nothing
}

static cf_t zero_mem[64 * 1024] = {};

#define print_usrp_error(h)                                                                                            \
  do {                                                                                                                 \
    ERROR("USRP reported the following error: %s\n", h->uhd->last_error.c_str());                                      \
  } while (false)

static void log_overflow(rf_uhd_handler_t* h)
{
  if (h->tx_state == RF_UHD_IMP_TX_STATE_BURST) {
    h->tx_state = RF_UHD_IMP_TX_STATE_END_OF_BURST;
  }

  if (h->uhd_error_handler != nullptr) {
    srslte_rf_error_t error;
    bzero(&error, sizeof(srslte_rf_error_t));
    error.type = srslte_rf_error_t::SRSLTE_RF_ERROR_OVERFLOW;
    h->uhd_error_handler(h->uhd_error_handler_arg, error);
  }
}

static void log_late(rf_uhd_handler_t* h, bool is_rx)
{
  if (h->tx_state == RF_UHD_IMP_TX_STATE_BURST) {
    h->tx_state = RF_UHD_IMP_TX_STATE_END_OF_BURST;
  }

  if (h->uhd_error_handler != nullptr) {
    srslte_rf_error_t error;
    bzero(&error, sizeof(srslte_rf_error_t));
    error.opt  = is_rx ? 1 : 0;
    error.type = srslte_rf_error_t::SRSLTE_RF_ERROR_LATE;
    h->uhd_error_handler(h->uhd_error_handler_arg, error);
  }
}

#if HAVE_ASYNC_THREAD
static void log_underflow(rf_uhd_handler_t* h)
{
  // Flag underflow
  if (h->tx_state == RF_UHD_IMP_TX_STATE_BURST) {
    h->tx_state = RF_UHD_IMP_TX_STATE_END_OF_BURST;
  }
  if (h->uhd_error_handler != nullptr) {
    srslte_rf_error_t error;
    bzero(&error, sizeof(srslte_rf_error_t));
    error.type = srslte_rf_error_t::SRSLTE_RF_ERROR_UNDERFLOW;
    h->uhd_error_handler(h->uhd_error_handler_arg, error);
  }
}
#endif

static void log_rx_error(rf_uhd_handler_t* h)
{
  if (h->uhd_error_handler) {
    ERROR("USRP reported the following error: %s\n", h->uhd->last_error.c_str());

    srslte_rf_error_t error;
    bzero(&error, sizeof(srslte_rf_error_t));
    error.type = srslte_rf_error_t::SRSLTE_RF_ERROR_RX;
    h->uhd_error_handler(h->uhd_error_handler_arg, error);
  }
}

#if HAVE_ASYNC_THREAD
static void* async_thread(void* h)
{
  rf_uhd_handler_t*     handler = (rf_uhd_handler_t*)h;
  uhd::async_metadata_t md;

  while (handler->async_thread_running) {
    std::unique_lock<std::mutex> lock(handler->async_mutex);
    bool                         valid = false;

    // If the Tx stream is NULL wait for tx_cvar
    if (not handler->uhd->is_tx_ready()) {
      handler->async_cvar.wait(lock);
    }

    if (handler->uhd->is_tx_ready()) {
      lock.unlock();
      if (handler->uhd->recv_async_msg(md, RF_UHD_IMP_ASYNCH_MSG_TIMEOUT_S, valid) != UHD_ERROR_NONE) {
        print_usrp_error(handler);
        return nullptr;
      }

      if (valid) {
        const uhd::async_metadata_t::event_code_t& event_code = md.event_code;
        if (event_code == uhd::async_metadata_t::EVENT_CODE_UNDERFLOW ||
            event_code == uhd::async_metadata_t::EVENT_CODE_UNDERFLOW_IN_PACKET) {
          log_underflow(handler);
        } else if (event_code == uhd::async_metadata_t::EVENT_CODE_TIME_ERROR) {
          log_late(handler, false);
        } else if (event_code == uhd::async_metadata_t::EVENT_CODE_BURST_ACK) {
          // Makes sure next block will be start of burst
          if (handler->tx_state == RF_UHD_IMP_TX_STATE_BURST) {
            handler->tx_state = RF_UHD_IMP_TX_STATE_START_BURST;
          }
        } else {
          ERROR("UHD unhandled event code %d\n", event_code);
        }
      } else {
        std::this_thread::sleep_for(RF_UHD_IMP_ASYNCH_MSG_SLEEP_MS);
      }
    }
  }

  return nullptr;
}
#endif

static inline void uhd_free(rf_uhd_handler_t* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;

  // NULL handler, return
  if (handler == nullptr) {
    return;
  }

#if HAVE_ASYNC_THREAD
  if (handler->async_thread_running) {
    handler->async_thread_running = false;
    handler->async_cvar.notify_all();
    handler->async_thread.join();
  }
#endif

  delete handler;
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
  for (size_t i = 0; i < n; i++) {
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
static int set_time_to_gps_time(rf_uhd_handler_t* handler)
{
  const std::string sensor_name = "gps_time";

  std::vector<std::string> sensors;
  if (handler->uhd->get_mboard_sensor_names(sensors) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  // Find sensor
  bool found = false;
  for (std::string& sensor : sensors) {
#if PRINT_SENSOR_NAMES
    printf("sensor %s present\n", sensor.c_str());
#endif

    if (sensor == sensor_name) {
      found = true;
      break;
    }
  }

  // No sensor found
  if (not found) {
    ERROR("Sensor '%s` not found.\n", sensor_name.c_str());
    return UHD_ERROR_NONE;
  }

  // Get actual sensor value
  double frac_secs = 0.0;
  if (handler->uhd->get_sensor(sensor_name, frac_secs) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  // Get time and set
  printf("Setting USRP time to %fs\n", frac_secs);
  if (handler->uhd->set_time_unknown_pps(uhd::time_spec_t(frac_secs)) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

// timeout in ms
static int wait_sensor_locked(rf_uhd_handler_t*  handler,
                              const std::string& sensor_name,
                              bool               is_mboard,
                              int                timeout,
                              bool&              is_locked)
{
  is_locked = false;

  // Get sensor list
  std::vector<std::string> sensors;
  if (is_mboard) {
    // motherboard sensor
    if (handler->uhd->get_mboard_sensor_names(sensors) != UHD_ERROR_NONE) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }
  } else {
    // daughterboard sensor
    if (handler->uhd->get_rx_sensor_names(sensors) != UHD_ERROR_NONE) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }
  }

  // Find sensor
  bool found = false;
  for (std::string& sensor : sensors) {
#if PRINT_SENSOR_NAMES
    printf("sensor %s present\n", sensor.c_str());
#endif

    if (sensor == sensor_name) {
      found = true;
      break;
    }
  }

  // No sensor found
  if (not found) {
    ERROR("Sensor '%s` not found.\n", sensor_name.c_str());
    return UHD_ERROR_NONE;
  }

  do {
    // Get actual sensor value
    if (is_mboard) {
      if (handler->uhd->get_sensor(sensor_name, is_locked) != UHD_ERROR_NONE) {
        print_usrp_error(handler);
        return SRSLTE_ERROR;
      }
    } else {
      if (handler->uhd->get_rx_sensor(sensor_name, is_locked) != UHD_ERROR_NONE) {
        print_usrp_error(handler);
        return SRSLTE_ERROR;
      }
    }

    // Read value and wait
    usleep(1000); // 1ms
    timeout -= 1; // 1ms
  } while (not is_locked and timeout > 0);

  return SRSLTE_SUCCESS;
}

const char* rf_uhd_devname(void* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  return handler->devname.c_str();
}

bool rf_uhd_rx_wait_lo_locked(void* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;

  // wait for clock source to lock
  std::string sensor_name = "lo_locked";
  bool        is_locked   = false;

  // blocks until sensor is blocked
  int error = wait_sensor_locked(handler, sensor_name, false, 300, is_locked);

  if (not is_locked and error == SRSLTE_SUCCESS) {
    ERROR("Could not lock reference clock source. Sensor: %s=%s\n", sensor_name.c_str(), is_locked ? "true" : "false");
  }

  return is_locked;
}

static inline int rf_uhd_start_rx_stream_unsafe(rf_uhd_handler_t* handler)
{
  // Check if stream was not created or started
  if (not handler->uhd->is_rx_ready() or handler->rx_stream_enabled) {
    // Ignores command, the stream will start as soon as the Rx sampling rate is set
    return SRSLTE_SUCCESS;
  }

  // Issue stream command
  if (handler->uhd->start_rx_stream(RF_UHD_IMP_STREAM_DELAY_S) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  handler->rx_stream_enabled = true;

  return SRSLTE_SUCCESS;
}

int rf_uhd_start_rx_stream(void* h, bool now)
{
  rf_uhd_handler_t*            handler = (rf_uhd_handler_t*)h;
  std::unique_lock<std::mutex> lock(handler->rx_mutex);

  return rf_uhd_start_rx_stream_unsafe(handler);
}

static inline int rf_uhd_stop_rx_stream_unsafe(rf_uhd_handler_t* handler)
{
  // Check if stream was created or stream was not started
  if (not handler->uhd->is_rx_ready() or not handler->rx_stream_enabled) {
    // Ignores command, the stream will start as soon as the Rx sampling rate is set
    return SRSLTE_SUCCESS;
  }

  // Issue stream command
  if (handler->uhd->stop_rx_stream() != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  handler->rx_stream_enabled = false;

  return SRSLTE_SUCCESS;
}

int rf_uhd_stop_rx_stream(void* h)
{
  rf_uhd_handler_t*            handler = (rf_uhd_handler_t*)h;
  std::unique_lock<std::mutex> lock(handler->rx_mutex);

  return rf_uhd_stop_rx_stream_unsafe(handler);
}

void rf_uhd_flush_buffer(void* h)
{
  rf_uhd_handler_t*            handler = (rf_uhd_handler_t*)h;
  std::unique_lock<std::mutex> lock(handler->rx_mutex);
  size_t                       rxd_samples               = 0;
  void*                        data[SRSLTE_MAX_CHANNELS] = {};

  // Set all pointers to zero buffer
  for (auto& i : data) {
    i = zero_mem;
  }

  // Receive until time out
  uhd::rx_metadata_t md;
  do {
    if (handler->uhd->receive(data, handler->rx_nof_samples, md, 0.0, false, rxd_samples) != UHD_ERROR_NONE) {
      log_rx_error(handler);
      print_usrp_error(handler);
      return;
    }
  } while (rxd_samples > 0 and md.error_code == uhd::rx_metadata_t::ERROR_CODE_NONE);
}

bool rf_uhd_has_rssi(void* h)
{
  return false;
}

float rf_uhd_get_rssi(void* h)
{
  return NAN;
}

int rf_uhd_open(char* args, void** h)
{
  return rf_uhd_open_multi(args, h, 1);
}

int rf_uhd_open_multi(char* args, void** h, uint32_t nof_channels)
{
  // Check valid handler pointer
  if (h == nullptr) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (nof_channels > SRSLTE_MAX_CHANNELS) {
    ERROR("Error opening UHD: maximum number of channels exceeded (%d>%d)\n", nof_channels, SRSLTE_MAX_CHANNELS);
    return SRSLTE_ERROR;
  }

  rf_uhd_handler_t* handler = new rf_uhd_handler_t;
  *h                        = handler;

  // Disable fast-path (U/L/O) messages
  setenv("UHD_LOG_FASTPATH_DISABLE", "1", 0);

  // Set priority to UHD threads
  uhd_set_thread_priority(uhd_default_thread_priority, true);

  // Allow NULL args
  char* empty_args = (char*)"";
  if (args == nullptr) {
    args = empty_args;
  }
  handler->devname = "";

  // Parse args into dictionary
  uhd::device_addr_t device_addr((std::string(args)));

  // Initialize handler
  handler->uhd_error_handler = nullptr;

  // Check external clock argument
  std::string clock_src = "internal";
  if (device_addr.has_key("clock")) {
    clock_src = device_addr.pop("clock");
  }

  // Logging level
#ifdef UHD_LOG_INFO
  uhd::log::severity_level severity_level = uhd::log::severity_level::info;
  if (device_addr.has_key("log_level")) {
    std::string log_level = device_addr.pop("log_level");

    for (auto& e : log_level) {
      e = std::toupper(e);
    }

    if (log_level == "WARNING") {
      severity_level = uhd::log::severity_level::warning;
    } else if (log_level == "INFO") {
      severity_level = uhd::log::severity_level::info;
    } else if (log_level == "DEBUG") {
      severity_level = uhd::log::severity_level::debug;
    } else if (log_level == "TRACE") {
      severity_level = uhd::log::severity_level::trace;
    } else {
      severity_level = uhd::log::severity_level::error;
    }
  }
  uhd::log::set_console_level(severity_level);
#endif

#if HAVE_ASYNC_THREAD
  bool start_async_thread = true;
  if (device_addr.has_key("silent")) {
    device_addr.pop("silent");
    start_async_thread = false;
  }
#endif

  // If device type or name not given in args, select device from found list
  if (not device_addr.has_key("type")) {
    // Find available devices
    uhd_string_vector_handle devices_str = {};
    uhd_string_vector_make(&devices_str);
    uhd_usrp_find("", &devices_str);

    std::string type;

    if (find_string(devices_str, "type=b200")) {
      type = "b200";
    } else if (find_string(devices_str, "type=x300")) {
      type = "x300";
    } else if (find_string(devices_str, "type=e3x0")) {
      type = "e3x0";
    } else if (find_string(devices_str, "type=n3xx")) {
      type = "n3xx";
    }

    if (not type.empty()) {
      device_addr.set("type", type);
    }

    uhd_string_vector_free(&devices_str);
  }

  // Parse/Select master clock rate
  if (not device_addr.has_key("master_clock_rate") and device_addr.has_key("type")) {
    // Default master clock rate for B200 series
    std::string mcr = "23.04e6";

    if (device_addr["type"] == "x300") {
      mcr = "184.32e6";
    } else if (device_addr["type"] == "n3xx") {
      mcr = "122.88e6";
    } else if (device_addr["type"] == "e3x0") {
      mcr = "30.72e6";
    }

    device_addr.set("master_clock_rate", mcr);
  }
  handler->current_master_clock = device_addr.cast("master_clock_rate", 0.0);

  // Set dynamic master clock rate configuration
  if (device_addr.has_key("type")) {
    handler->dynamic_master_rate = RH_UHD_IMP_FIX_MASTER_CLOCK_RATE_DEVICE_LIST.count(device_addr["type"]) == 0;
  }

  // Parse initial sample rate
  if (device_addr.has_key("sampling_rate")) {
    handler->tx_rate = device_addr.cast("sampling_rate", handler->tx_rate);
    handler->rx_rate = handler->tx_rate;
    device_addr.pop("sampling_rate");
  }

  // Create UHD handler
  printf("Opening USRP channels=%d, args: %s\n", nof_channels, device_addr.to_string().c_str());

  // If RFNOC is accessible
#ifdef UHD_ENABLE_RFNOC
  if (rf_uhd_rfnoc::is_required(device_addr)) {
    handler->uhd = std::make_shared<rf_uhd_rfnoc>();
  }
#endif // UHD_ENABLE_RFNOC

  // If UHD was not instanced, instance generic
  if (handler->uhd == nullptr) {
    handler->uhd = std::make_shared<rf_uhd_generic>();
  }

  // Make USRP
  if (handler->uhd->usrp_make(device_addr, nof_channels) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    uhd_free(handler);
    return SRSLTE_ERROR;
  }

  // Set device internal name, it sets the device name to B200 by default
  if (device_addr.has_key("type")) {
    if (device_addr["type"] == "x300") {
      handler->devname = DEVNAME_X300;
    } else if (device_addr["type"] == "n3xx") {
      handler->devname = DEVNAME_N300;
    } else if (device_addr["type"] == "e3x0") {
      handler->devname = DEVNAME_E3X0;
    } else if (device_addr["type"] == "b200") {
      handler->devname = DEVNAME_B200;
    }
  }

  // If device name is not set, get it from motherboard
  if (handler->devname.empty()) {
    std::string mboard_name;
    if (handler->uhd->get_mboard_name(mboard_name) != UHD_ERROR_NONE) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }

    // Make upper case
    for (auto& x : mboard_name) {
      x = std::toupper(x);
    }

    if (mboard_name.find("B2") != std::string::npos) {
      handler->devname = DEVNAME_B200;
    } else if (mboard_name.find("X3") != std::string::npos) {
      handler->devname = DEVNAME_X300;
    } else if (mboard_name.find("N3") != std::string::npos) {
      handler->devname = DEVNAME_N300;
    } else {
      handler->devname = DEVNAME_UNKNOWN;
    }
  }

  bool        is_locked = false;
  std::string sensor_name;

  // Set sync source
  if (handler->uhd->set_sync_source(clock_src) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  if (clock_src == "gpsdo") {
    set_time_to_gps_time(handler);
    sensor_name = "gps_locked";
  } else {
    sensor_name = "ref_locked";
  }

  // Wait until external reference / GPS is locked
  if (clock_src != "internal") {
    // blocks until clock source is locked
    int error = wait_sensor_locked(handler, sensor_name, true, 300, is_locked);
    // Print Not lock error if the return was succesful, wait_sensor_locked prints the error before returning
    if (not is_locked and error == SRSLTE_SUCCESS) {
      ERROR(
          "Could not lock reference clock source. Sensor: %s=%s\n", sensor_name.c_str(), is_locked ? "true" : "false");
    }
  }

  handler->nof_rx_channels = nof_channels;
  handler->nof_tx_channels = nof_channels;

  if (handler->uhd->set_rx_rate(handler->rx_rate) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }
  if (handler->uhd->set_tx_rate(handler->tx_rate) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  if (nof_channels > 1 and clock_src != "gpsdo") {
    handler->uhd->set_time_unknown_pps(uhd::time_spec_t());
  }

  if (handler->uhd->get_rx_stream(handler->rx_nof_samples) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  if (handler->uhd->get_tx_stream(handler->tx_nof_samples) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  // Populate RF device info
  uhd::gain_range_t tx_gain_range;
  uhd::gain_range_t rx_gain_range;
  if (handler->uhd->get_gain_range(tx_gain_range, rx_gain_range) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }
  handler->info.min_tx_gain = tx_gain_range.start();
  handler->info.max_tx_gain = tx_gain_range.stop();
  handler->info.min_rx_gain = rx_gain_range.start();
  handler->info.max_rx_gain = rx_gain_range.stop();

  // Set starting gain to half maximum in case of using AGC
  rf_uhd_set_rx_gain(handler, handler->info.max_rx_gain * 0.7);

#if HAVE_ASYNC_THREAD
  if (start_async_thread) {
    // Start low priority thread to receive async commands
    handler->async_thread_running = true;
    handler->async_thread         = std::thread(async_thread, handler);
  }
#endif

  // Restore priorities
  if (uhd_set_thread_priority(0, false) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int rf_uhd_close(void* h)
{
  // Makes sure Tx is ended
  void* buff[SRSLTE_MAX_CHANNELS] = {};
  rf_uhd_send_timed_multi(h, buff, 0, 0, 0, false, true, false, true);

  // Makes sure Rx stream is stopped
  rf_uhd_stop_rx_stream(h);

  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;

  /// Free all UHD safe class
  uhd_free(handler);

  return SRSLTE_SUCCESS;
}

static inline void rf_uhd_set_master_clock_rate_unsafe(rf_uhd_handler_t* handler, double rate)
{
  // Set master clock rate if it is allowed and change is required
  if (handler->dynamic_master_rate && handler->current_master_clock != rate) {
    if (handler->uhd->set_master_clock_rate(rate) != UHD_ERROR_NONE) {
      print_usrp_error(handler);
    }
    handler->current_master_clock = rate;
  }
}

static inline int rf_uhd_imp_end_burst(rf_uhd_handler_t* handler)
{
  uhd::tx_metadata_t md;
  void*              buffs_ptr[SRSLTE_MAX_CHANNELS] = {};
  size_t             txd_samples                    = 0;

  // Set buffer pointers
  for (int i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
    buffs_ptr[i] = zero_mem;
  }

  // Set metadata
  md.has_time_spec  = false;
  md.start_of_burst = false;
  md.end_of_burst   = true;

  // Actual base-band transmission
  if (handler->uhd->send(buffs_ptr, 0, md, RF_UHD_IMP_TRX_TIMEOUT_S, txd_samples) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  // Update TX state
  handler->tx_state = RF_UHD_IMP_TX_STATE_START_BURST;

  return SRSLTE_SUCCESS;
}

double rf_uhd_set_rx_srate(void* h, double freq)
{
  rf_uhd_handler_t*            handler = (rf_uhd_handler_t*)h;
  std::unique_lock<std::mutex> lock(handler->rx_mutex);

  // Early return if the current rate matches and Rx stream has been created
  if (freq == handler->rx_rate and handler->uhd->is_rx_ready()) {
    return freq;
  }

  // Stop RX streamer
  if (RF_UHD_IMP_PROHIBITED_STOP_START.count(handler->devname) == 0) {
    if (rf_uhd_stop_rx_stream_unsafe(handler) != SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }
  }

  // Set master clock rate
  if (fmod(handler->current_master_clock, freq) > 0.0) {
    rf_uhd_set_master_clock_rate_unsafe(handler, 4 * freq);
  }

  if (handler->nof_rx_channels > 1) {
    uhd::time_spec_t timespec;
    if (handler->uhd->get_time_now(timespec) != UHD_ERROR_NONE) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }
    timespec += RF_UHD_IMP_TIMED_COMMAND_DELAY_S;
    handler->uhd->set_command_time(timespec);
  }

  // Set RX rate
  if (handler->uhd->set_rx_rate(freq) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  if (RF_UHD_IMP_PROHIBITED_STREAM_REMAKE.count(handler->devname) == 0) {
    if (handler->uhd->get_rx_stream(handler->rx_nof_samples) != UHD_ERROR_NONE) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }
  }

  // Update current rate
  handler->rx_rate = freq;

  return freq;
}

double rf_uhd_set_tx_srate(void* h, double freq)
{
  rf_uhd_handler_t*            handler = (rf_uhd_handler_t*)h;
  std::unique_lock<std::mutex> lock(handler->tx_mutex);
#if HAVE_ASYNC_THREAD
  std::unique_lock<std::mutex> lock_async(handler->async_mutex);
#endif /* HAVE_ASYNC_THREAD */

  // Early return if the current rate matches and Tx stream has been created
  if (freq == handler->tx_rate and handler->uhd->is_tx_ready()) {
    return freq;
  }

  // End burst
  if (handler->uhd->is_tx_ready() and handler->tx_state != RF_UHD_IMP_TX_STATE_START_BURST) {
    if (rf_uhd_imp_end_burst(handler) != SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }
  }

  // Set master clock rate
  if (fmod(handler->current_master_clock, freq) > 0.0) {
    rf_uhd_set_master_clock_rate_unsafe(handler, 4 * freq);
  }

  if (handler->nof_tx_channels > 1) {
    uhd::time_spec_t timespec;
    if (handler->uhd->get_time_now(timespec) != UHD_ERROR_NONE) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }
    timespec += RF_UHD_IMP_TIMED_COMMAND_DELAY_S;
    handler->uhd->set_command_time(timespec);
  }

  // Set TX rate
  if (handler->uhd->set_tx_rate(freq) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  if (RF_UHD_IMP_PROHIBITED_STREAM_REMAKE.count(handler->devname) == 0) {
    if (handler->uhd->get_tx_stream(handler->tx_nof_samples) != UHD_ERROR_NONE) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }
  }

  handler->tx_rate = freq;

#if HAVE_ASYNC_THREAD
  // Notifies the Asynchronous thread about a tx stream change
  handler->async_cvar.notify_all();
#endif /* HAVE_ASYNC_THREAD */

  return freq;
}

int rf_uhd_set_rx_gain(void* h, double gain)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  for (size_t i = 0; i < handler->nof_rx_channels; i++) {
    if (rf_uhd_set_rx_gain_ch(h, i, gain)) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }
  }
  return SRSLTE_SUCCESS;
}

int rf_uhd_set_rx_gain_ch(void* h, uint32_t ch, double gain)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  if (handler->uhd->set_rx_gain(ch, gain) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

int rf_uhd_set_tx_gain(void* h, double gain)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  for (size_t i = 0; i < handler->nof_tx_channels; i++) {
    if (rf_uhd_set_tx_gain_ch(h, i, gain)) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }
  }
  return SRSLTE_SUCCESS;
}

int rf_uhd_set_tx_gain_ch(void* h, uint32_t ch, double gain)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  if (handler->uhd->set_tx_gain(ch, gain) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

double rf_uhd_get_rx_gain(void* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  double            gain    = 0.0;

  if (handler->uhd->get_rx_gain(gain) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  return gain;
}

double rf_uhd_get_tx_gain(void* h)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  double            gain    = 0.0;

  if (handler->uhd->get_tx_gain(gain) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
    return SRSLTE_ERROR;
  }

  return gain;
}

srslte_rf_info_t* rf_uhd_get_info(void* h)
{
  srslte_rf_info_t* info = nullptr;

  if (h != nullptr) {
    rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
    info                      = &handler->info;
  }

  return info;
}

double rf_uhd_set_rx_freq(void* h, uint32_t ch, double freq)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  if (ch < handler->nof_rx_channels) {
    if (handler->uhd->set_rx_freq(ch, freq, freq) != UHD_ERROR_NONE) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }
  } else {
    for (uint32_t i = 0; i < handler->nof_rx_channels; i++) {
      if (handler->uhd->set_rx_freq(i, freq, freq) != UHD_ERROR_NONE) {
        print_usrp_error(handler);
        return SRSLTE_ERROR;
      }
    }
  }
  rf_uhd_rx_wait_lo_locked(handler);
  return freq;
}

double rf_uhd_set_tx_freq(void* h, uint32_t ch, double freq)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  if (ch < handler->nof_tx_channels) {
    if (handler->uhd->set_tx_freq(ch, freq, freq) != UHD_ERROR_NONE) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }
  } else {
    for (uint32_t i = 0; i < handler->nof_tx_channels; i++) {
      if (handler->uhd->set_tx_freq(i, freq, freq) != UHD_ERROR_NONE) {
        print_usrp_error(handler);
        return SRSLTE_ERROR;
      }
    }
  }
  return freq;
}

void rf_uhd_get_time(void* h, time_t* secs, double* frac_secs)
{
  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  uhd::time_spec_t  timespec;
  if (handler->uhd->get_time_now(timespec) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
  }
  if (secs != nullptr) {
    *secs = timespec.get_full_secs();
  }

  if (frac_secs != nullptr) {
    *frac_secs = timespec.get_frac_secs();
  }
}

void rf_uhd_sync_pps(void* h)
{
  if (h == nullptr) {
    return;
  }

  rf_uhd_handler_t* handler = (rf_uhd_handler_t*)h;
  uhd::time_spec_t  timespec(0.0);
  if (handler->uhd->set_time_unknown_pps(timespec) != UHD_ERROR_NONE) {
    print_usrp_error(handler);
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
  rf_uhd_handler_t*            handler = (rf_uhd_handler_t*)h;
  std::unique_lock<std::mutex> lock(handler->rx_mutex);
  size_t                       rxd_samples       = 0;
  size_t                       rxd_samples_total = 0;
  uint32_t                     trials            = 0;
  int                          ret               = SRSLTE_ERROR;
  uhd::time_spec_t             timespec;
  uhd::rx_metadata_t           md;

  // Check Rx stream has been created
  if (not handler->uhd->is_rx_ready()) {
    // Ignores reception, the stream will start as soon as the Rx sampling rate is set
    return SRSLTE_SUCCESS;
  }

  // Start stream if not started
  if (not handler->rx_stream_enabled) {
    if (rf_uhd_start_rx_stream_unsafe(handler) != SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }
  }

  // Receive stream in multiple blocks
  while (rxd_samples_total < nsamples && trials < RF_UHD_IMP_MAX_RX_TRIALS) {
    void* buffs_ptr[SRSLTE_MAX_CHANNELS] = {};
    for (uint32_t i = 0; i < handler->nof_rx_channels; i++) {
      cf_t* data_c = (cf_t*)data[i];
      buffs_ptr[i] = &data_c[rxd_samples_total];
    }

    size_t num_samps_left = nsamples - rxd_samples_total;
    size_t num_rx_samples = (num_samps_left > handler->rx_nof_samples) ? handler->rx_nof_samples : num_samps_left;

    if (handler->uhd->receive(buffs_ptr, num_rx_samples, md, 1.0, false, rxd_samples) != UHD_ERROR_NONE) {
      log_rx_error(handler);
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }

    // Save timespec for first block
    if (rxd_samples_total == 0) {
      timespec = md.time_spec;
    }
    uhd::rx_metadata_t::error_code_t& error_code = md.error_code;

    rxd_samples_total += rxd_samples;
    trials++;

    if (error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW) {
      log_overflow(handler);
    } else if (error_code == uhd::rx_metadata_t::ERROR_CODE_LATE_COMMAND) {
      log_late(handler, true);
    } else if (error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
      ERROR("Error timed out while receiving samples from UHD.\n");

      if (RF_UHD_IMP_PROHIBITED_STOP_START.count(handler->devname) == 0) {
        // Stop Rx stream
        rf_uhd_stop_rx_stream_unsafe(handler);
      }

      return -1;
    } else if (error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
      ERROR("Error %s was returned during streaming. Aborting.\n", md.to_pp_string(true).c_str());
      INFO("Error %s was returned during streaming. Aborting.\n", md.to_pp_string(true).c_str());
    }
  }

  if (trials >= RF_UHD_IMP_MAX_RX_TRIALS) {
    return SRSLTE_ERROR;
  }

  ret = rxd_samples_total;

  // Set timestamp if provided
  if (secs != nullptr and frac_secs != nullptr) {
    *secs      = timespec.get_full_secs();
    *frac_secs = timespec.get_frac_secs();
  }

  return ret;
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
  void* _data[SRSLTE_MAX_CHANNELS] = {};

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
  rf_uhd_handler_t*            handler = (rf_uhd_handler_t*)h;
  std::unique_lock<std::mutex> lock(handler->tx_mutex);
  uhd::tx_metadata_t           md;
  void*                        buffs_ptr[SRSLTE_MAX_CHANNELS] = {};
  size_t                       txd_samples                    = 0;
  int                          n                              = 0;

  // Check Tx stream has been created
  if (not handler->uhd->is_tx_ready()) {
    return SRSLTE_ERROR;
  }

  // Run Underflow recovery state machine
  switch (handler->tx_state) {
    case RF_UHD_IMP_TX_STATE_BURST:
      // Normal case, do nothing
      break;
    case RF_UHD_IMP_TX_STATE_END_OF_BURST:
      // Send end of burst and ignore transmission
      if (rf_uhd_imp_end_burst(handler) != SRSLTE_SUCCESS) {
        return SRSLTE_ERROR;
      }

      // Flush receiver
      rf_uhd_flush_buffer(h);

      return SRSLTE_ERROR;
    case RF_UHD_IMP_TX_STATE_START_BURST:
      // Set tart of burst to false if recovering from the Underflow
      is_start_of_burst = true;
      handler->tx_state = RF_UHD_IMP_TX_STATE_BURST;
      break;
  }

  if (not has_time_spec) {
    // If it the beginning of a burst, set timestamp
    if (is_start_of_burst) {
      // It gets the USRP time for transmissions without time
      if (handler->uhd->get_time_now(md.time_spec) != UHD_ERROR_NONE) {
        print_usrp_error(handler);
        return SRSLTE_ERROR;
      }

      // Add time to metadata
      md.time_spec += RF_UHD_IMP_STREAM_DELAY_S;
    }
  } else {
    // Otherwise, it gets given time in the arguments
    md.time_spec = uhd::time_spec_t(secs, frac_secs);
  }

  // Generate transmission buffer pointers
  cf_t* data_c[SRSLTE_MAX_CHANNELS] = {};
  for (uint32_t i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
    if (i < handler->nof_tx_channels) {
      data_c[i] = (data[i] != nullptr) ? (cf_t*)(data[i]) : zero_mem;
    } else {
      data_c[i] = zero_mem;
    }
  }

  // it transmits in chunks of `handler->tx_nof_samples` except last block
  do {
    size_t tx_samples = handler->tx_nof_samples;

    // Set start of burst. Time spec only for the first packet in the burst
    md.start_of_burst = is_start_of_burst;
    // X300 devices work better if Timespec is sent at the start of the burst only
    if (!handler->devname.compare(DEVNAME_X300)) {
      md.has_time_spec = is_start_of_burst and has_time_spec;
    } else {
      md.has_time_spec = is_start_of_burst or has_time_spec;
    }

    // middle packets are never end of burst, last one as defined
    if (nsamples - n > (int)tx_samples) {
      md.end_of_burst = false;
    } else {
      tx_samples      = nsamples - n;
      md.end_of_burst = is_end_of_burst;
    }

    for (int i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
      void* buff   = (void*)&data_c[i][n];
      buffs_ptr[i] = buff;
    }

    if (handler->uhd->send(buffs_ptr, tx_samples, md, RF_UHD_IMP_TRX_TIMEOUT_S, txd_samples) != UHD_ERROR_NONE) {
      print_usrp_error(handler);
      return SRSLTE_ERROR;
    }

    // Next packets are not start of burst
    is_start_of_burst = false;
    md.time_spec += txd_samples / handler->tx_rate;

    n += txd_samples;
  } while (n < nsamples);

  // If end of burst, make sure it will be start next call
  if (md.end_of_burst and handler->tx_state == RF_UHD_IMP_TX_STATE_BURST) {
    handler->tx_state = RF_UHD_IMP_TX_STATE_START_BURST;
  }

  return nsamples;
}
