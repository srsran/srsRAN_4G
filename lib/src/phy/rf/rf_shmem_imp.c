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
#include <sys/ipc.h>
#include <sys/shm.h>

#include "srslte/srslte.h"
#include "rf_shmem_imp.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/resampling/resample_arb.h"

// J Giovatto Aug 08, 2018
// suggest running non DEBUG_MODE with n_prb of 15 or 25 anything higher has not 
// been able to achieve sync, yet ...
//
// changes to ue and enb conf resp:
// device_name = shmemrf
// device_args = ue or enb

// the device stop routine may not be called so check ipcs for orphaned shared mem segments
// cleaup using key_id:
//  sudo ipcrm -M 0x552a58cf
//  sudo ipcrm -M 0x442a58cf

// running as sudo/root will allow real time priorities to take effect
// 1) sudo ./srsepc ./epc.conf
// 2) sudo ./srsue  ./ue.conf
// 3) sudo ./srsenb ./enb.conf

// define to allow debug
// #define RF_SHMEM_DEBUG_MODE

#ifdef RF_SHMEM_DEBUG_MODE
static bool rf_shmem_log_dbug  = true;
static bool rf_shmem_log_info  = true;
static bool rf_shmem_log_warn  = true;
#else
static bool rf_shmem_log_dbug  = false;
static bool rf_shmem_log_info  = true;
static bool rf_shmem_log_warn  = true;
#endif

static char rf_shmem_node_type[2] = {0};

#define RF_SHMEM_LOG_FMT "%02d:%02d:%02d.%06ld [SRF.%s] [%c] %s,  "


#define RF_SHMEM_WARN(_fmt, ...) do {                                                   \
                                 if(rf_shmem_log_warn) {                                \
                                   struct timeval _tv_now;                              \
                                   struct tm _tm[1];                                    \
                                   gettimeofday(&_tv_now, NULL);                        \
                                   localtime_r(&_tv_now.tv_sec, &_tm[0]);               \
                                   fprintf(stdout, RF_SHMEM_LOG_FMT _fmt "\n",          \
                                           _tm[0].tm_hour,                              \
                                           _tm[0].tm_min,                               \
                                           _tm[0].tm_sec,                               \
                                           _tv_now.tv_usec,                             \
                                           rf_shmem_node_type,                          \
                                           'W',                                         \
                                           __func__,                                    \
                                           ##__VA_ARGS__);                              \
                                     }                                                  \
                                 } while(0);


#define RF_SHMEM_DBUG(_fmt, ...) do {                                                   \
                                 if(rf_shmem_log_dbug) {                                \
                                   struct timeval _tv_now;                              \
                                   struct tm _tm[1];                                    \
                                   gettimeofday(&_tv_now, NULL);                        \
                                   localtime_r(&_tv_now.tv_sec, &_tm[0]);               \
                                   fprintf(stdout, RF_SHMEM_LOG_FMT _fmt "\n",          \
                                           _tm[0].tm_hour,                              \
                                           _tm[0].tm_min,                               \
                                           _tm[0].tm_sec,                               \
                                           _tv_now.tv_usec,                             \
                                           rf_shmem_node_type,                          \
                                           'D',                                         \
                                           __func__,                                    \
                                           ##__VA_ARGS__);                              \
                                 }                                                      \
                             } while(0);

#define RF_SHMEM_INFO(_fmt, ...) do {                                                   \
                                 if(rf_shmem_log_info) {                                \
                                   struct timeval _tv_now;                              \
                                   struct tm _tm[1];                                    \
                                   gettimeofday(&_tv_now, NULL);                        \
                                   localtime_r(&_tv_now.tv_sec, &_tm[0]);               \
                                   fprintf(stdout, RF_SHMEM_LOG_FMT _fmt "\n",          \
                                           _tm[0].tm_hour,                              \
                                           _tm[0].tm_min,                               \
                                           _tm[0].tm_sec,                               \
                                           _tv_now.tv_usec,                             \
                                           rf_shmem_node_type,                          \
                                           'I',                                         \
                                           __func__,                                    \
                                           ##__VA_ARGS__);                              \
                                 }                                                      \
                             } while(0);


