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
#include "rf_limesdr_imp.h"
#include "srslte/rf/rf.h"
#include "lime/LimeSuite.h"

typedef struct {
  char           *devname;
  lms_dev_info_t *dev_info;
  lms_device_t   *device;
  lms_info_str_t  list[8];
  lms_stream_t    rx_stream;
  lms_stream_t    tx_stream;
  int             sampling_rate;
  bool            rx_is_streaming;
  bool            tx_is_streaming;
  int channel;
  
  int buffer_size;
  int num_buffers;
  
  lms_stream_meta_t tx_metadata; //Use metadata for additional control over sample receive function behaviour
  lms_stream_meta_t rx_metadata; //Use metadata for additional control over sample receive function behaviour
  
  lms_range_t rx_range;
  lms_range_t tx_range;
 
} rf_limesdr_handler_t;

int lime_error(void *h)
{
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;

  //print last error message
  fprintf(stderr, "Error: %s\n", LMS_GetLastErrorMessage());
  if(handler->device != NULL)
    LMS_Close(handler->device);

  return SRSLTE_ERROR;
}

void rf_limesdr_get_freq_range(void *h)
{
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  LMS_GetLOFrequencyRange(handler->device, LMS_CH_RX, &(handler->rx_range));
  LMS_GetLOFrequencyRange(handler->device, LMS_CH_TX, &(handler->tx_range));
}

void rf_limesdr_suppress_handler(const char *x)
{
  // not supported
}

void rf_limesdr_msg_handler(const char *msg)
{
  // not supported
}

void rf_limesdr_suppress_stdout(void *h)
{
  // not supported
}

void rf_limesdr_register_error_handler(void *notused, srslte_rf_error_handler_t new_handler)
{
  // not supported
}

static bool isLocked(rf_limesdr_handler_t *handler, char *sensor_name, void *value_h)
{
  // not supported
  return true;
}

char* rf_limesdr_devname(void* h)
{
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  handler->dev_info = LMS_GetDeviceInfo(handler);
     
  return handler->dev_info->deviceName;
}

bool rf_limesdr_rx_wait_lo_locked(void *h)
{
  // not supported
  return true;
}

void rf_limesdr_set_tx_cal(void *h, srslte_rf_cal_t *cal)
{
  // not supported
}

void rf_limesdr_set_rx_cal(void *h, srslte_rf_cal_t *cal) 
{
  // not supported
}

int rf_limesdr_start_rx_stream(void *h)
{
  printf("Starting rx stream\n");
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  if(LMS_StartStream(&(handler->rx_stream)) != 0){
    return lime_error(h);
  }
  return 0;
}


int rf_limesdr_start_tx_stream(void *h)
{
  printf("Starting tx stream\n");
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  if(LMS_StartStream(&(handler->tx_stream)) != 0){
    return lime_error(h);
  }
  return 0;
}

int rf_limesdr_stop_rx_stream(void *h)
{
  printf("Stopping rx stream\n");
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  //stream is stopped but can be started again with LMS_StartStream()
  if(LMS_StopStream(&(handler->rx_stream)) != 0){
    return lime_error(h);
  }
  return 0;
}
int rf_limesdr_stop_tx_stream(void *h)
{
  printf("Stopping tx stream\n");
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  //stream is stopped but can be started again with LMS_StartStream()
  if(LMS_StopStream(&(handler->tx_stream)) != 0){
    return lime_error(h);
  }
  return 0;
}

void rf_limesdr_flush_buffer(void *h)
{
  int n;
  cf_t tmp1[1024];
  cf_t tmp2[1024];
  void *data[2] = {tmp1, tmp2};
  do {
    n = rf_limesdr_recv_with_time_multi(h, data, 1024, 0, NULL, NULL);
  } while (n > 0);
}

bool rf_limesdr_has_rssi(void *h)
{
  return false;
}

float rf_limesdr_get_rssi(void *h)
{
  return 0.0;
}

//TODO: add multi-channel support
int rf_limesdr_open_multi(char *args, void **h, uint32_t nof_rx_antennas)
{
  return rf_limesdr_open(args, h);
}

