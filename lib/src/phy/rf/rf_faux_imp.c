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

#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>

#include <zmq.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "srslte/srslte.h"
#include "rf_faux_imp.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/resampling/resample_arb.h"

static bool _rf_faux_logStdout = true;

#define RF_FAUX_DEBUG(fmt, ...) do {                                                               \
                                 if(_rf_faux_logStdout) {                                          \
                                   struct timeval _tv_now;                                         \
                                   struct tm _tm;                                                  \
                                   gettimeofday(&_tv_now, NULL);                                   \
                                   localtime_r(&_tv_now.tv_sec, &_tm);                             \
                                   fprintf(stdout, "[DEBUG]: %02d.%02d.%02d.%06ld %s, " fmt "\n",  \
                                           _tm.tm_hour,                                            \
                                           _tm.tm_min,                                             \
                                           _tm.tm_sec,                                             \
                                           _tv_now.tv_usec,                                        \
                                           __func__,                                               \
                                           ##__VA_ARGS__);                                         \
                                 }                                                                 \
                             } while(0);

#define RF_FAUX_LOG_FUNC_TODO printf("XXX_TODO file:%s func:%s line:%d\n", \
                                     __FILE__,                             \
                                     __func__,                             \
                                     __LINE__);

#define RF_FAUX_BOOL_TO_STR(x) (x) ? "yes" : "no"

#define RF_FAUX_DL_PORT 43001
#define RF_FAUX_UL_PORT 43002
#define RF_FAUX_EP_LEN 128

#define BYTES_X_SAMPLE(x) ((x) * sizeof(cf_t))
#define SAMPLES_X_BYTE(x) ((x) / sizeof(cf_t))

static const char * RF_FAUX_DL_TOPIC        = "LTE.DOWNLINK";
#define RF_FAUX_DL_TOPIC_LEN (strlen(RF_FAUX_DL_TOPIC))

static const char * RF_FAUX_UL_TOPIC        = "LTE.UPLINK";
#define RF_FAUX_UL_TOPIC_LEN (strlen(RF_FAUX_UL_TOPIC))


#define RF_FAUX_DFL_SRATE (5760000.0)

#define RF_FAUX_SF_LEN 0xFFFF

#define RF_FAUX_NORM_SF_LEN(x) (((x) = ((x) / 10) * 10))


static void _rf_faux_tv_to_ts(struct timeval *tv, time_t *secs, double *frac)
{
  if(secs && frac)
    {
      *secs = tv->tv_sec; 
      *frac = tv->tv_usec / 1.0e6;
    }
}

static void _rf_faux_ts_to_tv(struct timeval *tv, time_t secs, double frac)
{
  if(tv)
    {
      tv->tv_sec  = secs;
      tv->tv_usec = frac * 1.0e6;
    }
}

static void _rf_faux_dif_time(time_t secs, double frac, struct timeval * tv_dif)
{
   struct timeval tv_now, tv_nxt;

   gettimeofday(&tv_now, NULL);

   _rf_faux_ts_to_tv(&tv_nxt, secs, frac);

   if(secs || frac)
    {
      timersub(&tv_nxt, &tv_now, tv_dif);
    }
   else
    {
      tv_dif->tv_sec  = 0;
      tv_dif->tv_usec = 0;
    }
}


#define RF_FAUX_TYPE_NONE   0
#define RF_FAUX_TYPE_UE     1
#define RF_FAUX_TYPE_ENB    2

typedef struct 
 {
   char             *dev_name;
   double           rx_gain;
   double           tx_gain;
   double           rx_srate;
   double           tx_srate;
   double           rx_freq;
   double           tx_freq;
   srslte_rf_cal_t  tx_cal;
   srslte_rf_cal_t  rx_cal;
   double           clock_rate;
   void (*error_handler)(srslte_rf_error_t error);
   bool             rx_stream;
   int              type;
   void *           zmqctx;
   void *           tx_handle;
   void *           rx_handle;
   int64_t          txseq;
   bool             rlock;
   pthread_mutex_t  rmtex;
   pthread_mutex_t  wmtex;
 } rf_faux_info_t;


