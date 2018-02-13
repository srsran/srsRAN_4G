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
#include <sys/un.h>

#include <net/if.h>

#include "srslte/srslte.h"
#include "rf_sock_imp.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/resampling/resample_arb.h"

// J Giovatto Feb 12 2018
// run builtin tests with -a (args) ue or enb, default is loop, (loopback)

// suggest running non DEBUG_MODE with n_prb of 6,15 or 25 for starters
// build 'make clean && cmake -DCMAKE_BUILD_TYPE=Release ../ && make'
// noted n_prb 6 and 15 are not reliable when DEBUG_MODE is enabled 

// n_prb or 6 thru 50 have worked with the increasing cpu utilization
// your mileage will vary.
// 
// running as sudo/root will allow real time priorities to take effect
// and allow unix socket endpoint creation in /tmp
// 1) sudo ./srsepc ./epc.conf
// 2) sudo ./srsue ./ue.conf
// 3) sudo ./srsenb ./enb.conf

#ifdef DEBUG_MODE
static bool rf_sock_log_dbug  = true;
static bool rf_sock_log_info  = true;
static bool rf_sock_log_warn  = true;
#else
static bool rf_sock_log_dbug  = false;
static bool rf_sock_log_info  = true;
static bool rf_sock_log_warn  = true;
#endif

#define RF_SOCK_LOG_FMT "%02d:%02d:%02d.%06ld [SKRF] [%c] [%05hu] %s,  "

#define RF_SOCK_WARN(_fmt, ...) do {                                                                       \
                                 if(rf_sock_log_warn) {                                                    \
                                   struct timeval _tv_now;                                                 \
                                   struct tm _tm;                                                          \
                                   gettimeofday(&_tv_now, NULL);                                           \
                                   localtime_r(&_tv_now.tv_sec, &_tm);                                     \
                                   fprintf(stdout, RF_SOCK_LOG_FMT  _fmt "\n",                             \
                                           _tm.tm_hour,                                                    \
                                           _tm.tm_min,                                                     \
                                           _tm.tm_sec,                                                     \
                                           _tv_now.tv_usec,                                                \
                                           'W',                                                            \
                                           0,                                                              \
                                           __func__,                                                       \
                                           ##__VA_ARGS__);                                                 \
                                     }                                                                     \
                                 } while(0);


#define RF_SOCK_DBUG(_fmt, ...) do {                                                                       \
                                 if(rf_sock_log_dbug) {                                                    \
                                   struct timeval _tv_now;                                                 \
                                   struct tm _tm;                                                          \
                                   gettimeofday(&_tv_now, NULL);                                           \
                                   localtime_r(&_tv_now.tv_sec, &_tm);                                     \
                                   fprintf(stdout, RF_SOCK_LOG_FMT  _fmt "\n",                             \
                                           _tm.tm_hour,                                                    \
                                           _tm.tm_min,                                                     \
                                           _tm.tm_sec,                                                     \
                                           _tv_now.tv_usec,                                                \
                                           'D',                                                            \
                                           0,                                                              \
                                           __func__,                                                       \
                                           ##__VA_ARGS__);                                                 \
                                 }                                                                         \
                             } while(0);

#define RF_SOCK_INFO(_fmt, ...) do {                                                                       \
                                 if(rf_sock_log_info) {                                                    \
                                   struct timeval _tv_now;                                                 \
                                   struct tm _tm;                                                          \
                                   gettimeofday(&_tv_now, NULL);                                           \
                                   localtime_r(&_tv_now.tv_sec, &_tm);                                     \
                                   fprintf(stdout, RF_SOCK_LOG_FMT  _fmt "\n",                             \
                                           _tm.tm_hour,                                                    \
                                           _tm.tm_min,                                                     \
                                           _tm.tm_sec,                                                     \
                                           _tv_now.tv_usec,                                                \
                                           'I',                                                            \
                                           0,                                                              \
                                           __func__,                                                       \
                                           ##__VA_ARGS__);                                                 \
                                 }                                                                         \
                             } while(0);


#define RF_SOCK_LOG_FUNC_TODO printf("XXX_TODO file:%s func:%s line:%d XXX_TODO\n", \
                                     __FILE__,                                      \
                                     __func__,                                      \
                                     __LINE__);

// unix socket endpoints should have visibility for LXC container
// deployment of ue/enb.
// Not sure if VM's are viable here due to timing constraints.
#define RF_SOCK_SERVER_PATH "/tmp/srslte-ipc-server-sock"
#define RF_SOCK_CLIENT_PATH "/tmp/srslte-ipc-client-sock"

