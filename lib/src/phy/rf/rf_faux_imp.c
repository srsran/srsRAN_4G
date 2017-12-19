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
#include <semaphore.h>
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

// socket port nums
#define RF_FAUX_DL_PORT 43001
#define RF_FAUX_UL_PORT 43002
#define RF_FAUX_EP_LEN  128

// zmq pub/sub info
#define RF_FAUX_DL_TOPIC      "RF_FAUX.DOWNLINK"
#define RF_FAUX_DL_TOPIC_LEN  (strlen(RF_FAUX_DL_TOPIC))

#define RF_FAUX_UL_TOPIC      "RF_FAUX.UPLINK"
#define RF_FAUX_UL_TOPIC_LEN  (strlen(RF_FAUX_UL_TOPIC))

// bytes per sample
#define BYTES_X_SAMPLE(x) ((x) * sizeof(cf_t))

// samples per byte
#define SAMPLES_X_BYTE(x) ((x) / sizeof(cf_t))

// target OTA SR
#define RF_FAUX_OTA_SRATE (5760000.0)

// max sf len
#define RF_FAUX_SF_LEN 0x4000

// normalize sf req len for pkt i/o
#define RF_FAUX_NORM_SF_LEN(x) (((x) = ((x) / 10) * 10))

// node type
#define RF_FAUX_NTYPE_NONE   0
#define RF_FAUX_NTYPE_UE     1
#define RF_FAUX_NTYPE_ENB    2

// tx offset (delay) workers
#define RF_FAUX_NOF_TX_WORKERS 10
#define RF_FAUX_SET_NEXT_WORKER(x) ((x) = ((x) + 1) % RF_FAUX_NOF_TX_WORKERS)

typedef struct {
  void * h;
  cf_t   data[RF_FAUX_SF_LEN];
  int    nsamples;
  struct timeval tx_time;
  int    flags;
  bool   is_sob;
  bool   is_eob;
} _rf_faux_tx_info_t;


typedef struct {
  void *               h;
  pthread_t            tid;
  sem_t                sem;
  int                  id;
  _rf_faux_tx_info_t * tx_info;
}_rf_faux_tx_worker_t;


typedef struct {
   char *               dev_name;
   double               rx_gain;
   double               tx_gain;
   double               rx_srate;
   double               tx_srate;
   double               rx_freq;
   double               tx_freq;
   srslte_rf_cal_t      tx_cal;
   srslte_rf_cal_t      rx_cal;
   double               clock_rate;
   void (*error_handler)(srslte_rf_error_t error);
   bool                 rx_stream;
   int                  ntype;
   void *               zmqctx;
   void *               tx_handle;
   void *               rx_handle;
   int64_t              tx_seq;
   int64_t              rx_seq;
   bool                 in_rx;
   pthread_mutex_t      rx_mutex;
   pthread_mutex_t      tx_mutex;
   pthread_t            tx_tid;
   _rf_faux_tx_worker_t tx_workers[RF_FAUX_NOF_TX_WORKERS];
   int                  tx_worker_next;
   int                  nof_tx_workers;
} _rf_faux_info_t;


typedef struct {
 void * msg_base;
 int    msg_len;
 int    msg_rc;
} _rf_faux_msg_t;


typedef struct {
  uint64_t       seqnum;
  double         srate;
  struct timeval tx_time;
} _rf_faux_hdr_t;



static void _rf_faux_handle_error(srslte_rf_error_t error)
{
  RF_FAUX_DEBUG("%s:%s type %s, opt %d, msg %s\b", 
                error.type == SRSLTE_RF_ERROR_LATE      ? "late"      :
                error.type == SRSLTE_RF_ERROR_UNDERFLOW ? "underflow" :
                error.type == SRSLTE_RF_ERROR_OVERFLOW  ? "overflow"  :
                error.type == SRSLTE_RF_ERROR_OTHER     ? "other"     :
                "unknown error");
}


