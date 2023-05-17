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

#include "rf_dev.h"
#include "srsran/phy/rf/rf.h"
#include "srsran/phy/utils/debug.h"
#include <dlfcn.h>
#include <string.h>

int rf_get_available_devices(char** devnames, int max_strlen)
{
  int i = 0;
  while (rf_plugins[i] != NULL) {
    if (rf_plugins[i]->rf_api != NULL) {
      strncpy(devnames[i], rf_plugins[i]->rf_api->name, max_strlen);
    }
    i++;
  }
  return i;
}

int srsran_rf_set_rx_gain_th(srsran_rf_t* rf, double gain)
{
  pthread_mutex_lock(&rf->mutex);
  if (gain > rf->cur_rx_gain + 2 || gain < rf->cur_rx_gain - 2) {
    rf->new_rx_gain = gain;
    pthread_cond_signal(&rf->cond);
  }
  pthread_mutex_unlock(&rf->mutex);
  return SRSRAN_SUCCESS;
}

void srsran_rf_set_tx_rx_gain_offset(srsran_rf_t* rf, double offset)
{
  rf->tx_rx_gain_offset = offset;
}

/* This thread listens for set_rx_gain commands to the USRP */
static void* thread_gain_fcn(void* h)
{
  srsran_rf_t* rf = (srsran_rf_t*)h;

  while (rf->thread_gain_run) {
    pthread_mutex_lock(&rf->mutex);
    while (rf->cur_rx_gain == rf->new_rx_gain && rf->thread_gain_run) {
      pthread_cond_wait(&rf->cond, &rf->mutex);
    }
    if (rf->new_rx_gain != rf->cur_rx_gain) {
      srsran_rf_set_rx_gain(h, rf->new_rx_gain);
      rf->cur_rx_gain = srsran_rf_get_rx_gain(h);
      rf->new_rx_gain = rf->cur_rx_gain;
    }
    if (rf->tx_gain_same_rx) {
      srsran_rf_set_tx_gain(h, rf->cur_rx_gain + rf->tx_rx_gain_offset);
    }
    pthread_mutex_unlock(&rf->mutex);
  }
  return NULL;
}

/* Create auxiliary thread and mutexes for AGC */
int srsran_rf_start_gain_thread(srsran_rf_t* rf, bool tx_gain_same_rx)
{
  rf->tx_gain_same_rx   = tx_gain_same_rx;
  rf->tx_rx_gain_offset = 0.0;
  if (pthread_mutex_init(&rf->mutex, NULL)) {
    return -1;
  }
  if (pthread_cond_init(&rf->cond, NULL)) {
    return -1;
  }
  rf->thread_gain_run = true;
  if (pthread_create(&rf->thread_gain, NULL, thread_gain_fcn, rf)) {
    perror("pthread_create");
    rf->thread_gain_run = false;
    return -1;
  }
  return 0;
}

const char* srsran_rf_get_devname(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->name;
}

int srsran_rf_open_devname(srsran_rf_t* rf, const char* devname, char* args, uint32_t nof_channels)
{
  rf->thread_gain_run = false;

  bool no_rf_devs_detected = true;
  printf("Supported RF device list:");
  for (unsigned int i = 0; rf_plugins[i] && rf_plugins[i]->rf_api; i++) {
    no_rf_devs_detected = false;
    printf(" %s", rf_plugins[i]->rf_api->name);
  }
  printf("%s\n", no_rf_devs_detected ? " <none>" : "");

  // Try to open the device if name is provided
  if (devname && devname[0] != '\0') {
    int i = 0;
    while (rf_plugins[i] != NULL) {
      if (rf_plugins[i]->rf_api) {
        if (!strcasecmp(rf_plugins[i]->rf_api->name, devname)) {
          rf->dev = rf_plugins[i]->rf_api;
          return rf_plugins[i]->rf_api->srsran_rf_open_multi(args, &rf->handler, nof_channels);
        }
      }
      i++;
    }

    ERROR("RF device '%s' not found. Please check the available srsRAN CMAKE options to verify if this device is being "
          "detected in your system",
          devname);
    // provided device not found, abort
    return SRSRAN_ERROR;
  }

  // auto-mode, try to open in order of apperance in rf_plugins[] array
  int i = 0;
  while (rf_plugins[i] != NULL && rf_plugins[i]->rf_api != NULL) {
    printf("Trying to open RF device '%s'\n", rf_plugins[i]->rf_api->name);
    if (!rf_plugins[i]->rf_api->srsran_rf_open_multi(args, &rf->handler, nof_channels)) {
      rf->dev = rf_plugins[i]->rf_api;
      printf("RF device '%s' successfully opened\n", rf_plugins[i]->rf_api->name);
      return SRSRAN_SUCCESS;
    }
    printf("Unable to open RF device '%s'\n", rf_plugins[i]->rf_api->name);
    i++;
  }

  ERROR(
      "Failed to open a RF frontend device. Please check the available srsRAN CMAKE options to verify what RF frontend "
      "devices have been detected in your system");
  return SRSRAN_ERROR;
}

