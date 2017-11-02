/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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


#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "srslte/srslte.h"
#include "rf_soapy_imp.h"
#include "srslte/phy/rf/rf.h"

#include <SoapySDR/Device.h>
#include <SoapySDR/Formats.h>

typedef struct {
    SoapySDRKwargs args;
    SoapySDRDevice *device;
    SoapySDRRange *ranges;
    SoapySDRStream *rxStream;
    SoapySDRStream *txStream;
    bool tx_stream_active;
    bool rx_stream_active;
} rf_soapy_handler_t;


cf_t zero_mem[64*1024];



int soapy_error(void *h)
{
  return 0;
}


void rf_soapy_get_freq_range(void *h)
{

}


void rf_soapy_suppress_handler(const char *x)
{
    // not supported
}


void rf_soapy_msg_handler(const char *msg)
{
    // not supported
}


void rf_soapy_suppress_stdout(void *h)
{
    // not supported
}


void rf_soapy_register_error_handler(void *notused, srslte_rf_error_handler_t new_handler)
{
    // not supported
}


char* rf_soapy_devname(void* h)
{
  return "soapy";
}


bool rf_soapy_rx_wait_lo_locked(void *h)
{
  printf("TODO: implement rf_soapy_rx_wait_lo_locked()\n");
  return true;
}


void rf_soapy_set_tx_cal(void *h, srslte_rf_cal_t *cal)
{
  printf("TODO: implement rf_soapy_rx_wait_lo_locked()\n");
  // not supported
}


void rf_soapy_set_rx_cal(void *h, srslte_rf_cal_t *cal)
{
  printf("TODO: implement rf_soapy_set_rx_cal()\n");
}


int rf_soapy_start_rx_stream(void *h)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  if(handler->rx_stream_active == false){
    if(SoapySDRDevice_activateStream(handler->device, handler->rxStream, 0, 0, 0) != 0)
      return SRSLTE_ERROR;
    handler->rx_stream_active = true;
   }
  return SRSLTE_SUCCESS;
}


int rf_soapy_start_tx_stream(void *h)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  if(handler->tx_stream_active == false){
    if(SoapySDRDevice_activateStream(handler->device, handler->txStream, 0, 0, 0) != 0)
      return SRSLTE_ERROR;
    handler->tx_stream_active = true;  
   }
  return SRSLTE_SUCCESS;
}


int rf_soapy_stop_rx_stream(void *h)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  if (SoapySDRDevice_deactivateStream(handler->device, handler->rxStream, 0, 0) != 0)
    return SRSLTE_ERROR;
  
  handler->rx_stream_active = false;
  return SRSLTE_SUCCESS;
}


int rf_soapy_stop_tx_stream(void *h)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  if(SoapySDRDevice_deactivateStream(handler->device, handler->txStream, 0, 0) != 0)
    return SRSLTE_ERROR;

  handler->tx_stream_active = false;
  return SRSLTE_SUCCESS;
}


void rf_soapy_flush_buffer(void *h)
{
  int n;
  cf_t tmp1[1024];
  cf_t tmp2[1024];
  void *data[2] = {tmp1, tmp2};
  do {
    n = rf_soapy_recv_with_time_multi(h, data, 1024, 0, NULL, NULL);
  } while (n > 0);
}


bool rf_soapy_has_rssi(void *h)
{
  // TODO: implement rf_soapy_has_rssi()
  return false;
}


float rf_soapy_get_rssi(void *h)
{
  printf("TODO: implement rf_soapy_get_rssi()\n");
  return 0.0;
}


//TODO: add multi-channel support
int rf_soapy_open_multi(char *args, void **h, uint32_t nof_rx_antennas)
{
  size_t length;
  const SoapySDRKwargs *soapy_args = SoapySDRDevice_enumerate(NULL, &length);
  
  if (length == 0) {
    printf("No Soapy devices found.\n");
    return SRSLTE_ERROR;
  }
  
  for (size_t i = 0; i < length; i++) {
    printf("Soapy has Found device #%d: ", (int)i);
    for (size_t j = 0; j < soapy_args[i].size; j++) {
      printf("%s=%s, ", soapy_args[i].keys[j], soapy_args[i].vals[j]);
    }
    printf("\n");
  }
  
  SoapySDRDevice *sdr = SoapySDRDevice_make(&(soapy_args[0]));
  if (sdr == NULL) {
    printf("failed to create SOAPY object\n");
    return SRSLTE_ERROR;
  }
  
  // create handler
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) malloc(sizeof(rf_soapy_handler_t));
  bzero(handler, sizeof(rf_soapy_handler_t));
  *h = handler;
  handler->device = sdr;
  handler->tx_stream_active = false;
  handler->rx_stream_active = false;

  if(SoapySDRDevice_getNumChannels(handler->device,SOAPY_SDR_RX) > 0){     
    printf("setting up RX stream\n");
    if(SoapySDRDevice_setupStream(handler->device, &(handler->rxStream), SOAPY_SDR_RX, SOAPY_SDR_CF32, NULL, 0, NULL) != 0) {
      printf("Rx setupStream fail: %s\n", SoapySDRDevice_lastError());
      return SRSLTE_ERROR;
    } 
  }

  if(SoapySDRDevice_getNumChannels(handler->device,SOAPY_SDR_TX) > 0){
    printf("setting up TX stream\n");
    if (SoapySDRDevice_setupStream(handler->device, &(handler->txStream), SOAPY_SDR_TX, SOAPY_SDR_CF32, NULL, 0, NULL) != 0) {
      printf("Tx setupStream fail: %s\n", SoapySDRDevice_lastError());
      return SRSLTE_ERROR;
    }     
  }

  return SRSLTE_SUCCESS;
}


