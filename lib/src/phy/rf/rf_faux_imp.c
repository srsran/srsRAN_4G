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

static bool _faux_rf_logStdout = true;

#define FAUX_RF_DEBUG(fmt, ...) do {                                                               \
                                 if(_faux_rf_logStdout) {                                          \
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

#define FAUX_RF_LOG_FUNC_TODO printf("XXX_TODO file:%s func:%s line:%d\n", __FILE__, __func__, __LINE__)

#define FAUX_RF_BOOL_TO_STR(x) (x) ? "yes" : "no"

#define FAUX_RF_DL_PORT 43001
#define FAUX_RF_UL_PORT 43002

#define BYTES_X_SAMPLE(x) ((x) * sizeof(cf_t))
#define SAMPLE_X_BYTE(x)  ((x) / sizeof(cf_t))

static const size_t FAUX_RX_TOPIC_LEN = 12;
static const char * FAUX_RF_TOPIC     = "LTE.DOWNLINK";

static const size_t FAUX_RF_RESPONSE_LEN = 2;
static const char * FAUX_RF_RESPONSE     = "OK";

#define FAUX_RF_SRATE  (5760000.0)

static void _faux_rf_tv_to_ts(struct timeval *tv, time_t *s, double *f)
{
  if(s && f)
    {
      *s = tv->tv_sec; 
      *f = tv->tv_usec / 1.0e6;
    }
}

static void _faux_rf_ts_to_tv(struct timeval *tv, time_t s, double f)
{
  if(tv)
    {
      tv->tv_sec  = s;
      tv->tv_usec = f * 1.0e6;
    }
}

static void _faux_rf_dif_time(time_t secs, double frac, struct timeval * tv_dif)
{
   struct timeval tv_now, tv_nxt;

   gettimeofday(&tv_now, NULL);

   _faux_rf_ts_to_tv(&tv_nxt, secs, frac);

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


#define FAUX_RF_TYPE_NONE   0
#define FAUX_RF_TYPE_UE     1
#define FAUX_RF_TYPE_ENB    2

typedef struct 
 {
   char  *devName;
   double rxGain;
   double txGain;
   double rxSrate;
   double txSrate;
   double rxFreq;
   double txFreq;
   double txCal;
   double rxCal;
   double clockRate;
   void   (*error_handler)(srslte_rf_error_t error);
   bool   rxStream;
   int    type;
   void * zmqctx;
   void * txSock;
   void * rxSock;
   int64_t txseq;
   bool readlocked;
 } faux_rf_info_t;


static bool _faux_rf_is_enb(faux_rf_info_t * info)
{
  return (info->type == FAUX_RF_TYPE_ENB);
}


static bool _faux_rf_is_ue(faux_rf_info_t * info)
{
  return (info->type == FAUX_RF_TYPE_UE);
}


static const char * make_zmq_endpoint(char buff[64], 
                                      const char * type,
                                      const char * addr,
                                      unsigned short port)
{
  snprintf(buff, 64, "%s://%s:%hu", type, addr, port); 

  return buff;
}

struct faux_rf_msg {
 void * msg_base;
 int    msg_len;
 int    msg_rc;
};


struct faux_rf_hdr {
  uint64_t seqnum;
  double    srate;
};


static int _faux_rf_vecio_send(struct faux_rf_msg * iov, 
                               int n, 
                               const faux_rf_info_t * info, 
                               int flags)
{
   int sum = 0;

   for(int i = 0; i < n; ++i)
     {
        iov[i].msg_rc = zmq_send(info->txSock, 
                                 iov[i].msg_base, 
                                 iov[i].msg_len, i < (n - 1) ? flags | ZMQ_SNDMORE : flags);

        if(iov[i].msg_rc > 0)
          {
            sum += iov[i].msg_rc;
          }
        else
          {
            FAUX_RF_DEBUG("send error %s", strerror(errno));

            break;
          }
     }

  return sum;
}


static int _faux_rf_vecio_recv(struct faux_rf_msg * iov, 
                               int n, 
                               const faux_rf_info_t * info, 
                               int flags)
{
   int sum = 0;

   for(int i = 0; i < n; ++i)
     {
        iov[i].msg_rc = zmq_recv(info->rxSock, 
                             iov[i].msg_base, 
                             iov[i].msg_len, flags);

        if(iov[i].msg_rc > 0)
          {
            sum += iov[i].msg_rc;
          }
        else
          {
            if(errno != EAGAIN)
              {
                FAUX_RF_DEBUG("recv error %s", strerror(errno));
              }

            break;
          }
     }

   return sum;
}




static int _faux_rf_open_ipc_enb(faux_rf_info_t * info)
{
  char ep[64] = {0};

  if((info->txSock = zmq_socket(info->zmqctx, ZMQ_PUB)) == NULL)
    {
      FAUX_RF_DEBUG("error opening PUB sock %s", strerror(errno));

      return -1;
    }

  make_zmq_endpoint(ep, "tcp", "127.0.0.1", FAUX_RF_DL_PORT);

  FAUX_RF_DEBUG("PUB listen for downlink subscribers on %s\n", ep);

  if(zmq_bind(info->txSock, ep) < 0)
   {
     FAUX_RF_DEBUG("error binding PUB sock %s", strerror(errno));
 
     return -1;
   }

  if((info->rxSock = zmq_socket(info->zmqctx, ZMQ_REP)) == NULL)
    {
      FAUX_RF_DEBUG("error opening REP sock %s", strerror(errno));

      return -1;
    }

  make_zmq_endpoint(ep, "tcp", "127.0.0.1", FAUX_RF_UL_PORT);

  FAUX_RF_DEBUG("REP listen to uplink on %s\n", ep);

  if(zmq_bind(info->rxSock, ep) < 0)
    {
      FAUX_RF_DEBUG("error binding REP sock %s", strerror(errno));

      return -1;
    }

  // timeout 1 msec (1 sf)
  const int timeout = 1;

  if(zmq_setsockopt(info->rxSock, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
      FAUX_RF_DEBUG("error set snd timoeout to %d for REP sock %s", timeout, strerror(errno));

      return -1;
    }

  FAUX_RF_DEBUG("set rcv timoeout to %d for REP sock %d", timeout);
 
  return 0;
}

static int _faux_rf_open_ipc_ue(faux_rf_info_t * info)
{
  char ep[64] = {0};

  if((info->rxSock = zmq_socket(info->zmqctx, ZMQ_SUB)) == NULL)
    {
      FAUX_RF_DEBUG("error opening SUB sock %s", strerror(errno));

      return -1;
    }

  make_zmq_endpoint(ep, "tcp", "127.0.0.1", FAUX_RF_DL_PORT);

  FAUX_RF_DEBUG("SUB connect to downlink publisher on %s\n", ep);

  if(zmq_connect(info->rxSock, ep) < 0)
    {
      FAUX_RF_DEBUG("error connect SUB sock %s", strerror(errno));
 
      return -1;
    }

  if(zmq_setsockopt(info->rxSock, ZMQ_SUBSCRIBE, FAUX_RF_TOPIC, FAUX_RX_TOPIC_LEN) < 0)
    {
      FAUX_RF_DEBUG("error subscribing to topic %s SUB sock %s", FAUX_RF_TOPIC, strerror(errno));
  
      return -1;
    }

  if((info->txSock = zmq_socket(info->zmqctx, ZMQ_REQ)) == NULL)
    {
      FAUX_RF_DEBUG("error opening REQ sock %s", strerror(errno));

      return -1;
    }

  make_zmq_endpoint(ep, "tcp", "127.0.0.1", FAUX_RF_UL_PORT);

  FAUX_RF_DEBUG("REQ connect to uplink on %s\n", ep);

  if(zmq_connect(info->txSock, ep) < 0)
    {
      FAUX_RF_DEBUG("error connect REQ sock %s", strerror(errno));

      return -1;
    }

  // timeout 1 msec (1 sf)
  const int timeout = 1;

  if(zmq_setsockopt(info->rxSock, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
      FAUX_RF_DEBUG("error set rcv timoeout to %d for SUB sock %s", timeout, strerror(errno));

      return -1;
    }

  FAUX_RF_DEBUG("set snd/rcv timoeout to %d for REQ/SUB sock %d", timeout);
 
  return 0;
}


static int _faux_rf_open_ipc_zmq(faux_rf_info_t * info)
{
  info->zmqctx = zmq_ctx_new();

  if(info != NULL)
    {
      if(_faux_rf_is_enb(info))
        {
          return _faux_rf_open_ipc_enb(info);
        }
      else
        {
          return _faux_rf_open_ipc_ue(info);
        }
    }
  else
    {
      FAUX_RF_DEBUG("zmq context error %s", strerror(errno));
 
      return -1;
    }
}



static void faux_rf_handle_error(srslte_rf_error_t error)
{
  FAUX_RF_DEBUG("%s:%s type %s, opt %d, msg %s\b", 
                error.type == SRSLTE_RF_ERROR_LATE      ? "late"      :
                error.type == SRSLTE_RF_ERROR_UNDERFLOW ? "underflow" :
                error.type == SRSLTE_RF_ERROR_OVERFLOW  ? "overflow"  :
                error.type == SRSLTE_RF_ERROR_OTHER     ? "other"     :
                "unknown error");
}

static  faux_rf_info_t _faux_rf_info = { .devName       = "faux",
                                         .rxGain        = 0.0,
                                         .txGain        = 0.0,
                                         .rxSrate       = FAUX_RF_SRATE,
                                         .txSrate       = FAUX_RF_SRATE,
                                         .rxFreq        = 0.0,
                                         .txFreq        = 0.0,
                                         .rxCal         = 0.0,
                                         .txCal         = 0.0,
                                         .clockRate     = 0.0,
                                         .error_handler = faux_rf_handle_error,
                                         .rxStream      = false,
                                         .type          = FAUX_RF_TYPE_NONE,
                                         .zmqctx        = NULL,
                                         .txSock        = NULL,
                                         .rxSock        = NULL,
                                         .txseq         = 0,
                                         .readlocked    = false,
                                       };

// could just as well use _faux_rf_info for single antenna mode
#define GET_FAUX_INFO(h)  assert(h); faux_rf_info_t *_info = (faux_rf_info_t *)(h)

char* rf_faux_devname(void *h)
 {
   GET_FAUX_INFO(h);

   return _info->devName;
 }


bool rf_faux_rx_wait_lo_locked(void *h)
 {
   FAUX_RF_LOG_FUNC_TODO;

   return false;
 }


int rf_faux_start_rx_stream(void *h)
 {
   GET_FAUX_INFO(h);
   
   FAUX_RF_DEBUG("");

   _info->rxStream = true;

   return 0;
 }


int rf_faux_stop_rx_stream(void *h)
 {
   GET_FAUX_INFO(h);

   FAUX_RF_DEBUG("");

   _info->rxStream = false;

   return 0;
 }


void rf_faux_flush_buffer(void *h)
 {
   FAUX_RF_LOG_FUNC_TODO;
 }


bool rf_faux_has_rssi(void *h)
 {
   FAUX_RF_LOG_FUNC_TODO;

   return false;
 }


float rf_faux_get_rssi(void *h)
 {
   FAUX_RF_LOG_FUNC_TODO;

   return 0.0;
 }


void rf_faux_suppress_stdout(void *h)
 {
   // _faux_rf_logStdout = false;
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
   FAUX_RF_DEBUG("channels %u, args [%s]", nof_channels, args);

   if(strncmp(args, "enb", strlen("enb")) == 0)
    {
      _faux_rf_info.type = FAUX_RF_TYPE_ENB;
    }
   else if(strncmp(args, "ue", strlen("ue")) == 0)
    {
      _faux_rf_info.type = FAUX_RF_TYPE_UE;
    }
   else
    {
      FAUX_RF_DEBUG("default type is ue");

      _faux_rf_info.type = FAUX_RF_TYPE_UE;
    }
       
   if(nof_channels != 1)
    {
      FAUX_RF_DEBUG("only supporting 1 channel, not %d", nof_channels);

      return -1;
    }

   if(_faux_rf_open_ipc_zmq(&_faux_rf_info) < 0)
     {
       FAUX_RF_DEBUG("could not creat ipc channel");

       return -1;
     }

   *h = &_faux_rf_info;

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

   FAUX_RF_DEBUG("rate %lf to %lf", _info->clockRate, rate);

   _info->clockRate = rate;
 }


bool rf_faux_is_master_clock_dynamic(void *h)
 {
   FAUX_RF_LOG_FUNC_TODO;

   return false;
 }


double rf_faux_set_rx_srate(void *h, double rate)
 {
   GET_FAUX_INFO(h);

   FAUX_RF_DEBUG("rate %lf to %lf", _info->rxSrate, rate);

   _info->rxSrate = rate;

   return _info->rxSrate;
 }


double rf_faux_set_rx_gain(void *h, double gain)
 {
   GET_FAUX_INFO(h);

   FAUX_RF_DEBUG("gain %lf to %lf", _info->rxGain, gain);

   _info->rxGain = gain;

   return _info->rxGain;
 }


double rf_faux_set_tx_gain(void *h, double gain)
 {
   GET_FAUX_INFO(h);

   FAUX_RF_DEBUG("gain %lf to %lf", _info->txGain, gain);

   _info->txGain = gain;

   return _info->txGain;
 }


double rf_faux_get_rx_gain(void *h)
 {
   GET_FAUX_INFO(h);

   FAUX_RF_DEBUG("gain %lf", _info->rxGain);

   return _info->rxGain;
 }


double rf_faux_get_tx_gain(void *h)
 {
   GET_FAUX_INFO(h);

   FAUX_RF_DEBUG("gain %lf", _info->txGain);

   return _info->txGain;
 }


double rf_faux_set_rx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h);

   FAUX_RF_DEBUG("freq %lf to %lf", _info->rxFreq, freq);

   _info->rxFreq = freq;

   return _info->rxFreq;
 }


double rf_faux_set_tx_srate(void *h, double rate)
 {
   GET_FAUX_INFO(h);

   FAUX_RF_DEBUG("rate %lf to %lf", _info->txSrate, rate);

   _info->txSrate = rate;

   return _info->txSrate;
 }


double rf_faux_set_tx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h);

   FAUX_RF_DEBUG("freq %lf to %lf", _info->txFreq, freq);

   _info->txFreq = freq;

   return _info->txFreq;
 }


