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
#include <iio.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "srslte/srslte.h"
#include "rf_zed_imp.h"
#include "srslte/rf/rf.h"

#define FMCOMMS_RF_RX_BUFF_L   0x4000
#define AGC_SLOW1    "SLOW"
#define AGC_SLOW2    "FAST"
#define AGC_SLOW3    "HYBRID"
#define AGC_SLOW4    "MANUAL"
#define PORT_SELECT1 "A_BALANCED"
#define PORT_SELECT_TX "A"
#define True 1
#define False 1

typedef struct {

  struct iio_context *ctx;
  struct iio_device *rxdev, *txdev, *phy;
  struct iio_channel *rxch0, *txch0, *rxch1, *txch1, *phych0;
  struct iio_buffer *Txbuf, *Rxbuf;

  unsigned int num_buffers;
  unsigned int buffer_size;
  unsigned int num_transfers;
  unsigned int rx_timeout_ms;
  unsigned int tx_timeout_ms;

  size_t rx_nof_samples;
  size_t tx_nof_samples;
  double tx_rate;
  bool dynamic_rate;
  bool has_rssi;
//  uhd_sensor_value_handle rssi_value;
} rf_zed_handler_t;


void rf_zed_suppress_stdout(void *h) {
}

void rf_zed_register_error_handler(void *notused, srslte_rf_error_handler_t new_handler)
{
}

bool rf_zed_rx_wait_lo_locked(void *h)
{
  usleep(1000);
  return true;
}

void rf_zed_set_tx_cal(void *h, srslte_rf_cal_t *cal)
{

}

void rf_zed_set_rx_cal(void *h, srslte_rf_cal_t *cal)
{

}


int rf_zed_start_rx_stream(void *h)
{
  return 0;
}

int rf_zed_stop_rx_stream(void *h)
{
  return 0;
}

void rf_zed_flush_buffer(void *h)
{
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  iio_buffer_refill(handler->Rxbuf);
}

bool rf_zed_has_rssi(void *h) {
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  bool ret = handler->rxch0 && iio_channel_find_attr(handler->rxch0, "rssi");

  return ret;
}

float rf_zed_get_rssi(void *h) {
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  //  char buf[1024];
  double rssi = 0.0;
  if (handler->has_rssi) {
      if(iio_channel_attr_read_double(handler->rxch0, "rssi", &rssi) == NULL)
	fprintf(stderr, "error reading rssi attribute");
      return rssi;
  } else {
      return 0.0;
  }
}