// bytes per sample
#define BYTES_PER_SAMPLE(x) ((x)*sizeof(cf_t))

// samples per byte
#define SAMPLES_PER_BYTE(x) ((x)/sizeof(cf_t))

// max sf len
#define RF_SOCK_MAX_SF_LEN (0x8000)

// max msg len in bytes
#define RF_SOCK_MAX_MSG_LEN (RF_SOCK_MAX_SF_LEN * sizeof(cf_t))

// node type
#define RF_SOCK_NTYPE_LOOP  (0)  // XXX TODO need more mileage w/test apps in loopback mode
#define RF_SOCK_NTYPE_UE    (1)  // currently only 1 ue and 1 enb per test deplyoment
                                 // no signal mixing implemented as of yet but open to ideas.
#define RF_SOCK_NTYPE_ENB   (2)

// tx_offset (delay) workers tti+4 and a few more
#define RF_SOCK_NOF_TX_WORKERS (8)
#define RF_SOCK_SET_NEXT_WORKER(x) ((x) = ((x) + 1) % RF_SOCK_NOF_TX_WORKERS)

// the idea here was to 'scale' timeout/timerdelay/offset and usleep() calls
// by say a factor of 10 to slow down relative time throughout the entire codebase.
// Meaning subframes are now 10 msec internally, externally thruput will suffer
// but for basic sanity testing should not be an immediate issue,
// found to be useful on limited resource (cpu) hardware or tracking logs/gdb.
#define FAUX_TIME_SCALE 1

#define RF_SOCK_IS_LATE(tv, s, u) ((tv).tv_sec > (s) || (tv).tv_usec > (u))

 // rx op before next tti each recv method should wait a fair amount of time
 // to recv any message in the current/correct tti.
 // smaller waits longer  TTI0  RRRRRRRRRRRRRRRAAAAA  TTI1
static const struct timeval tv_rx_abort = {0, 1000 * FAUX_TIME_SCALE / 4};
static const struct timeval tv_zero     = {0, 0};
static const struct timeval tv_tti_step = {0, FAUX_TIME_SCALE * 1000};

// tx msg info for work threads
typedef struct {
  void *   h;
  cf_t     cf_data[RF_SOCK_MAX_SF_LEN];
  int      nsamples;
  struct   timeval tx_time;
  bool     is_sob;
  bool     is_eob;
} rf_sock_tx_info_t;


// tx worker thread info
typedef struct {
  void *               h;
  pthread_t            tid;
  sem_t                sem;
  int                  id;
  rf_sock_tx_info_t *  tx_info;
  bool                 is_pending;
  bool                 is_running;
} rf_sock_tx_worker_t;


// rf dev info
typedef struct {
   char *               dev_name;
   int                  nodetype;
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
   int                  tx_handle;
   int                  rx_handle;
   int64_t              tx_seqn;
   pthread_mutex_t      rx_lock;
   pthread_mutex_t      tx_workers_lock;
   pthread_t            tx_tid;
   rf_sock_tx_worker_t  tx_workers[RF_SOCK_NOF_TX_WORKERS];
   int                  tx_worker_next;
   int                  tx_nof_workers;
   struct timeval       tv_sos;   // start of stream
   struct timeval       tv_next_tti;
   int                  rx_n_tries;
   size_t               tx_errors;
   struct sockaddr_un   tx_addr;
   cf_t *               sf_in;
} rf_sock_info_t;


// tx msg meta data
typedef struct {
  uint64_t       io_seqnum;
  uint32_t       io_nbytes;
  float          io_srate;
  float          io_gain;
  struct timeval io_time;
} rf_sock_iohdr_t;


void rf_sock_suppress_stdout(void *h)
 {
    rf_sock_log_dbug = false;
    rf_sock_log_info = false;
    rf_sock_log_warn = true;
 }


static void rf_sock_handle_error(srslte_rf_error_t error)
{
  // XXX TODO make more use of this handler
  RF_SOCK_INFO("%s:%s type %s, opt %d, msg %s\b", 
                error.type == SRSLTE_RF_ERROR_LATE      ? "late"      :
                error.type == SRSLTE_RF_ERROR_UNDERFLOW ? "underflow" :
                error.type == SRSLTE_RF_ERROR_OVERFLOW  ? "overflow"  :
                error.type == SRSLTE_RF_ERROR_OTHER     ? "other"     :
                "unknown error");
}