int srsran_rf_open_file(srsran_rf_t* rf, FILE** rx_files, FILE** tx_files, uint32_t nof_channels, uint32_t base_srate)
{
  rf->dev = &srsran_rf_dev_file;

  // file abstraction has custom "open" function with file-related args
  return rf_file_open_file(&rf->handler, rx_files, tx_files, nof_channels, base_srate);
}

const char* srsran_rf_name(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_devname(rf->handler);
}

int srsran_rf_start_rx_stream(srsran_rf_t* rf, bool now)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_start_rx_stream(rf->handler, now);
}

int srsran_rf_stop_rx_stream(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_stop_rx_stream(rf->handler);
}

void srsran_rf_flush_buffer(srsran_rf_t* rf)
{
  ((rf_dev_t*)rf->dev)->srsran_rf_flush_buffer(rf->handler);
}

bool srsran_rf_has_rssi(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_has_rssi(rf->handler);
}

float srsran_rf_get_rssi(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_get_rssi(rf->handler);
}

void srsran_rf_suppress_stdout(srsran_rf_t* rf)
{
  ((rf_dev_t*)rf->dev)->srsran_rf_suppress_stdout(rf->handler);
}

void srsran_rf_register_error_handler(srsran_rf_t* rf, srsran_rf_error_handler_t error_handler, void* arg)
{
  ((rf_dev_t*)rf->dev)->srsran_rf_register_error_handler(rf->handler, error_handler, arg);
}

int srsran_rf_open(srsran_rf_t* h, char* args)
{
  return srsran_rf_open_devname(h, NULL, args, 1);
}

int srsran_rf_open_multi(srsran_rf_t* h, char* args, uint32_t nof_channels)
{
  return srsran_rf_open_devname(h, NULL, args, nof_channels);
}

int srsran_rf_close(srsran_rf_t* rf)
{
  // Stop gain thread
  pthread_mutex_lock(&rf->mutex);
  if (rf->thread_gain_run) {
    rf->thread_gain_run = false;
  }
  pthread_mutex_unlock(&rf->mutex);
  pthread_cond_signal(&rf->cond);
  if (rf->thread_gain) {
    pthread_join(rf->thread_gain, NULL);
  }

  return ((rf_dev_t*)rf->dev)->srsran_rf_close(rf->handler);
}

double srsran_rf_set_rx_srate(srsran_rf_t* rf, double freq)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_rx_srate(rf->handler, freq);
}

int srsran_rf_set_rx_gain(srsran_rf_t* rf, double gain)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_rx_gain(rf->handler, gain);
}

int srsran_rf_set_rx_gain_ch(srsran_rf_t* rf, uint32_t ch, double gain)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_rx_gain_ch(rf->handler, ch, gain);
}

double srsran_rf_get_rx_gain(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_get_rx_gain(rf->handler);
}

double srsran_rf_get_tx_gain(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_get_tx_gain(rf->handler);
}

srsran_rf_info_t* srsran_rf_get_info(srsran_rf_t* rf)
{
  srsran_rf_info_t* ret = NULL;
  if (((rf_dev_t*)rf->dev)->srsran_rf_get_info) {
    ret = ((rf_dev_t*)rf->dev)->srsran_rf_get_info(rf->handler);
  }
  return ret;
}