void rf_faux_get_time(void *h, time_t *secs, double *frac_secs)
 {
   struct timeval tv;

   gettimeofday(&tv, NULL);

   _faux_rf_tv_to_ts(&tv, secs, frac_secs);
 }


int rf_faux_recv_with_time(void *h, void *data, uint32_t n_samples, 
                           bool blocking, time_t *secs, double *frac_secs)
 {
   GET_FAUX_INFO(h);

   const int nb_req = BYTES_X_SAMPLE(n_samples);

   int nb_pending = nb_req;

   char topic[FAUX_RX_TOPIC_LEN + 1];

   const int flags = !blocking ? ZMQ_NOBLOCK : 0;

   struct faux_rf_hdr hdr;

   int n_tries = _faux_rf_is_ue(_info) ? 100 : 1;

   uint8_t * p = (uint8_t *) data;

   FAUX_RF_DEBUG("nsamples %u/%d, blocking %s",
                  n_samples,
                  nb_req,
                  FAUX_RF_BOOL_TO_STR(blocking));
 
   if(_info->readlocked)
    {
      FAUX_RF_DEBUG("LOCKED");

      goto out;
    }

   if(n_samples < 2000)
    {
       FAUX_RF_DEBUG("SHORT");
      
       goto out;
    }

   _info->readlocked = true;

   while(nb_pending > 0 && n_tries--)
     {   
       int nb_recv = 0, rc;

       cf_t sf_in [0x4000] = {0.0, 0.0};

       memset(topic, 0x0, sizeof(topic));

       if(_faux_rf_is_ue(_info))
        {
          struct faux_rf_msg iov[3] = {{(void*)topic, sizeof(topic), 0}, 
                                       {(void*)&hdr,  sizeof(hdr),   0},
                                       {(void*)sf_in, nb_pending,    0}};

          rc = _faux_rf_vecio_recv(iov, 3, _info, flags);

          nb_recv = iov[2].msg_rc;
        }
       else
        {
          struct faux_rf_msg iov[2] = {{(void*)&hdr,  sizeof(hdr),  0},
                                       {(void*)sf_in, nb_pending,   0}};

          rc = _faux_rf_vecio_recv(iov, 2, _info, flags);

          nb_recv = iov[1].msg_rc;
        }

      if(rc <= 0)
        {
          if(errno != EAGAIN)
            {
              FAUX_RF_DEBUG("recv error %s", strerror(errno));

              goto out;
            }
        }
      else
       {
         if(nb_recv > 0)
           {
             if(_faux_rf_is_enb(_info))
               {
                 if(zmq_send(_info->rxSock, FAUX_RF_RESPONSE, FAUX_RF_RESPONSE_LEN, 0) < 0)
                   {
                     FAUX_RF_DEBUG("send response error %s", strerror(errno));
                   }
                 else
                   {
                     FAUX_RF_DEBUG("send response %s", FAUX_RF_RESPONSE);
                   }
               } 

            if(nb_pending == nb_req)
              {
                int idx = hdr.seqnum % 10;

                if(idx != 0 && idx != 5)
                  {
                    FAUX_RF_DEBUG("discard seqnum %ld, idx %d", hdr.seqnum, idx);

                    continue;
                  }
              }

            int nb_out = nb_recv;

            const double sratio = _info->rxSrate / hdr.srate;

            if(sratio != 1.0f)
              {
                // Resample
                srslte_resample_arb_t r;

                srslte_resample_arb_init(&r, sratio);

                nb_out = BYTES_X_SAMPLE(srslte_resample_arb_compute(&r, sf_in, (cf_t*)p, SAMPLE_X_BYTE(nb_recv)));
              }
            else
              {
                memcpy(p, sf_in, nb_out);
              }

            p += nb_out;

            nb_pending -= nb_out;

            FAUX_RF_DEBUG("nb_recv %d, seqnum %ld, sratio %f, nb_out %d, nb_pending %d, %d more tries",
                          nb_recv, 
                          hdr.seqnum,
                          sratio,
                          nb_out,
                          nb_pending,
                          n_tries);
          }
       }
    }

    FAUX_RF_DEBUG("nb_req %d, nb_pending %d",
                  nb_req,
                  nb_pending);
 
out:

   rf_faux_get_time(h, secs, frac_secs);

   _info->readlocked = false;

   return n_samples;
 }