#define RF_SHMEM_LOG_FUNC_TODO fprintf(stderr, "XXX_TODO file:%s func:%s line:%d XXX_TODO\n", \
                                       __FILE__,                                              \
                                       __func__,                                              \
                                       __LINE__);

// bytes per sample
#define RF_SHMEM_BYTES_X_SAMPLE(x) ((x)*sizeof(cf_t))

// samples per byte
#define RF_SHMEM_SAMPLES_X_BYTE(x) ((x)/sizeof(cf_t))

#define RF_SHMEM_NTYPE_NONE  (0)  
#define RF_SHMEM_NTYPE_UE    (1)  
#define RF_SHMEM_NTYPE_ENB   (2)

#define RF_SHMEM_MAX_NOF_PORTS  1 // up to 4 ports someday ....

static const struct timeval tv_zero  = {};
static const struct timeval tv_step  = {0, 1000};
static const struct timeval tv_4step = {0, 4000};

// sf len at nprb=100 is 184320
// subtract the other fields of the struct to align mem size to 256k per bin
#define RF_SHMEM_MAX_CF_LEN RF_SHMEM_SAMPLES_X_BYTE((256000 - 12 - 2 * sizeof(int) - 2 * sizeof(struct timeval) - sizeof(float)))

// tx msg header info type (not for stack allocation)
typedef struct {
  uint64_t       seqnum;                    // seq num
  uint32_t       nof_bytes;                 // nbytes
  float          tx_srate;                  // tx sample rate
  struct timeval tv_tx_tti;                 // tti time from upper layers
  struct timeval tv_tx_time;                // actual tx time
  int            is_sob;                    // start of burst
  int            is_eob;                    // end of burst
  cf_t           data[RF_SHMEM_MAX_CF_LEN]; // data
} rf_shmem_element_t;

#define RF_SHMEM_NUM_SF_X_FRAME 10

typedef struct {
  rf_shmem_element_t elements[RF_SHMEM_NUM_SF_X_FRAME];
} rf_shmem_segment_t;

#define RF_SHMEM_MAX_CE_SYMBOLS 23040

cf_t *cf_tmp = NULL;

#define RF_SHMEM__DATA_SEGMENT_SIZE sizeof(rf_shmem_segment_t)

const char * printMsg(const rf_shmem_element_t * p, char * buff, int buff_len)
 {
   snprintf(buff, buff_len, "seqnum %lu, nof_bytes %u, srate %6.4f MHz, tti_tx %ld:%06ld, sob %d, eob %d",
            p->seqnum,
            p->nof_bytes,
            p->tx_srate/1e6,
            p->tv_tx_tti.tv_sec,
            p->tv_tx_tti.tv_usec,
            p->is_sob,
            p->is_eob);
    
    return buff;
 }

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
   srslte_rf_cal_t      tx_cal;
   srslte_rf_cal_t      rx_cal;
   double               clock_rate;
   void (*error_handler)(srslte_rf_error_t error);
   bool                 rx_stream;
   uint64_t             tx_seqnum;
   pthread_mutex_t      rx_lock;
   struct timeval       tv_sos;      // start of stream
   struct timeval       tv_this_tti;
   struct timeval       tv_next_tti;
   size_t               tx_nof_late;
   size_t               tx_nof_ok;
   srslte_rf_info_t     rf_info;
   key_t                shm_dl_key;
   key_t                shm_ul_key;
   int                  shm_dl_id;
   int                  shm_ul_id;
   void *               shm_dl;
   void *               shm_ul;
   rf_shmem_segment_t * rxp;
   rf_shmem_segment_t * txp;
   sem_t *              sem;
} rf_shmem_info_t;


void rf_shmem_suppress_stdout(void *h)
 {
#ifndef RF_SHMEM_DEBUG_MODE
    rf_shmem_log_dbug = false;
#endif
 }

static time_t tv_to_usec(const struct timeval * tv)
 {
   return (tv->tv_sec * 1000000) + tv->tv_usec;
 }

static uint32_t get_bin(const struct timeval * tv)
 {
   return (tv_to_usec(tv) / tv_to_usec(&tv_step)) % RF_SHMEM_NUM_SF_X_FRAME;
 }