typedef struct rf_faux_msg {
 void * msg_base;
 int    msg_len;
 int    msg_rc;
} rf_faux_msg_t;


typedef struct rf_faux_hdr {
  uint64_t seqnum;
  double    srate;
} rf_faux_hdr_t;


static bool _rf_faux_is_enb(rf_faux_info_t * info)
{
  return (info->type == RF_FAUX_TYPE_ENB);
}


static bool _rf_faux_is_ue(rf_faux_info_t * info)
{
  return (info->type == RF_FAUX_TYPE_UE);
}


static const char * _rf_faux_make_zmq_endpoint(char buff[RF_FAUX_EP_LEN], 
                                               const char * type,
                                               const char * addr,
                                               uint16_t port)
{
  snprintf(buff, RF_FAUX_EP_LEN, "%s://%s:%hu", type, addr, port); 

  return buff;
}


static int _rf_faux_vecio_send(rf_faux_msg_t * iov, 
                               int n, 
                               const rf_faux_info_t * info, 
                               int flags)
{
   int sum = 0;

   for(int i = 0; i < n; ++i)
     {
        iov[i].msg_rc = zmq_send(info->tx_handle, 
                                 iov[i].msg_base, 
                                 iov[i].msg_len, i < (n - 1) ? flags | ZMQ_SNDMORE : flags);

        RF_FAUX_DEBUG("req %d, send %d", iov[i].msg_len, iov[i].msg_rc);

        if(iov[i].msg_rc > 0)
          {
            sum += iov[i].msg_rc;
          }
        else
          {
            RF_FAUX_DEBUG("send error %s", strerror(errno));

            break;
          }
     }

  return sum;
}


static int _rf_faux_vecio_recv(rf_faux_msg_t * iov, 
                               int n, 
                               const rf_faux_info_t * info, 
                               int flags)
{
   int sum = 0;

   for(int i = 0; i < n; ++i)
     {
        // XXX TODO RCVMORE
        iov[i].msg_rc = zmq_recv(info->rx_handle, 
                                 iov[i].msg_base, 
                                 iov[i].msg_len, 
                                 flags);

        RF_FAUX_DEBUG("req %d, recv %d", iov[i].msg_len, iov[i].msg_rc);

        if(iov[i].msg_rc > 0)
          {
            sum += iov[i].msg_rc;
          }
        else
          {
            if(errno != EAGAIN)
              {
                RF_FAUX_DEBUG("recv error %s", strerror(errno));
              }
            break;
          }
     }

   return sum;
}