static  _rf_faux_info_t _rf_faux_info = { .dev_name      = "faux",
                                          .rx_gain        = 0.0,
                                          .tx_gain        = 0.0,
                                          .rx_srate       = RF_FAUX_OTA_SRATE,
                                          .tx_srate       = RF_FAUX_OTA_SRATE,
                                          .rx_freq        = 0.0,
                                          .tx_freq        = 0.0,
                                          .rx_cal         = {0.0, 0.0, 0.0, 0.0},
                                          .tx_cal         = {0.0, 0.0, 0.0, 0.0},
                                          .clock_rate     = 0.0,
                                          .error_handler  = _rf_faux_handle_error,
                                          .rx_stream      = false,
                                          .ntype          = RF_FAUX_NTYPE_NONE,
                                          .zmqctx         = NULL,
                                          .tx_handle      = NULL,
                                          .rx_handle      = NULL,
                                          .tx_seq         = 1,
                                          .rx_seq         = 0,
                                          .in_rx          = false,
                                          .rx_mutex       = PTHREAD_MUTEX_INITIALIZER,
                                          .tx_mutex       = PTHREAD_MUTEX_INITIALIZER,
                                          .tx_worker_next = 0,
                                          .nof_tx_workers = 0,
                                       };

// could just as well use _rf_faux_info for single antenna mode
#define GET_FAUX_INFO(h)  assert(h); _rf_faux_info_t *_info = (_rf_faux_info_t *)(h)


static void _rf_faux_tv_to_ts(struct timeval *tv, time_t *full_secs, double *frac_secs)
{
  if(full_secs && frac_secs)
    {
      *full_secs = tv->tv_sec; 
      *frac_secs = tv->tv_usec / 1.0e6;
    }
}


