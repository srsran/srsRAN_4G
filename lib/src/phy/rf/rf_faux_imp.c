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
#include <assert.h>
#include <sys/time.h>

#include "srslte/srslte.h"
#include "rf_faux_imp.h"
#include "srslte/phy/rf/rf.h"

static bool g_logStdout = true;

#define FAUX_DEBUG(_fmt, ...) if(g_logStdout) fprintf(stdout, "%s" _fmt, __func__, ##__VA_ARGS__)

#define LOG_FUNC_TODO printf("XXX_TODO file:%s func:%s line:%d\n", __FILE__, __func__, __LINE__)


typedef struct 
 {
   char *devName;
   double rxGain;
   double txGain;
   double rxRate;
   double txRate;
   double rxFreq;
   double txFreq;
   double txCal;
   double rxCal;
   double clockRate;
   void (*error_handler)(srslte_rf_error_t error);
   bool  rxStream;
 } faux_rf_info_t;

static void faux_rf_handle_error(srslte_rf_error_t error)
{
  FAUX_DEBUG("%s:%s type %s, opt %d, msg %s\b", 
         error.type == SRSLTE_RF_ERROR_LATE      ? "late"      :
         error.type == SRSLTE_RF_ERROR_UNDERFLOW ? "underflow" :
         error.type == SRSLTE_RF_ERROR_OVERFLOW  ? "overflow"  :
         error.type == SRSLTE_RF_ERROR_OTHER     ? "other"     :
         "unknown error");
}

static  faux_rf_info_t _info = { .devName       = "fauxRf0",
                                 .rxGain        =  0.0,
                                 .txGain        =  0.0,
                                 .rxRate        =  0.0,
                                 .txRate        =  0.0,
                                 .rxFreq        =  0.0,
                                 .txFreq        =  0.0,
                                 .rxCal         =  0.0,
                                 .txCal         =  0.0,
                                 .clockRate     =  0.0,
                                 .error_handler = faux_rf_handle_error,
                                 .rxStream      = false,
                               };

#define GET_FAUX_INFO(h)  assert(h); faux_rf_info_t *p = (faux_rf_info_t *)(h);

char* rf_faux_devname(void *h)
 {
   GET_FAUX_INFO(h)

   return p->devName;
 }

bool rf_faux_rx_wait_lo_locked(void *h)
 {
   LOG_FUNC_TODO;

   return false;
 }

int rf_faux_start_rx_stream(void *h)
 {
   GET_FAUX_INFO(h)

   p->rxStream = true;

   return 0;
 }

int rf_faux_stop_rx_stream(void *h)
 {
   GET_FAUX_INFO(h)

   p->rxStream = false;

   return 0;
 }

void rf_faux_flush_buffer(void *h)
 {
   LOG_FUNC_TODO;
 }

bool rf_faux_has_rssi(void *h)
 {
   LOG_FUNC_TODO;

   return false;
 }

float rf_faux_get_rssi(void *h)
 {
   LOG_FUNC_TODO;

   return 0.0;
 }

void rf_faux_suppress_stdout(void *h)
 {
   // g_logStdout = false;
 }

void rf_faux_register_error_handler(void *h, srslte_rf_error_handler_t error_handler)
 {
   GET_FAUX_INFO(h)

   p->error_handler = error_handler;
 }

int rf_faux_open(char *args, void **h)
 {
   return rf_faux_open_multi(args, h, 1);
 }

int rf_faux_open_multi(char *args, void **h, uint32_t nof_channels)
 {
   FAUX_DEBUG("args %s, channels %u\n", args, nof_channels);

   if(nof_channels == 1)
    {
      *h = &_info;

      return 0;
    }
   else
    {
      *h = NULL;

      return -1;
    }
 }

int rf_faux_close(void *h)
 {
   LOG_FUNC_TODO;

   return 0;
 }

void rf_faux_set_master_clock_rate(void *h, double rate)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("rate %lf to %lf\n", p->clockRate, rate);

   p->clockRate = rate;
 }

bool rf_faux_is_master_clock_dynamic(void *h)
 {
   LOG_FUNC_TODO;

   return false;
 }

double rf_faux_set_rx_srate(void *h, double rate)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("rate %lf to %lf\n", p->rxRate, rate);

   p->rxRate = rate;

   return p->rxRate;
 }

double rf_faux_set_rx_gain(void *h, double gain)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("gain %lf to %lf\n", p->rxGain, gain);

   p->rxGain = gain;

   return p->rxGain;
 }

double rf_faux_set_tx_gain(void *h, double gain)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("gain %lf to %lf\n", p->txGain, gain);

   p->txGain = gain;

   return p->txGain;
 }

double rf_faux_get_rx_gain(void *h)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("gain %lf\n", p->rxGain);

   return p->rxGain;
 }

double rf_faux_get_tx_gain(void *h)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("gain %lf\n", p->txGain);

   return p->txGain;
 }

double rf_faux_set_rx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("freq %lf to %lf\n", p->rxFreq, freq);

   p->rxFreq = freq;

   return p->rxFreq;
 }

double rf_faux_set_tx_srate(void *h, double rate)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("rate %lf to %lf\n", p->txRate, rate);

   p->txRate = rate;

   return p->txRate;
 }

double rf_faux_set_tx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("freq %lf to %lf\n", p->txFreq, freq);

   p->txFreq = freq;

   return p->txFreq;
 }

void rf_faux_get_time(void *h, time_t *secs, double *frac_secs)
 {
   FAUX_DEBUG("secs %ld, frac %lf\n", 
           *secs,
           *frac_secs);
 }

int rf_faux_recv_with_time(void *h, void *data, uint32_t nsamples, 
                           bool blocking, time_t *secs, double *frac_secs)
 {
   GET_FAUX_INFO(h)

   struct timeval tv;

   gettimeofday(&tv, NULL);

   *secs = tv.tv_sec;

   *frac_secs = tv.tv_usec / 1000000.0;

   FAUX_DEBUG("nsmaples %u, blocking %s, secs %ld, frac %lf\n", 
           nsamples, 
           blocking ? "yes" : "no",
           *secs,
           *frac_secs);

   usleep(100000);

   return 0;
 }

int rf_faux_recv_with_time_multi(void *h, void **data, uint32_t nsamples, 
                                 bool blocking, time_t *secs, double *frac_secs)
{
   return rf_faux_recv_with_time(h, data[0], nsamples, 
                                 blocking, secs, frac_secs);
}

int rf_faux_send_timed(void *h, void *data, int nsamples,
                       time_t secs, double frac_secs, bool has_time_spec,
                       bool blocking, bool is_start_of_burst, bool is_end_of_burst)
{
   LOG_FUNC_TODO;

   return 0;
}

int rf_faux_send_timed_multi(void *h, void *data[4], int nsamples,
                             time_t secs, double frac_secs, bool has_time_spec,
                             bool blocking, bool is_start_of_burst, bool is_end_of_burst)
{
   LOG_FUNC_TODO;

   return 0;
}

void rf_faux_set_tx_cal(void *h, srslte_rf_cal_t *cal)
{
   FAUX_DEBUG("gain %f, phase %f, i %f, q %f\n", 
           cal->dc_gain, cal->dc_phase, cal->iq_i, cal->iq_q);
}

void rf_faux_set_rx_cal(void *h, srslte_rf_cal_t *cal)
{
   FAUX_DEBUG("gain %f, phase %f, i %f, q %f\n", 
           cal->dc_gain, cal->dc_phase, cal->iq_i, cal->iq_q);

}