static int _rf_faux_open_ipc_pub_sub(rf_faux_info_t * info, 
                                     uint16_t subPort, 
                                     uint16_t pubPort,
                                     const char * topic)
{
  char ep[RF_FAUX_EP_LEN] = {0};

  if((info->rx_handle = zmq_socket(info->zmqctx, ZMQ_SUB)) == NULL)
    {
      RF_FAUX_DEBUG("SUB sock open error %s", strerror(errno));

      return -1;
    }

  _rf_faux_make_zmq_endpoint(ep, "tcp", "127.0.0.1", subPort);

  RF_FAUX_DEBUG("SUB connect to publisher on %s", ep);

  if(zmq_connect(info->rx_handle, ep) < 0)
    {
      RF_FAUX_DEBUG("SUB connect error %s", strerror(errno));
 
      return -1;
    }

  if(zmq_setsockopt(info->rx_handle, ZMQ_SUBSCRIBE, topic, strlen(topic)) < 0)
    {
      RF_FAUX_DEBUG("SUB subscribing to topic error %s %s", topic, strerror(errno));
  
      return -1;
    }
  else
    {
      RF_FAUX_DEBUG("SUB subscribing to topic %s", topic);
    }


  if((info->tx_handle = zmq_socket(info->zmqctx, ZMQ_PUB)) == NULL)
    {
      RF_FAUX_DEBUG("PUN open sock error %s", strerror(errno));

      return -1;
    }

  _rf_faux_make_zmq_endpoint(ep, "tcp", "127.0.0.1", pubPort);

  RF_FAUX_DEBUG("PUB listen for subscribers on %s", ep);

  if(zmq_bind(info->tx_handle, ep) < 0)
   {
     RF_FAUX_DEBUG("PUB bind error %s", strerror(errno));
 
     return -1;
   }

  // timeout 1 msec (1 sf)
  const int timeout = 1;

  if(zmq_setsockopt(info->rx_handle, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
      RF_FAUX_DEBUG("SUB set rcv timoeout %d error %s", timeout, strerror(errno));

      return -1;
    }

  RF_FAUX_DEBUG("SUB set rcv timoeout to %d msec", timeout);
 
  return 0;
}


static int _rf_faux_open_ipc_zmq(rf_faux_info_t * info)
{
  info->zmqctx = zmq_ctx_new();

  if(info != NULL)
    {
      if(_rf_faux_is_enb(info))
        {
          return _rf_faux_open_ipc_pub_sub(info, 
                                           RF_FAUX_DL_PORT,
                                           RF_FAUX_UL_PORT,
                                           RF_FAUX_UL_TOPIC);
        }
      else
        {
          return _rf_faux_open_ipc_pub_sub(info, 
                                           RF_FAUX_UL_PORT,
                                           RF_FAUX_DL_PORT,
                                           RF_FAUX_DL_TOPIC);
        }
    }
  else
    {
      RF_FAUX_DEBUG("zmq context error %s", strerror(errno));
 
      return -1;
    }
}


static int _rf_faux_resample(double srate_in, 
                             double srate_out, 
                             void * in, 
                             void * out, 
                             int ns_in)
{
  const double sratio = srate_out / srate_in;
 
  int ns_out = ns_in;
  
   // resample
  if(sratio != 1.0)
   {
     srslte_resample_arb_t r;

     srslte_resample_arb_init(&r, sratio);

     ns_out = srslte_resample_arb_compute(&r, (cf_t*)in, (cf_t*)out, ns_in);
   }
  else
   {
     memcpy(out, in, BYTES_X_SAMPLE(ns_out));
   }

  RF_FAUX_DEBUG("srate %5.4lf/%5.4lf, sratio %5.4lf, ns_in %d, ns_out %d",
                srate_in,
                srate_out,
                sratio, 
                ns_in, 
                ns_out);

  return ns_out;
}



static void rf_faux_handle_error(srslte_rf_error_t error)
{
  RF_FAUX_DEBUG("%s:%s type %s, opt %d, msg %s\b", 
                error.type == SRSLTE_RF_ERROR_LATE      ? "late"      :
                error.type == SRSLTE_RF_ERROR_UNDERFLOW ? "underflow" :
                error.type == SRSLTE_RF_ERROR_OVERFLOW  ? "overflow"  :
                error.type == SRSLTE_RF_ERROR_OTHER     ? "other"     :
                "unknown error");
}

static  rf_faux_info_t _rf_faux_info = { .dev_name       = "faux",
                                         .rx_gain        = 0.0,
                                         .tx_gain        = 0.0,
                                         .rx_srate       = RF_FAUX_DFL_SRATE,
                                         .tx_srate       = RF_FAUX_DFL_SRATE,
                                         .rx_freq        = 0.0,
                                         .tx_freq        = 0.0,
                                         .rx_cal         = {0.0, 0.0, 0.0, 0.0},
                                         .tx_cal         = {0.0, 0.0, 0.0, 0.0},
                                         .clock_rate     = 0.0,
                                         .error_handler  = rf_faux_handle_error,
                                         .rx_stream      = false,
                                         .type           = RF_FAUX_TYPE_NONE,
                                         .zmqctx         = NULL,
                                         .tx_handle      = NULL,
                                         .rx_handle      = NULL,
                                         .txseq          = 0,
                                         .rlock          = false,
                                         .rmtex          = PTHREAD_MUTEX_INITIALIZER,
                                         .wmtex          = PTHREAD_MUTEX_INITIALIZER,
                                       };

// could just as well use _rf_faux_info for single antenna mode
#define GET_FAUX_INFO(h)  assert(h); rf_faux_info_t *_info = (rf_faux_info_t *)(h)

char* rf_faux_devname(void *h)
 {
   GET_FAUX_INFO(h);

   return _info->dev_name;
 }


bool rf_faux_rx_wait_lo_locked(void *h)
 {
   RF_FAUX_LOG_FUNC_TODO;

   return false;
 }


int rf_faux_start_rx_stream(void *h)
 {
   GET_FAUX_INFO(h);
   
   RF_FAUX_DEBUG("");

   _info->rx_stream = true;

   return 0;
 }


int rf_faux_stop_rx_stream(void *h)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("");

   _info->rx_stream = false;

   return 0;
 }