static  rf_sock_info_t rf_sock_info = { .dev_name        = "sockrf",
                                        .nodetype        = RF_SOCK_NTYPE_LOOP,
                                        .rx_gain         = 0.0,
                                        .tx_gain         = 0.0,
                                        .rx_srate        = 0.0,
                                        .tx_srate        = 0.0,
                                        .rx_freq         = 0.0,
                                        .tx_freq         = 0.0,
                                        .rx_cal          = {0.0, 0.0, 0.0, 0.0},
                                        .tx_cal          = {0.0, 0.0, 0.0, 0.0},
                                        .clock_rate      = 0.0,
                                        .error_handler   = rf_sock_handle_error,
                                        .rx_stream       = false,
                                        .tx_handle       = -1,
                                        .rx_handle       = -1,
                                        .tx_seqn         = 0,
                                        .rx_lock         = PTHREAD_MUTEX_INITIALIZER,
                                        .tx_workers_lock = PTHREAD_MUTEX_INITIALIZER,
                                        .tx_worker_next  = 0,
                                        .tx_nof_workers  = 0,
                                        .tv_sos          = {0,0},
                                        .tv_next_tti     = {0,0},
                                        .rx_n_tries      = 0,
                                        .tx_errors       = 0,
                                        .tx_addr         = {0,{0}},
                                        .sf_in           = NULL,
                                      };

#define GET_DEV_INFO(h)  assert(h); rf_sock_info_t *_info = (rf_sock_info_t *)(h)

static void rf_sock_tv_to_ts(const struct timeval *tv, time_t *full_secs, double *frac_secs)
{
  if(full_secs && frac_secs)
    {
      *full_secs = tv->tv_sec; 
      *frac_secs = tv->tv_usec / 1.0e6;
    }
}


static void rf_sock_ts_to_tv(struct timeval *tv, time_t full_secs, double frac_secs)
{
  if(tv)
    {
      tv->tv_sec  = full_secs;
      tv->tv_usec = frac_secs * 1.0e6;
    }
}


static int rf_sock_resample(double srate_in, 
                            double srate_out, 
                            void * in, 
                            void * out, 
                            int nsamples_in)
{
  if(srate_in != srate_out)
   {
     const double sratio = srate_out / srate_in;

     // have noticed that when 'upsample' unable to sync
     if(sratio > 1.0)
      { 
        RF_SOCK_WARN("srate %4.2lf/%4.2lf MHz, sratio %3.3lf, upsample may not decode",
                     srate_in  / 1e6,
                     srate_out / 1e6,
                     sratio);
      }

     srslte_resample_arb_t r;

     srslte_resample_arb_init(&r, sratio, 0);

     return srslte_resample_arb_compute(&r, (cf_t*)in, (cf_t*)out, nsamples_in);
   }
 else
   {
     // no changes, samples in == samples out
     memcpy(out, in, BYTES_PER_SAMPLE(nsamples_in));

     return nsamples_in;
   }
}


static bool rf_sock_is_loopback(rf_sock_info_t * info)
{
  return (info->nodetype == RF_SOCK_NTYPE_LOOP);
}


static bool rf_sock_is_enb(rf_sock_info_t * info)
{
  return (info->nodetype == RF_SOCK_NTYPE_ENB);
}


static bool rf_sock_is_ue(rf_sock_info_t * info)
{
  return (info->nodetype == RF_SOCK_NTYPE_UE);
}



static int rf_sock_vecio_recv(void *h, struct iovec iov[2])
{
   GET_DEV_INFO(h);

   fd_set rfds;

   FD_ZERO(&rfds);

   FD_SET(_info->rx_handle, &rfds);

   struct timeval tv_delay;

   // enb or loopmode are next tti aware
   if(rf_sock_is_enb(&rf_sock_info) || rf_sock_is_loopback(&rf_sock_info))
     {
       struct timeval tv_now, tv_timeout;

       timersub(&_info->tv_next_tti, &tv_rx_abort, &tv_timeout);

       gettimeofday(&tv_now, NULL);

       timersub(&tv_timeout, &tv_now, &tv_delay);
     }
   // at this point ue is just along for the ride as samples arrive
   // or 1 sf timeout
   else
    {
       tv_delay = tv_tti_step;
    }

   // within rx window, briefly wait for UL msg
   // if too late, then go ahead and do a non blocking read
   // and see what we can grab
   if(timercmp(&tv_delay, &tv_zero, >))
     {
       if(select(_info->rx_handle + 1, &rfds, NULL, NULL, &tv_delay) <= 0 ||
                (! FD_ISSET(_info->rx_handle, &rfds)))
        {
          // nothing to read
          return 0;
       }
     }

   const int rc = readv(_info->rx_handle, iov, 2);

   if(rc < 0)
     {
       RF_SOCK_WARN("RX reqlen %d, error %s", iov[0].iov_len + iov[1].iov_len, strerror(errno));
     }

   return rc;
}




