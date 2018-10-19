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

// J Giovatto Oct 03, 2018
// The approach is simple the enb and ue transmit raw IQ data which
// would have been sent to the radio device driver (UHD) etc. Samples
// are sent and received verbatim except in the case where the ue is in
// cell search where downsampling is applied. At some point some artifical
// "jammming" of the data stream my be desireable by substiting some random values.
// Shared memory was chosen over unix/inet sockets to allow for the fastest
// data transfer and possible combining IQ data in a single buffer. Each ul and dl subframe
// worth of iqdata and metadata  is held in a "bin" where tx bin index is 4 sf 
// ahead of the rx bin index.

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
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */

#include "srslte/srslte.h"
#include "rf_shmem_imp.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/resampling/resample_arb.h"

// define to allow debug
// #define RF_SHMEM_DEBUG_MODE
#undef RF_SHMEM_DEBUG_MODE

#ifdef RF_SHMEM_DEBUG_MODE
static bool rf_shmem_log_dbug  = true;
static bool rf_shmem_log_info  = true;
static bool rf_shmem_log_warn  = true;
#else
static bool rf_shmem_log_dbug  = false;
static bool rf_shmem_log_info  = true;
static bool rf_shmem_log_warn  = true;
#endif

static char rf_shmem_node_type = ' ';

#define RF_SHMEM_LOG_FMT "%02d:%02d:%02d.%06ld [SRF.%c] [%c] %s,  "


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

#define RF_SHMEM_NUM_SF_X_FRAME 10

static const struct timeval tv_zero  = {0,0};
static const struct timeval tv_step  = {0, 1000}; // 1 sf
static const struct timeval tv_4step = {0, 4000}; // 4 sf


// msg element meta data
typedef struct {
  uint64_t       seqnum;                      // seq num
  uint32_t       nof_bytes;                   // num bytes
  uint32_t       nof_sf;                      // num subframes
  float          tx_srate;                    // tx sample rate
  struct timeval tv_tx_tti;                   // tti time (tti + 4)
  struct timeval tv_tx_time;                  // actual tx time
  int            is_sob;                      // is start of burst
  int            is_eob;                      // is end of burst
} rf_shmem_element_meta_t;


// sf len at nprb=100 is 184320 bytes or 23040 samples
// subtract the other fields of the struct to align mem size to 256k per bin
// to avoid shmget failure
#define RF_SHMEM_MAX_CF_LEN RF_SHMEM_SAMPLES_X_BYTE((256000 - sizeof(rf_shmem_element_meta_t)))

// msg element (not for stack allocation)
typedef struct {
  rf_shmem_element_meta_t meta;                        // meta data
  cf_t                    iqdata[RF_SHMEM_MAX_CF_LEN]; // data
} rf_shmem_element_t;

sem_t * sem[RF_SHMEM_NUM_SF_X_FRAME] = {0};  // element r/w bin locks

// msg element bins 1 for each sf (tti)
typedef struct {
  rf_shmem_element_t elements[RF_SHMEM_NUM_SF_X_FRAME];
} rf_shmem_segment_t;

#define RF_SHMEM_DATA_SEGMENT_SIZE sizeof(rf_shmem_segment_t)

const char * printMsg(const rf_shmem_element_t * element, char * buff, int buff_len)
 {
   snprintf(buff, buff_len, "seqnum %lu, nof_bytes %u, nof_sf %u, srate %6.4f MHz, tti_tx %ld:%06ld, sob %d, eob %d",
            element->meta.seqnum,
            element->meta.nof_bytes,
            element->meta.nof_sf,
            element->meta.tx_srate/1e6,
            element->meta.tv_tx_tti.tv_sec,
            element->meta.tv_tx_tti.tv_usec,
            element->meta.is_sob,
            element->meta.is_eob);
    
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
   double               clock_rate;
   void (*error_handler)(srslte_rf_error_t error);
   bool                 rx_stream;
   uint64_t             tx_seqnum;
   pthread_mutex_t      state_lock;
   struct timeval       tv_sos;      // start of stream
   struct timeval       tv_this_tti;
   struct timeval       tv_next_tti;
   size_t               tx_nof_late;
   size_t               tx_nof_ok;
   size_t               tx_nof_drop;
   srslte_rf_info_t     rf_info;
   int                  shm_dl_fd;
   int                  shm_ul_fd;
   void *               shm_dl;      // dl shared mem
   void *               shm_ul;      // ul shared mem
   rf_shmem_segment_t * rx_segment;  // rx bins
   rf_shmem_segment_t * tx_segment;  // tx bins
   int                  rand_loss;   // random loss 0=none, 100=all
} rf_shmem_state_t;


