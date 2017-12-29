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
#include <sys/uio.h>

#include <net/if.h>

#include "srslte/srslte.h"
#include "rf_faux_imp.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/resampling/resample_arb.h"

static bool rf_faux_log_verbose = false;
static bool rf_faux_log_debug   = true;
static bool rf_faux_log_info    = true;


#define RF_FAUX_WARN(_fmt, ...) do {                                                                       \
                                   struct timeval _tv_now;                                                 \
                                   struct tm _tm;                                                          \
                                   gettimeofday(&_tv_now, NULL);                                           \
                                   localtime_r(&_tv_now.tv_sec, &_tm);                                     \
                                   fprintf(stdout, "[WARN ]: [%05hu] %02d.%02d.%02d.%06ld %s, " _fmt "\n", \
                                           g_tti,                                                          \
                                           _tm.tm_hour,                                                    \
                                           _tm.tm_min,                                                     \
                                           _tm.tm_sec,                                                     \
                                           _tv_now.tv_usec,                                                \
                                           __func__,                                                       \
                                           ##__VA_ARGS__);                                                 \
                                 } while(0);


#define RF_FAUX_DBUG(_fmt, ...) do {                                                                       \
                                 if(rf_faux_log_debug) {                                                   \
                                   struct timeval _tv_now;                                                 \
                                   struct tm _tm;                                                          \
                                   gettimeofday(&_tv_now, NULL);                                           \
                                   localtime_r(&_tv_now.tv_sec, &_tm);                                     \
                                   fprintf(stdout, "[DEBUG]: [%05hu] %02d.%02d.%02d.%06ld %s, " _fmt "\n", \
                                           g_tti,                                                          \
                                           _tm.tm_hour,                                                    \
                                           _tm.tm_min,                                                     \
                                           _tm.tm_sec,                                                     \
                                           _tv_now.tv_usec,                                                \
                                           __func__,                                                       \
                                           ##__VA_ARGS__);                                                 \
                                 }                                                                         \
                             } while(0);

#define RF_FAUX_INFO(_fmt, ...) do {                                                                       \
                                 if(rf_faux_log_info) {                                                    \
                                   struct timeval _tv_now;                                                 \
                                   struct tm _tm;                                                          \
                                   gettimeofday(&_tv_now, NULL);                                           \
                                   localtime_r(&_tv_now.tv_sec, &_tm);                                     \
                                   fprintf(stdout, "[INFO ]: [%05hu] %02d.%02d.%02d.%06ld %s, " _fmt "\n", \
                                           g_tti,                                                          \
                                           _tm.tm_hour,                                                    \
                                           _tm.tm_min,                                                     \
                                           _tm.tm_sec,                                                     \
                                           _tv_now.tv_usec,                                                \
                                           __func__,                                                       \
                                           ##__VA_ARGS__);                                                 \
                                 }                                                                         \
                             } while(0);


#define RF_FAUX_LOG_FUNC_TODO printf("XXX_TODO file:%s func:%s line:%d\n", \
                                     __FILE__,                             \
                                     __func__,                             \
                                     __LINE__);

#define RF_FAUX_BOOL_TO_STR(x) (x) ? "yes" : "no"

#define RF_FAUX_NORM_DIFF(x, y)  abs((x) + (~(y) + 1))

// socket port nums
#define RF_FAUX_DL_PORT (43301)
#define RF_FAUX_UL_PORT (43302)

#define RF_FAUX_MC_ADDR "224.4.3.2"
#define RF_FAUX_MC_DEV  "lo"
#define RF_FAUX_SOCK_BUFF_SIZE (2 * 1024 * 1024)
// bytes per sample
#define BYTES_X_SAMPLE(x) ((x)*sizeof(cf_t))

// samples per byte
#define SAMPLES_X_BYTE(x) ((x)/sizeof(cf_t))

// target OTA SR
#define RF_FAUX_OTA_SRATE (5760000.0)

// max sf len
#define RF_FAUX_SF_LEN (0x2000)

// normalize sf req len for pkt i/o
#define RF_FAUX_NORM_SF_LEN(x) (((x) = ((x)/10)*10))