void rf_sock_send_msg(rf_sock_tx_worker_t * worker, uint64_t seqn)
{
   GET_DEV_INFO(worker->tx_info->h);

   rf_sock_tx_info_t * tx_info = worker->tx_info;

   const int nbytes_out = BYTES_PER_SAMPLE(tx_info->nsamples);

   const rf_sock_iohdr_t hdr = { seqn, 
                                 nbytes_out,
                                 _info->tx_srate,
                                 _info->tx_gain,
                                 tx_info->tx_time,
                               };

   struct iovec iov[2] = { {(void*)&hdr,             sizeof(hdr)},
                           {(void*)tx_info->cf_data, nbytes_out }
                         };

   struct msghdr mhdr = { &_info->tx_addr,      
                          sizeof(_info->tx_addr),
                          iov,                  
                          2,
                          NULL,
                          0,
                          0
                        };

   // unix socket blocking should help keep tx/rx in sync 
   const int rc = sendmsg(_info->tx_handle, &mhdr, 0);

   if(rc != (iov[0].iov_len + iov[1].iov_len))
     {
       if(errno == ENOTCONN || errno == ECONNREFUSED || errno == EAGAIN)
         {
           if(! (++_info->tx_errors % 1000))
             {
               RF_SOCK_WARN("semdmsg, peer not connected, please re-start UE");
             }
         }
       else if(errno == EPERM || errno == EACCES)
        {
          RF_SOCK_WARN("sendmsg error %s, check file permission and sudo priviledge, shutting down now", 
                       strerror(errno));
        
          exit(0);
        }
      else
        {
           RF_SOCK_WARN("sendmsg error %s, shutting down now", strerror(errno));

           exit(0);
        }
     }
}



static void * rf_sock_tx_worker_proc(void * arg)
{
   rf_sock_tx_worker_t * worker = (rf_sock_tx_worker_t*) arg;

   GET_DEV_INFO(worker->h);

   struct timeval tv_now, tv_delay;

   while(worker->is_running)
     {
       sem_wait(&worker->sem);

       gettimeofday(&tv_now, NULL);

       timersub(&worker->tx_info->tx_time, &tv_now, &tv_delay);

       if(timercmp(&tv_delay, &tv_zero, >))
         {
           select(0, NULL, NULL, NULL, &tv_delay);
         }

       pthread_mutex_lock(&_info->tx_workers_lock);

       _info->tx_nof_workers -= 1;

#ifdef DEBUG_MODE
       RF_SOCK_DBUG("fire tx_worker %02d, %d tx_workers pending",
                     worker->id,
                    _info->tx_nof_workers);
#endif

       rf_sock_send_msg(worker, _info->tx_seqn++);

       worker->is_pending = false;

       pthread_mutex_unlock(&_info->tx_workers_lock);
    }     

   return NULL;
}


int rf_sock_set_sock_block(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
 
  if(flags < 0)
   {
     RF_SOCK_WARN("get flags error %s", strerror(errno));

     return -1;
   } 
  
  if(fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) < 0)
    {
      RF_SOCK_WARN("set flags error %s", strerror(errno));

      return -1;
    }
 
  return 0;
}



int rf_sock_set_sock_nonblock(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
 
  if(flags < 0)
   {
     RF_SOCK_WARN("get flags error %s", strerror(errno));

     return -1;
   } 
  
  if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
      RF_SOCK_WARN("set flags error %s", strerror(errno));

      return -1;
    }
 
  return 0;
}


static int rf_sock_open_unix_sock(rf_sock_info_t * info, 
                                  const char * local,
                                  const char * remote)
{
  struct sockaddr_un lcl_addr;

  int rx_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

  if(rx_fd < 0)
    {
      RF_SOCK_WARN("unix sock open error %s", strerror(errno));

      return -1;
    }

  int tx_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

  if(tx_fd < 0)
    {
      RF_SOCK_WARN("unix sock open error %s", strerror(errno));

      return -1;
    }

  memset(&lcl_addr, 0, sizeof(lcl_addr));
  lcl_addr.sun_family = AF_UNIX;
  strncpy(lcl_addr.sun_path, local, strlen(local));

  if(unlink(local) < 0)
    {
      if(errno == EPERM || errno == EACCES)
        {
          RF_SOCK_WARN("unlink %s error %s, check file permission and sudo priviledge", 
                       local, strerror(errno));
        
          return -1;
        }
      else if(errno != ENOENT)
        {
          RF_SOCK_WARN("unlink %s error %s", local, strerror(errno));

          return -1;
        }
    }

  if(bind(rx_fd, (struct sockaddr *) &lcl_addr, sizeof(lcl_addr)) < 0)
    {
      RF_SOCK_WARN("unix sock bind %s error %s", local, strerror(errno));

      return -1;
    }
  else
    {
      RF_SOCK_INFO("unix sock bind to %s", local);
    }

  memset(&info->tx_addr, 0, sizeof(info->tx_addr));
  info->tx_addr.sun_family = AF_UNIX;
  strncpy(info->tx_addr.sun_path, remote, strlen(remote));

  RF_SOCK_INFO("unix sock send to %s", remote);

  info->tx_handle = tx_fd;
  info->rx_handle = rx_fd;

  return 0;
}


