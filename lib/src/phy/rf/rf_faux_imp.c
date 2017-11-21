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

#define FAUX_DEBUG(fmt, ...) do {                                                           \
                                 if(g_logStdout) {                                          \
                                   struct timeval _tv_now;                                  \
                                   struct tm tm;                                            \
                                   gettimeofday(&_tv_now, NULL);                            \
                                   localtime_r(&_tv_now.tv_sec, &tm);                       \
                                   fprintf(stdout, "%02d.%02d.%02d.%06ld %s [DEBUG], " fmt "\n",  \
                                           tm.tm_hour,                                      \
                                           tm.tm_min,                                       \
                                           tm.tm_sec,                                       \
                                           _tv_now.tv_usec,                                 \
                                           __func__,                                        \
                                           ##__VA_ARGS__);                                  \
                                 }                                                          \
                             } while(0);

#define LOG_FUNC_TODO printf("XXX_TODO file:%s func:%s line:%d\n", __FILE__, __func__, __LINE__)

#define BOOL_TO_STR(x) (x) ? "yes" : "no"

#define USEC_X_SEC 1000000

void TV_TO_TS(struct timeval *tv, time_t *s, double *f)
{
  *s = tv->tv_sec; 
  *f = tv->tv_usec / 1.0e6;
}

void TS_TO_TV(struct timeval *tv, time_t s, double f)
{
  tv->tv_sec  = s;
  tv->tv_usec = f * 1.0e6;
}

void TS_DIFF(time_t secs, double frac, struct timeval * tv_dif)
{
   struct timeval tv_now, tv_nxt;

   gettimeofday(&tv_now, NULL);

   TS_TO_TV(&tv_nxt, secs, frac);

   if(secs || frac)
    {
      timersub(&tv_nxt, &tv_now, tv_dif);
    }
   else
    {
      tv_dif->tv_sec = 0;
      tv_dif->tv_usec = 0;
    }
}

#define FAUX_RF_DEV_NAMELEN 16

typedef struct 
 {
   char   devName[FAUX_RF_DEV_NAMELEN];
   double rxGain;
   double txGain;
   double rxRate;
   double txRate;
   double rxFreq;
   double txFreq;
   double txCal;
   double rxCal;
   double clockRate;
   void   (*error_handler)(srslte_rf_error_t error);
   bool   rxStream;
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

static bool faux_rf_is_enb(faux_rf_info_t * h)
{
  return strncmp(h->devName, "enb", FAUX_RF_DEV_NAMELEN) == 0;
}

static  faux_rf_info_t _info = { .devName       =  {0},
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
   
   FAUX_DEBUG("");

   p->rxStream = true;

   return 0;
 }


int rf_faux_stop_rx_stream(void *h)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("");

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
   FAUX_DEBUG("channels %u, args [%s]", nof_channels, args);

   if(strncmp(args, "enb", FAUX_RF_DEV_NAMELEN) == 0)
    {
      strncpy(_info.devName, "faux_enb", strlen("faux_enb"));
    }
   else if(strncmp(args, "ue", FAUX_RF_DEV_NAMELEN) == 0)
    {
      strncpy(_info.devName, "faux_ue", strlen("faux_eu"));
    }
   else
    {
      FAUX_DEBUG("expected arg [ue or enb]", args);
   
      return -1;
    }
      
   if(nof_channels == 1)
    {
      *h = &_info;

      return 0;
    }
   else
    {
      FAUX_DEBUG("only supporting 1 channel, not %d", nof_channels);

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

   FAUX_DEBUG("rate %lf to %lf", p->clockRate, rate);

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

   FAUX_DEBUG("rate %lf to %lf", p->rxRate, rate);

   p->rxRate = rate;

   return p->rxRate;
 }


double rf_faux_set_rx_gain(void *h, double gain)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("gain %lf to %lf", p->rxGain, gain);

   p->rxGain = gain;

   return p->rxGain;
 }


double rf_faux_set_tx_gain(void *h, double gain)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("gain %lf to %lf", p->txGain, gain);

   p->txGain = gain;

   return p->txGain;
 }


double rf_faux_get_rx_gain(void *h)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("gain %lf", p->rxGain);

   return p->rxGain;
 }


double rf_faux_get_tx_gain(void *h)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("gain %lf", p->txGain);

   return p->txGain;
 }


double rf_faux_set_rx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("freq %lf to %lf", p->rxFreq, freq);

   p->rxFreq = freq;

   return p->rxFreq;
 }


double rf_faux_set_tx_srate(void *h, double rate)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("rate %lf to %lf", p->txRate, rate);

   p->txRate = rate;

   return p->txRate;
 }


double rf_faux_set_tx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h)

   FAUX_DEBUG("freq %lf to %lf", p->txFreq, freq);

   p->txFreq = freq;

   return p->txFreq;
 }


void rf_faux_get_time(void *h, time_t *secs, double *frac_secs)
 {
   struct timeval tv;

   gettimeofday(&tv, NULL);

   TV_TO_TS(&tv, secs, frac_secs);
 }


int rf_faux_recv_with_time(void *h, void *data, uint32_t nsamples, 
                           bool blocking, time_t *secs, double *frac_secs)
 {
   FAUX_DEBUG("nsamples %u, blocking %s", 
              nsamples, 
              blocking ? "yes" : "no");

   usleep(USEC_X_SEC/2);
  
   rf_faux_get_time(h, secs, frac_secs);

   return 0;
 }


int rf_faux_recv_with_time_multi(void *h, void **data, uint32_t nsamples, 
                                 bool blocking, time_t *secs, double *frac_secs)
{
   return rf_faux_recv_with_time(h, 
                                 data[0],
                                 nsamples, 
                                 blocking,
                                 secs,
                                 frac_secs);
}


int rf_faux_send_timed(void *h, void *data, int nsamples,
                       time_t secs, double frac_secs, bool has_time_spec,
                       bool blocking, bool is_start_of_burst, bool is_end_of_burst)
{
   struct timeval tv_dif;

   TS_DIFF(secs, frac_secs, &tv_dif);

   FAUX_DEBUG("nsamples %u, blocking %s, offset %ld:%06ld, sob %s, eob %s", 
              nsamples, 
              BOOL_TO_STR(blocking),
              tv_dif.tv_sec,
              tv_dif.tv_usec,
              BOOL_TO_STR(is_start_of_burst),
              BOOL_TO_STR(is_end_of_burst));

   usleep((tv_dif.tv_sec * 1e6) + tv_dif.tv_usec);

   return nsamples;
}


int rf_faux_send_timed_multi(void *h, void *data[4], int nsamples,
                             time_t secs, double frac_secs, bool has_time_spec,
                             bool blocking, bool is_start_of_burst, bool is_end_of_burst)
{
  return rf_faux_send_timed(h, 
                            data[0], 
                            nsamples,
                            secs,
                            frac_secs,
                            has_time_spec,
                            blocking,
                            is_start_of_burst,
                            is_end_of_burst);
}


void rf_faux_set_tx_cal(void *h, srslte_rf_cal_t *cal)
{
   FAUX_DEBUG("gain %f, phase %f, i %f, q %f", 
              cal->dc_gain, cal->dc_phase, cal->iq_i, cal->iq_q);
}


void rf_faux_set_rx_cal(void *h, srslte_rf_cal_t *cal)
{
   FAUX_DEBUG("gain %f, phase %f, i %f, q %f", 
           cal->dc_gain, cal->dc_phase, cal->iq_i, cal->iq_q);

}