static void _rf_faux_ts_to_tv(struct timeval *tv, time_t full_secs, double frac_secs)
{
  if(tv)
    {
      tv->tv_sec  = full_secs;
      tv->tv_usec = frac_secs * 1.0e6;
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

  RF_FAUX_DEBUG("srate %4.2lf/%4.2lf MHz, sratio %3.2lf, ns_in %d, ns_out %d",
                srate_in  / 1e6,
                srate_out / 1e6,
                sratio, 
                ns_in, 
                ns_out);

  return ns_out;
}



static bool _rf_faux_is_enb(_rf_faux_info_t * info)
{
  return (info->ntype == RF_FAUX_NTYPE_ENB);
}



static bool _rf_faux_is_ue(_rf_faux_info_t * info)
{
  return (info->ntype == RF_FAUX_NTYPE_UE);
}



static int _rf_faux_vecio_recv(_rf_faux_msg_t * iov, 
                               int n, 
                               const _rf_faux_info_t * info, 
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


static int _rf_faux_vecio_send(_rf_faux_msg_t * iov, 
                               int n, 
                               const _rf_faux_info_t * info, 
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



void _rf_faux_tx_send(_rf_faux_tx_info_t * tx_info)
{
   GET_FAUX_INFO(tx_info->h);

   cf_t sf_out[RF_FAUX_SF_LEN] = {0.0, 0.0};

   const int nb_in = BYTES_X_SAMPLE(tx_info->nsamples);

   // resample to match the ota sr if needed
   const int ns_out = _rf_faux_resample(_info->tx_srate,
                                        RF_FAUX_OTA_SRATE,
                                        tx_info->data,
                                        sf_out, 
                                        tx_info->nsamples);

   const int nb_out = BYTES_X_SAMPLE(ns_out);

   RF_FAUX_DEBUG("in %u/%d, seqnum %ld, sob %s, eob %s, srate %5.2lf MHz, out %d/%d", 
                 tx_info->nsamples,
                 nb_in,
                 _info->tx_seq,
                 RF_FAUX_BOOL_TO_STR(tx_info->is_sob),
                 RF_FAUX_BOOL_TO_STR(tx_info->is_eob),
                 RF_FAUX_OTA_SRATE / 1e6,
                 ns_out,
                 nb_out);

   _rf_faux_hdr_t hdr = {_info->tx_seq, 
                         RF_FAUX_OTA_SRATE, 
                         tx_info->tx_time};

   const char * topic  = _rf_faux_is_ue(_info) ?
                         RF_FAUX_UL_TOPIC  :
                         RF_FAUX_DL_TOPIC;

   _rf_faux_msg_t iov[3] = {{(void*)topic,  strlen(topic), 0}, 
                           {(void*)&(hdr),  sizeof(hdr),   0},
                           {(void*)sf_out,  nb_out,        0}};

   int nb_sent = 0;

   if(nb_out > 0)
    {
      const int rc = _rf_faux_vecio_send(iov, 3, _info, tx_info->flags);

      nb_sent = iov[2].msg_rc;

      if(rc <= 0)
        {
          RF_FAUX_DEBUG("send error %s", strerror(errno));

          goto txout;
        }
      else
        {
          ++_info->tx_seq;
        }
    }

txout:
   RF_FAUX_DEBUG("sent %d bytes of %d", nb_sent, nb_out);
}





static void * _rf_faux_tx_worker(void * arg)
{
   _rf_faux_tx_worker_t * worker = (_rf_faux_tx_worker_t*) arg;

   GET_FAUX_INFO(worker->h);

   RF_FAUX_DEBUG("worker %d created, ready for duty", worker->id);

   struct timeval tv_now, tv_delay;

   while(1)
     {
       sem_wait(&(worker->sem));

       gettimeofday(&tv_now, NULL);

       pthread_mutex_lock(&(_info->tx_mutex));

       timersub(&(worker->tx_info->tx_time), &tv_now, &tv_delay);

       RF_FAUX_DEBUG("worker %d, tx_delay %ld:%06ld", 
                      worker->id, tv_delay.tv_sec, tv_delay.tv_usec);

       // account for ipc and scheduling delays
       if(tv_delay.tv_usec > 200)
         {
           tv_delay.tv_usec -= 200;
         }

       if(tv_delay.tv_usec > 200)
         {
           select(0, NULL, NULL, NULL, &tv_delay);
         }

       RF_FAUX_DEBUG("worker %d, fire", worker->id);

       _rf_faux_tx_send(worker->tx_info);

       _info->nof_tx_workers -= 1;

       free(worker->tx_info);

       worker->tx_info = NULL;

       pthread_mutex_unlock(&(_info->tx_mutex));
    }

   return NULL;
}



static const char * _rf_faux_make_zmq_endpoint(char buff[RF_FAUX_EP_LEN], 
                                               const char * type,
                                               const char * addr,
                                               uint16_t port)
{
  snprintf(buff, RF_FAUX_EP_LEN, "%s://%s:%hu", type, addr, port); 

  return buff;
}



static int _rf_faux_open_ipc_pub_sub(_rf_faux_info_t * info, 
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

  // timeout block ue, 1 msec (1 sf) for enb
  const int timeout = _rf_faux_is_ue(info) ? -1 : 1;

  if(zmq_setsockopt(info->rx_handle, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
      RF_FAUX_DEBUG("SUB set rcv timoeout %d error %s", timeout, strerror(errno));

      return -1;
    }

  RF_FAUX_DEBUG("SUB set rcv timoeout to %d msec", timeout);
 
  return 0;
}


static int _rf_faux_open_ipc_zmq(_rf_faux_info_t * info)
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
   
   pthread_mutex_lock(&(_info->rx_mutex));

   RF_FAUX_DEBUG("");

   _info->rx_stream = true;

   pthread_mutex_unlock(&(_info->rx_mutex));

   return 0;
 }


int rf_faux_stop_rx_stream(void *h)
 {
   GET_FAUX_INFO(h);

   pthread_mutex_lock(&(_info->rx_mutex));

   RF_FAUX_DEBUG("");

   _info->rx_stream = false;

   pthread_mutex_unlock(&(_info->rx_mutex));

   return 0;
 }


void rf_faux_flush_buffer(void *h)
 {
   RF_FAUX_LOG_FUNC_TODO;
 }


bool rf_faux_has_rssi(void *h)
 {
   RF_FAUX_DEBUG("yes");

   return true;
 }


float rf_faux_get_rssi(void *h)
 {
   const float rssi = -60.0;

   RF_FAUX_DEBUG("rssi %4.3f", rssi);

   return rssi;
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
      _rf_faux_info.ntype = RF_FAUX_NTYPE_ENB;
    }
   else if(strncmp(args, "ue", strlen("ue")) == 0)
    {
      _rf_faux_info.ntype = RF_FAUX_NTYPE_UE;
    }
   else
    {
      RF_FAUX_DEBUG("default ntype is ue");

      _rf_faux_info.ntype = RF_FAUX_NTYPE_UE;
    }
       
   if(nof_channels != 1)
    {
      RF_FAUX_DEBUG("only supporting 1 channel, not %d", nof_channels);

      return -1;
    }

   if(_rf_faux_open_ipc_zmq(&_rf_faux_info) < 0)
    {
      RF_FAUX_DEBUG("could not create ipc channel");

      return -1;
    }

   for(int id = 0; id < RF_FAUX_NOF_TX_WORKERS; ++id)
     {
       _rf_faux_tx_worker_t * worker =  &_rf_faux_info.tx_workers[id];

       if(sem_init(&(worker->sem), 0, 0) < 0)
         {
           RF_FAUX_DEBUG("could not initialize tx_worker semaphore %s", strerror(errno));

           return -1;
         }

       worker->tx_info = NULL;
       worker->id      = id;
       worker->h       = &_rf_faux_info;

       if(pthread_create(&(worker->tid), 
                         NULL, 
                         _rf_faux_tx_worker, 
                         worker) < 0)
        {
           RF_FAUX_DEBUG("could not create tx_worker thread %s", strerror(errno));

           return -1;
        }
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

   RF_FAUX_DEBUG("rate %4.2lf Mhz to %4.2lf Mhz", 
                 _info->clock_rate / 1e6, rate / 1e6);

   _info->clock_rate = rate;
 }


bool rf_faux_is_master_clock_dynamic(void *h)
 {
   RF_FAUX_LOG_FUNC_TODO;

   return false;
 }


double rf_faux_set_rx_gain(void *h, double gain)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("gain %3.2lf to %3.2lf", _info->rx_gain, gain);

   _info->rx_gain = gain;

   return _info->rx_gain;
 }


double rf_faux_set_tx_gain(void *h, double gain)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("gain %3.2lf to %3.2lf", _info->tx_gain, gain);

   _info->tx_gain = gain;

   return _info->tx_gain;
 }


double rf_faux_get_rx_gain(void *h)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("gain %3.2lf", _info->rx_gain);

   return _info->rx_gain;
 }