static int rf_sock_open_ipc(rf_sock_info_t * info)
{
  if(rf_sock_is_enb(info))
    {
      return rf_sock_open_unix_sock(info,
                                    RF_SOCK_SERVER_PATH,
                                    RF_SOCK_CLIENT_PATH);
    }
  else if(rf_sock_is_ue(info))
    {
      return rf_sock_open_unix_sock(info,
                                    RF_SOCK_CLIENT_PATH, 
                                    RF_SOCK_SERVER_PATH);
    }
  else
    {
      return rf_sock_open_unix_sock(info,
                                    RF_SOCK_CLIENT_PATH, 
                                    RF_SOCK_CLIENT_PATH);
    }
}



char* rf_sock_devname(void *h)
 {
   GET_DEV_INFO(h);

   return _info->dev_name;
 }


bool rf_sock_rx_wait_lo_locked(void *h)
 {
   RF_SOCK_LOG_FUNC_TODO;

   return false;
 }


int rf_sock_start_rx_stream(void *h, bool now)
 {
   GET_DEV_INFO(h);
   
   pthread_mutex_lock(&_info->rx_lock);

   gettimeofday(&_info->tv_sos, NULL);

   // aligin time on the top of the second
   usleep(1000000 - _info->tv_sos.tv_usec);
   _info->tv_sos.tv_sec += 1; 
   _info->tv_sos.tv_usec = 0;

   // set next tti time
   timeradd(&_info->tv_sos, &tv_tti_step, &_info->tv_next_tti);

   RF_SOCK_INFO("begin rx stream, time_0 %ld:%06ld, next_tti %ld:%06ld", 
                 _info->tv_sos.tv_sec, 
                 _info->tv_sos.tv_usec,
                 _info->tv_next_tti.tv_sec, 
                 _info->tv_next_tti.tv_usec);

   _info->rx_stream = true;

   pthread_mutex_unlock(&_info->rx_lock);

   return 0;
 }


int rf_sock_stop_rx_stream(void *h)
 {
   GET_DEV_INFO(h);

   pthread_mutex_lock(&_info->rx_lock);

   RF_SOCK_INFO("end rx stream");

   _info->rx_stream = false;

   pthread_mutex_unlock(&_info->rx_lock);

   return 0;
 }


void rf_sock_flush_buffer(void *h)
 {
   RF_SOCK_LOG_FUNC_TODO;
 }


bool rf_sock_has_rssi(void *h)
 {
   RF_SOCK_DBUG("yes");

   return true;
 }


float rf_sock_get_rssi(void *h)
 {
   const float rssi = -50.0;  // XXX TODO what value ???

   RF_SOCK_DBUG("rssi %4.3f", rssi);

   return rssi;
 }


void rf_sock_register_error_handler(void *h, srslte_rf_error_handler_t error_handler)
 {
   GET_DEV_INFO(h);

   _info->error_handler = error_handler;
 }


int rf_sock_open(char *args, void **h)
 {
   return rf_sock_open_multi(args, h, 1);
 }