static void rf_shmem_handle_error(srslte_rf_error_t error)
{
  // XXX TODO make use of this handler
  printf("type %s", 
          error.type == SRSLTE_RF_ERROR_LATE      ? "late"      :
          error.type == SRSLTE_RF_ERROR_UNDERFLOW ? "underflow" :
          error.type == SRSLTE_RF_ERROR_OVERFLOW  ? "overflow"  :
          error.type == SRSLTE_RF_ERROR_OTHER     ? "other"     :
          "unknown error");
}


static  rf_shmem_info_t rf_shmem_info = { .dev_name        = "shmemrf",
                                          .nodetype        = RF_SHMEM_NTYPE_NONE,
                                          .nof_tx_ports    = 1,
                                          .nof_rx_ports    = 1,
                                          .rx_gain         = 0.0,
                                          .tx_gain         = 0.0,
                                          .rx_srate        = SRSLTE_CS_SAMP_FREQ,
                                          .tx_srate        = SRSLTE_CS_SAMP_FREQ,
                                          .rx_freq         = 0.0,
                                          .tx_freq         = 0.0,
                                          .rx_cal          = {},
                                          .tx_cal          = {},
                                          .clock_rate      = 0.0,
                                          .error_handler   = rf_shmem_handle_error,
                                          .rx_stream       = false,
                                          .tx_seqnum       = 0,
                                          .rx_lock         = PTHREAD_MUTEX_INITIALIZER,
                                          .tv_sos          = {},
                                          .tv_this_tti     = {},
                                          .tv_next_tti     = {},
                                          .tx_nof_late     = 0,
                                          .tx_nof_ok       = 0,
                                          .rf_info         = {},
                                          .shm_dl_key      = 0,
                                          .shm_ul_key      = 0,
                                          .shm_dl_id       = 0,
                                          .shm_ul_id       = 0,
                                          .shm_dl          = NULL,
                                          .shm_ul          = NULL,
                                          .rxp             = NULL,
                                          .txp             = NULL,
                                          .sem             = 0
                                        };

#define RF_SHMEM_GET_DEV_INFO(h)  assert(h); rf_shmem_info_t *_info = (rf_shmem_info_t *)(h)

static bool rf_shmem_is_enb(rf_shmem_info_t * _info)
{
  return (_info->nodetype == RF_SHMEM_NTYPE_ENB);
}

// timeval to full and frac seconds
static void rf_shmem_tv_to_fs(const struct timeval *tv, time_t *full_secs, double *frac_secs)
{
  if(full_secs && frac_secs)
    {
      *full_secs = tv->tv_sec; 
      *frac_secs = tv->tv_usec / 1.0e6;
    }
}

// get fractional seconds from a timeval
static double rf_shmem_get_fs(const struct timeval *tv)
{
  return tv->tv_sec + tv->tv_usec / 1.0e6;
}


// downsample during initial sync
static int rf_shmem_resample(double srate_in, 
                             double srate_out, 
                             cf_t * data_in, 
                             cf_t * data_out,
                             int nof_bytes)
{
  int result = nof_bytes;

  const int nof_samples = RF_SHMEM_SAMPLES_X_BYTE(nof_bytes);

  memset(cf_tmp, 0x0, sizeof(cf_t) * RF_SHMEM_MAX_CE_SYMBOLS);

  if(srate_in && srate_out && (srate_in != srate_out))
   {
     const double sratio = srate_out / srate_in;

     // 'upsample' unable to sync
     if(sratio > 1.0)
      { 
        RF_SHMEM_WARN("srate %4.2lf/%4.2lf MHz, sratio %3.3lf, upsample may not decode",
                     srate_in/1e6,
                     srate_out/1e6,
                     sratio);
      }
     else
      {
        RF_SHMEM_DBUG("srate %4.2lf/%4.2lf MHz, sratio %3.3lf",
                     srate_in/1e6,
                     srate_out/1e6,
                     sratio);
      }

     srslte_resample_arb_t r;
     srslte_resample_arb_init(&r, sratio, 0);

     result = RF_SHMEM_BYTES_X_SAMPLE(srslte_resample_arb_compute(&r, 
                                                                  data_in, 
                                                                  cf_tmp, 
                                                                  nof_samples));

     // XXX TODO is this correct ???
     for(int i = 0; i < nof_samples; ++i) {
       data_out[i] += cf_tmp[i];
     }
   }
  else
   {
     // XXX TODO is this correct ???
     for(int i = 0; i < nof_samples; ++i) {
       data_out[i] += data_in[i];
     }
   }

  return result;
}