double rf_faux_get_tx_gain(void *h)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("gain %3.2lf", _info->tx_gain);

   return _info->tx_gain;
 }


double rf_faux_set_rx_srate(void *h, double rate)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("rate %4.2lf Mhz to %4.2lf Mhz", 
                 _info->rx_srate / 1e6, rate / 1e6);

   _info->rx_srate = rate;

   return _info->rx_srate;
 }


double rf_faux_set_tx_srate(void *h, double rate)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("freq %4.2lf MHz to %4.2lf MHz", 
                 _info->tx_srate / 1e6, rate / 1e6);

   _info->tx_srate = rate;

   return _info->tx_srate;
 }


double rf_faux_set_rx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("freq %4.2lf MHz to %4.2lf Mhz", 
                 _info->rx_freq / 1e6, freq / 1e6);

   _info->rx_freq = freq;

   return _info->rx_freq;
 }


double rf_faux_set_tx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("freq %4.2lf MHz to %4.4lf MHz", 
                 _info->tx_freq / 1e6, freq / 1e6);

   _info->tx_freq = freq;

   return _info->tx_freq;
 }


void rf_faux_get_time(void *h, time_t *full_secs, double *frac_secs)
 {
   struct timeval tv;

   gettimeofday(&tv, NULL);

   _rf_faux_tv_to_ts(&tv, full_secs, frac_secs);
 }



int rf_faux_recv_with_time(void *h, void *data, uint32_t nsamples, 
                           bool blocking, time_t *full_secs, double *frac_secs)
 {
   GET_FAUX_INFO(h);

   pthread_mutex_lock(&(_info->rx_mutex));

   _info->in_rx = true;

   // sometimes we get a request for a few extra samples (1922 vs 1920) that 
   // throws off our pkt based stream
   RF_FAUX_NORM_SF_LEN(nsamples);

   const int nb_req = BYTES_X_SAMPLE(nsamples);

   int nb_pending = nb_req;

   int ns_pending = nsamples;

   char topic[RF_FAUX_DL_TOPIC_LEN + 1];

   const int flags = blocking ? 0 : ZMQ_NOBLOCK;

   _rf_faux_hdr_t hdr;

   int max_tries = _rf_faux_is_ue(_info) ? 10 : 1;

   int n_tries = 0;

   uint8_t * p = (uint8_t *) data;

   struct timeval tv_now, tv_delay;

   RF_FAUX_DEBUG("begin_rx req %u/%d, blocking %s, streaming %s",
                  nsamples,
                  nb_req,
                  RF_FAUX_BOOL_TO_STR(blocking),
                  RF_FAUX_BOOL_TO_STR(_info->rx_stream));

   memset(data, 0x0, nb_req);

   while(nb_pending > 0 && n_tries++ < max_tries)
     {   
       // crude way to read 1 sf
       const int nb_sf = BYTES_X_SAMPLE(RF_FAUX_OTA_SRATE / 1000);

       cf_t sf_in [RF_FAUX_SF_LEN] = {0.0, 0.0};

       memset(topic, 0x0, sizeof(topic));

       const int topic_len = _rf_faux_is_ue(_info) ?
                             RF_FAUX_DL_TOPIC_LEN  :
                             RF_FAUX_UL_TOPIC_LEN;

       _rf_faux_msg_t iov[3] = {{(void*)topic, topic_len,   0}, 
                               {(void*)&hdr,  sizeof(hdr), 0},
                               {(void*)sf_in, nb_sf,       0}};

       const int rc = _rf_faux_vecio_recv(iov, 3, _info, flags);

       gettimeofday(&tv_now, NULL);

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

            timersub(&tv_now, &(hdr.tx_time), &tv_delay);

            if((_info->rx_seq + 1) !=  hdr.seqnum)
              {
                 RF_FAUX_DEBUG("TODO recv oof pkt, last seqnum %lu, this seqnum %lu", _info->rx_seq, hdr.seqnum);
              }

            _info->rx_seq = hdr.seqnum;

            RF_FAUX_DEBUG("recv %d/%d, txtime :%06ld, rx_delay %ld:%06ld, seqnum %ld, srate %4.2lf MHz, added %d/%d, pending %d/%d, try %d/%d",
                          ns_in, 
                          nb_recv,
                          hdr.tx_time.tv_usec,
                          tv_delay.tv_sec,
                          tv_delay.tv_usec,
                          hdr.seqnum,
                          hdr.srate / 1e6,
                          ns_out,
                          nb_out,
                          ns_pending,
                          nb_pending,
                          n_tries,
                          max_tries);
          }
       }
    }