int rf_faux_recv_with_time_multi(void *h, void **data, uint32_t n_samples, 
                                 bool blocking, time_t *secs, double *frac_secs)
{
   return rf_faux_recv_with_time(h, 
                                 data[0],
                                 n_samples, 
                                 blocking,
                                 secs,
                                 frac_secs);
}


int rf_faux_send_timed(void *h, void *data, int n_samples,
                       time_t secs, double frac_secs, bool has_time_spec,
                       bool blocking, bool is_start_of_burst, bool is_end_of_burst)
{
   GET_FAUX_INFO(h);

   struct timeval tv_dif;

   _faux_rf_dif_time(secs, frac_secs, &tv_dif);

   const int flags = !blocking ? ZMQ_NOBLOCK : 0;

   const int nb_req = BYTES_X_SAMPLE(n_samples);

   FAUX_RF_DEBUG("nsamples %u/%d, offset %ld:%06ld, seqnum %ld, sob %s, eob %s, srate %lf, blocking %s", 
                 n_samples,
                 nb_req,
                 tv_dif.tv_sec,
                 tv_dif.tv_usec,
                 _info->txseq,
                 FAUX_RF_BOOL_TO_STR(is_start_of_burst),
                 FAUX_RF_BOOL_TO_STR(is_end_of_burst),
                 _info->txSrate,
                 FAUX_RF_BOOL_TO_STR(blocking));

   struct faux_rf_hdr hdr = {_info->txseq, _info->txSrate};

   int nb_sent = 0, rc;

   if(_faux_rf_is_enb(_info))
    {
      struct faux_rf_msg iov[3] = {{(void*)FAUX_RF_TOPIC, FAUX_RX_TOPIC_LEN, 0}, 
                                   {(void*)&(hdr),        sizeof(hdr),       0},
                                   {(void*)data,          nb_req,            0}};

      rc = _faux_rf_vecio_send(iov, 3, _info, flags);

      nb_sent = iov[2].msg_rc;
    }
   else
    {
      struct faux_rf_msg iov[2] = {{(void*)&(hdr), sizeof(hdr), 0},
                                   {(void*)data,   nb_req,      0}};

      rc = _faux_rf_vecio_send(iov, 2, _info, flags);

      nb_sent = iov[1].msg_rc;
    }

   if(rc <= 0)
     {
       FAUX_RF_DEBUG("send error %s", strerror(errno));

       return -1;
     }
   else
     {
       ++_info->txseq;

       if(_faux_rf_is_ue(_info))
         {
           char resp[FAUX_RF_RESPONSE_LEN + 1];

           memset(resp, 0x0, sizeof(resp));

           FAUX_RF_DEBUG("wait for response");

           if(zmq_recv(_info->txSock, resp, FAUX_RF_RESPONSE_LEN, flags) < 0)
             {
               FAUX_RF_DEBUG("recv response error %s", strerror(errno));
             }
           else
             {
               FAUX_RF_DEBUG("recv response %s", resp);
             }
         }
     }

   FAUX_RF_DEBUG("sent %d bytes of %d", nb_sent, nb_req);

   return n_samples;
}


int rf_faux_send_timed_multi(void *h, void *data[4], int n_samples,
                             time_t secs, double frac_secs, bool has_time_spec,
                             bool blocking, bool is_start_of_burst, bool is_end_of_burst)
{
  return rf_faux_send_timed(h, 
                            data[0], 
                            n_samples,
                            secs,
                            frac_secs,
                            has_time_spec,
                            blocking,
                            is_start_of_burst,
                            is_end_of_burst);
}


void rf_faux_set_tx_cal(void *h, srslte_rf_cal_t *cal)
{
   FAUX_RF_DEBUG("gain %f, phase %f, i %f, q %f", 
              cal->dc_gain, cal->dc_phase, cal->iq_i, cal->iq_q);
}


void rf_faux_set_rx_cal(void *h, srslte_rf_cal_t *cal)
{
   FAUX_RF_DEBUG("gain %f, phase %f, i %f, q %f", 
           cal->dc_gain, cal->dc_phase, cal->iq_i, cal->iq_q);
}
