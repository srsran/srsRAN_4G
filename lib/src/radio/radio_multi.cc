#include "srslte/radio/radio_multi.h"

namespace srslte {
  
bool radio_multi::init_multi(uint32_t nof_rx_antennas, char* args, char* devname)
{
 if (srslte_rf_open_devname(&rf_device, devname, args, nof_rx_antennas)) {
    fprintf(stderr, "Error opening RF device\n");
    return false;
  }
  
  tx_adv_negative = false; 
  agc_enabled = false; 
  burst_preamble_samples = 0; 
  burst_preamble_time_rounded = 0; 
  cur_tx_srate = 0; 
  is_start_of_burst = true;

  // Suppress radio stdout
  srslte_rf_suppress_stdout(&rf_device);

  tx_adv_auto = true; 
  // Set default preamble length each known device
  // We distinguish by device family, maybe we should calibrate per device
  if (strstr(srslte_rf_name(&rf_device), "uhd")) {
    burst_preamble_sec = uhd_default_burst_preamble_sec;
  } else if (strstr(srslte_rf_name(&rf_device), "bladerf")) {
    burst_preamble_sec = blade_default_burst_preamble_sec;
  } else {
    printf("\nWarning burst preamble is not calibrated for device %s. Set a value manually\n\n", srslte_rf_name(&rf_device));
  }

  if (args) {
    strncpy(saved_args, args, 127);
  }
  if (devname) {
    strncpy(saved_devname, devname, 127);
  }

  is_initialized = true;
  return true;    
}

bool radio_multi::rx_now(cf_t *buffer[SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t* rxd_time)
{
  void *ptr[SRSLTE_MAX_PORTS];
  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    ptr[i] = buffer[i];
  }
  if (!radio_is_streaming) {
    srslte_rf_start_rx_stream(&rf_device, false);
    radio_is_streaming = true;
  }
  if (srslte_rf_recv_with_time_multi(&rf_device, ptr, nof_samples, true,
    rxd_time?&rxd_time->full_secs:NULL, rxd_time?&rxd_time->frac_secs:NULL) > 0) {
    return true; 
  } else {
    return false; 
  }
}

  
}