// node type
#define RF_FAUX_NTYPE_NONE  (0)
#define RF_FAUX_NTYPE_UE    (1)
#define RF_FAUX_NTYPE_ENB   (2)

// tx offset (delay) workers
#define RF_FAUX_NOF_TX_WORKERS (25)
#define RF_FAUX_SET_NEXT_WORKER(x) ((x) = ((x) + 1) % RF_FAUX_NOF_TX_WORKERS)

static const struct timeval tv_tx_window = {0, 666}; 
static const struct timeval tv_rx_window = {0, 333}; // delta_t before next tti (1/3)
static const struct timeval tv_zero      = {0, 0};
static const struct timeval tv_step      = {0, 1000};

uint32_t       g_tti     = 0;
struct timeval g_tv_next = {0, 0};

typedef struct {
  void * h;
  cf_t   data[RF_FAUX_SF_LEN];
  int    nsamples;
  struct timeval tx_time;
  bool   is_sob;
  bool   is_eob;
} rf_faux_tx_info_t;


typedef struct {
  void *               h;
  pthread_t            tid;
  sem_t                sem;
  int                  id;
  rf_faux_tx_info_t *  tx_info;
  bool                 is_pending;
} rf_faux_tx_worker_t;


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
   bool                 rx_timeout;
   int64_t              tx_seqn;
   int64_t              rx_seqn;
   pthread_mutex_t      rx_lock;
   pthread_mutex_t      tx_workers_lock;
   pthread_t            tx_tid;
   rf_faux_tx_worker_t tx_workers[RF_FAUX_NOF_TX_WORKERS];
   int                  tx_worker_next;
   int                  nof_tx_workers;
   cf_t                 sf_out[RF_FAUX_SF_LEN];
} rf_faux_info_t;


typedef struct {
  uint64_t       seqnum;
  uint32_t       msglen;
  float          srate;
  struct timeval tx_time;
} rf_faux_iohdr_t;



static void rf_faux_handle_error(srslte_rf_error_t error)
{
  RF_FAUX_INFO("%s:%s type %s, opt %d, msg %s\b", 
                error.type == SRSLTE_RF_ERROR_LATE      ? "late"      :
                error.type == SRSLTE_RF_ERROR_UNDERFLOW ? "underflow" :
                error.type == SRSLTE_RF_ERROR_OVERFLOW  ? "overflow"  :
                error.type == SRSLTE_RF_ERROR_OTHER     ? "other"     :
                "unknown error");
}


static  rf_faux_info_t rf_faux_info = { .dev_name        = "faux",
                                          .rx_gain         = 0.0,
                                          .tx_gain         = 0.0,
                                          .rx_srate        = RF_FAUX_OTA_SRATE,
                                          .tx_srate        = RF_FAUX_OTA_SRATE,
                                          .rx_freq         = 0.0,
                                          .tx_freq         = 0.0,
                                          .rx_cal          = {0.0, 0.0, 0.0, 0.0},
                                          .tx_cal          = {0.0, 0.0, 0.0, 0.0},
                                          .clock_rate      = 0.0,
                                          .error_handler   = rf_faux_handle_error,
                                          .rx_stream       = false,
                                          .ntype           = RF_FAUX_NTYPE_NONE,
                                          .tx_handle       = -1,
                                          .rx_handle       = -1,
                                          .rx_timeout      = false,
                                          .tx_seqn         = 1,
                                          .rx_seqn         = 0,
                                          .rx_lock         = PTHREAD_MUTEX_INITIALIZER,
                                          .tx_workers_lock = PTHREAD_MUTEX_INITIALIZER,
                                          .tx_worker_next  = 0,
                                          .nof_tx_workers  = 0,
                                       };

// could just as well use rf_faux_info for single antenna mode
#define GET_FAUX_INFO(h)  assert(h); rf_faux_info_t *_info = (rf_faux_info_t *)(h)


static void rf_faux_tv_to_ts(const struct timeval *tv, time_t *full_secs, double *frac_secs)
{
  if(full_secs && frac_secs)
    {
      *full_secs = tv->tv_sec; 
      *frac_secs = tv->tv_usec / 1.0e6;
    }
}


