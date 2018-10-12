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

#include "srslte/srslte.h"
#include "rf_dummy_imp.h"
#include "srslte/phy/rf/rf.h"

#include <assert.h>
#include <unistd.h>

static bool log_stdout = true;

#define LOG_FMT "%02d:%02d:%02d.%06ld [DMY] [%c] %s "

#define LOG_INFO(_fmt, ...) do { if(log_stdout) {                             \
                                   struct timeval _tv_now;                    \
                                   struct tm _tm;                             \
                                   gettimeofday(&_tv_now, NULL);              \
                                   localtime_r(&_tv_now.tv_sec, &_tm);        \
                                   fprintf(stdout, LOG_FMT _fmt "\n",         \
                                           _tm.tm_hour,                       \
                                           _tm.tm_min,                        \
                                           _tm.tm_sec,                        \
                                           _tv_now.tv_usec,                   \
                                           'I',                               \
                                           __func__,                          \
                                           ##__VA_ARGS__);                    \
                               }                                              \
                             } while(0);


// rf dev info
typedef struct {
   char *               dev_name;
   int                  nodetype;
   uint32_t             nof_tx_ports;
   uint32_t             nof_rx_ports;
   double               rx_gain;
   double               tx_gain;
   double               rx_srate;
   double               tx_srate;
   double               rx_freq;
   double               tx_freq;
   double               clock_rate;
   void (*error_handler)(srslte_rf_error_t error);
   bool                 rx_stream;
   srslte_rf_info_t     rf_info;
} rf_dummy_info_t;



static void rf_dummy_handle_error(srslte_rf_error_t error)
{
  printf("type %s", 
          error.type == SRSLTE_RF_ERROR_LATE      ? "late"      :
          error.type == SRSLTE_RF_ERROR_UNDERFLOW ? "underflow" :
          error.type == SRSLTE_RF_ERROR_OVERFLOW  ? "overflow"  :
          error.type == SRSLTE_RF_ERROR_OTHER     ? "other"     :
          "unknown error");
}


static  rf_dummy_info_t rf_dummy_info = { .dev_name        = "dummyrf",
                                          .nof_tx_ports    = 1,
                                          .nof_rx_ports    = 1,
                                          .rx_gain         = 0.0,
                                          .tx_gain         = 0.0,
                                          .rx_srate        = SRSLTE_CS_SAMP_FREQ,
                                          .tx_srate        = SRSLTE_CS_SAMP_FREQ,
                                          .rx_freq         = 0.0,
                                          .tx_freq         = 0.0,
                                          .clock_rate      = 0.0,
                                          .error_handler   = rf_dummy_handle_error,
                                          .rx_stream       = false,
                                          .rf_info         = {}
                                        };

#define GET_DEV_INFO(h)  assert(h); rf_dummy_info_t *_info = (rf_dummy_info_t *)(h)


// begin RF API

void rf_dummy_suppress_stdout(void *h)
 {
   // log_stdout = false;
 }


char* rf_dummy_devname(void *h)
 {
   GET_DEV_INFO(h);

   LOG_INFO("dev name %s", _info->dev_name);

   return _info->dev_name;
 }


bool rf_dummy_rx_wait_lo_locked(void *h)
 {
   LOG_INFO("");

   return false;
 }


int rf_dummy_start_rx_stream(void *h, bool now)
 {
   GET_DEV_INFO(h);
   
   _info->rx_stream = true;

   LOG_INFO("");

   return 0;
 }


int rf_dummy_stop_rx_stream(void *h)
 {
   GET_DEV_INFO(h);

   _info->rx_stream = false;

   LOG_INFO("");

   return 0;
 }


void rf_dummy_flush_buffer(void *h)
 {
   LOG_INFO("");
 }


bool rf_dummy_has_rssi(void *h)
 {
   LOG_INFO("false");

   return false;
 }


float rf_dummy_get_rssi(void *h)
 {
   const float rssi = 0.0;

   LOG_INFO("rssi %f", rssi);

   return rssi;
 }


void rf_dummy_register_error_handler(void *h, srslte_rf_error_handler_t error_handler)
 {
   GET_DEV_INFO(h);

   LOG_INFO("");

   _info->error_handler = error_handler;
 }


int rf_dummy_open(char *args, void **h)
 {
   LOG_INFO("");

   return rf_dummy_open_multi(args, h, 1);
 }


int rf_dummy_open_multi(char *args, void **h, uint32_t nof_channels)
 {
   LOG_INFO("num_channels %d, args %s", nof_channels, args);

   *h = &rf_dummy_info;

   return 0;
 }


int rf_dummy_close(void *h)
 {
   LOG_INFO("");

   return 0;
 }


void rf_dummy_set_master_clock_rate(void *h, double rate)
 {
   GET_DEV_INFO(h);

   LOG_INFO("rate %6.4lf to %6.4lf", _info->clock_rate, rate);

   _info->clock_rate = rate;
 }