static int rf_shmem_open_ipc(rf_shmem_info_t * _info)
{
  int dl_shm_flags = 0;
  int ul_shm_flags = 0;

  bool wait_for_create = false;

  if(rf_shmem_is_enb(_info))
    {
      rf_shmem_node_type[0] = 'E';

      dl_shm_flags = IPC_CREAT | 0666;
      ul_shm_flags = IPC_CREAT | 0666;
    }
  else
    {
      rf_shmem_node_type[0] = 'U';

      dl_shm_flags = 0666;
      ul_shm_flags = 0666;

      // let enb create all resources
      wait_for_create = true;
    }

  // dl shm key
  if((_info->shm_dl_key = ftok("/tmp", 'D')) < 0)
    {
      RF_SHMEM_WARN("failed to get shm_dl_key %s", strerror(errno));

      return -1;
    }
  else
    {
      RF_SHMEM_WARN("got shm_dl_key 0x%x", _info->shm_dl_key);
    }


  // ul shm key
  if((_info->shm_ul_key = ftok("/tmp", 'U')) < 0)
    {
      RF_SHMEM_WARN("failed to get shm_ul_key %s", strerror(errno));

      return -1;
    }
  else
    {
      RF_SHMEM_WARN("got shm_ul_key 0x%x, use ipcs -m to find me later", _info->shm_ul_key);
    }


  do {
    // dl shm id
    if((_info->shm_dl_id = shmget(_info->shm_dl_key, RF_SHMEM__DATA_SEGMENT_SIZE, dl_shm_flags)) < 0)
      {
        if(wait_for_create == false)
         {
           RF_SHMEM_WARN("failed to get shm_dl_id %s, abort", strerror(errno));

           return -1;
         }
        else
         {
           RF_SHMEM_WARN("failed to get shm_dl_id %s, retry", strerror(errno));

           sleep(1);
         }
      }
   } while(_info->shm_dl_id < 0);


  do {
    // ul shm id
    if((_info->shm_ul_id = shmget(_info->shm_ul_key, RF_SHMEM__DATA_SEGMENT_SIZE, ul_shm_flags)) < 0)
      {
        if(wait_for_create == false)
         {
           RF_SHMEM_WARN("failed to get shm_ul_id %s, abort", strerror(errno));

           return -1;
         }
        else
         {
           RF_SHMEM_WARN("failed to get shm_ul_id %s, retry", strerror(errno));

           sleep(1);
         }
      }
   } while(_info->shm_ul_id < 0);


  // dl shm addr
  if((_info->shm_dl = shmat(_info->shm_dl_id, NULL, 0)) == (void *) -1)
    {
      RF_SHMEM_WARN("failed to attach shm_dl %s", strerror(errno));

      rf_shmem_close(_info);

      return -1;
    }

  // ul shm addr
  if((_info->shm_ul = shmat(_info->shm_ul_id, NULL, 0)) == (void *) -1)
    {
      RF_SHMEM_WARN("failed to attach shm_ul %s", strerror(errno));

      rf_shmem_close(_info);

      return -1;
    }

  if(rf_shmem_is_enb(_info))
    {
      _info->txp = (rf_shmem_segment_t *) _info->shm_dl;
      _info->rxp = (rf_shmem_segment_t *) _info->shm_ul;

     if((_info->sem = sem_open("/shmemrf", O_CREAT, 0644, 0)) == NULL)
      {
        RF_SHMEM_WARN("failed to create sem %s", strerror(errno));

        rf_shmem_close(_info);

        return -1;
      }
    }
  else
    {
      _info->txp = (rf_shmem_segment_t *) _info->shm_ul;
      _info->rxp = (rf_shmem_segment_t *) _info->shm_dl;

     if((_info->sem = sem_open("/shmemrf", 0, 0644, 0)) == NULL)
      {
        RF_SHMEM_WARN("failed to open sem %s", strerror(errno));

        rf_shmem_close(_info);

        return -1;
      }
    }

  memset(_info->shm_ul, 0x0, RF_SHMEM__DATA_SEGMENT_SIZE);
  memset(_info->shm_dl, 0x0, RF_SHMEM__DATA_SEGMENT_SIZE);

  cf_tmp = srslte_vec_malloc(sizeof(cf_t) * RF_SHMEM_MAX_CE_SYMBOLS);
  if (!cf_tmp) {
     RF_SHMEM_WARN("cf_tmp vec_malloc error %s", strerror(errno));
     return -1;
  }

  return 0;
}