int rf_zed_open(char *args, void **h)
{
  char *device_rx = "cf-ad9361-lpc";
  char *device_phy = "ad9361-phy";
  char *device_tx = "cf-ad9361-dds-core-lpc";

  if (h) {
    *h = NULL;

    rf_zed_handler_t *handler = (rf_zed_handler_t*) malloc(sizeof(rf_zed_handler_t));
    if (!handler) {
      perror("malloc");
      return -1;
    }
    *h = handler;


    char args2[512];

    handler->dynamic_rate = true;

    // Allow NULL parameter
    if (args == NULL) {
      args = "192.168.1.10";
    }
    /* If device IP Address not given in args, choose a default one */
    if (args[0]=='\0') {
	args = "192.168.1.10";
    }
    /* Create UHD handler */
    printf("Opening IIO Zedboard with args: %s\n", args);
    handler->ctx = iio_create_network_context(args);
    if (handler->ctx == NULL) {
      fprintf(stderr, "Error creating iio context: code %d\n");
      exit(1);
    }

    handler->rxdev = iio_context_find_device(handler->ctx, device_rx);
    handler->phy = iio_context_find_device(handler->ctx, device_phy);
    handler->txdev = iio_context_find_device(handler->ctx, device_tx);


    int nb_channels = iio_device_get_channels_count(handler->txdev);
    for (int i = 0; i < nb_channels; i++)
            iio_channel_disable(iio_device_get_channel(handler->txdev, i));

    nb_channels = iio_device_get_channels_count(handler->rxdev);
    for (int i = 0; i < nb_channels; i++)
            iio_channel_disable(iio_device_get_channel(handler->rxdev, i));

    handler->rxch0 = iio_device_find_channel(handler->rxdev, "voltage0", false); // input
    handler->rxch1 = iio_device_find_channel(handler->rxdev, "voltage1", false); // input
    handler->txch0 = iio_device_find_channel(handler->txdev, "voltage0", true); // output
    handler->txch1 = iio_device_find_channel(handler->txdev, "voltage1", true); // output

    if (!handler->txch0 || !handler->txch1 || !handler->rxch0 || !handler->rxch1) {
    	fprintf(stderr, "Channel not found\n");
        exit(1);
    }

    iio_channel_enable(handler->rxch0);
    iio_channel_enable(handler->rxch1);
    iio_channel_enable(handler->txch0);
    iio_channel_enable(handler->txch1);

    handler->has_rssi = rf_zed_has_rssi(handler);

    const char *attr = NULL;
    int ret = iio_device_identify_filename(handler->phy, "in_voltage0_rf_port_select", &handler->phych0, &attr);
    ret = iio_channel_attr_write(handler->phych0, attr, PORT_SELECT1);
    if (ret < 0)
      {
	fprintf(stderr, "Unable to set RF port select (%i)\n", ret);
	exit(1);
      }

    ret = iio_device_identify_filename(handler->phy, "in_voltage_rf_dc_offset_tracking_en", &handler->phych0, &attr);
    ret = iio_channel_attr_write_bool(handler->phych0, attr, True);
    if (ret < 0)
      {
	fprintf(stderr, "Unable to enable RF DC (%i)\n", ret);
	exit(1);
      }

    ret = iio_device_identify_filename(handler->phy, "in_voltage_bb_dc_offset_tracking_en", &handler->phych0, &attr);
    ret = iio_channel_attr_write_bool(handler->phych0, attr, True);
    if (ret < 0)
      {
	fprintf(stderr, "Unable to enable BB DC (%i)\n", ret);
	exit(1);
      }

    return 0;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}


int rf_zed_close(void *h)
{
  rf_zed_stop_rx_stream(h);
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;

  int rxstatus, txstatus;
  // Disable RX module, shutting down our underlying RX stream
  if (handler->Txbuf != NULL)
    {
      iio_buffer_destroy(handler->Txbuf);
    }

  if (handler->Rxbuf != NULL)
    {
      iio_buffer_destroy(handler->Rxbuf);
    }

  if (handler->rxch0)
    iio_channel_disable(handler->rxch0);

  if (handler->rxch1)
    iio_channel_disable(handler->rxch1);

  if (handler->txch0)
    iio_channel_disable(handler->txch0);

  if (handler->txch1)
    iio_channel_disable(handler->txch1);

  if (handler->ctx)
    iio_context_destroy(handler->ctx);

  return 0;
}

void rf_zed_set_master_clock_rate(void *h, double rate) {

}

bool rf_zed_is_master_clock_dynamic(void *h) {
  return 1;
}

double rf_zed_set_rx_srate(void *h, double freq)
{
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  const char *attr = NULL;
  int ret = iio_device_identify_filename(handler->phy, "in_voltage_sampling_frequency", &handler->phych0, &attr);
  ret = iio_channel_attr_write_double(handler->phych0, attr, freq);
  if (ret < 0)
    {
      fprintf(stderr, "Unable to set samplerate (%i)\n", ret);
      exit(1);
    }

  if(freq == (double)1500 * 1536)
    handler->rx_nof_samples  = 2048;
  if(freq == (double) 1500 * 768)
    handler->rx_nof_samples  = 2048;
  if(freq == (double) 1500 * 384)
    handler->rx_nof_samples  = 1024;
  if(freq == (double) 1500 * 128)
    handler->rx_nof_samples  = 256;

  handler->buffer_size = handler->rx_nof_samples * sizeof(int32_t);
  handler->Rxbuf = iio_device_create_buffer(handler->rxdev, handler->buffer_size, false);


  if (!handler->Rxbuf)
    {
	fprintf(stderr, "Unable to create rx buffer\n");
	exit(1);
    }

  return freq;
}

double rf_zed_set_tx_srate(void *h, double freq)
{
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  const char *attr = NULL;
  int ret = iio_device_identify_filename(handler->phy, "out_voltage_sampling_frequency", &handler->phych0, &attr);
  ret = iio_channel_attr_write_double(handler->phych0, attr, freq);
  if (ret < 0)
  {
      fprintf(stderr, "Unable to set samplerate (%i)\n", ret);
      exit(1);
  }
  handler->tx_rate = freq;

  if((long long)freq == 23040000)
    handler->tx_nof_samples  = 2048;
  if((long long)freq == 11520000)
    handler->tx_nof_samples  = 2048;
  if((long long)freq == 5760000)
    handler->tx_nof_samples  = 1024;
  if((long long)freq == 1920000)
    handler->tx_nof_samples  = 256;

  handler->buffer_size = handler->tx_nof_samples * sizeof(int32_t);
  handler->Txbuf = iio_device_create_buffer(handler->txdev, handler->buffer_size, false);


  if (!handler->Txbuf)
    {
	fprintf(stderr, "Unable to create tx buffer\n");
	exit(1);
    }

  return freq;
}

double rf_zed_set_rx_gain(void *h, double gain)
{
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  const char *attr = NULL;
  int ret = iio_device_identify_filename(handler->phy, "in_voltage0_hardwaregain", &handler->phych0, &attr);
  ret = iio_channel_attr_write_double(handler->phych0, attr, gain);
  if (ret < 0)
    {
      fprintf(stderr, "Unable to set rx gain (%i)\n", ret);
      exit(1);
    }
  return gain;
}

double rf_zed_set_tx_gain(void *h, double gain)
{
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  const char *attr = NULL;
  int ret = iio_device_identify_filename(handler->phy, "out_voltage0_hardwaregain", &handler->phych0, &attr);
  ret = iio_channel_attr_write_double(handler->phych0, attr, 0);
  if (ret < 0)
    {
      fprintf(stderr, "Unable to set tx attenuation (%i)\n", ret);
      exit(1);
    }
  return gain;
}

double rf_zed_get_rx_gain(void *h)
{
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  double gain;
  const char *attr = NULL;
  int ret = iio_device_identify_filename(handler->phy, "in_voltage0_hardwaregain", &handler->phych0, &attr);
  ret = iio_channel_attr_read_double(handler->phych0, attr, &gain);
  if (ret < 0)
    {
      fprintf(stderr, "Unable to read rx gain (%i)\n", ret);
      exit(1);
    }

  return gain;
}

double rf_zed_get_tx_gain(void *h)
{
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  double gain;
  const char *attr = NULL;
  int ret = iio_device_identify_filename(handler->phy, "out_voltage0_hardwaregain", &handler->phych0, &attr);
  ret = iio_channel_attr_read_double(handler->phych0, attr, &gain);
  if (ret < 0)
    {
      fprintf(stderr, "Unable to read tx attenuation (%i)\n", ret);
      exit(1);
    }

  return gain;
}

double rf_zed_set_rx_freq(void *h, double freq)
{
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  const char *attr = NULL;
  int ret = iio_device_identify_filename(handler->phy, "out_altvoltage0_RX_LO_frequency", &handler->phych0, &attr);
  ret = iio_channel_attr_write_longlong(handler->phych0, attr, (long long)freq);
  if (ret < 0)
    {
      fprintf(stderr, "Unable to set LO frequency (%i)\n", ret);
      exit(1);
    }
  return freq;
}

double rf_zed_set_tx_freq(void *h, double freq)
{
  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  const char *attr = NULL;
  int ret = iio_device_identify_filename(handler->phy, "out_altvoltage1_TX_LO_frequency", &handler->phych0, &attr);
  ret = iio_channel_attr_write_longlong(handler->phych0, attr, (long long)freq);
  if (ret < 0)
    {
      fprintf(stderr, "Unable to set LO frequency (%i)\n", ret);
      exit(1);
    }
  return freq;
}


void rf_zed_get_time(void *h, time_t *secs, double *frac_secs) {
}

int rf_zed_recv_with_time(void *h,
                    void *data,
                    uint32_t nsamples,
                    bool blocking,
                    time_t *secs,
                    double *frac_secs)
{

  rf_zed_handler_t *handler = (rf_zed_handler_t*) h;
  int ret = iio_buffer_refill(handler->Rxbuf);
  if (ret < 0){
      exit(1);
  }

  void *samples = data;

  void *buf_start0 = iio_buffer_first(handler->Rxbuf, handler->rxch0);
  void *buf_start1 = iio_buffer_first(handler->Rxbuf, handler->rxch1);
  void *buf_end = iio_buffer_end(handler->Rxbuf);
  ptrdiff_t buf_step = iio_buffer_step(handler->Rxbuf);
  void * src_ptr0, *src_ptr1;
  int32_t *dst_ptr = (int32_t*)malloc(sizeof(int32_t)*nsamples);
  memset(dst_ptr, 0, sizeof(int32_t)*nsamples);

  int i = 0;
  for (src_ptr0 = buf_start0, src_ptr1 = buf_start1;
      src_ptr0 < buf_end && src_ptr1 < buf_end && i < nsamples;
      src_ptr0 += buf_step, src_ptr1 += buf_step, i++)
    {
      int16_t  i_s, q_s;
      iio_channel_convert(handler->rxch0, (void *)&i_s, (const void *) src_ptr0);
      iio_channel_convert(handler->rxch1, (void *)&q_s, (const void *) src_ptr1);
      dst_ptr[i] = (((int32_t)q_s << 16) + i_s);
    }
  memcpy(samples, dst_ptr, i * sizeof(int32_t));
  free(dst_ptr);

  return nsamples;
}

int rf_zed_send_timed(void *h,
                     void *data,
                     int nsamples,
                     time_t secs,
                     double frac_secs,
                     bool has_time_spec,
                     bool blocking,
                     bool is_start_of_burst,
                     bool is_end_of_burst)
{
  rf_zed_handler_t* handler = (rf_zed_handler_t*) h;

  void *samples = data;
  void *buf_start0 = iio_buffer_first(handler->Txbuf, handler->txch0);
  void *buf_start1 = iio_buffer_first(handler->Txbuf, handler->txch1);
  void *buf_end = iio_buffer_end(handler->Txbuf);
  ptrdiff_t buf_step = iio_buffer_step(handler->Txbuf);
  void * dst_ptr0, *dst_ptr1;
  int i = 0;

  for (dst_ptr0 = buf_start0, dst_ptr1 = buf_start1;
      dst_ptr0 < buf_end && dst_ptr1 < buf_end && i < nsamples;
      dst_ptr0 += buf_step, dst_ptr1 += buf_step, i++)  // for each two sample received from I channel we write one sample (int32)
    {
      int32_t  s = *((int32_t *)samples);
      int16_t i_s = (int16_t)(s & 0xFFFF);
      int16_t q_s = (int16_t)(((uint32_t)(s & 0xFFFF0000)) >> 16);
      iio_channel_convert_inverse(handler->txch0, dst_ptr0, (const void *) &i_s);
      iio_channel_convert_inverse(handler->txch1, dst_ptr1, (const void *) &q_s);
      samples = (int32_t *)samples + 1;
    }

  int ret = iio_buffer_push(handler->Txbuf);
  if (ret < 0)
    exit(1);
  return ret;
}