void rf_faux_flush_buffer(void *h)
 {
   RF_FAUX_LOG_FUNC_TODO;
 }


bool rf_faux_has_rssi(void *h)
 {
   RF_FAUX_LOG_FUNC_TODO;

   return false;
 }


float rf_faux_get_rssi(void *h)
 {
   RF_FAUX_LOG_FUNC_TODO;

   return 0.0;
 }


void rf_faux_suppress_stdout(void *h)
 {
   // _rf_faux_logStdout = false;
 }


void rf_faux_register_error_handler(void *h, srslte_rf_error_handler_t error_handler)
 {
   GET_FAUX_INFO(h);

   _info->error_handler = error_handler;
 }


int rf_faux_open(char *args, void **h)
 {
   return rf_faux_open_multi(args, h, 1);
 }


int rf_faux_open_multi(char *args, void **h, uint32_t nof_channels)
 {
   RF_FAUX_DEBUG("channels %u, args [%s]", nof_channels, args);

   if(strncmp(args, "enb", strlen("enb")) == 0)
    {
      _rf_faux_info.type = RF_FAUX_TYPE_ENB;
    }
   else if(strncmp(args, "ue", strlen("ue")) == 0)
    {
      _rf_faux_info.type = RF_FAUX_TYPE_UE;
    }
   else
    {
      RF_FAUX_DEBUG("default type is ue");

      _rf_faux_info.type = RF_FAUX_TYPE_UE;
    }
       
   if(nof_channels != 1)
    {
      RF_FAUX_DEBUG("only supporting 1 channel, not %d", nof_channels);

      return -1;
    }

   if(_rf_faux_open_ipc_zmq(&_rf_faux_info) < 0)
     {
       RF_FAUX_DEBUG("could not creat ipc channel");

       return -1;
     }

   *h = &_rf_faux_info;

   return 0;
 }


int rf_faux_close(void *h)
 {
   GET_FAUX_INFO(h);

   zmq_ctx_destroy(_info->zmqctx);

   return 0;
 }


void rf_faux_set_master_clock_rate(void *h, double rate)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("rate %lf to %lf", _info->clock_rate, rate);

   _info->clock_rate = rate;
 }


bool rf_faux_is_master_clock_dynamic(void *h)
 {
   RF_FAUX_LOG_FUNC_TODO;

   return false;
 }


double rf_faux_set_rx_srate(void *h, double rate)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("rate %lf to %lf", _info->rx_srate, rate);

   _info->rx_srate = rate;

   return _info->rx_srate;
 }


double rf_faux_set_rx_gain(void *h, double gain)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("gain %5.4lf to %5.4lf", _info->rx_gain, gain);

   _info->rx_gain = gain;

   return _info->rx_gain;
 }


double rf_faux_set_tx_gain(void *h, double gain)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("gain %5.4lf to %5.4lf", _info->tx_gain, gain);

   _info->tx_gain = gain;

   return _info->tx_gain;
 }


double rf_faux_get_rx_gain(void *h)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("gain %5.4lf", _info->rx_gain);

   return _info->rx_gain;
 }


double rf_faux_get_tx_gain(void *h)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("gain %5.4lf", _info->tx_gain);

   return _info->tx_gain;
 }


double rf_faux_set_rx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("freq %5.4lf to %5.4lf", _info->rx_freq, freq);

   _info->rx_freq = freq;

   return _info->rx_freq;
 }


