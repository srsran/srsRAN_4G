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

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>

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

#define RF_FAUX_NORM_DIFF(x, y)  abs((x) + (~(y) + 1))

// socket port nums
#define RF_FAUX_DL_PORT (43001)
#define RF_FAUX_UL_PORT (43002)

#define RF_FAUX_MC_ADDR "224.4.3.2"
#define RF_FAUX_MC_DEV  "lo"

// bytes per sample
#define BYTES_X_SAMPLE(x) ((x)*sizeof(cf_t))

// samples per byte
#define SAMPLES_X_BYTE(x) ((x)/sizeof(cf_t))

// target OTA SR
#define RF_FAUX_OTA_SRATE (5760000.0)

// max sf len
#define RF_FAUX_SF_LEN (0x2000)

// tx offset enable
#define RF_FAUX_TX_DELAY_ENABLE  (1)

// normalize sf req len for pkt i/o
#define RF_FAUX_NORM_SF_LEN(x) (((x) = ((x)/10)*10))

// node type
#define RF_FAUX_NTYPE_NONE  (0)
#define RF_FAUX_NTYPE_UE    (1)
#define RF_FAUX_NTYPE_ENB   (2)

// tx offset (delay) workers
#define RF_FAUX_NOF_TX_WORKERS (25)
#define RF_FAUX_SET_NEXT_WORKER(x) ((x) = ((x) + 1) % RF_FAUX_NOF_TX_WORKERS)

 uint32_t  g_tti     = 0;
 uint32_t  g_tti_tx  = 0;

typedef struct {
  void * h;
  cf_t   data[RF_FAUX_SF_LEN];
  int    nsamples;
  struct timeval tx_time;
  bool   is_sob;
  bool   is_eob;
  uint32_t tti_tx;
} _rf_faux_tx_info_t;


typedef struct {
  void *               h;
  pthread_t            tid;
  sem_t                sem;
  int                  id;
  _rf_faux_tx_info_t * tx_info;
} _rf_faux_tx_worker_t;


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
   int                  tx_handle;
   int                  rx_handle;
   size_t               rx_timeout;
   int                  rx_tries;
   int64_t              tx_seqn;
   int64_t              rx_seqn;
   bool                 in_rx;
   pthread_mutex_t      rx_lock;
   pthread_mutex_t      tx_workers_lock;
   pthread_t            tx_tid;
   _rf_faux_tx_worker_t tx_workers[RF_FAUX_NOF_TX_WORKERS];
   int                  tx_worker_next;
   int                  nof_tx_workers;
   cf_t                 sf_out[RF_FAUX_SF_LEN];
} _rf_faux_info_t;


typedef struct {
  uint64_t       seqnum;
  uint32_t       msglen;
  float          srate;
  struct timeval tx_time;
  uint32_t       tx_tti;
} _rf_faux_iohdr_t;



static void _rf_faux_handle_error(srslte_rf_error_t error)
{
  RF_FAUX_DEBUG("%s:%s type %s, opt %d, msg %s\b", 
                error.type == SRSLTE_RF_ERROR_LATE      ? "late"      :
                error.type == SRSLTE_RF_ERROR_UNDERFLOW ? "underflow" :
                error.type == SRSLTE_RF_ERROR_OVERFLOW  ? "overflow"  :
                error.type == SRSLTE_RF_ERROR_OTHER     ? "other"     :
                "unknown error");
}