static void rf_shmem_wait_next_tti(void *h, struct timeval * tv_ref)
{
   RF_SHMEM_GET_DEV_INFO(h);

   struct timeval tv_diff;

   // this is where we set the pace for the system TTI
   timersub(&_info->tv_next_tti, tv_ref, &tv_diff);

   if(timercmp(&tv_diff, &tv_zero, >))
     {
       RF_SHMEM_DBUG("wait %6.6lf for next tti", rf_shmem_get_fs(&tv_diff));
       select(0, NULL, NULL, NULL, &tv_diff);
     }
    else
     {
       RF_SHMEM_DBUG("late %6.6lf for this tti", rf_shmem_get_fs(&tv_diff));
     }

   _info->tv_this_tti = _info->tv_next_tti;

   timeradd(&_info->tv_next_tti, &tv_step, &_info->tv_next_tti);

   gettimeofday(tv_ref, NULL);
}



// begin RF API

char* rf_shmem_devname(void *h)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   return _info->dev_name;
 }


bool rf_shmem_rx_wait_lo_locked(void *h)
 {
   RF_SHMEM_LOG_FUNC_TODO;

   return false;
 }


int rf_shmem_start_rx_stream(void *h, bool now)
 {
   RF_SHMEM_GET_DEV_INFO(h);
   
   pthread_mutex_lock(&_info->rx_lock);

   gettimeofday(&_info->tv_sos, NULL);

   // aligin time on the second, 1 in a million chance its on 
   if(_info->tv_sos.tv_usec > 0)
    {
      usleep(1000000 - _info->tv_sos.tv_usec);
   
      _info->tv_sos.tv_sec  += 1;
      _info->tv_sos.tv_usec = 0;
    }

   // initial tti and next
   _info->tv_this_tti = _info->tv_sos;
   timeradd(&_info->tv_sos, &tv_step, &_info->tv_next_tti);

   RF_SHMEM_WARN("start rx stream, time_0 %ld:%06ld, next_tti %ld:%06ld", 
                 _info->tv_sos.tv_sec, 
                 _info->tv_sos.tv_usec,
                 _info->tv_next_tti.tv_sec, 
                 _info->tv_next_tti.tv_usec);

   _info->rx_stream = true;

   pthread_mutex_unlock(&_info->rx_lock);

   return 0;
 }


int rf_shmem_stop_rx_stream(void *h)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   pthread_mutex_lock(&_info->rx_lock);

   // XXX how important is this
   RF_SHMEM_WARN("end rx stream");

   _info->rx_stream = false;

   pthread_mutex_unlock(&_info->rx_lock);

   return 0;
 }


void rf_shmem_flush_buffer(void *h)
 {
   RF_SHMEM_LOG_FUNC_TODO;
 }


bool rf_shmem_has_rssi(void *h)
 {
   return false;
 }


float rf_shmem_get_rssi(void *h)
 {
   const float rssi = 0.0;  // XXX TODO what value ???

   RF_SHMEM_INFO("rssi %4.3f", rssi);

   return rssi;
 }


void rf_shmem_register_error_handler(void *h, srslte_rf_error_handler_t error_handler)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   _info->error_handler = error_handler;
 }


int rf_shmem_open(char *args, void **h)
 {
   return rf_shmem_open_multi(args, h, 1);
 }