int rf_limesdr_open(char *args, void **h)
{
  printf("Opening device\n");
  *h = NULL;

  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) malloc(sizeof(rf_limesdr_handler_t));
  if (!handler) {
    perror("malloc");
    return -1;
  }
  *h = handler;

  handler->device = NULL;

  handler->buffer_size = 1024;
  handler->num_buffers = 8;
  handler->channel = 0;
  
  
  int n;
  if ((n = LMS_GetDeviceList(handler->list)) < 0) //NULL can be passed to only get number of devices
    return SRSLTE_ERROR;

  if (LMS_Open(&(handler->device), handler->list[0], NULL))
    return SRSLTE_ERROR;

  if (LMS_Init(handler->device) != 0)
    return SRSLTE_ERROR;

  if (LMS_EnableChannel(handler->device, LMS_CH_RX, handler->channel, true) != 0)
    return lime_error(handler);

  if (LMS_EnableChannel(handler->device, LMS_CH_TX, handler->channel, true) != 0)
    return lime_error(handler);

  rf_limesdr_get_freq_range(handler);

  handler->rx_is_streaming                = false;
  handler->rx_stream.channel              = handler->channel; //channel number
  handler->rx_stream.fifoSize             = 1024 * 1024; //fifo size in samples
  handler->rx_stream.throughputVsLatency  = 1.0; //optimize for max throughput
  handler->rx_stream.isTx                 = false; //RX channel
  handler->rx_stream.dataFmt              = LMS_FMT_F32;
  handler->rx_metadata.flushPartialPacket = false; //Do not discard data remainder when read size differs from packet size
  handler->rx_metadata.waitForTimestamp   = false; //Do not wait for specific timestamps

  if (LMS_SetupStream(handler->device, &(handler->rx_stream)) != 0)
    return lime_error(handler);

  handler->tx_is_streaming                = false;
  handler->tx_stream.channel              = handler->channel; //channel number
  handler->tx_stream.fifoSize             = 1024 * 1024; //fifo size in samples
  handler->tx_stream.throughputVsLatency  = 1.0; //optimize for max throughput
  handler->tx_stream.isTx                 = true; //TX channel
  handler->rx_stream.dataFmt              = LMS_FMT_F32;
  handler->tx_metadata.flushPartialPacket = false; //Do not discard data remainder when read size differs from packet size
  handler->tx_metadata.waitForTimestamp   = false; //Do not wait for specific timestamps

  if (LMS_SetupStream(handler->device, &(handler->tx_stream)) != 0)
    return lime_error(handler);

  return SRSLTE_SUCCESS;
}


int rf_limesdr_close(void *h)
{
  printf("Closing device\n");
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  if(handler->rx_is_streaming) {
    LMS_StopStream(&(handler->rx_stream));
  }
  LMS_DestroyStream(handler->device, &(handler->rx_stream)); //stream is deallocated and can no longer be used

  if(handler->tx_is_streaming) {
    LMS_StopStream(&(handler->tx_stream));
  }
  LMS_DestroyStream(handler->device, &(handler->tx_stream)); //stream is deallocated and can no longer be used

  LMS_Close(handler->device);
  return SRSLTE_SUCCESS;
}

void rf_limesdr_set_master_clock_rate(void *h, double rate)
{
  // Allow the limesdr to automatically set the appropriate clock rate
}

bool rf_limesdr_is_master_clock_dynamic(void *h) 
{
  return true;
}

double rf_limesdr_set_rx_srate(void *h, double rate)
{
  fprintf(stdout, "Setting rx rate: %f\n", rate);
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  if (LMS_SetSampleRate(handler->device, rate, 0) != 0)
    return lime_error(handler);

  handler->sampling_rate = rate;
  return rate;
}

double rf_limesdr_set_tx_srate(void *h, double rate)
{
  fprintf(stdout, "Setting tx rate: %f\n", rate);
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  if (LMS_SetSampleRate(handler->device, rate, 0) != 0)
    return lime_error(handler);

  handler->sampling_rate = rate;
  return rate;
}

double rf_limesdr_set_rx_gain(void *h, double gain)
{
  fprintf(stdout, "Setting rx gain: %f\n", gain);
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  if (LMS_SetNormalizedGain(handler->device, LMS_CH_RX, handler->channel, gain) != 0)
    return lime_error(handler);
    
  return gain;
}

double rf_limesdr_set_tx_gain(void *h, double gain)
{
  fprintf(stdout, "Setting tx gain: %f\n", gain);
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  if (LMS_SetNormalizedGain(handler->device, LMS_CH_TX, handler->channel, gain) != 0)
    return lime_error(handler);
    
  return gain;
}

double rf_limesdr_get_rx_gain(void *h)
{
  double gain;
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  if(LMS_GetNormalizedGain(handler->device, LMS_CH_RX,handler->channel,&gain) != 0)
    return lime_error(handler);

  return gain;
}

double rf_limesdr_get_tx_gain(void *h)
{
  double gain;
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  if(LMS_GetNormalizedGain(handler->device, LMS_CH_TX, handler->channel, &gain) != 0)
    return lime_error(handler);

  return gain;
}