static  _rf_faux_info_t _rf_faux_info = { .dev_name        = "faux",
                                          .rx_gain         = 0.0,
                                          .tx_gain         = 0.0,
                                          .rx_srate        = RF_FAUX_OTA_SRATE,
                                          .tx_srate        = RF_FAUX_OTA_SRATE,
                                          .rx_freq         = 0.0,
                                          .tx_freq         = 0.0,
                                          .rx_cal          = {0.0, 0.0, 0.0, 0.0},
                                          .tx_cal          = {0.0, 0.0, 0.0, 0.0},
                                          .clock_rate      = 0.0,
                                          .error_handler   = _rf_faux_handle_error,
                                          .rx_stream       = false,
                                          .ntype           = RF_FAUX_NTYPE_NONE,
                                          .tx_handle       = -1,
                                          .rx_handle       = -1,
                                          .rx_timeout      = 0,
                                          .rx_tries        = 1,
                                          .tx_seqn         = 1,
                                          .rx_seqn         = 0,
                                          .in_rx           = false,
                                          .rx_lock         = PTHREAD_MUTEX_INITIALIZER,
                                          .tx_workers_lock = PTHREAD_MUTEX_INITIALIZER,
                                          .tx_worker_next  = 0,
                                          .nof_tx_workers  = 0,
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
  if(sratio < 1.0)
   {
     srslte_resample_arb_t r;

     srslte_resample_arb_init(&r, sratio);

     ns_out = srslte_resample_arb_compute(&r, (cf_t*)in, (cf_t*)out, ns_in);
   }
  else
   {
     memcpy(out, in, BYTES_X_SAMPLE(ns_in));
   }

  RF_FAUX_DEBUG("srate %4.2lf/%4.2lf MHz, sratio %3.3lf, ns_in %d, ns_out %d",
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



static int _rf_faux_vecio_recv(void *h, struct iovec iov[2])
{
   GET_FAUX_INFO(h);

   const int nb_req = iov[0].iov_len + iov[1].iov_len;

   // initial wait if non-blocking
   if(_info->rx_timeout)
     {
       fd_set rfds;

       FD_ZERO(&rfds);
       FD_SET(_info->rx_handle, &rfds);

       struct timeval tv_wait = {0, _info->rx_timeout};

       const int n_fd = select(_info->rx_handle + 1, &rfds, NULL, NULL, &tv_wait);

       if(n_fd <= 0 || (! FD_ISSET(_info->rx_handle, &rfds)))
         {
           RF_FAUX_DEBUG("req %d, timedout", nb_req);
 
           errno = EAGAIN;

           return 0;
         }
     }

   const int rc = readv(_info->rx_handle, iov, 2);

   if(rc <= 0)
     {
       RF_FAUX_DEBUG("recv reqlen %d, error %s", nb_req, strerror(errno));
     }
   else
     { 
       RF_FAUX_DEBUG("recv %d of %d", rc, nb_req);
    }

   return rc;
}


static int _rf_faux_vecio_send(void *h, struct iovec iov[2])
{
   GET_FAUX_INFO(h);

   const int nb_req = iov[0].iov_len + iov[1].iov_len;

   int rc = writev(_info->tx_handle, iov, 2);

   if(rc < 0)
     {
       RF_FAUX_DEBUG("send error %s", strerror(errno));
     }
   else
     {
       RF_FAUX_DEBUG("sent %d of %d", rc, nb_req);
     }

  return rc;
}



void _rf_faux_tx_msg(_rf_faux_tx_info_t * tx_info, uint64_t seqn)
{
   GET_FAUX_INFO(tx_info->h);

   const int nb_in = BYTES_X_SAMPLE(tx_info->nsamples);

   // resample to match the ota sr if needed
   const int ns_out = _rf_faux_resample(_info->tx_srate,
                                        RF_FAUX_OTA_SRATE,
                                        tx_info->data,
                                        _info->sf_out, 
                                        tx_info->nsamples);

   const int nb_out = BYTES_X_SAMPLE(ns_out);

   const _rf_faux_iohdr_t hdr = { seqn, 
                                  nb_out,
                                  RF_FAUX_OTA_SRATE, 
                                  tx_info->tx_time,
                                  g_tti_tx };

   struct iovec iov[2] = { {(void*)&(hdr),        sizeof(hdr)},
                           {(void*)_info->sf_out, nb_out     }};


   const int rc = _rf_faux_vecio_send(_info, iov);

   if(rc <= 0)
     {
       RF_FAUX_DEBUG("send reqlen %d, error %s", nb_out, strerror(errno));
     }
   else
     {
       RF_FAUX_DEBUG("TX my/m_tti %u/%u, in %u/%d, seqn %lu, sob %s, eob %s, srate %4.2lf MHz, out %d/%d", 
                    g_tti,
                    tx_info->tti_tx,
                    tx_info->nsamples,
                    nb_in,
                    seqn,
                    RF_FAUX_BOOL_TO_STR(tx_info->is_sob),
                    RF_FAUX_BOOL_TO_STR(tx_info->is_eob),
                    RF_FAUX_OTA_SRATE / 1e6,
                    ns_out,
                    nb_out);
     }
}





static void * _rf_faux_tx_worker_proc(void * arg)
{
   _rf_faux_tx_worker_t * worker = (_rf_faux_tx_worker_t*) arg;

   GET_FAUX_INFO(worker->h);

   RF_FAUX_DEBUG("worker %d created, ready for duty", worker->id);

   struct timeval tv_now, tx_delay;

   bool running = true;

   while(running)
     {
       sem_wait(&(worker->sem));

       gettimeofday(&tv_now, NULL);

       timersub(&(worker->tx_info->tx_time), &tv_now, &tx_delay);

       if(RF_FAUX_TX_DELAY_ENABLE)
         {
           RF_FAUX_DEBUG("worker %d, my_tti %u, apply tx_delay %ld:%06ld", 
                         worker->id,
                         g_tti,
                         tx_delay.tv_sec,
                         tx_delay.tv_usec);

           select(0, NULL, NULL, NULL, &tx_delay);
         }
       else
         {
           RF_FAUX_DEBUG("worker %d, my_tx %u, skip tx_delay %ld:%06ld", 
                          worker->id, 
                          g_tti,
                          tx_delay.tv_sec,
                          tx_delay.tv_usec);
         }

        pthread_mutex_lock(&(_info->tx_workers_lock));

        const int n_diff = RF_FAUX_NORM_DIFF(g_tti, worker->tx_info->tti_tx);

        RF_FAUX_DEBUG("TX my/tx/diff_txtti %u/%u/%d, fire worker %d", 
                      g_tti, 
                      worker->tx_info->tti_tx,
                      n_diff,
                      worker->id);

        _rf_faux_tx_msg(worker->tx_info, (_info->tx_seqn)++);

        _info->nof_tx_workers -= 1;

        free(worker->tx_info);

        worker->tx_info = NULL;

        pthread_mutex_unlock(&(_info->tx_workers_lock));
     }

   return NULL;
}





static int _rf_faux_open_sock(_rf_faux_info_t * info, 
                             uint16_t rxport, 
                             uint16_t txport,
                             const char * mcaddr)
{
  int rx_fd, tx_fd;

  if((rx_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      RF_FAUX_DEBUG("rx sock open error %s", strerror(errno));

      return -1;
    }


  int nbytes = 0;

  socklen_t optlen = sizeof(nbytes);

  if(getsockopt(rx_fd, SOL_SOCKET, SO_RCVBUF, &nbytes, &optlen) < 0) 
    {
      RF_FAUX_DEBUG("rx sock get rxbuf size %s", strerror(errno));

      return -1;
    }

  nbytes *= 10;

  if(setsockopt(rx_fd, SOL_SOCKET, SO_RCVBUF, &nbytes, sizeof(nbytes)) < 0) 
    {
      RF_FAUX_DEBUG("rx sock set rxbuf size to %d, %s", nbytes, strerror(errno));

      return -1;
    }
  else
    {
      RF_FAUX_DEBUG("rx sock set rxbuf size to %d", nbytes);
    }


  const int opt_on = 1;

  if(setsockopt(rx_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt_on, sizeof(opt_on)) < 0) 
    {
      RF_FAUX_DEBUG("rx sock set resue error %s", strerror(errno));

      return -1;
    }

  if((tx_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      RF_FAUX_DEBUG("tx sock open error %s", strerror(errno));

      return -1;
    }

  nbytes = 0;

  optlen = sizeof(nbytes);

  if(getsockopt(tx_fd, SOL_SOCKET, SO_SNDBUF, &nbytes, &optlen) < 0) 
    {
      RF_FAUX_DEBUG("tx sock get txbuf size %s", strerror(errno));

      return -1;
    }

  nbytes *= 10;

  if(setsockopt(tx_fd, SOL_SOCKET, SO_SNDBUF, &nbytes, sizeof(nbytes)) < 0) 
    {
      RF_FAUX_DEBUG("tx sock set txbuf size to %d, %s", nbytes, strerror(errno));

      return -1;
    }
  else
    {
      RF_FAUX_DEBUG("tx sock set txbuf size to %d", nbytes);
    }

  if(setsockopt(tx_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt_on, sizeof(opt_on)) < 0) 
    {
      RF_FAUX_DEBUG("rx sock set resue error %s", strerror(errno));

      return -1;
    }

  struct ifreq ifr;
  memset(&ifr,  0, sizeof(ifr));

  strncpy(ifr.ifr_name, RF_FAUX_MC_DEV, IFNAMSIZ);

  if(ioctl(tx_fd, SIOCGIFADDR, &ifr) < 0) 
    {
      RF_FAUX_DEBUG("tx sock get addr error %s", strerror(errno));

      return -1;
    }

  const in_addr_t if_addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;

  if(ioctl(tx_fd, SIOCGIFINDEX, &ifr) < 0)
    {
      RF_FAUX_DEBUG("tx sock get ifindex error %s", strerror(errno));

      return -1;
    }

  const int if_index = ifr.ifr_ifindex;

  if(setsockopt(tx_fd, SOL_SOCKET, SO_BSDCOMPAT, (char *) &opt_on, sizeof(opt_on)) < 0) 
    {
      RF_FAUX_DEBUG("rx sock set bsdcompat error %s", strerror(errno));

      return -1;
    }

  struct sockaddr_in sin_rx;
  memset(&sin_rx, 0x0, sizeof(sin_rx));
  sin_rx.sin_family      = AF_INET;
  sin_rx.sin_port        = htons(rxport);
  sin_rx.sin_addr.s_addr = inet_addr(mcaddr);


  struct sockaddr_in sin_tx;
  memset(&sin_tx, 0x0, sizeof(sin_tx));
  sin_tx.sin_family      = AF_INET;
  sin_tx.sin_port        = htons(txport);
  sin_tx.sin_addr.s_addr = inet_addr(mcaddr);


  struct ip_mreq mreq;
  memset(&mreq, 0x0, sizeof(mreq));
  mreq.imr_multiaddr.s_addr = inet_addr(mcaddr);
  mreq.imr_interface.s_addr = if_addr;

  if(bind(rx_fd, (struct sockaddr*)&sin_rx, sizeof(sin_rx)) < 0)
    {
      RF_FAUX_DEBUG("rx sock bind error %s", strerror(errno));

      return -1;
    }

  if(setsockopt(rx_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) 
    {
      RF_FAUX_DEBUG("rx sock add membership error %s", strerror(errno));

      return -1;
    }
  else
    {
      RF_FAUX_DEBUG("joined group %s port %hu", inet_ntoa(mreq.imr_multiaddr), rxport);
    }


  if(connect(tx_fd,(struct sockaddr*)&sin_tx, sizeof(sin_tx)) < 0)
    {
      RF_FAUX_DEBUG("tx sock connect error %s", strerror(errno));

      return -1;
    }

  int no_df = IP_PMTUDISC_DONT;

  if(setsockopt(tx_fd, IPPROTO_IP, IP_MTU_DISCOVER, &no_df, sizeof(no_df)) < 0) 
   {
     RF_FAUX_DEBUG("tx sock set mtu opt error %s", strerror(errno));

     return -1;
   }


  struct ip_mreqn mreqn;
  memset(&mreqn, 0, sizeof(mreqn));
  mreqn.imr_multiaddr.s_addr = 0;
  mreqn.imr_address.s_addr   = if_addr;
  mreqn.imr_ifindex          = if_index;

  if(setsockopt(tx_fd, SOL_IP, IP_MULTICAST_IF, &mreqn, sizeof(mreqn)) < 0) 
   {
     RF_FAUX_DEBUG("tx sock set mcif error %s", strerror(errno));

     return -1; 
   }

  const int flags = fcntl(tx_fd, F_GETFL, 0);
  fcntl(tx_fd, F_SETFL, flags | O_NONBLOCK);

  info->tx_handle = tx_fd;
  info->rx_handle = rx_fd;

  // timeout block ue, no block enb
  info->rx_timeout = _rf_faux_is_ue(info) ? 0 : 250;

  return 0;
}


static int _rf_faux_open_ipc(_rf_faux_info_t * info)
{
  if(_rf_faux_is_enb(info))
    {
      return _rf_faux_open_sock(info, 
                                RF_FAUX_UL_PORT,  // rx
                                RF_FAUX_DL_PORT,  // tx
                                RF_FAUX_MC_ADDR);
    }
  else
    {
      return _rf_faux_open_sock(info, 
                                RF_FAUX_DL_PORT,  // rx
                                RF_FAUX_UL_PORT,  // tx
                                RF_FAUX_MC_ADDR);
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
   
   pthread_mutex_lock(&(_info->rx_lock));

   RF_FAUX_DEBUG("");

   _info->rx_stream = true;

   pthread_mutex_unlock(&(_info->rx_lock));

   return 0;
 }


int rf_faux_stop_rx_stream(void *h)
 {
   GET_FAUX_INFO(h);

   pthread_mutex_lock(&(_info->rx_lock));

   RF_FAUX_DEBUG("");

   _info->rx_stream = false;

   pthread_mutex_unlock(&(_info->rx_lock));

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

   if(_rf_faux_open_ipc(&_rf_faux_info) < 0)
    {
      RF_FAUX_DEBUG("could not create ipc channel");

      return -1;
    }

   for(int id = 0; id < RF_FAUX_NOF_TX_WORKERS; ++id)
     {
       _rf_faux_tx_worker_t * worker =  &(_rf_faux_info.tx_workers[id]);

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
                         _rf_faux_tx_worker_proc, 
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

   close(_info->rx_handle);

   close(_info->tx_handle);

   return 0;
 }


void rf_faux_set_master_clock_rate(void *h, double rate)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("rate %4.2lf MHz to %4.2lf MHz", 
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

   RF_FAUX_DEBUG("rate %4.2lf MHz to %4.2lf MHz", 
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

   RF_FAUX_DEBUG("freq %4.2lf MHz to %4.2lf MHz", 
                 _info->rx_freq / 1e6, freq / 1e6);

   _info->rx_freq = freq;

   return _info->rx_freq;
 }


double rf_faux_set_tx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_DEBUG("freq %4.2lf MHz to %4.2lf MHz", 
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

   struct timeval tv_in, tv_out, tv_diff;

   gettimeofday(&tv_in, NULL);

   pthread_mutex_lock(&(_info->rx_lock));

   _info->in_rx = true;

   // sometimes we get a request for a few extra samples (1922 vs 1920) that 
   // throws off our pkt based stream
   RF_FAUX_NORM_SF_LEN(nsamples);

   const int nb_req = BYTES_X_SAMPLE(nsamples);

   int nb_pending = nb_req;

   int ns_pending = nsamples;

   _rf_faux_iohdr_t hdr;

   int n_tries = 0;

   uint8_t * p = (uint8_t *) data;

   struct timeval rx_time, ota_delay;

   RF_FAUX_DEBUG("begin try 1 of %d, my_tti %u, req %u/%d, blocking %s, streaming %s",
                  _info->rx_tries,
                  g_tti,
                  nsamples,
                  nb_req,
                  RF_FAUX_BOOL_TO_STR(blocking),
                  RF_FAUX_BOOL_TO_STR(_info->rx_stream));

   memset(data, 0x0, nb_req);

   // read 1 sf
   const int nb_sf = BYTES_X_SAMPLE(RF_FAUX_OTA_SRATE / 1000);

   cf_t sf_in [RF_FAUX_SF_LEN];

   int rc = 0;

   while(nb_pending > 0 && (n_tries++ < _info->rx_tries))
     {   
       memset(sf_in, 0x0, BYTES_X_SAMPLE(RF_FAUX_SF_LEN));

       struct iovec iov[2] = { {(void*)&hdr,  sizeof(hdr)},
                               {(void*)sf_in, nb_sf      }};

       rc = _rf_faux_vecio_recv(h, iov);

       gettimeofday(&rx_time, NULL);

       timersub(&rx_time, &(hdr.tx_time), &ota_delay);

       if(rc <= 0)
        {
          if(errno != EAGAIN)
            {
              RF_FAUX_DEBUG("recv error %s", strerror(errno));
            }

           break;
        }
      else
       {
         const int nb_in = rc - sizeof(hdr);

         if(nb_in != hdr.msglen)
           {
             RF_FAUX_DEBUG("RX seqn %lu, len error expected %d, got %d, DROP", hdr.seqnum, hdr.msglen, nb_in);

             break;
           }
        
         const int ns_in  =  SAMPLES_X_BYTE(nb_in);

         const int ns_out = _rf_faux_resample(hdr.srate,
                                              _info->rx_srate,
                                              sf_in,
                                              p, 
                                              ns_in);

         const int nb_out = BYTES_X_SAMPLE(ns_out);

         p += nb_out;

         nb_pending -= nb_out;

         ns_pending -= ns_out;

         const uint64_t this_seqn = _info->rx_seqn + 1;

         _info->rx_seqn = hdr.seqnum;

         if(this_seqn != hdr.seqnum)
           {
             RF_FAUX_DEBUG("RX OOS pkt, expected %lu, got seqn %lu", this_seqn, hdr.seqnum);
           }

         const int n_diff = RF_FAUX_NORM_DIFF(g_tti, hdr.tx_tti);

         RF_FAUX_DEBUG("RX my/rx/diff_rxtti %u/%u/%d, rc %d, %d/%d, ota_delay %ld:%06ld, seqn %lu, added %d/%d, pending %d/%d, try %d/%d",
                       g_tti,
                       hdr.tx_tti,
                       n_diff,
                       rc,
                       ns_in, 
                       nb_in,
                       ota_delay.tv_sec,
                       ota_delay.tv_usec,
                       hdr.seqnum,
                       ns_out,
                       nb_out,
                       ns_pending,
                       nb_pending,
                       n_tries,
                       _info->rx_tries);

          --n_tries;
       }
    }

   gettimeofday(&tv_out, NULL);

   timersub(&tv_out, &tv_in, &tv_diff);

   if(nb_req == nb_pending)
     {
       RF_FAUX_DEBUG("req %d/%d, timed_out, delta_t %ld:%06ld",
                     nsamples, 
                     nb_req,
                     tv_diff.tv_sec,
                     tv_diff.tv_usec);
     }
   else
     {
       RF_FAUX_DEBUG("req %d/%d, delta_t %ld:%06ld, pending %d/%d, out %d/%d", 
                     nsamples,
                     nb_req, 
                     ns_pending, 
                     tv_diff.tv_sec,
                     tv_diff.tv_usec,
                     nb_pending,
                     nsamples - ns_pending,
                     nb_req - nb_pending);

     }

   _info->in_rx = false;

   pthread_mutex_unlock(&(_info->rx_lock));

   _rf_faux_tv_to_ts(&tv_in, full_secs, frac_secs);

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
   if(nsamples == 0)
     {
       return nsamples;
     }

   GET_FAUX_INFO(h);

   pthread_mutex_lock(&(_info->tx_workers_lock));

   if(_info->nof_tx_workers >= RF_FAUX_NOF_TX_WORKERS)
     {
       RF_FAUX_DEBUG("FAILED worker pool full %d, DROP", RF_FAUX_NOF_TX_WORKERS);

       return 0;
     }

   _rf_faux_tx_info_t * e = malloc(sizeof(_rf_faux_tx_info_t));

   if(e == NULL)
     {
       RF_FAUX_DEBUG("FAILED to allocate memory for worker, DROP");

       return 0;
     }

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
   e->is_sob   = is_sob;
   e->is_eob   = is_eob;
   e->tti_tx   = g_tti_tx;

   _rf_faux_tx_worker_t * worker;

   // get available worker
   while((worker = &(_info->tx_workers[_info->tx_worker_next]))->tx_info)
    {
      RF_FAUX_DEBUG("skipping pending worker %d", worker->id);

      RF_FAUX_SET_NEXT_WORKER(_info->tx_worker_next);
    }

   worker->tx_info = e;

   _info->nof_tx_workers += 1;
 
   RF_FAUX_SET_NEXT_WORKER(_info->tx_worker_next);

   RF_FAUX_DEBUG("add tx_worker %d, next %d, time spec %s, tx_time %ld:%0.6lf, %d workers pending",
                  worker->id,
                  _info->tx_worker_next,
                  RF_FAUX_BOOL_TO_STR(has_time_spec),
                  full_secs,
                  frac_secs,
                 _info->nof_tx_workers);

   sem_post(&(worker->sem));

   pthread_mutex_unlock(&(_info->tx_workers_lock));

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

   RF_FAUX_DEBUG("gain %3.2lf, phase %3.2lf, I %3.2lf, Q %3.2lf", 
                 cal->dc_gain, 
                 cal->dc_phase, 
                 cal->iq_i,
                 cal->iq_q);
}


void rf_faux_set_rx_cal(void *h, srslte_rf_cal_t *cal)
{
   GET_FAUX_INFO(h);

   memcpy(&_info->rx_cal, cal, sizeof(srslte_rf_cal_t));

   RF_FAUX_DEBUG("gain %3.2lf, phase %3.2lf, I %3.2lf, Q %3.2lf", 
                 cal->dc_gain,
                 cal->dc_phase,
                 cal->iq_i,
                 cal->iq_q);
}