int rf_shmem_open_multi(char *args, void **h, uint32_t nof_channels)
 {
   rf_shmem_info_t * _info = &rf_shmem_info;

   RF_SHMEM_INFO("channels %u, args [%s]", nof_channels, args ? args : "none");

   if(nof_channels > RF_SHMEM_MAX_NOF_PORTS)
    {
      RF_SHMEM_WARN("only supporting up to %d channels, not %d", RF_SHMEM_MAX_NOF_PORTS, nof_channels);

      return -1;
    }

   if(args && strncmp(args, "enb", strlen("enb")) == 0)
    {
      _info->nof_rx_ports = nof_channels;

      rf_shmem_info.nodetype = RF_SHMEM_NTYPE_ENB;
    }
   else if(args && strncmp(args, "ue", strlen("ue")) == 0)
    {
      _info->nof_rx_ports = 1;

      rf_shmem_info.nodetype = RF_SHMEM_NTYPE_UE;
    }
   else
    {
      if(!args)
       {
         RF_SHMEM_WARN("expected node type enb or ue\n");
       }
      else
       {
         RF_SHMEM_WARN("unexpected node type %s\n", args);
       }

      return -1;
    }
       
   if(rf_shmem_open_ipc(&rf_shmem_info) < 0)
    {
      RF_SHMEM_WARN("could not create ipc channel");

      return -1;
    }

   *h = &rf_shmem_info;

   return 0;
 }


int rf_shmem_close(void *h)
 {
   // XXX this does not seem to get called on shutdown as othen as I'd expect

   RF_SHMEM_GET_DEV_INFO(h);

   if(rf_shmem_is_enb(_info))
    {
      if(_info->shm_dl)
        {
          shmdt(_info->shm_dl);

          shmctl(_info->shm_dl_id, IPC_RMID, NULL);

          _info->shm_dl = NULL;
        }

      if(_info->shm_ul)
        {
          shmdt(_info->shm_ul);

          shmctl(_info->shm_ul_id, IPC_RMID, NULL);

          _info->shm_ul = NULL;
        }

      if(_info->sem)
        {
          sem_close(_info->sem);

          _info->sem = NULL;
        }
    }

   if(cf_tmp) 
    {
      free(cf_tmp);
    }

   return 0;
 }


void rf_shmem_set_master_clock_rate(void *h, double rate)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   RF_SHMEM_INFO("rate %4.2lf MHz to %4.2lf MHz", 
                 _info->clock_rate / 1e6, rate / 1e6);

   _info->clock_rate = rate;
 }


bool rf_shmem_is_master_clock_dynamic(void *h)
 {
   RF_SHMEM_LOG_FUNC_TODO;

   return false;
 }


double rf_shmem_set_rx_gain(void *h, double gain)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   RF_SHMEM_INFO("gain %3.2lf to %3.2lf", _info->rx_gain, gain);

   _info->rx_gain = gain;

   return _info->rx_gain;
 }


double rf_shmem_set_tx_gain(void *h, double gain)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   RF_SHMEM_INFO("gain %3.2lf to %3.2lf", _info->tx_gain, gain);

   _info->tx_gain = gain;

   return _info->tx_gain;
 }


srslte_rf_info_t * rf_shmem_get_rf_info(void *h)
  {
     RF_SHMEM_GET_DEV_INFO(h);

     RF_SHMEM_DBUG("tx_gain min/max %3.2lf/%3.2lf, rx_gain min/max %3.2lf/%3.2lf",
                  _info->rf_info.min_tx_gain,
                  _info->rf_info.max_tx_gain,
                  _info->rf_info.min_rx_gain,
                  _info->rf_info.max_rx_gain);

     return &_info->rf_info;
  }


double rf_shmem_get_rx_gain(void *h)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   RF_SHMEM_DBUG("gain %3.2lf", _info->rx_gain);

   return _info->rx_gain;
 }


double rf_shmem_get_tx_gain(void *h)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   RF_SHMEM_DBUG("gain %3.2lf", _info->tx_gain);

   return _info->tx_gain;
 }