int rf_sock_open_multi(char *args, void **h, uint32_t nof_channels)
 {
   RF_SOCK_INFO("channels %u, args [%s]", nof_channels, args ? args : "none");

   if(args && strncmp(args, "enb", strlen("enb")) == 0)
    {
      rf_sock_info.nodetype = RF_SOCK_NTYPE_ENB;
    }
   else if(args && strncmp(args, "ue", strlen("ue")) == 0)
    {
      rf_sock_info.nodetype = RF_SOCK_NTYPE_UE;
    }
   else
    {
      RF_SOCK_INFO("default nodetype is loopback");

      rf_sock_info.nodetype = RF_SOCK_NTYPE_LOOP;
    }
       
   if(nof_channels != 1)
    {
      RF_SOCK_INFO("only supporting 1 channel, not %d", nof_channels);

      return -1;
    }

   if(rf_sock_open_ipc(&rf_sock_info) < 0)
    {
      RF_SOCK_WARN("could not create ipc channel");

      return -1;
    }

   if(rf_sock_is_ue(&rf_sock_info))
    {
      // added tries for initial sync
      rf_sock_info.rx_n_tries = 25;
    }
   else if(rf_sock_is_enb(&rf_sock_info) || rf_sock_is_loopback(&rf_sock_info))
    {
      // one shot on rx
      rf_sock_info.rx_n_tries = 1;
    }

   // set rx to non block, will use select w/timeout to throttle
   if(rf_sock_set_sock_nonblock(rf_sock_info.rx_handle) < 0)
     {
       RF_SOCK_WARN("rx sock set non block error %s", strerror(errno));

       return -1; 
     }

   for(int id = 0; id < RF_SOCK_NOF_TX_WORKERS; ++id)
     {
       rf_sock_tx_worker_t * worker =  &rf_sock_info.tx_workers[id];

       if(sem_init(&worker->sem, 0, 0) < 0)
         {
           RF_SOCK_WARN("could not initialize worker semaphore %s", strerror(errno));

           return -1;
         }

       worker->tx_info = malloc(sizeof(rf_sock_tx_info_t));

       if(worker->tx_info == NULL)
        {
          RF_SOCK_WARN("FAILED to allocate memory for worker %02d", worker->id);

          return -1;
        }

       memset(worker->tx_info, 0x0, sizeof(rf_sock_tx_info_t));

       worker->is_pending = false;
       worker->id         = id;
       worker->h          = &rf_sock_info;
       worker->is_running = true;

       if(pthread_create(&worker->tid, 
                         NULL, 
                         rf_sock_tx_worker_proc, 
                         worker) < 0)
        {
           RF_SOCK_WARN("could not create worker thread %s", strerror(errno));

           return -1;
        }

       const int policy = SCHED_FIFO;

       const struct sched_param param = {sched_get_priority_max(policy) - 1}; // XXX what prio ???
       
       if(pthread_setschedparam(worker->tid, policy, &param) < 0)
        {
           RF_SOCK_WARN("could not set worker thread rt_priority %s", strerror(errno));

           return -1;
        }
       else
        {
           RF_SOCK_DBUG("set worker thread priority/policy %d:%d", param.sched_priority, policy);
        }
     }

   rf_sock_info.sf_in = malloc (sizeof(cf_t) * RF_SOCK_MAX_SF_LEN);

   if(rf_sock_info.sf_in == NULL)
     {
       RF_SOCK_WARN("FAILED to allocate memory for input buffer");

       return -1;
     }

   *h = &rf_sock_info;

   return 0;
 }


int rf_sock_close(void *h)
 {
   GET_DEV_INFO(h);

   close(_info->rx_handle);

   close(_info->tx_handle);

   return 0;
 }


void rf_sock_set_master_clock_rate(void *h, double rate)
 {
   GET_DEV_INFO(h);

   RF_SOCK_INFO("rate %4.2lf MHz to %4.2lf MHz", 
                 _info->clock_rate / 1e6, rate / 1e6);

   _info->clock_rate = rate;
 }


bool rf_sock_is_master_clock_dynamic(void *h)
 {
   RF_SOCK_LOG_FUNC_TODO;

   return false;
 }


double rf_sock_set_rx_gain(void *h, double gain)
 {
   GET_DEV_INFO(h);

   RF_SOCK_INFO("gain %3.2lf to %3.2lf", _info->rx_gain, gain);

   _info->rx_gain = gain;

   return _info->rx_gain;
 }


double rf_sock_set_tx_gain(void *h, double gain)
 {
   GET_DEV_INFO(h);

   RF_SOCK_INFO("gain %3.2lf to %3.2lf", _info->tx_gain, gain);

   _info->tx_gain = gain;

   return _info->tx_gain;
 }


double rf_sock_get_rx_gain(void *h)
 {
   GET_DEV_INFO(h);

   RF_SOCK_INFO("gain %3.2lf", _info->rx_gain);

   return _info->rx_gain;
 }


double rf_sock_get_tx_gain(void *h)
 {
   GET_DEV_INFO(h);

   RF_SOCK_INFO("gain %3.2lf", _info->tx_gain);

   return _info->tx_gain;
 }


double rf_sock_set_rx_srate(void *h, double rate)
 {
   GET_DEV_INFO(h);

   RF_SOCK_INFO("srate %4.2lf MHz to %4.2lf MHz", 
                 _info->rx_srate / 1e6, rate / 1e6);

   _info->rx_srate = rate;

   return _info->rx_srate;
 }