double srsran_rf_set_rx_freq(srsran_rf_t* rf, uint32_t ch, double freq)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_rx_freq(rf->handler, ch, freq);
}

int srsran_rf_recv(srsran_rf_t* rf, void* data, uint32_t nsamples, bool blocking)
{
  return srsran_rf_recv_with_time(rf, data, nsamples, blocking, NULL, NULL);
}

int srsran_rf_recv_multi(srsran_rf_t* rf, void** data, uint32_t nsamples, bool blocking)
{
  return srsran_rf_recv_with_time_multi(rf, data, nsamples, blocking, NULL, NULL);
}

int srsran_rf_recv_with_time(srsran_rf_t* rf,
                             void*        data,
                             uint32_t     nsamples,
                             bool         blocking,
                             time_t*      secs,
                             double*      frac_secs)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_recv_with_time(rf->handler, data, nsamples, blocking, secs, frac_secs);
}

int srsran_rf_recv_with_time_multi(srsran_rf_t* rf,
                                   void**       data,
                                   uint32_t     nsamples,
                                   bool         blocking,
                                   time_t*      secs,
                                   double*      frac_secs)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_recv_with_time_multi(rf->handler, data, nsamples, blocking, secs, frac_secs);
}

int srsran_rf_set_tx_gain(srsran_rf_t* rf, double gain)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_tx_gain(rf->handler, gain);
}

int srsran_rf_set_tx_gain_ch(srsran_rf_t* rf, uint32_t ch, double gain)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_tx_gain_ch(rf->handler, ch, gain);
}

double srsran_rf_set_tx_srate(srsran_rf_t* rf, double freq)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_tx_srate(rf->handler, freq);
}

double srsran_rf_set_tx_freq(srsran_rf_t* rf, uint32_t ch, double freq)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_tx_freq(rf->handler, ch, freq);
}

void srsran_rf_get_time(srsran_rf_t* rf, time_t* secs, double* frac_secs)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_get_time(rf->handler, secs, frac_secs);
}

int srsran_rf_sync(srsran_rf_t* rf)
{
  int ret = SRSRAN_ERROR;

  if (((rf_dev_t*)rf->dev)->srsran_rf_sync_pps) {
    ((rf_dev_t*)rf->dev)->srsran_rf_sync_pps(rf->handler);

    ret = SRSRAN_SUCCESS;
  }

  return ret;
}

int srsran_rf_send_timed3(srsran_rf_t* rf,
                          void*        data,
                          int          nsamples,
                          time_t       secs,
                          double       frac_secs,
                          bool         has_time_spec,
                          bool         blocking,
                          bool         is_start_of_burst,
                          bool         is_end_of_burst)
{
  return ((rf_dev_t*)rf->dev)
      ->srsran_rf_send_timed(
          rf->handler, data, nsamples, secs, frac_secs, has_time_spec, blocking, is_start_of_burst, is_end_of_burst);
}

int srsran_rf_send_timed_multi(srsran_rf_t* rf,
                               void**       data,
                               int          nsamples,
                               time_t       secs,
                               double       frac_secs,
                               bool         blocking,
                               bool         is_start_of_burst,
                               bool         is_end_of_burst)
{
  return ((rf_dev_t*)rf->dev)
      ->srsran_rf_send_timed_multi(
          rf->handler, data, nsamples, secs, frac_secs, true, blocking, is_start_of_burst, is_end_of_burst);
}

int srsran_rf_send_multi(srsran_rf_t* rf,
                         void**       data,
                         int          nsamples,
                         bool         blocking,
                         bool         is_start_of_burst,
                         bool         is_end_of_burst)
{
  return ((rf_dev_t*)rf->dev)
      ->srsran_rf_send_timed_multi(
          rf->handler, data, nsamples, 0, 0, false, blocking, is_start_of_burst, is_end_of_burst);
}

int srsran_rf_send(srsran_rf_t* rf, void* data, uint32_t nsamples, bool blocking)
{
  return srsran_rf_send2(rf, data, nsamples, blocking, true, true);
}