void rf_shmem_suppress_stdout(void *h)
 {
#ifndef RF_SHMEM_DEBUG_MODE
    rf_shmem_log_dbug = false;
#endif
 }

static inline time_t tv_to_usec(const struct timeval * tv)
 {
   return (tv->tv_sec * 1000000) + tv->tv_usec;
 }

static inline uint32_t get_bin(const struct timeval * tv)
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


static rf_shmem_state_t rf_shmem_state = { .dev_name        = "shmemrf",
                                           .nodetype        = RF_SHMEM_NTYPE_NONE,
                                           .nof_tx_ports    = 1,
                                           .nof_rx_ports    = 1,
                                           .rx_gain         = 0.0,
                                           .tx_gain         = 0.0,
                                           .rx_srate        = SRSLTE_CS_SAMP_FREQ,
                                           .tx_srate        = SRSLTE_CS_SAMP_FREQ,
                                           .rx_freq         = 0.0,
                                           .tx_freq         = 0.0,
                                           .clock_rate      = 0.0,
                                           .error_handler   = rf_shmem_handle_error,
                                           .rx_stream       = false,
                                           .tx_seqnum       = 0,
                                           .state_lock      = PTHREAD_MUTEX_INITIALIZER,
                                           .tv_sos          = {},
                                           .tv_this_tti     = {},
                                           .tv_next_tti     = {},
                                           .tx_nof_late     = 0,
                                           .tx_nof_ok       = 0,
                                           .tx_nof_drop     = 0,
                                           .rf_info         = {},
                                           .shm_dl_fd       = 0,
                                           .shm_ul_fd       = 0,
                                           .shm_dl          = NULL,
                                           .shm_ul          = NULL,
                                           .rx_segment      = NULL,
                                           .tx_segment      = NULL,
                                           .rand_loss       = 0,
                                        };

#define RF_SHMEM_GET_STATE(h)  assert(h); rf_shmem_state_t *_state = (rf_shmem_state_t *)(h)

static inline bool rf_shmem_is_enb(rf_shmem_state_t * _state)
{
  return (_state->nodetype == RF_SHMEM_NTYPE_ENB);
}

// timeval to full and frac seconds
static inline void rf_shmem_tv_to_fs(const struct timeval *tv, time_t *full_secs, double *frac_secs)
{
  if(full_secs && frac_secs)
    {
      *full_secs = tv->tv_sec; 
      *frac_secs = tv->tv_usec / 1.0e6;
    }
}

// get fractional seconds from a timeval
static inline double rf_shmem_get_fs(const struct timeval *tv)
{
  return tv->tv_sec + tv->tv_usec / 1.0e6;
}


// XXX this could be a place where we introduce noise/jamming effects
static int rf_shmem_resample(double srate_in, 
                             double srate_out, 
                             cf_t * data_in, 
                             cf_t * data_out,
                             int nof_bytes)
{
  // downsample needed during initial sync since ue is at lowest sample rate
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

     return RF_SHMEM_BYTES_X_SAMPLE(srslte_resample_arb_compute(&r, 
                                                                data_in, 
                                                                data_out, 
                                                                RF_SHMEM_SAMPLES_X_BYTE(nof_bytes)));
   }
  else
   {
     // no resampling needed
     memcpy(data_out, data_in, nof_bytes);

     return nof_bytes;
   }
}