int rf_soapy_open(char *args, void **h)
{
  return rf_soapy_open_multi(args, h, 1);
}


int rf_soapy_close(void *h)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  if (handler->tx_stream_active) {
    rf_soapy_stop_tx_stream(handler);
    SoapySDRDevice_closeStream(handler->device, handler->txStream);
  }
  
  if (handler->rx_stream_active) {
    rf_soapy_stop_rx_stream(handler);
    SoapySDRDevice_closeStream(handler->device, handler->rxStream);
  }
  
  SoapySDRDevice_unmake(handler->device);
  free(handler);
  
  return SRSLTE_SUCCESS;
}

void rf_soapy_set_master_clock_rate(void *h, double rate)
{
  // Allow the soapy to automatically set the appropriate clock rate
  // TODO: implement this function
}


bool rf_soapy_is_master_clock_dynamic(void *h)
{
  printf("TODO: implement rf_soapy_is_master_clock_dynamic()\n");
  return false;
}


double rf_soapy_set_rx_srate(void *h, double rate)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  if (SoapySDRDevice_setSampleRate(handler->device, SOAPY_SDR_RX, 0, rate) != 0) {
    printf("setSampleRate Rx fail: %s\n", SoapySDRDevice_lastError());
    return SRSLTE_ERROR;
  }

  return SoapySDRDevice_getSampleRate(handler->device, SOAPY_SDR_RX,0);
}

double rf_soapy_set_tx_srate(void *h, double rate)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  if (SoapySDRDevice_setSampleRate(handler->device, SOAPY_SDR_TX, 0, rate) != 0) {
    printf("setSampleRate Tx fail: %s\n", SoapySDRDevice_lastError());
    return SRSLTE_ERROR;
  }

  return SoapySDRDevice_getSampleRate(handler->device, SOAPY_SDR_TX,0);
}


double rf_soapy_set_rx_gain(void *h, double gain)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  if (SoapySDRDevice_setGain(handler->device, SOAPY_SDR_RX, 0, gain) != 0)
  {
    printf("setGain fail: %s\n", SoapySDRDevice_lastError());
    return SRSLTE_ERROR;
  }
  return rf_soapy_get_rx_gain(h);
}


double rf_soapy_set_tx_gain(void *h, double gain)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  if (SoapySDRDevice_setGain(handler->device, SOAPY_SDR_TX, 0, gain) != 0)
  {
    printf("setGain fail: %s\n", SoapySDRDevice_lastError());
    return SRSLTE_ERROR;
  }
  return rf_soapy_get_tx_gain(h);
}


double rf_soapy_get_rx_gain(void *h)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  return SoapySDRDevice_getGain(handler->device,SOAPY_SDR_RX,0);
}


double rf_soapy_get_tx_gain(void *h)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  return SoapySDRDevice_getGain(handler->device,SOAPY_SDR_TX,0);
}


double rf_soapy_set_rx_freq(void *h, double freq)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  if (SoapySDRDevice_setFrequency(handler->device, SOAPY_SDR_RX, 0, freq, NULL) != 0)
  {
    printf("setFrequency fail: %s\n", SoapySDRDevice_lastError());
    return SRSLTE_ERROR;
  }

  // Todo: expose antenna setting
  if (SoapySDRDevice_setAntenna(handler->device, SOAPY_SDR_RX, 0, "LNAH") != 0) {
    fprintf(stderr, "Failed to set Rx antenna.\n");
  }

  char *ant = SoapySDRDevice_getAntenna(handler->device, SOAPY_SDR_RX, 0);
  printf("Rx antenna set to %s\n", ant);

  return SoapySDRDevice_getFrequency(handler->device, SOAPY_SDR_RX, 0);
}

double rf_soapy_set_tx_freq(void *h, double freq)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  if (SoapySDRDevice_setFrequency(handler->device, SOAPY_SDR_TX, 0, freq, NULL) != 0)
  {
    printf("setFrequency fail: %s\n", SoapySDRDevice_lastError());
    return SRSLTE_ERROR;
  }

  // Todo: expose antenna name in arguments
  if (SoapySDRDevice_setAntenna(handler->device, SOAPY_SDR_TX, 0, "BAND1") != 0) {
    fprintf(stderr, "Failed to set Tx antenna.\n");
  }

  char *ant = SoapySDRDevice_getAntenna(handler->device, SOAPY_SDR_TX, 0);
  printf("Tx antenna set to %s\n", ant);

  return SoapySDRDevice_getFrequency(handler->device, SOAPY_SDR_TX, 0);
}