int srsran_rf_send2(srsran_rf_t* rf,
                    void*        data,
                    uint32_t     nsamples,
                    bool         blocking,
                    bool         start_of_burst,
                    bool         end_of_burst)
{
  return srsran_rf_send_timed3(rf, data, nsamples, 0, 0, false, blocking, start_of_burst, end_of_burst);
}

int srsran_rf_send_timed(srsran_rf_t* rf, void* data, int nsamples, time_t secs, double frac_secs)
{
  return srsran_rf_send_timed2(rf, data, nsamples, secs, frac_secs, true, true);
}

int srsran_rf_send_timed2(srsran_rf_t* rf,
                          void*        data,
                          int          nsamples,
                          time_t       secs,
                          double       frac_secs,
                          bool         is_start_of_burst,
                          bool         is_end_of_burst)
{
  return srsran_rf_send_timed3(rf, data, nsamples, secs, frac_secs, true, true, is_start_of_burst, is_end_of_burst);
}

#ifdef ENABLE_RF_PLUGINS
static void unload_plugin(srsran_rf_plugin_t* rf_plugin)
{
  if (rf_plugin == NULL) {
    return;
  }
  if (rf_plugin->dl_handle != NULL) {
    rf_plugin->rf_api = NULL;
    dlclose(rf_plugin->dl_handle);
    rf_plugin->dl_handle = NULL;
  }
}

static int load_plugin(srsran_rf_plugin_t* rf_plugin)
{
  if (rf_plugin->rf_api != NULL) {
    // already loaded
    return SRSRAN_SUCCESS;
  }

  rf_plugin->dl_handle = dlopen(rf_plugin->plugin_name, RTLD_NOW);
  if (rf_plugin->dl_handle == NULL) {
    // Not an error, if loading failed due to missing dependencies.
    // Flag this plugin as not available and return SUCCESS.
    // Note: as this function is called before log-level is configured, use plain printf for any messages < ERROR
    printf("Skipping RF plugin %s: %s\n", rf_plugin->plugin_name, dlerror());
    rf_plugin->rf_api = NULL;
    return SRSRAN_SUCCESS;
  }

  // clear errors
  dlerror();
  char* err = NULL;

  // load symbols
  int (*register_plugin)(rf_dev_t * *rf_api) = dlsym(rf_plugin->dl_handle, "register_plugin");
  if ((err = dlerror()) != NULL) {
    ERROR("Error loading symbol '%s': %s", "register_plugin", err);
    goto clean_exit;
  }

  // register plugin
  int ret = register_plugin(&rf_plugin->rf_api);
  if (ret != SRSRAN_SUCCESS) {
    ERROR("Failed to register RF API for plugin %s", rf_plugin->plugin_name);
    goto clean_exit;
  }
  return SRSRAN_SUCCESS;
clean_exit:
  unload_plugin(rf_plugin);
  return SRSRAN_ERROR;
}
#endif /* ENABLE_RF_PLUGINS */

int srsran_rf_load_plugins()
{
#ifdef ENABLE_RF_PLUGINS
  for (unsigned int i = 0; rf_plugins[i]; i++) {
    if (load_plugin(rf_plugins[i]) != SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  }

  printf("Active RF plugins:");
  for (unsigned int i = 0; rf_plugins[i]; i++) {
    if (rf_plugins[i]->dl_handle != NULL) {
      printf(" %s", rf_plugins[i]->plugin_name);
    }
  }
  printf("\n");

  printf("Inactive RF plugins:");
  for (unsigned int i = 0; rf_plugins[i]; i++) {
    if (rf_plugins[i]->dl_handle == NULL) {
      printf(" %s", rf_plugins[i]->plugin_name);
    }
  }
  printf("\n");

#endif /* ENABLE_RF_PLUGINS */
  return SRSRAN_SUCCESS;
}

// Search and load plugins when this library is loaded (shared) or right before main (static)
void __attribute__((constructor)) init()
{
  if (srsran_rf_load_plugins() != SRSRAN_SUCCESS) {
    ERROR("Failed to load RF plugins");
  }
}