bool rf_dummy_is_master_clock_dynamic(void *h)
 {
   LOG_INFO("false");

   return false;
 }


double rf_dummy_set_rx_gain(void *h, double gain)
 {
   GET_DEV_INFO(h);

   LOG_INFO("gain %3.2lf to %3.2lf", _info->rx_gain, gain);

   _info->rx_gain = gain;

   return _info->rx_gain;
 }


double rf_dummy_set_tx_gain(void *h, double gain)
 {
   GET_DEV_INFO(h);

   LOG_INFO("gain %3.2lf to %3.2lf", _info->tx_gain, gain);

   _info->tx_gain = gain;

   return _info->tx_gain;
 }


double rf_dummy_get_rx_gain(void *h)
 {
   GET_DEV_INFO(h);

   LOG_INFO("gain %3.2lf", _info->rx_gain);

   return _info->rx_gain;
 }


double rf_dummy_get_tx_gain(void *h)
 {
   GET_DEV_INFO(h);

   LOG_INFO("gain %3.2lf", _info->tx_gain);

   return _info->tx_gain;
 }

srslte_rf_info_t * rf_dummy_get_rf_info(void *h)
  {
     GET_DEV_INFO(h);

     LOG_INFO("tx_gain min/max %3.2lf/%3.2lf, rx_gain min/max %3.2lf/%3.2lf",
                  _info->rf_info.min_tx_gain,
                  _info->rf_info.max_tx_gain,
                  _info->rf_info.min_rx_gain,
                  _info->rf_info.max_rx_gain);

     return &_info->rf_info;
  }

double rf_dummy_set_rx_srate(void *h, double rate)
 {
   GET_DEV_INFO(h);

   LOG_INFO("srate %4.2lf MHz to %4.2lf MHz", 
            _info->rx_srate / 1e6, rate / 1e6);

   _info->rx_srate = rate;

   return _info->rx_srate;
 }


double rf_dummy_set_tx_srate(void *h, double rate)
 {
   GET_DEV_INFO(h);

   LOG_INFO("srate %4.2lf MHz to %4.2lf MHz", 
            _info->tx_srate / 1e6, rate / 1e6);

   _info->tx_srate = rate;

   return _info->tx_srate;
 }


double rf_dummy_set_rx_freq(void *h, double freq)
 {
   GET_DEV_INFO(h);

   LOG_INFO("freq %4.2lf MHz to %4.2lf MHz", 
            _info->rx_freq / 1e6, freq / 1e6);

   _info->rx_freq = freq;

   return _info->rx_freq;
 }


double rf_dummy_set_tx_freq(void *h, double freq)
 {
   GET_DEV_INFO(h);

   LOG_INFO("freq %4.2lf MHz to %4.2lf MHz", 
             _info->tx_freq / 1e6, freq / 1e6);

   _info->tx_freq = freq;

   return _info->tx_freq;
 }


void rf_dummy_get_time(void *h, time_t *full_secs, double *frac_secs)
 {
   if(full_secs && frac_secs)
     {
       struct timeval tv;
       gettimeofday(&tv, NULL);

       *full_secs = tv.tv_sec; 
       *frac_secs = tv.tv_usec / 1e6;
     }
 }



int rf_dummy_recv_with_time(void *h, void *data, uint32_t nsamples, 
                            bool blocking, time_t *full_secs, double *frac_secs)
 {
   void *d[4] = {data, NULL, NULL, NULL};

   return rf_dummy_recv_with_time_multi(h, 
                                        d,
                                        nsamples, 
                                        blocking,
                                        full_secs,
                                        frac_secs);
 }



int rf_dummy_recv_with_time_multi(void *h, void **data, uint32_t nsamples, 
                                  bool blocking, time_t *full_secs, double *frac_secs)
{
   usleep(1000);

   rf_dummy_get_time(h, full_secs, frac_secs);

   if(full_secs && frac_secs)
    {
      LOG_INFO("nsamples %u %ld:%0.6lf", nsamples, *full_secs, *frac_secs);
    }

   return nsamples;
}


int rf_dummy_send_timed(void *h, void *data, int nsamples,
                       time_t full_secs, double frac_secs, bool has_time_spec,
                       bool blocking, bool is_sob, bool is_eob)
{
   void *d[4] = {data, NULL, NULL, NULL};

   return rf_dummy_send_timed_multi(h, d, nsamples, full_secs, frac_secs, has_time_spec, blocking, is_sob, is_eob);
}


int rf_dummy_send_timed_multi(void *h, void *data[4], int nsamples,
                             time_t full_secs, double frac_secs, bool has_time_spec,
                             bool blocking, bool is_sob, bool is_eob)
{
   LOG_INFO("nsamples %u, sob %d, eob %d", nsamples, is_sob, is_eob);

   return nsamples;
}