double rf_shmem_set_rx_srate(void *h, double rate)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   RF_SHMEM_INFO("srate %4.2lf MHz to %4.2lf MHz", 
                 _info->rx_srate / 1e6, rate / 1e6);

   _info->rx_srate = rate;

   return _info->rx_srate;
 }


double rf_shmem_set_tx_srate(void *h, double rate)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   RF_SHMEM_INFO("srate %4.2lf MHz to %4.2lf MHz", 
                 _info->tx_srate / 1e6, rate / 1e6);

   _info->tx_srate = rate;

   return _info->tx_srate;
 }


double rf_shmem_set_rx_freq(void *h, double freq)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   RF_SHMEM_INFO("freq %4.2lf MHz to %4.2lf MHz", 
                 _info->rx_freq / 1e6, freq / 1e6);

   _info->rx_freq = freq;

   return _info->rx_freq;
 }


double rf_shmem_set_tx_freq(void *h, double freq)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   RF_SHMEM_INFO("freq %4.2lf MHz to %4.2lf MHz", 
                 _info->tx_freq / 1e6, freq / 1e6);

   _info->tx_freq = freq;

   return _info->tx_freq;
 }


void rf_shmem_set_tx_cal(void *h, srslte_rf_cal_t *cal)
{
   RF_SHMEM_GET_DEV_INFO(h);

   memcpy(&(_info->tx_cal), cal, sizeof(srslte_rf_cal_t));

   RF_SHMEM_INFO("gain %3.2lf, phase %3.2lf, I %3.2lf, Q %3.2lf", 
                 cal->dc_gain, 
                 cal->dc_phase, 
                 cal->iq_i,
                 cal->iq_q);
}


void rf_shmem_set_rx_cal(void *h, srslte_rf_cal_t *cal)
{
   RF_SHMEM_GET_DEV_INFO(h);

   memcpy(&(_info->rx_cal), cal, sizeof(srslte_rf_cal_t));

   RF_SHMEM_INFO("gain %3.2lf, phase %3.2lf, I %3.2lf, Q %3.2lf", 
                 cal->dc_gain,
                 cal->dc_phase,
                 cal->iq_i,
                 cal->iq_q);
}


void rf_shmem_get_time(void *h, time_t *full_secs, double *frac_secs)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   RF_SHMEM_INFO("XXX");

   rf_shmem_tv_to_fs(&_info->tv_this_tti, full_secs, frac_secs);
 }



int rf_shmem_recv_with_time(void *h, void *data, uint32_t nsamples, 
                           bool blocking, time_t *full_secs, double *frac_secs)
 {
   void *d[4] = {data, NULL, NULL, NULL};

   return rf_shmem_recv_with_time_multi(h, 
                                       d,
                                       nsamples, 
                                       blocking,
                                       full_secs,
                                       frac_secs);
 }



int rf_shmem_recv_with_time_multi(void *h, void **data, uint32_t nsamples, 
                                  bool blocking, time_t *full_secs, double *frac_secs)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   struct timeval tv_now, tv_diff;

   const uint32_t nof_bytes = RF_SHMEM_BYTES_X_SAMPLE(nsamples);

   memset(data[0], 0x0, nof_bytes);

   // working in units of subframes
   const int nof_sf = (nsamples / (_info->rx_srate / 1000.0f));

   RF_SHMEM_DBUG("req: nof_samples %u, nof_bytes %u, nof_sf %d", 
                 nsamples,
                 nof_bytes,
                 nof_sf);


   uint32_t nof_bytes_in = 0;

   for(int i = 0; i < nof_sf; ++i)
     { 
        gettimeofday(&tv_now, NULL);

        // wait for the next tti
        rf_shmem_wait_next_tti(h, &tv_now);

        // find bin for this tti
        const uint32_t bin = get_bin(&_info->tv_this_tti);

        // XXX TODO semaphore
        rf_shmem_element_t * p = &_info->rxp->elements[bin];
      
        if(timercmp(&_info->tv_this_tti, &p->tv_tx_tti, ==))
         {
           const int new_len = rf_shmem_resample(p->tx_srate,
                                                 _info->rx_srate,
                                                 p->data,
                                                 (cf_t*)(((uint8_t*)data[0]) + nof_bytes_in),
                                                 p->nof_bytes);

           nof_bytes_in += new_len;

#ifdef RF_SHMEM_DEBUG_MODE
           char logbuff[256] = {0};
           RF_SHMEM_DBUG("RX, bin %u, new_len %u, total %u, %s", 
                         bin, new_len, nof_bytes_in, printMsg(p, logbuff, sizeof(logbuff)));
#endif
         }
       else
         {
          // empty or stale entry
          if(timercmp(&p->tv_tx_tti, &tv_zero, !=))
           {
             timersub(&p->tv_tx_tti, &tv_now, &tv_diff);

#ifdef RF_SHMEM_DEBUG_MODE
             char logbuff[256] = {0};
             RF_SHMEM_DBUG("RX, bin %u, orphan, overrun %6.6lf, %s", 
                           bin, 
                           -rf_shmem_get_fs(&tv_diff),
                           printMsg(p, logbuff, sizeof(logbuff)));
#endif

             // cleanup
             memset(p, 0x0, sizeof(rf_shmem_element_t));
           }
          else
           {
             RF_SHMEM_DBUG("RX, bin %u, empty", bin);
           }
        }
     }

   rf_shmem_tv_to_fs(&_info->tv_this_tti, full_secs, frac_secs);

   return nsamples;
 }