static void rf_faux_ts_to_tv(struct timeval *tv, time_t full_secs, double frac_secs)
{
  if(tv)
    {
      tv->tv_sec  = full_secs;
      tv->tv_usec = frac_secs * 1.0e6;
    }
}


static int rf_faux_resample(double srate_in, 
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

  if(rf_faux_log_verbose)
  RF_FAUX_DBUG("srate %4.2lf/%4.2lf MHz, sratio %3.3lf, ns_in %d, ns_out %d",
                srate_in  / 1e6,
                srate_out / 1e6,
                sratio, 
                ns_in, 
                ns_out);

  return ns_out;
}



static bool rf_faux_is_enb(rf_faux_info_t * info)
{
  return (info->ntype == RF_FAUX_NTYPE_ENB);
}



static bool rf_faux_is_ue(rf_faux_info_t * info)
{
  return (info->ntype == RF_FAUX_NTYPE_UE);
}



static int rf_faux_vecio_recv(void *h, struct iovec iov[2])
{
   GET_FAUX_INFO(h);

   const int nb_req = iov[0].iov_len + iov[1].iov_len;

   // initial wait if non-blocking
   if(_info->rx_timeout)
     {
       fd_set rfds;

       FD_ZERO(&rfds);

       FD_SET(_info->rx_handle, &rfds);

       struct timeval tv_now, delta_t, tv_timeout;

       timersub(&g_tv_next, &tv_rx_window, &tv_timeout);

       gettimeofday(&tv_now, NULL);

       timersub(&tv_timeout, &tv_now, &delta_t);

       if(timercmp(&tv_timeout, &tv_zero, >))
         {
           RF_FAUX_DBUG("RX waiting %ld:%06ld", delta_t.tv_sec, delta_t.tv_usec);

           if(select(_info->rx_handle + 1, &rfds, NULL, NULL, &delta_t) <= 0 ||
                    (! FD_ISSET(_info->rx_handle, &rfds)))
            {
              return 0;
            }
         }
       else
         {
           RF_FAUX_WARN("RX wait time missed %ld:%06ld", delta_t.tv_sec, delta_t.tv_usec);
         }
     }

   const int rc = readv(_info->rx_handle, iov, 2);

   if(rc <= 0)
     {
       RF_FAUX_WARN("RX reqlen %d, error %s", nb_req, strerror(errno));
     }
   else
     { 
       RF_FAUX_DBUG("RX ****** %d of %d ******", rc, nb_req);
     }

   return rc;
}


static int rf_faux_vecio_send(void *h, struct iovec iov[2])
{
   GET_FAUX_INFO(h);

   const int nb_req = iov[0].iov_len + iov[1].iov_len;

   int rc = writev(_info->tx_handle, iov, 2);

   if(rc < 0)
     {
       RF_FAUX_WARN("send error %s", strerror(errno));
     }
   else
     {
       RF_FAUX_DBUG("sent %d of %d", rc, nb_req);
     }

  return rc;
}



void rf_faux_tx_msg(rf_faux_tx_info_t * tx_info, uint64_t seqn)
{
   GET_FAUX_INFO(tx_info->h);

   const int nb_in = BYTES_X_SAMPLE(tx_info->nsamples);

   // resample to match the SR if needed
   const int ns_out = rf_faux_resample(_info->tx_srate,
                                        RF_FAUX_OTA_SRATE,
                                        tx_info->data,
                                        _info->sf_out, 
                                        tx_info->nsamples);

   const int nb_out = BYTES_X_SAMPLE(ns_out);

   struct timeval tv_now, tv_diff;

   gettimeofday(&tv_now, NULL);

   const rf_faux_iohdr_t hdr = { seqn, 
                                  nb_out,
                                  RF_FAUX_OTA_SRATE,
                                  tx_info->tx_time };

   struct iovec iov[2] = { {(void*)&(hdr),        sizeof(hdr)},
                           {(void*)_info->sf_out, nb_out     }};

   const int rc = rf_faux_vecio_send(_info, iov);

   if(rc <= 0)
     {
       RF_FAUX_WARN("send reqlen %d, error %s", nb_out, strerror(errno));
     }
   else
     {
       timersub(&tv_now, &(tx_info->tx_time), &tv_diff);

       if(timercmp(&tv_tx_window, &tv_diff, <))
         {
           RF_FAUX_WARN("TX overrun %ld:%06ld, in %u/%d, seqn %lu, out %d/%d", 
                         tv_diff.tv_sec,
                         tv_diff.tv_usec,
                         tx_info->nsamples,
                         nb_in,
                         seqn,
                         ns_out,
                         nb_out);
         }
       else
         {
           RF_FAUX_DBUG("TX in %u/%d, seqn %lu, out %d/%d", 
                        tx_info->nsamples,
                        nb_in,
                        seqn,
                        ns_out,
                        nb_out);
         }
     }
}