double rf_sock_set_tx_srate(void *h, double rate)
 {
   GET_DEV_INFO(h);

   RF_SOCK_INFO("srate %4.2lf MHz to %4.2lf MHz", 
                 _info->tx_srate / 1e6, rate / 1e6);

   _info->tx_srate = rate;

   return _info->tx_srate;
 }


double rf_sock_set_rx_freq(void *h, double freq)
 {
   GET_DEV_INFO(h);

   RF_SOCK_INFO("freq %4.2lf MHz to %4.2lf MHz", 
                 _info->rx_freq / 1e6, freq / 1e6);

   _info->rx_freq = freq;

   return _info->rx_freq;
 }


double rf_sock_set_tx_freq(void *h, double freq)
 {
   GET_DEV_INFO(h);

   RF_SOCK_INFO("freq %4.2lf MHz to %4.2lf MHz", 
                 _info->tx_freq / 1e6, freq / 1e6);

   _info->tx_freq = freq;

   return _info->tx_freq;
 }


void rf_sock_get_time(void *h, time_t *full_secs, double *frac_secs)
 {
   struct timeval tv;

   gettimeofday(&tv, NULL);

   rf_sock_tv_to_ts(&tv, full_secs, frac_secs);
 }



int rf_sock_recv_with_time(void *h, void *data, uint32_t nsamples, 
                           bool blocking, time_t *full_secs, double *frac_secs)
 {
   GET_DEV_INFO(h);

   struct timeval tv_in;

   gettimeofday(&tv_in, NULL);

   // throttle back enb for each new tti here
   if(rf_sock_is_enb(_info) || rf_sock_is_loopback(_info))
     {
       struct timeval tv_delay;

       timersub(&_info->tv_next_tti, &tv_in, &tv_delay);

       if(timercmp(&tv_delay, &tv_zero, >))
        {
          select(0, NULL, NULL, NULL, &tv_delay);
        }

#ifdef DEBUG_MODE
        struct timeval tv_now, tv_diff;

        gettimeofday(&tv_now, NULL);

        timersub(&tv_now, &_info->tv_next_tti, &tv_diff);

        if(RF_SOCK_IS_LATE(tv_diff, 1, 1000))
         {
           RF_SOCK_WARN("tv_tti %ld:%06ld, overrun %ld:%06ld",
                         _info->tv_next_tti.tv_sec,
                         _info->tv_next_tti.tv_usec,
                         tv_diff.tv_sec,
                         tv_diff.tv_usec);
         }
#endif

        do{ // makeup any tti over runs here
           timeradd(&_info->tv_next_tti, &tv_tti_step, &_info->tv_next_tti);
        } while(timercmp(&tv_in, &_info->tv_next_tti, >=));
     }

   // set tv_rx_timestamp to time now (this tti)
   struct timeval tv_rx_timestamp;

   gettimeofday(&tv_rx_timestamp, NULL);

   int nbytes_pending = BYTES_PER_SAMPLE(nsamples);

   int nsamples_pending = nsamples;

   rf_sock_iohdr_t hdr;

   int n_tries = _info->rx_n_tries;

   uint8_t * p2data = (uint8_t *)data;

   // allow for slight mis-match between rx/tx 1922 vs 1920 etc
   while(((float)nsamples_pending > (.01f * nsamples)) && (n_tries--) > 0)
     {   
       struct iovec iov[2] = { {(void*)&hdr,  sizeof(hdr) },
                               {(void*)_info->sf_in, RF_SOCK_MAX_MSG_LEN }};

       const int rc = rf_sock_vecio_recv(h, iov);

       if(rc < 0)
        {
          // error bail out
          goto rxout;
        }
       else if(rc == 0)
        {
          // select timed out continue until tries exhausted
        }
       else
        {
          const int nbytes_this_msg = rc - sizeof(hdr);

          if(nbytes_this_msg != hdr.io_nbytes)
           {
             RF_SOCK_WARN("RX seqn %lu, msglen error expected %d, got %d, drop", 
                           hdr.io_seqnum, hdr.io_nbytes, nbytes_this_msg);

             // error bail out
             goto rxout;
           }

         // use tx time tag as the rx time to help align tti boundry
         tv_rx_timestamp = hdr.io_time;

         // works for downsample, have not able to sync when upsample
         const int nsamples_out = rf_sock_resample(hdr.io_srate,
                                                   _info->rx_srate,
                                                   _info->sf_in,
                                                   p2data, 
                                                   SAMPLES_PER_BYTE(nbytes_this_msg));

         const int nbytes_out = BYTES_PER_SAMPLE(nsamples_out);

         p2data += nbytes_out;

         nbytes_pending -= nbytes_out;

         nsamples_pending -= nsamples_out;
       }
    }

rxout:
   rf_sock_tv_to_ts(&tv_rx_timestamp, full_secs, frac_secs);

#ifdef DEBUG_MODE
   RF_SOCK_DBUG("RX req %u, pending %d/%d", 
                 nsamples,
                 nsamples_pending, 
                 nbytes_pending);
#endif

   // we always just return what was asked for
   // enb doenst seem to care and ue is not happy with 0
   return nsamples;
 }