double rf_limesdr_set_rx_freq(void *h, double freq)
{
  fprintf(stdout, "Setting rx freq: %f\n", freq);
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;

  if(freq > handler->rx_range.max || freq <  handler->rx_range.min) {
    fprintf(stderr, "Requested freq outside supported range. freq: %f, min: %f, max: %f\n", freq, handler->rx_range.min, handler->rx_range.max);
    return SRSLTE_ERROR;
  }

  if(LMS_SetLOFrequency(handler->device, LMS_CH_RX, handler->channel, freq) != 0)
    return lime_error(handler);

  // Automatic antenna port selection doesn't work - so set manually
  int ant_port = 1; // manually select antenna index 1 (LNA_H)
  if(freq < 1.5e9) {
    ant_port = 2;   // manually select antenna index 2 (LNA_L)
  }
  if (LMS_SetAntenna(handler->device, LMS_CH_RX, handler->channel, ant_port) != 0)
    return lime_error(handler);

  lms_name_t antenna_list[10];    //large enough list for antenna names.
                                  //Alternatively, NULL can be passed to LMS_GetAntennaList() to find out number of available antennae
  int n = 0;
  if ((n = LMS_GetAntennaList(handler->device, LMS_CH_RX, 0, antenna_list)) < 0)
    return lime_error(handler);

  fprintf(stdout, "Available antennae:\n");            //print available antennae names
  for(int i = 0; i < n; i++)
    fprintf(stdout, "%d : %s\n", i, antenna_list[i]);

  if((n = LMS_GetAntenna(handler->device, LMS_CH_RX, handler->channel)) < 0) //get currently selected antenna index
      return lime_error(handler);
  fprintf(stdout, "Selected antenna: %d : %s\n", n, antenna_list[n]); //print antenna index and name

  return freq;
}

double rf_limesdr_set_tx_freq(void *h, double freq)
{
  fprintf(stdout, "Setting tx freq: %f\n", freq);
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  if(freq > handler->tx_range.max || freq <  handler->tx_range.min) {
    fprintf(stderr, "Requested freq outside supported range. freq: %f, min: %f, max: %f\n", freq, handler->rx_range.min, handler->rx_range.max);
    return SRSLTE_ERROR;
  }

  if(LMS_SetLOFrequency(handler->device, LMS_CH_TX, handler->channel, freq) != 0)
      return lime_error(handler);

  return freq;
}


void rf_limesdr_get_time(void *h, time_t *secs, double *frac_secs) {
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  LMS_RecvStream(&(handler->rx_stream),NULL,0, &(handler->rx_metadata), 0);
  if (secs && frac_secs) {
    *secs =  (handler->rx_metadata.timestamp) / (handler->sampling_rate);
    int remainder = handler->rx_metadata.timestamp % handler->sampling_rate;
    *frac_secs = remainder/(handler->sampling_rate);
  }
}

//TODO: add multi-channel support
int rf_limesdr_recv_with_time_multi(void *h,
                                    void **data,
                                    uint32_t nsamples,
                                    bool blocking,
                                    time_t *secs,
                                    double *frac_secs)
{
  return rf_limesdr_recv_with_time(h, *data, nsamples, blocking, secs, frac_secs);
}

int rf_limesdr_recv_with_time(void *h,
                    void *data,
                    uint32_t nsamples,
                    bool blocking,
                    time_t *secs,
                    double *frac_secs) 
{
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;
  int samples = LMS_RecvStream(&(handler->rx_stream),data,nsamples, &(handler->rx_metadata), blocking ? 1000:0);
  if (secs && frac_secs) {
    *secs =  (handler->rx_metadata.timestamp) / (handler->sampling_rate);
    int remainder = handler->rx_metadata.timestamp % handler->sampling_rate;
    *frac_secs = remainder/(handler->sampling_rate);
  }

  return samples;
}
            

int rf_limesdr_send_timed(void *h,
                     void *data,
                     int nsamples,
                     time_t secs,
                     double frac_secs,                      
                     bool has_time_spec,
                     bool blocking,
                     bool is_start_of_burst,
                     bool is_end_of_burst) 
{
  rf_limesdr_handler_t *handler = (rf_limesdr_handler_t*) h;

  //float *data_in  = (float*) data;

  if(!handler->tx_is_streaming)
      rf_limesdr_start_tx_stream(h);

  handler->tx_metadata.timestamp = secs*handler->sampling_rate;
  handler->tx_metadata.timestamp += frac_secs*handler->sampling_rate;

  LMS_SendStream(&(handler->rx_stream), data, nsamples, &(handler->tx_metadata), blocking ? 1000:0);

  return 1;
}