static void * rf_faux_tx_worker_proc(void * arg)
{
   rf_faux_tx_worker_t * tx_worker = (rf_faux_tx_worker_t*) arg;

   GET_FAUX_INFO(tx_worker->h);

   RF_FAUX_DBUG("tx_worker %d created, ready for duty", tx_worker->id);

   struct timeval tv_now, delta_t;

   bool running = true;

   while(running)
     {
       sem_wait(&(tx_worker->sem));

       gettimeofday(&tv_now, NULL);

       timersub(&(tx_worker->tx_info->tx_time), &tv_now, &delta_t);

       if(timercmp(&delta_t, &tv_zero, >))
         {
           RF_FAUX_DBUG("tx_worker %d, apply tx_delay %ld:%06ld", 
                         tx_worker->id,
                         delta_t.tv_sec,
                         delta_t.tv_usec);

           select(0, NULL, NULL, NULL, &delta_t);
         }
       else
         {
           timersub(&tv_now, &(tx_worker->tx_info->tx_time), &delta_t);

           RF_FAUX_WARN("tx_worker %d, skip tx_delay overrun %ld:%06ld", 
                         tx_worker->id,
                         delta_t.tv_sec,
                         delta_t.tv_usec);
         }

        pthread_mutex_lock(&(_info->tx_workers_lock));

        rf_faux_tx_msg(tx_worker->tx_info, (_info->tx_seqn)++);

        _info->nof_tx_workers -= 1;

        tx_worker->is_pending = false;

        pthread_mutex_unlock(&(_info->tx_workers_lock));
     }

   return NULL;
}


int rf_faux_set_sock_block(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
 
  if(flags < 0)
   {
     RF_FAUX_WARN("get flags error %s", strerror(errno));

     return -1;
   } 
  
  if(fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) < 0)
    {
      RF_FAUX_WARN("set flags error %s", strerror(errno));

      return -1;
    }
 
  return 0;
}



int rf_faux_set_sock_nonblock(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
 
  if(flags < 0)
   {
     RF_FAUX_WARN("get flags error %s", strerror(errno));

     return -1;
   } 
  
  if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
      RF_FAUX_WARN("set flags error %s", strerror(errno));

      return -1;
    }
 
  return 0;
}


static int rf_faux_set_sock_buff_size(int fd, const int n_req, const int opt)
{
  int len = 0;

  socklen_t optlen = sizeof(len);

  if(getsockopt(fd, SOL_SOCKET, opt, &len, &optlen) < 0) 
    {
      RF_FAUX_WARN("get size error %s", strerror(errno));

      return -1;
    }

  if(len < n_req)
   {
     len = n_req;

     if(setsockopt(fd, SOL_SOCKET, opt, &len, sizeof(len)) < 0) 
       {
         RF_FAUX_DBUG("set buf size to %d, %s", len, strerror(errno));

         return -1;
       }
     else
       {
         RF_FAUX_DBUG("reset buf size to %d", len);
       }
    }
   else
    {
      RF_FAUX_DBUG("keep curernt buf size %d", len);
    }

   return len;
}