int rf_sock_recv_with_time_multi(void *h, void **data, uint32_t nsamples, 
                                 bool blocking, time_t *full_secs, double *frac_secs)
{
   return rf_sock_recv_with_time(h, 
                                 data[0],
                                 nsamples, 
                                 blocking,
                                 full_secs,
                                 frac_secs);
}



int rf_sock_send_timed(void *h, void *data, int nsamples,
                       time_t full_secs, double frac_secs, bool has_time_spec,
                       bool blocking, bool is_sob, bool is_eob)
{
   if(nsamples <= 0)
     {
       RF_SOCK_WARN("msg len %d, sob %d, eob %d, ignore?", nsamples, is_sob, is_eob);

       return 0;
     }

   GET_DEV_INFO(h);

   pthread_mutex_lock(&(_info->tx_workers_lock));

   if(_info->tx_nof_workers >= RF_SOCK_NOF_TX_WORKERS)
     {
       RF_SOCK_WARN("FAILED worker pool full %d, drop", RF_SOCK_NOF_TX_WORKERS);

       goto txout;
     }

   rf_sock_tx_worker_t * worker = NULL;

   // get next available worker
   while((worker = &_info->tx_workers[_info->tx_worker_next])->is_pending)
    {
      RF_SOCK_SET_NEXT_WORKER(_info->tx_worker_next);
    }

   rf_sock_tx_info_t * tx_info = worker->tx_info;

   // set the abs tx time
   if(has_time_spec)
     {
       rf_sock_ts_to_tv(&tx_info->tx_time, full_secs, frac_secs);
     }
   else
     {
       gettimeofday(&(tx_info->tx_time), NULL);
     }

   memcpy(tx_info->cf_data, data, BYTES_PER_SAMPLE(nsamples));
   tx_info->h          = h;
   tx_info->nsamples   = nsamples;
   tx_info->is_sob     = is_sob;
   tx_info->is_eob     = is_eob;
   worker->is_pending = true;

   _info->tx_nof_workers += 1;
 
#ifdef DEBUG_MODE
   RF_SOCK_DBUG("add tx_worker %02d, time spec %s, tx_time %ld:%0.6lf, %d tx_workers pending",
                  worker->id,
                  has_time_spec ? "yes" : "no",
                  full_secs,
                  frac_secs,
                 _info->tx_nof_workers);
#endif

   sem_post(&worker->sem);

txout:
   pthread_mutex_unlock(&(_info->tx_workers_lock));

   return nsamples;
}


int rf_sock_send_timed_multi(void *h, void *data[4], int nsamples,
                             time_t full_secs, double frac_secs, bool has_time_spec,
                             bool blocking, bool is_sob, bool is_eob)
{
  return rf_sock_send_timed(h, 
                            data[0], 
                            nsamples,
                            full_secs,
                            frac_secs,
                            has_time_spec,
                            blocking,
                            is_sob,
                            is_eob);
}


void rf_sock_set_tx_cal(void *h, srslte_rf_cal_t *cal)
{
   GET_DEV_INFO(h);

   memcpy(&(_info->tx_cal), cal, sizeof(srslte_rf_cal_t));

   RF_SOCK_INFO("gain %3.2lf, phase %3.2lf, I %3.2lf, Q %3.2lf", 
                 cal->dc_gain, 
                 cal->dc_phase, 
                 cal->iq_i,
                 cal->iq_q);
}


void rf_sock_set_rx_cal(void *h, srslte_rf_cal_t *cal)
{
   GET_DEV_INFO(h);

   memcpy(&(_info->rx_cal), cal, sizeof(srslte_rf_cal_t));

   RF_SOCK_INFO("gain %3.2lf, phase %3.2lf, I %3.2lf, Q %3.2lf", 
                 cal->dc_gain,
                 cal->dc_phase,
                 cal->iq_i,
                 cal->iq_q);
}