double rf_faux_set_tx_srate(void *h, double rate)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("rate %5.4lf to %5.4lf", _info->tx_srate, rate);

   _info->tx_srate = rate;

   return _info->tx_srate;
 }


double rf_faux_set_tx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("freq %5.4lf to %5.4lf", _info->tx_freq, freq);

   _info->tx_freq = freq;

   return _info->tx_freq;
 }


void rf_faux_get_time(void *h, time_t *secs, double *frac_secs)
 {
   struct timeval tv;

   gettimeofday(&tv, NULL);

   _rf_faux_tv_to_ts(&tv, secs, frac_secs);
 }


int rf_faux_recv_with_time(void *h, void *data, uint32_t ns_req, 
                           bool blocking, time_t *secs, double *frac_secs)
 {
   GET_FAUX_INFO(h);

   pthread_mutex_lock(&(_info->rmtex));

   RF_FAUX_NORM_SF_LEN(ns_req);

   const int nb_req = BYTES_X_SAMPLE(ns_req);

   int nb_pending = nb_req;

   int ns_pending = ns_req;

   char topic[RF_FAUX_DL_TOPIC_LEN + 1];

   const int flags = !blocking ? ZMQ_NOBLOCK : 0;

   rf_faux_hdr_t hdr;

   int n_tries = _rf_faux_is_ue(_info) ? 10 : 1;

   uint8_t * p = (uint8_t *) data;

   RF_FAUX_DEBUG("begin_rx req %u/%d, blocking %s",
                  ns_req,
                  nb_req,
                  RF_FAUX_BOOL_TO_STR(blocking));

   while(nb_pending > 0 && n_tries--)
     {   
       // crude for now to read 1 sf
       const int nb_sf = BYTES_X_SAMPLE(RF_FAUX_DFL_SRATE / 1000);

       cf_t sf_in [RF_FAUX_SF_LEN] = {0.0, 0.0};

       memset(topic, 0x0, sizeof(topic));

       const int topic_len = _rf_faux_is_ue(_info) ?
                             RF_FAUX_DL_TOPIC_LEN  :
                             RF_FAUX_UL_TOPIC_LEN;

       rf_faux_msg_t iov[3] = {{(void*)topic, topic_len,   0}, 
                               {(void*)&hdr,  sizeof(hdr), 0},
                               {(void*)sf_in, nb_sf,       0}};

       const int rc = _rf_faux_vecio_recv(iov, 3, _info, flags);

       const int nb_recv = iov[2].msg_rc;

       if(rc <= 0)
        {
          if(errno != EAGAIN)
            {
              RF_FAUX_DEBUG("recv error %s", strerror(errno));

              goto rxout;
            }
        }
      else
       {
         if(nb_recv > 0)
           {
            const int ns_in  =  SAMPLES_X_BYTE(nb_recv);

            const int ns_out = _rf_faux_resample(hdr.srate,
                                                 _info->rx_srate,
                                                 sf_in,
                                                 p, 
                                                 ns_in);

            const int nb_out = BYTES_X_SAMPLE(ns_out);

            p += nb_out;

            nb_pending -= nb_out;

            ns_pending -= ns_out;

            RF_FAUX_DEBUG("recv %d/%d, seqnum %ld, srate %5.4lf, added %d/%d, pending %d/%d, %d more tries",
                          ns_in, 
                          nb_recv, 
                          hdr.seqnum,
                          hdr.srate,
                          ns_out,
                          nb_out,
                          ns_pending,
                          nb_pending,
                          n_tries);
          }
       }
    }

rxout:
   RF_FAUX_DEBUG("nb_req %d, nb_pending %d", nb_req, nb_pending);
 
   rf_faux_get_time(h, secs, frac_secs);

   pthread_mutex_unlock(&(_info->rmtex));

   return ns_req;
 }


int rf_faux_recv_with_time_multi(void *h, void **data, uint32_t ns_req, 
                                 bool blocking, time_t *secs, double *frac_secs)
{
   return rf_faux_recv_with_time(h, 
                                 data[0],
                                 ns_req, 
                                 blocking,
                                 secs,
                                 frac_secs);
}