static int rf_faux_open_sock(rf_faux_info_t * info, 
                             uint16_t rxport, 
                             uint16_t txport,
                             const char * mcaddr)
{
  int rx_fd, tx_fd, opt_on = 1;

  if((rx_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      RF_FAUX_WARN("rx sock open error %s", strerror(errno));

      return -1;
    }

  if((tx_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      RF_FAUX_WARN("tx sock open error %s", strerror(errno));

      return -1;
    }

  if(setsockopt(rx_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt_on, sizeof(opt_on)) < 0) 
    {
      RF_FAUX_WARN("rx sock set resue error %s", strerror(errno));

      return -1;
    }

  if(setsockopt(tx_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt_on, sizeof(opt_on)) < 0) 
    {
      RF_FAUX_WARN("rx sock set resue error %s", strerror(errno));

      return -1;
    }

  if(rf_faux_set_sock_buff_size(tx_fd, RF_FAUX_SOCK_BUFF_SIZE, SO_SNDBUF) < 0)
    {
      RF_FAUX_WARN("rx sock set rxbuf size error %s", strerror(errno));

      return -1;
    }

  if(rf_faux_set_sock_buff_size(rx_fd, RF_FAUX_SOCK_BUFF_SIZE, SO_RCVBUF) < 0)
    {
      RF_FAUX_WARN("rx sock set rxbuf size error %s", strerror(errno));

      return -1;
    }

  struct ifreq ifr;
  memset(&ifr,  0, sizeof(ifr));

  strncpy(ifr.ifr_name, RF_FAUX_MC_DEV, IFNAMSIZ);

  if(ioctl(tx_fd, SIOCGIFADDR, &ifr) < 0) 
    {
      RF_FAUX_WARN("tx sock get addr error %s", strerror(errno));

      return -1;
    }

  const in_addr_t if_addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;

  if(ioctl(tx_fd, SIOCGIFINDEX, &ifr) < 0)
    {
      RF_FAUX_WARN("tx sock get ifindex error %s", strerror(errno));

      return -1;
    }

  const int if_index = ifr.ifr_ifindex;

  if(setsockopt(tx_fd, SOL_SOCKET, SO_BSDCOMPAT, (char *) &opt_on, sizeof(opt_on)) < 0) 
    {
      RF_FAUX_WARN("rx sock set bsdcompat error %s", strerror(errno));

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
      RF_FAUX_WARN("rx sock bind error %s", strerror(errno));

      return -1;
    }

  if(setsockopt(rx_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) 
    {
      RF_FAUX_WARN("rx sock add membership error %s", strerror(errno));

      return -1;
    }
  else
    {
      RF_FAUX_INFO("joined group %s port %hu, iface %s", 
                     inet_ntoa(mreq.imr_multiaddr), rxport, RF_FAUX_MC_DEV);
    }


  if(connect(tx_fd,(struct sockaddr*)&sin_tx, sizeof(sin_tx)) < 0)
    {
      RF_FAUX_WARN("tx sock connect error %s", strerror(errno));

      return -1;
    }
  else
    {
      RF_FAUX_INFO("connect to group %s port %hu", 
                     inet_ntoa(mreq.imr_multiaddr), txport);
    }

  int no_df = IP_PMTUDISC_DONT;

  if(setsockopt(tx_fd, IPPROTO_IP, IP_MTU_DISCOVER, &no_df, sizeof(no_df)) < 0) 
   {
     RF_FAUX_WARN("tx sock set mtu opt error %s", strerror(errno));

     return -1;
   }

  struct ip_mreqn mreqn;
  memset(&mreqn, 0, sizeof(mreqn));
  mreqn.imr_multiaddr.s_addr = 0;
  mreqn.imr_address.s_addr   = if_addr;
  mreqn.imr_ifindex          = if_index;

  if(setsockopt(tx_fd, SOL_IP, IP_MULTICAST_IF, &mreqn, sizeof(mreqn)) < 0) 
   {
     RF_FAUX_WARN("tx sock set mcif error %s", strerror(errno));

     return -1; 
   }

  if(rf_faux_set_sock_nonblock(tx_fd) < 0)
    {
      RF_FAUX_WARN("tx sock set non block error %s", strerror(errno));

      return -1; 
    }

  // rx timeout block ue, no block enb
  if(rf_faux_is_ue(info))
    {
      info->rx_timeout = false;

      if(rf_faux_set_sock_block(rx_fd) < 0)
       {
         RF_FAUX_WARN("rx sock set block error %s", strerror(errno));

         return -1; 
       }
    }
  else
    {
      info->rx_timeout = true;

      if(rf_faux_set_sock_nonblock(rx_fd) < 0)
       {
         RF_FAUX_WARN("rx sock set non block error %s", strerror(errno));

         return -1; 
       }
    }

  info->tx_handle = tx_fd;
  info->rx_handle = rx_fd;

  return 0;
}


static int rf_faux_open_ipc(rf_faux_info_t * info)
{
  if(rf_faux_is_enb(info))
    {
      return rf_faux_open_sock(info, 
                                RF_FAUX_UL_PORT,  // rx
                                RF_FAUX_DL_PORT,  // tx
                                RF_FAUX_MC_ADDR);
    }
  else
    {
      return rf_faux_open_sock(info, 
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

   if(rf_faux_log_verbose)
   RF_FAUX_DBUG("");

   _info->rx_stream = true;

   pthread_mutex_unlock(&(_info->rx_lock));

   return 0;
 }


int rf_faux_stop_rx_stream(void *h)
 {
   GET_FAUX_INFO(h);

   pthread_mutex_lock(&(_info->rx_lock));

   if(rf_faux_log_verbose)
   RF_FAUX_DBUG("");

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
   if(rf_faux_log_verbose)
   RF_FAUX_DBUG("yes");

   return true;
 }


float rf_faux_get_rssi(void *h)
 {
   const float rssi = -60.0;

   RF_FAUX_INFO("rssi %4.3f", rssi);

   return rssi;
 }


void rf_faux_suppress_stdout(void *h)
 {
   rf_faux_log_verbose = false;
   //rf_faux_log_debug   = false;
   //rf_faux_log_info    = false;
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
   RF_FAUX_INFO("channels %u, args [%s]", nof_channels, args);

   if(strncmp(args, "enb", strlen("enb")) == 0)
    {
      rf_faux_info.ntype = RF_FAUX_NTYPE_ENB;
    }
   else if(strncmp(args, "ue", strlen("ue")) == 0)
    {
      rf_faux_info.ntype = RF_FAUX_NTYPE_UE;
    }
   else
    {
      RF_FAUX_INFO("default ntype is ue");

      rf_faux_info.ntype = RF_FAUX_NTYPE_UE;
    }
       
   if(nof_channels != 1)
    {
      RF_FAUX_INFO("only supporting 1 channel, not %d", nof_channels);

      return -1;
    }

   if(rf_faux_open_ipc(&rf_faux_info) < 0)
    {
      RF_FAUX_WARN("could not create ipc channel");

      return -1;
    }

   for(int id = 0; id < RF_FAUX_NOF_TX_WORKERS; ++id)
     {
       rf_faux_tx_worker_t * tx_worker =  &(rf_faux_info.tx_workers[id]);

       if(sem_init(&(tx_worker->sem), 0, 0) < 0)
         {
           RF_FAUX_WARN("could not initialize tx_worker semaphore %s", strerror(errno));

           return -1;
         }

       tx_worker->tx_info = malloc(sizeof(rf_faux_tx_info_t));

       if(tx_worker->tx_info == NULL)
        {
          RF_FAUX_WARN("FAILED to allocate memory for tx_worker %d", tx_worker->id);

          return -1;
        }

       tx_worker->is_pending = false;
       tx_worker->id      = id;
       tx_worker->h       = &rf_faux_info;

       if(pthread_create(&(tx_worker->tid), 
                         NULL, 
                         rf_faux_tx_worker_proc, 
                         tx_worker) < 0)
        {
           RF_FAUX_WARN("could not create tx_worker thread %s", strerror(errno));

           return -1;
        }

       const struct sched_param param = {10};
   
       const int policy = SCHED_RR;
       
       if(pthread_setschedparam(tx_worker->tid, policy, &param) < 0)
        {
           RF_FAUX_WARN("could not set tx_worker thread rt_priority %s", strerror(errno));

           return -1;
        }
       else
        {
           RF_FAUX_DBUG("set tx_worker thread priority/policy %d:%d", param.sched_priority, policy);
        }
     }

   *h = &rf_faux_info;

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

   RF_FAUX_INFO("rate %4.2lf MHz to %4.2lf MHz", 
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

   RF_FAUX_INFO("gain %3.2lf to %3.2lf", _info->rx_gain, gain);

   _info->rx_gain = gain;

   return _info->rx_gain;
 }


double rf_faux_set_tx_gain(void *h, double gain)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_INFO("gain %3.2lf to %3.2lf", _info->tx_gain, gain);

   _info->tx_gain = gain;

   return _info->tx_gain;
 }


double rf_faux_get_rx_gain(void *h)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_INFO("gain %3.2lf", _info->rx_gain);

   return _info->rx_gain;
 }


double rf_faux_get_tx_gain(void *h)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_INFO("gain %3.2lf", _info->tx_gain);

   return _info->tx_gain;
 }


double rf_faux_set_rx_srate(void *h, double rate)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_INFO("rate %4.2lf MHz to %4.2lf MHz", 
                 _info->rx_srate / 1e6, rate / 1e6);

   _info->rx_srate = rate;

   return _info->rx_srate;
 }


double rf_faux_set_tx_srate(void *h, double rate)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_INFO("freq %4.2lf MHz to %4.2lf MHz", 
                 _info->tx_srate / 1e6, rate / 1e6);

   _info->tx_srate = rate;

   return _info->tx_srate;
 }


double rf_faux_set_rx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_INFO("freq %4.2lf MHz to %4.2lf MHz", 
                 _info->rx_freq / 1e6, freq / 1e6);

   _info->rx_freq = freq;

   return _info->rx_freq;
 }