static int rf_shmem_open_ipc(rf_shmem_state_t * _state)
{
  int dl_shm_flags = 0;
  int ul_shm_flags = 0;

  // assume 1 enb which is resposible for creating all shared resources
  bool wait_for_create = false;
  mode_t mode = S_IRWXU;// | S_IRWXG | S_IRWXO;

  if(rf_shmem_is_enb(_state))
    {
      rf_shmem_node_type = 'E';

      dl_shm_flags = O_CREAT | O_TRUNC | O_RDWR;
      ul_shm_flags = O_CREAT | O_TRUNC | O_RDWR;
    }
  else
    {
      rf_shmem_node_type = 'U';

      dl_shm_flags = O_RDWR;
      ul_shm_flags = O_RDWR;

      // let enb create all resources
      wait_for_create = true;
    }

  do {
    if((_state->shm_dl_fd = shm_open("/srslte_shm_dl", dl_shm_flags, mode)) < 0)
      {
        if(wait_for_create == false)
          {
            RF_SHMEM_WARN("failed to get shm_dl_fd %s", strerror(errno));

            return -1;
          }
        else
          {
            RF_SHMEM_WARN("failed to get shm_dl_fd %s, retry", strerror(errno));

            sleep(1);
          }
      }
    else
      {
        if(rf_shmem_node_type == 'E')
          {
            ftruncate(_state->shm_dl_fd, RF_SHMEM_DATA_SEGMENT_SIZE);
          }
        RF_SHMEM_WARN("got shm_dl_fd 0x%x", _state->shm_dl_fd);
      }
  } while(_state->shm_dl_fd < 0);
    
  


  // ul shm key
  do {
    if((_state->shm_ul_fd = shm_open("/srslte_shm_ul", ul_shm_flags, mode)) < 0)
      {
        if(wait_for_create == false)
          {
            RF_SHMEM_WARN("failed to get shm_ul_fd %s", strerror(errno));

            return -1;
          }
        else
          {
            RF_SHMEM_WARN("failed to get shm_ul_fd %s, retry", strerror(errno));

            sleep(1);
          }
      }
    else
      {
        if(rf_shmem_node_type == 'E')
          {
            ftruncate(_state->shm_ul_fd, RF_SHMEM_DATA_SEGMENT_SIZE);
          }
        RF_SHMEM_WARN("got shm_ul_fd 0x%x", _state->shm_ul_fd);
      }
  } while(_state->shm_ul_fd < 0);



  // dl shm addr
  if((_state->shm_dl = mmap(0, RF_SHMEM_DATA_SEGMENT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, _state->shm_dl_fd, 0)) == (void *) -1)
    {
      RF_SHMEM_WARN("failed to map shm_dl %s", strerror(errno));

      rf_shmem_close(_state);

      return -1;
    }

  // ul shm addr
  if((_state->shm_ul = mmap(0, RF_SHMEM_DATA_SEGMENT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, _state->shm_ul_fd, 0)) == (void *) -1)
    {
      RF_SHMEM_WARN("failed to map shm_ul %s", strerror(errno));

      rf_shmem_close(_state);

      return -1;
    }

  // set ul/dl bins to avoid ue/enb cross talk
  if(rf_shmem_is_enb(_state))
    {
      _state->tx_segment = (rf_shmem_segment_t *) _state->shm_dl;
      _state->rx_segment = (rf_shmem_segment_t *) _state->shm_ul;
    }
  else
    {
      _state->tx_segment = (rf_shmem_segment_t *) _state->shm_ul;
      _state->rx_segment = (rf_shmem_segment_t *) _state->shm_dl;
    }


  // shared sems, 1 for each bin
  for(int i = 0; i < RF_SHMEM_NUM_SF_X_FRAME; ++i)
    {
      char name[32] = {0};

      snprintf(name, sizeof(name), "/shmemrf_sem_%d", i);

      if(rf_shmem_is_enb(_state))
        {
          // cleanup any orphans
          sem_unlink(name);
 
          // initial value 1
          if((sem[i] = sem_open(name, O_CREAT, 0600, 1)) == NULL)
           {
             RF_SHMEM_WARN("failed to create sem %s, %s", name, strerror(errno));

             rf_shmem_close(_state);

             return -1;
           }
          else
           {
             RF_SHMEM_WARN("created sem %s", name);
           }
        }
       else
        {
          if((sem[i] = sem_open(name, 0)) == NULL)
           {
             RF_SHMEM_WARN("failed to open sem %s, %s", name, strerror(errno));

             rf_shmem_close(_state);

             return -1;
           }
          else
           {
             RF_SHMEM_WARN("opened sem %s", name);
           }
        }
    }

  // clear data segments
  memset(_state->shm_ul, 0x0, RF_SHMEM_DATA_SEGMENT_SIZE);
  memset(_state->shm_dl, 0x0, RF_SHMEM_DATA_SEGMENT_SIZE);

  return 0;
}