void rf_soapy_get_time(void *h, time_t *secs, double *frac_secs)
{
  printf("Todo: implement rf_soapy_get_time()\n");
}


//TODO: add multi-channel support
int  rf_soapy_recv_with_time_multi(void *h,
                                   void **data,
                                   uint32_t nsamples,
                                   bool blocking,
                                   time_t *secs,
                                   double *frac_secs)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t*) h;
  int flags; //flags set by receive operation
  int num_channels = 1; // temp
  
  int trials = 0;
  int ret = 0;
  long long timeNs; //timestamp for receive buffer
  int n = 0;
  do {
    size_t rx_samples = nsamples;
    
    if (rx_samples > nsamples - n){
      rx_samples = nsamples - n;
    }
    void *buffs_ptr[4];
    for (int i=0; i<num_channels; i++){
      cf_t *data_c = (cf_t*) data[i];
      buffs_ptr[i] = &data_c[n];
    }
    ret = SoapySDRDevice_readStream(handler->device, handler->rxStream, buffs_ptr, rx_samples, &flags, &timeNs, 10000);
    if(ret < 0) {
      // continue when getting overflows
      if (ret == SOAPY_SDR_OVERFLOW) {
        fprintf(stderr, "O");
        fflush(stderr);
        continue;
      } else {
        return SRSLTE_ERROR;
      }
    }

    // update rx time
    if (secs != NULL && frac_secs != NULL) {
      *secs = timeNs / 1e9;
      *frac_secs = (timeNs % 1000000000)/1e9;
      //printf("rx_time: secs=%d, frac_secs=%lf timeNs=%lld\n", *secs, *frac_secs, timeNs);
    }

    n += ret;
    trials++;
  } while (n < nsamples && trials < 100);
  
  

  return n;
}


int rf_soapy_recv_with_time(void *h,
                            void *data,
                            uint32_t nsamples,
                            bool blocking,
                            time_t *secs,
                            double *frac_secs)
{
  return rf_soapy_recv_with_time_multi(h, &data, nsamples, blocking, secs, frac_secs);
}


int rf_soapy_send_timed(void *h,
                      void *data,
                      int nsamples,
                      time_t secs,
                      double frac_secs,
                      bool has_time_spec,
                      bool blocking,
                      bool is_start_of_burst,
                      bool is_end_of_burst)
{
  void *_data[SRSLTE_MAX_PORTS]= {data, zero_mem, zero_mem, zero_mem};
  return rf_soapy_send_timed_multi(h, _data, nsamples, secs, frac_secs, has_time_spec, blocking, is_start_of_burst, is_end_of_burst);
}


// Todo: Check correct handling of flags, use RF metrics API, fix timed transmissions
int rf_soapy_send_timed_multi(void *h,
                            void *data[SRSLTE_MAX_PORTS],
                            int nsamples,
                            time_t secs,
                            double frac_secs,
                            bool has_time_spec,
                            bool blocking,
                            bool is_start_of_burst,
                            bool is_end_of_burst)
{
  rf_soapy_handler_t *handler = (rf_soapy_handler_t *) h;
  int flags = 0;
  const long timeoutUs = 2000; // arbitrarily chosen
  long long timeNs = 0;
  int trials = 0;
  int ret = 0;
  int n = 0;


  if (!handler->tx_stream_active) {
    rf_soapy_start_tx_stream(h);
  }

  if (is_start_of_burst && is_end_of_burst) {
    flags |= SOAPY_SDR_ONE_PACKET;
  }

  if (is_end_of_burst) {
    flags |= SOAPY_SDR_END_BURST;
  }

  if (has_time_spec) {
    flags |= SOAPY_SDR_HAS_TIME;
    timeNs = secs * 1000000000;
    timeNs = timeNs + (frac_secs * 1000000000);
    //printf("time_spec: secs=%d, frac_secs=%lf timeNs=%lld\n", secs, frac_secs, timeNs);
  }

  do {
    size_t tx_samples = nsamples;
    if (tx_samples > nsamples - n) {
      tx_samples = nsamples - n;
    }

    ret = SoapySDRDevice_writeStream(handler->device, handler->txStream, (const void *)data, tx_samples, &flags, timeNs, timeoutUs);
    if (ret == SOAPY_SDR_TIMEOUT) {
      printf("L");
      continue;
    }
    if (ret == SOAPY_SDR_OVERFLOW) {
      printf("O");
      continue;
    }
    if (ret == SOAPY_SDR_UNDERFLOW) {
      printf("U");
      continue;
    }
    if (ret < 0) {
      fprintf(stderr, "Error during writeStream\n");
      exit(-1);
      return SRSLTE_ERROR;
    }

    n += ret;
    trials++;
  } while (n < nsamples && trials < 100);

  if (n != nsamples) {
    fprintf(stderr, "Couldn't write all samples.\n");
    return SRSLTE_ERROR;
  }

  return ret;
}