rxout:
   RF_FAUX_DEBUG("req %d/%d, pending %d/%d, out %d/%d", 
                 nsamples,
                 nb_req, 
                 ns_pending, 
                 nb_pending,
                 nsamples - ns_pending,
                 nb_req - nb_pending);
 
   rf_faux_get_time(h, full_secs, frac_secs);

   _info->in_rx = false;

   pthread_mutex_unlock(&(_info->rx_mutex));

   return nsamples;
 }



int rf_faux_recv_with_time_multi(void *h, void **data, uint32_t nsamples, 
                                 bool blocking, time_t *full_secs, double *frac_secs)
{
   return rf_faux_recv_with_time(h, 
                                 data[0],
                                 nsamples, 
                                 blocking,
                                 full_secs,
                                 frac_secs);
}



int rf_faux_send_timed(void *h, void *data, int nsamples,
                       time_t full_secs, double frac_secs, bool has_time_spec,
                       bool blocking, bool is_sob, bool is_eob)
{
   GET_FAUX_INFO(h);


   pthread_mutex_lock(&(_info->tx_mutex));

   _rf_faux_tx_info_t * e = malloc(sizeof(_rf_faux_tx_info_t));

   if(has_time_spec)
     {
       _rf_faux_ts_to_tv(&(e->tx_time), full_secs, frac_secs);
     }
   else
     {
       gettimeofday(&(e->tx_time), NULL);
     }

   memcpy(e->data, data, BYTES_X_SAMPLE(nsamples));

   e->h        = h;
   e->nsamples = nsamples;
   e->flags    = blocking ? 0 : ZMQ_NOBLOCK;
   e->is_sob   = is_sob;
   e->is_eob   = is_eob;

   // get next worker
   _rf_faux_tx_worker_t * worker = &(_info->tx_workers[_info->tx_worker_next]);

   worker->tx_info = e;

   sem_post(&(worker->sem));

   _info->nof_tx_workers += 1;
 
   RF_FAUX_SET_NEXT_WORKER(_info->tx_worker_next);

   RF_FAUX_DEBUG("set tx worker %d, has time spec %s, offset %ld:%0.6lf, %d workers pending",
                  worker->id,
                  RF_FAUX_BOOL_TO_STR(has_time_spec),
                  full_secs,
                  frac_secs,
                 _info->nof_tx_workers);

   pthread_mutex_unlock(&(_info->tx_mutex));

   return nsamples;
}



int rf_faux_send_timed_multi(void *h, void *data[4], int nsamples,
                             time_t full_secs, double frac_secs, bool has_time_spec,
                             bool blocking, bool is_sob, bool is_eob)
{
  return rf_faux_send_timed(h, 
                            data[0], 
                            nsamples,
                            full_secs,
                            frac_secs,
                            has_time_spec,
                            blocking,
                            is_sob,
                            is_eob);
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