static void rf_shmem_wait_next_tti(void *h, struct timeval * tv_ref)
{
   RF_SHMEM_GET_STATE(h);

   struct timeval tv_diff;

   // this is where we set the pace for the system TTI
   timersub(&_state->tv_next_tti, tv_ref, &tv_diff);

   if(timercmp(&tv_diff, &tv_zero, >))
     {
       RF_SHMEM_DBUG("wait %6.6lf for next tti", rf_shmem_get_fs(&tv_diff));
       select(0, NULL, NULL, NULL, &tv_diff);
     }
    else
     {
       RF_SHMEM_DBUG("late %6.6lf for this tti", rf_shmem_get_fs(&tv_diff));
     }

   _state->tv_this_tti = _state->tv_next_tti;

   timeradd(&_state->tv_next_tti, &tv_step, &_state->tv_next_tti);

   gettimeofday(tv_ref, NULL);
}



// ************ begin RF API ************

char* rf_shmem_devname(void *h)
 {
   RF_SHMEM_GET_STATE(h);

   return _state->dev_name;
 }


bool rf_shmem_rx_wait_lo_locked(void *h)
 {
   RF_SHMEM_LOG_FUNC_TODO;

   return false;
 }


int rf_shmem_start_rx_stream(void *h, bool now)
 {
   RF_SHMEM_GET_STATE(h);
   
   pthread_mutex_lock(&_state->state_lock);

   gettimeofday(&_state->tv_sos, NULL);

   // aligin time on the second, 1 in a million chance its on 
   if(_state->tv_sos.tv_usec > 0)
    {
      usleep(1000000 - _state->tv_sos.tv_usec);
   
      _state->tv_sos.tv_sec  += 1;
      _state->tv_sos.tv_usec = 0;
    }

   // initial tti and next
   _state->tv_this_tti = _state->tv_sos;
   timeradd(&_state->tv_sos, &tv_step, &_state->tv_next_tti);

   RF_SHMEM_WARN("start rx stream, time_0 %ld:%06ld, next_tti %ld:%06ld", 
                 _state->tv_sos.tv_sec, 
                 _state->tv_sos.tv_usec,
                 _state->tv_next_tti.tv_sec, 
                 _state->tv_next_tti.tv_usec);

   _state->rx_stream = true;

   pthread_mutex_unlock(&_state->state_lock);

   return 0;
 }


int rf_shmem_stop_rx_stream(void *h)
 {
   RF_SHMEM_GET_STATE(h);

   pthread_mutex_lock(&_state->state_lock);

   // XXX how important is this
   RF_SHMEM_WARN("end rx stream");

   _state->rx_stream = false;

   pthread_mutex_unlock(&_state->state_lock);

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
   const float rssi = 0.0;

   RF_SHMEM_INFO("rssi %4.3f", rssi);

   return rssi;
 }


void rf_shmem_register_error_handler(void *h, srslte_rf_error_handler_t error_handler)
 {
   RF_SHMEM_GET_STATE(h);

   _state->error_handler = error_handler;
 }