int rf_shmem_send_timed(void *h, void *data, int nsamples,
                       time_t full_secs, double frac_secs, bool has_time_spec,
                       bool blocking, bool is_sob, bool is_eob)
 {
   void *d[4] = {data, NULL, NULL, NULL};

   return rf_shmem_send_timed_multi(h, d, nsamples, full_secs, frac_secs, has_time_spec, blocking, is_sob, is_eob);
 }


int rf_shmem_send_timed_multi(void *h, void *data[4], int nsamples,
                             time_t full_secs, double frac_secs, bool has_time_spec,
                             bool blocking, bool is_sob, bool is_eob)
 {
   RF_SHMEM_GET_DEV_INFO(h);

   if(nsamples <= 0)
     {
       RF_SHMEM_DBUG("msg len %d, sob %d, eob %d", nsamples, is_sob, is_eob);

       return 0;
     }

   struct timeval tv_now, tv_tx_tti;

   // assume all tx are 4 tti in the future
   // code base may advance timespec slightly which can mess up our bins
   timeradd(&_info->tv_this_tti, &tv_4step, &tv_tx_tti);

   gettimeofday(&tv_now, NULL);

   // this msg tx tti time has passed it should be well into the future
   if(timercmp(&tv_tx_tti, &tv_now, <))
     {
       struct timeval tv_diff;

       timersub(&tv_tx_tti, &tv_now, &tv_diff);

       ++_info->tx_nof_late;

       RF_SHMEM_WARN("TX late, seqnum %lu, tx_tti %ld:%06ld, overrun %6.6lf, total late %zu",
                     _info->tx_seqnum++,
                     tv_tx_tti.tv_sec,
                     tv_tx_tti.tv_usec,
                     -rf_shmem_get_fs(&tv_diff),
                     _info->tx_nof_late);
     }
   else
     {
       const uint32_t nof_bytes = RF_SHMEM_BYTES_X_SAMPLE(nsamples);

       const uint32_t bin = get_bin(&tv_tx_tti);

       // XXX TODO semaphore
       rf_shmem_element_t * p = &_info->txp->elements[bin];
       
       p->is_sob       = is_sob;
       p->is_eob       = is_eob;
       p->tx_srate     = _info->tx_srate;
       p->seqnum       = _info->tx_seqnum++;
       p->nof_bytes    = nof_bytes;
       p->tv_tx_tti    = tv_tx_tti;
       p->tv_tx_time   = tv_now;

       memcpy(p->data, data[0], nof_bytes);

       ++_info->tx_nof_ok;

#ifdef RF_SHMEM_DEBUG_MODE
       char logbuff[256] = {0};
       RF_SHMEM_DBUG("TX, bin %u, %s", bin, printMsg(p, logbuff, sizeof(logbuff)));
#endif
     }

   return nsamples;
 }