double rf_faux_set_tx_freq(void *h, double freq)
 {
   GET_FAUX_INFO(h);

   RF_FAUX_INFO("freq %4.2lf MHz to %4.2lf MHz", 
                 _info->tx_freq / 1e6, freq / 1e6);

   _info->tx_freq = freq;

   return _info->tx_freq;
 }


void rf_faux_get_time(void *h, time_t *full_secs, double *frac_secs)
 {
   struct timeval tv;

   gettimeofday(&tv, NULL);

   rf_faux_tv_to_ts(&tv, full_secs, frac_secs);
 }



int rf_faux_recv_with_time(void *h, void *data, uint32_t nsamples, 
                           bool blocking, time_t *full_secs, double *frac_secs)
 {
   GET_FAUX_INFO(h);

   pthread_mutex_lock(&(_info->rx_lock));

   struct timeval tv_in;

   gettimeofday(&tv_in, NULL);

   // sometimes we get a request for a few extra samples (1922 vs 1920) that 
   // throws off our pkt based stream
   RF_FAUX_NORM_SF_LEN(nsamples);

   const int nb_req = BYTES_X_SAMPLE(nsamples);

   int nb_pending = nb_req;

   int ns_pending = nsamples;

   rf_faux_iohdr_t hdr;

   uint8_t * p = (uint8_t *) data;

   struct timeval rx_time, rx_delay;

   RF_FAUX_DBUG("RX begin nreq %u/%d",
                  nsamples,
                  nb_req);

   memset(data, 0x0, nb_req);

   // read 1 sf
   const int nb_sf = BYTES_X_SAMPLE(RF_FAUX_OTA_SRATE / 1000);

   cf_t sf_in [RF_FAUX_SF_LEN];

   int rc = 0;

   int n_tries = rf_faux_is_ue(_info) ? 10 : 1;

   while(nb_pending > 0 && (n_tries-- > 0))
     {   
       memset(sf_in, 0x0, BYTES_X_SAMPLE(RF_FAUX_SF_LEN));

       struct iovec iov[2] = { {(void*)&hdr,  sizeof(hdr)},
                               {(void*)sf_in, nb_sf      }};

       rc = rf_faux_vecio_recv(h, iov);

       gettimeofday(&rx_time, NULL);

       timersub(&rx_time, &(hdr.tx_time), &rx_delay);

       if(rc <= 0)
        {
          if(rc < 0)
            {
              RF_FAUX_WARN("recv error %s", strerror(errno));
            }

           break;
        }
      else
       {
         const int nb_in = rc - sizeof(hdr);

         if(nb_in != hdr.msglen)
           {
             RF_FAUX_WARN("RX seqn %lu, len error expected %d, got %d, DROP", hdr.seqnum, hdr.msglen, nb_in);

             break;
           }
        
         const int ns_in  =  SAMPLES_X_BYTE(nb_in);

         const int ns_out = rf_faux_resample(hdr.srate,
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
             RF_FAUX_WARN("RX OOS msg, expected seqn %lu, got seqn %lu", this_seqn, hdr.seqnum);
           }

         if(timercmp(&rx_delay, &tv_step, >))
           {
             RF_FAUX_WARN("RX rc %d, %d/%d, rx_delay %ld:%06ld, seqn %lu, pending %d/%d",
                          rc,
                          ns_in, 
                          nb_in,
                          rx_delay.tv_sec,
                          rx_delay.tv_usec,
                          hdr.seqnum,
                          ns_pending,
                          nb_pending);
           }
         else
           {
             RF_FAUX_INFO("RX rc %d, %d/%d, rx_delay %ld:%06ld, seqn %lu, pending %d/%d",
                          rc,
                          ns_in, 
                          nb_in,
                          rx_delay.tv_sec,
                          rx_delay.tv_usec,
                          hdr.seqnum,
                          ns_pending,
                          nb_pending);
           }
       }
    }

   RF_FAUX_DBUG("RX nreq %d/%d, out %d/%d", 
                  nsamples,
                  nb_req, 
                  nsamples - ns_pending,
                  nb_req - nb_pending);

   pthread_mutex_unlock(&(_info->rx_lock));

   rf_faux_tv_to_ts(&tv_in, full_secs, frac_secs);

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
       RF_FAUX_WARN("FAILED tx_worker pool full %d, DROP", RF_FAUX_NOF_TX_WORKERS);

       return 0;
     }

   rf_faux_tx_worker_t * tx_worker = NULL;

   // get next available tx_worker
   while((tx_worker = &(_info->tx_workers[_info->tx_worker_next]))->is_pending)
    {
      if(rf_faux_log_verbose)
      RF_FAUX_DBUG("skipping pending tx_worker %d", tx_worker->id);

      RF_FAUX_SET_NEXT_WORKER(_info->tx_worker_next);
    }

   rf_faux_tx_info_t * e = tx_worker->tx_info;

   // set the abs tx time
   if(has_time_spec)
     {
       rf_faux_ts_to_tv(&(e->tx_time), full_secs, frac_secs);
     }
   else
     {
       gettimeofday(&(e->tx_time), NULL);
     }

   struct timeval tv_now, tv_diff;

   gettimeofday(&tv_now, NULL);

   timersub(&(e->tx_time), & tv_now, &tv_diff); 
  
   memcpy(e->data, data, BYTES_X_SAMPLE(nsamples));
   e->h          = h;
   e->nsamples   = nsamples;
   e->is_sob     = is_sob;
   e->is_eob     = is_eob;
   tx_worker->is_pending = true;

   _info->nof_tx_workers += 1;
 
   RF_FAUX_INFO("add tx_worker %d, time spec %s, tx_time %ld:%0.6lf, %d workers pending",
                  tx_worker->id,
                  RF_FAUX_BOOL_TO_STR(has_time_spec),
                  full_secs,
                  frac_secs,
                 _info->nof_tx_workers);

   sem_post(&(tx_worker->sem));

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

   RF_FAUX_INFO("gain %3.2lf, phase %3.2lf, I %3.2lf, Q %3.2lf", 
                 cal->dc_gain, 
                 cal->dc_phase, 
                 cal->iq_i,
                 cal->iq_q);
}


void rf_faux_set_rx_cal(void *h, srslte_rf_cal_t *cal)
{
   GET_FAUX_INFO(h);

   memcpy(&_info->rx_cal, cal, sizeof(srslte_rf_cal_t));

   RF_FAUX_INFO("gain %3.2lf, phase %3.2lf, I %3.2lf, Q %3.2lf", 
                 cal->dc_gain,
                 cal->dc_phase,
                 cal->iq_i,
                 cal->iq_q);
}