int rf_shmem_open(char *args, void **h)
 {
   return rf_shmem_open_multi(args, h, 1);
 }


int rf_shmem_open_multi(char *args, void **h, uint32_t nof_channels)
 {
   rf_shmem_state_t * _state = &rf_shmem_state;

   RF_SHMEM_INFO("channels %u, args [%s]", nof_channels, args ? args : "none");

   if(nof_channels > RF_SHMEM_MAX_NOF_PORTS)
    {
      RF_SHMEM_WARN("only supporting up to %d channels, not %d", RF_SHMEM_MAX_NOF_PORTS, nof_channels);

      return -1;
    }

   if(args && strncmp(args, "enb", strlen("enb")) == 0)
    {
      _state->nof_rx_ports = nof_channels;

      rf_shmem_state.nodetype = RF_SHMEM_NTYPE_ENB;
    }
   else if(args && strncmp(args, "ue", strlen("ue")) == 0)
    {
      _state->nof_rx_ports = 1;

      rf_shmem_state.nodetype = RF_SHMEM_NTYPE_UE;
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
       
   if(rf_shmem_open_ipc(&rf_shmem_state) < 0)
    {
      RF_SHMEM_WARN("could not create ipc channel");

      return -1;
    }

   *h = &rf_shmem_state;

   return 0;
 }


int rf_shmem_close(void *h)
 {
   // XXX this does not seem to get called on shutdown as othen as I'd expect

   RF_SHMEM_GET_STATE(h);

   // enb creats/cleans up all shared resources
   if(rf_shmem_is_enb(_state))
    {
      if(_state->shm_dl)
        {
          munmap(_state->shm_dl, RF_SHMEM_DATA_SEGMENT_SIZE);

          shm_unlink("/srslte_shm_dl");

          if(close(_state->shm_dl_fd) < 0)
            {
              RF_SHMEM_WARN("failed to close shm_dl_fd %s", strerror(errno));
            }

          _state->shm_dl = NULL;
        }

      if(_state->shm_ul)
        {
          munmap(_state->shm_ul, RF_SHMEM_DATA_SEGMENT_SIZE);

          shm_unlink("/srslte_shm_ul");

          if(close(_state->shm_ul_fd) < 0)
            {
              RF_SHMEM_WARN("failed to close shm_ul_fd %s", strerror(errno));
            }

          _state->shm_ul = NULL;
        }
    }

  for(int i = 0; i < RF_SHMEM_NUM_SF_X_FRAME; ++i)
    {
      if(sem[i])
       {
         sem_close(sem[i]);

         sem[i] = NULL;
       }
    }
 
   return 0;
 }


void rf_shmem_set_master_clock_rate(void *h, double rate)
 {
   RF_SHMEM_GET_STATE(h);

   RF_SHMEM_INFO("rate %4.2lf MHz to %4.2lf MHz", 
                 _state->clock_rate / 1e6, rate / 1e6);

   _state->clock_rate = rate;
 }


bool rf_shmem_is_master_clock_dynamic(void *h)
 {
   RF_SHMEM_LOG_FUNC_TODO;

   return false;
 }


double rf_shmem_set_rx_gain(void *h, double gain)
 {
   RF_SHMEM_GET_STATE(h);

   RF_SHMEM_INFO("gain %3.2lf to %3.2lf", _state->rx_gain, gain);

   _state->rx_gain = gain;

   return _state->rx_gain;
 }


double rf_shmem_set_tx_gain(void *h, double gain)
 {
   RF_SHMEM_GET_STATE(h);

   RF_SHMEM_INFO("gain %3.2lf to %3.2lf", _state->tx_gain, gain);

   _state->tx_gain = gain;

   return _state->tx_gain;
 }


srslte_rf_info_t * rf_shmem_get_rf_info(void *h)
  {
     RF_SHMEM_GET_STATE(h);

     RF_SHMEM_DBUG("tx_gain min/max %3.2lf/%3.2lf, rx_gain min/max %3.2lf/%3.2lf",
                  _state->rf_info.min_tx_gain,
                  _state->rf_info.max_tx_gain,
                  _state->rf_info.min_rx_gain,
                  _state->rf_info.max_rx_gain);

     return &_state->rf_info;
  }


double rf_shmem_get_rx_gain(void *h)
 {
   RF_SHMEM_GET_STATE(h);

   RF_SHMEM_DBUG("gain %3.2lf", _state->rx_gain);

   return _state->rx_gain;
 }


double rf_shmem_get_tx_gain(void *h)
 {
   RF_SHMEM_GET_STATE(h);

   RF_SHMEM_DBUG("gain %3.2lf", _state->tx_gain);

   return _state->tx_gain;
 }


double rf_shmem_set_rx_srate(void *h, double rate)
 {
   RF_SHMEM_GET_STATE(h);

   RF_SHMEM_INFO("srate %4.2lf MHz to %4.2lf MHz", 
                 _state->rx_srate / 1e6, rate / 1e6);

   _state->rx_srate = rate;

   return _state->rx_srate;
 }


double rf_shmem_set_tx_srate(void *h, double rate)
 {
   RF_SHMEM_GET_STATE(h);

   RF_SHMEM_INFO("srate %4.2lf MHz to %4.2lf MHz", 
                 _state->tx_srate / 1e6, rate / 1e6);

   _state->tx_srate = rate;

   return _state->tx_srate;
 }


double rf_shmem_set_rx_freq(void *h, double freq)
 {
   RF_SHMEM_GET_STATE(h);

   RF_SHMEM_INFO("freq %4.2lf MHz to %4.2lf MHz", 
                 _state->rx_freq / 1e6, freq / 1e6);

   _state->rx_freq = freq;

   return _state->rx_freq;
 }


double rf_shmem_set_tx_freq(void *h, double freq)
 {
   RF_SHMEM_GET_STATE(h);

   RF_SHMEM_INFO("freq %4.2lf MHz to %4.2lf MHz", 
                 _state->tx_freq / 1e6, freq / 1e6);

   _state->tx_freq = freq;

   return _state->tx_freq;
 }



void rf_shmem_get_time(void *h, time_t *full_secs, double *frac_secs)
 {
   RF_SHMEM_GET_STATE(h);

   rf_shmem_tv_to_fs(&_state->tv_this_tti, full_secs, frac_secs);
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
   RF_SHMEM_GET_STATE(h);

   struct timeval tv_now;

   // nof bytes requested
   const uint32_t nof_bytes = RF_SHMEM_BYTES_X_SAMPLE(nsamples);

   // working in units of subframes
   const int nof_sf = (nsamples / (_state->rx_srate / 1000.0f));

   RF_SHMEM_DBUG("get: nof_samples %u, nof_bytes %u, nof_sf %d", 
                 nsamples,
                 nof_bytes,
                 nof_sf);

   uint32_t nof_bytes_in = 0;

   memset(data[0], 0x0, nof_bytes);

   // for each requested subframe
   for(int i = 0; i < nof_sf; ++i)
     { 
        gettimeofday(&tv_now, NULL);

        // wait for the next tti
        rf_shmem_wait_next_tti(h, &tv_now);

        // find bin for this tti
        const uint32_t bin = get_bin(&_state->tv_this_tti);

        // lock this bin
        if(sem_wait(sem[bin]) < 0)
         {
           RF_SHMEM_WARN("sem_wait error %s", strerror(errno));
         }

        rf_shmem_element_t * element = &_state->rx_segment->elements[bin];
       
        // check current tti w/bin tti 
        if(timercmp(&_state->tv_this_tti, &element->meta.tv_tx_tti, ==))
         {
           const int new_len = rf_shmem_resample(element->meta.tx_srate,
                                                 _state->rx_srate,
                                                 element->iqdata,
                                                 (cf_t*)(((uint8_t*)data[0]) + nof_bytes_in),
                                                 element->meta.nof_bytes);

           nof_bytes_in += new_len;

#ifdef RF_SHMEM_DEBUG_MODE
           char logbuff[256] = {0};
           RF_SHMEM_DBUG("RX, bin %u, new_len %u, total %u, %s", 
                         bin, new_len, nof_bytes_in, printMsg(element, logbuff, sizeof(logbuff)));
#endif
         }

        if(rf_shmem_is_enb(_state))
         {
           // enb clear ul bin on every rx
           memset(element, 0x0, sizeof(*element));
         }

       // unlock
       sem_post(sem[bin]);
     }

   // set rx timestamp to this tti
   rf_shmem_tv_to_fs(&_state->tv_this_tti, full_secs, frac_secs);

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
   RF_SHMEM_GET_STATE(h);

   if(nsamples <= 0)
     {
       RF_SHMEM_DBUG("msg len %d, sob %d, eob %d", nsamples, is_sob, is_eob);

       return 0;
     }

   // some random loss the higher the number the more loss (0-100)
   // xxx todo set via config
   if(_state->rand_loss > 0)
    {
      if((rand() % 100) < _state->rand_loss)
       {
         ++_state->tx_nof_drop;
         return nsamples;
       }
    } 

   struct timeval tv_now, tv_tx_tti;

   // all tx are 4 tti in the future
   // code base may advance timespec slightly which can mess up our bin index
   // so we just force the tx_time here to be 4 sf ahead
   timeradd(&_state->tv_this_tti, &tv_4step, &tv_tx_tti);

   gettimeofday(&tv_now, NULL);

   // this msg tx tti time has passed it should be well into the future
   if(timercmp(&tv_tx_tti, &tv_now, <))
     {
       struct timeval tv_diff;

       timersub(&tv_tx_tti, &tv_now, &tv_diff);

       ++_state->tx_nof_late;

       RF_SHMEM_WARN("TX late, seqnum %lu, tx_tti %ld:%06ld, overrun %6.6lf, total late %zu",
                     _state->tx_seqnum++,
                     tv_tx_tti.tv_sec,
                     tv_tx_tti.tv_usec,
                     -rf_shmem_get_fs(&tv_diff),
                     _state->tx_nof_late);
     }
   else
     {
       const uint32_t nof_bytes = RF_SHMEM_BYTES_X_SAMPLE(nsamples);

       // get the bin for this tx_tti
       const uint32_t bin = get_bin(&tv_tx_tti);

       // lock this bin
       if(sem_wait(sem[bin]) < 0)
         {
           RF_SHMEM_WARN("sem_wait error %s", strerror(errno));
         }

       rf_shmem_element_t * element = &_state->tx_segment->elements[bin];

       // 1 and only 1 enb for tx
       if(rf_shmem_is_enb(_state))
         {
           // enb clears stale dl bin before tx
           memset(element, 0x0, sizeof(*element));
         }

       // new bin entry
       if(element->meta.nof_sf == 0)
        {
          memcpy(element->iqdata, data[0], nof_bytes);

          element->meta.is_sob       = is_sob;
          element->meta.is_eob       = is_eob;
          element->meta.tx_srate     = _state->tx_srate;
          element->meta.seqnum       = _state->tx_seqnum++;
          element->meta.nof_bytes    = nof_bytes;
          element->meta.tv_tx_time   = tv_now;
          element->meta.tv_tx_tti    = tv_tx_tti;
        }
       else
        {
          cf_t * q = (cf_t*)data[0];

          // XXX TODO I/Q data from multiple UL transmission needs to be summed
          for(int i = 0; i < nsamples; ++i)
           {
             // is this correct, just sum iq data ???
             element->iqdata[i] += q[i];
           }
        }

       ++element->meta.nof_sf;

       ++_state->tx_nof_ok;

#ifdef RF_SHMEM_DEBUG_MODE
       char logbuff[256] = {0};
       RF_SHMEM_DBUG("TX, bin %u, %s", bin, printMsg(element, logbuff, sizeof(logbuff)));
#endif

       // unlock
       sem_post(sem[bin]);
     }

   return nsamples;
 }