int rf_faux_send_timed(void *h, void *data, int ns_in,
                       time_t secs, double frac_secs, bool has_time_spec,
                       bool blocking, bool is_start_of_burst, bool is_end_of_burst)
{
   GET_FAUX_INFO(h);

   pthread_mutex_lock(&(_info->wmtex));

   struct timeval tv_dif;

   _rf_faux_dif_time(secs, frac_secs, &tv_dif);

   const int flags = !blocking ? ZMQ_NOBLOCK : 0;

   cf_t sf_out[RF_FAUX_SF_LEN] = {0.0, 0.0};

   const int nb_in  = BYTES_X_SAMPLE(ns_in);

   const int ns_out = _rf_faux_resample(_info->tx_srate,
                                        RF_FAUX_DFL_SRATE,
                                        data,
                                        sf_out, 
                                        ns_in);

   const int nb_out = BYTES_X_SAMPLE(ns_out);

   RF_FAUX_DEBUG("in %u/%d, offset %ld:%06ld, seqnum %ld, sob %s, eob %s, srate %5.4lf, blocking %s, out %d/%d", 
                 ns_in,
                 nb_in,
                 tv_dif.tv_sec,
                 tv_dif.tv_usec,
                 _info->txseq,
                 RF_FAUX_BOOL_TO_STR(is_start_of_burst),
                 RF_FAUX_BOOL_TO_STR(is_end_of_burst),
                 RF_FAUX_DFL_SRATE,
                 RF_FAUX_BOOL_TO_STR(blocking),
                 ns_out,
                 nb_out);

   rf_faux_hdr_t hdr = {_info->txseq, RF_FAUX_DFL_SRATE};


   const char * topic  = _rf_faux_is_ue(_info) ?
                         RF_FAUX_UL_TOPIC  :
                         RF_FAUX_DL_TOPIC;

   rf_faux_msg_t iov[3] = {{(void*)topic,  strlen(topic), 0}, 
                           {(void*)&(hdr), sizeof(hdr),   0},
                           {(void*)data,   nb_out,        0}};

   const int rc = _rf_faux_vecio_send(iov, 3, _info, flags);

   const int nb_sent = iov[2].msg_rc;

   if(rc <= 0)
     {
       RF_FAUX_DEBUG("send error %s", strerror(errno));

       goto txout;
     }
   else
     {
       ++_info->txseq;
     }

txout:

   RF_FAUX_DEBUG("sent %d bytes of %d", nb_sent, nb_out);

   pthread_mutex_unlock(&(_info->wmtex));

   return ns_in;
}


int rf_faux_send_timed_multi(void *h, void *data[4], int ns_in,
                             time_t secs, double frac_secs, bool has_time_spec,
                             bool blocking, bool is_start_of_burst, bool is_end_of_burst)
{
  return rf_faux_send_timed(h, 
                            data[0], 
                            ns_in,
                            secs,
                            frac_secs,
                            has_time_spec,
                            blocking,
                            is_start_of_burst,
                            is_end_of_burst);
}


void rf_faux_set_tx_cal(void *h, srslte_rf_cal_t *cal)
{
   GET_FAUX_INFO(h);

   memcpy(&_info->tx_cal, cal, sizeof(srslte_rf_cal_t));

   RF_FAUX_DEBUG("gain %5.4lf, phase %5.4lf, I %5.4lf, Q %5.4lf", 
                 cal->dc_gain, 
                 cal->dc_phase, 
                 cal->iq_i,
                 cal->iq_q);
}


void rf_faux_set_rx_cal(void *h, srslte_rf_cal_t *cal)
{
   GET_FAUX_INFO(h);

   memcpy(&_info->rx_cal, cal, sizeof(srslte_rf_cal_t));

   RF_FAUX_DEBUG("gain %5.4lf, phase %5.4lf, I %5.4lf, Q %5.4lf", 
                 cal->dc_gain,
                 cal->dc_phase,
                 cal->iq_i,
                 cal->iq_q);
}
