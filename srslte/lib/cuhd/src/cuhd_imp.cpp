/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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

#include <uhd/utils/thread_priority.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <iostream>
#include <complex>
#include <cstdio>
#include <uhd/utils/msg.hpp>
#include <sys/time.h>

#include "cuhd_handler.hpp"
#include "srslte/cuhd/cuhd.h"

//#define METADATA_VERBOSE

//#define HIDE_MESSAGES

void my_handler(uhd::msg::type_t type, const std::string & msg)
{
  //handle the message...
}

typedef _Complex float complex_t;

#define SAMPLE_SZ sizeof(complex_t)

bool isLocked(void *h)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  std::vector < std::string > mb_sensors =
    handler->usrp->get_mboard_sensor_names();
  std::vector < std::string > rx_sensors =
    handler->usrp->get_rx_sensor_names(0);
  if (std::find(rx_sensors.begin(), rx_sensors.end(), "lo_locked") !=
      rx_sensors.end()) {
    return handler->usrp->get_rx_sensor("lo_locked", 0).to_bool();
  } else if (std::find(mb_sensors.begin(), mb_sensors.end(), "ref_locked") !=
             mb_sensors.end()) {
    return handler->usrp->get_mboard_sensor("ref_locked", 0).to_bool();
  } else {
    usleep(500);
    return true;
  }
}

bool cuhd_rx_wait_lo_locked(void *h)
{

  double report = 0.0;
  while (isLocked(h) && report < 30.0) {
    report += 0.1;
    usleep(1000);
  }
  return isLocked(h);
}

int cuhd_start_rx_stream(void *h)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
  cmd.time_spec = handler->usrp->get_time_now();
  cmd.stream_now = true;
  handler->usrp->issue_stream_cmd(cmd);
  return 0;
}

int cuhd_stop_rx_stream(void *h)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
  cmd.time_spec = handler->usrp->get_time_now();
  cmd.stream_now = true;
  handler->usrp->issue_stream_cmd(cmd);
  return 0;
}

void cuhd_flush_buffer(void *h)
{
  int n; 
  _Complex float tmp[1024];
  do {
    n = cuhd_recv(h, tmp, 1024, 0);
  } while (n > 0);  
}

int cuhd_start_rx_stream_nsamples(void *h, uint32_t nsamples)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_MORE);
  cmd.time_spec = handler->usrp->get_time_now();
  cmd.stream_now = true;
  cmd.num_samps = nsamples;
  handler->usrp->issue_stream_cmd(cmd);
  return 0;
}

double cuhd_set_rx_gain_th(void *h, double gain)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  gain = handler->rx_gain_range.clip(gain);     
  if (gain != handler->new_rx_gain) {
    pthread_mutex_lock(&handler->mutex);
    handler->new_rx_gain = gain; 
    pthread_cond_signal(&handler->cond);
    pthread_mutex_unlock(&handler->mutex);
  }
  return gain; 
}

void cuhd_set_tx_rx_gain_offset(void *h, double offset) {
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  handler->tx_rx_gain_offset = offset; 
}

/* This thread listens for set_rx_gain commands to the USRP */
static void* thread_gain_fcn(void *h) {
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  while(1) {
    pthread_mutex_lock(&handler->mutex);
    while(handler->cur_rx_gain == handler->new_rx_gain) {
      pthread_cond_wait(&handler->cond, &handler->mutex);
    }
    handler->cur_rx_gain = handler->new_rx_gain; 
    pthread_mutex_unlock(&handler->mutex);
    cuhd_set_rx_gain(h, handler->cur_rx_gain);
    if (handler->tx_gain_same_rx) {
      cuhd_set_tx_gain(h, handler->cur_rx_gain+handler->tx_rx_gain_offset);
    }
    //printf("Set gain %.2f\n", handler->cur_rx_gain);
  }
}

float cuhd_get_rx_gain_offset(void *h) {
  return 15; 
}

int cuhd_open_(char *args, void **h, bool create_thread_gain, bool tx_gain_same_rx)
{
  uhd::set_thread_priority_safe();
  cuhd_handler *handler = new cuhd_handler();
  std::string _args = std::string(args);
  handler->usrp = uhd::usrp::multi_usrp::make(_args + ", master_clock_rate=30720000, recv_frame_size=7696,num_recv_frames=64,send_frame_size=7696,num_send_frames=64");
  handler->usrp->set_clock_source("internal");
  
#ifdef HIDE_MESSAGES
  uhd::msg::register_handler(my_handler);
#endif
  
  std::string otw, cpu;
  otw = "sc16";
  cpu = "fc32";
  uhd::stream_args_t stream_args(cpu, otw);
  handler->rx_stream = handler->usrp->get_rx_stream(stream_args);
  handler->tx_stream = handler->usrp->get_tx_stream(stream_args);

  handler->rx_nof_samples = handler->rx_stream->get_max_num_samps();
  handler->tx_nof_samples = handler->tx_stream->get_max_num_samps();
  
  handler->tx_gain_same_rx = tx_gain_same_rx; 
  handler->tx_rx_gain_offset = 0.0; 
  handler->rx_gain_range = handler->usrp->get_rx_gain_range();

  
  *h = handler;

  if (create_thread_gain) {
    if (pthread_mutex_init(&handler->mutex, NULL)) {
      return -1; 
    }
    if (pthread_cond_init(&handler->cond, NULL)) {
      return -1; 
    }

    if (pthread_create(&handler->thread_gain, NULL, thread_gain_fcn, *h)) {
      perror("pthread_create");
      return -1; 
    }
  }
  
  return 0;
}

int cuhd_open(char *args, void **h) {
  return cuhd_open_(args, h, false, false); 
}

int cuhd_open_th(char *args, void **h, bool tx_gain_same_rx) {
  return cuhd_open_(args, h, true, tx_gain_same_rx); 
}


int cuhd_close(void *h)
{
  cuhd_stop_rx_stream(h);
  /** Something else to close the USRP?? */
  return 0;
}


double cuhd_set_rx_srate(void *h, double freq)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  handler->usrp->set_rx_rate(freq);  
  return freq; 
}

double cuhd_set_rx_gain(void *h, double gain)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  handler->usrp->set_rx_gain(gain);
  return handler->usrp->get_rx_gain();
}

double cuhd_get_rx_gain(void *h)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  return handler->usrp->get_rx_gain();
}

double cuhd_get_tx_gain(void *h)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  return handler->usrp->get_tx_gain();
}

double cuhd_set_rx_freq(void *h, double freq)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  handler->usrp->set_rx_freq(freq);
  return freq;
}

double cuhd_set_rx_freq_offset(void *h, double freq,  double off) {
  cuhd_handler* handler = static_cast<cuhd_handler*>(h);
  handler->usrp->set_rx_freq(uhd::tune_request_t(freq, off));
  return handler->usrp->get_rx_freq();
}

int cuhd_recv(void *h, void *data, uint32_t nsamples, bool blocking)
{
  return cuhd_recv_with_time(h, data, nsamples, blocking, NULL, NULL);
}

int cuhd_recv_with_time(void *h,
                    void *data,
                    uint32_t nsamples,
                    bool blocking,
                    time_t *secs,
                    double *frac_secs) 
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  uhd::rx_metadata_t md, md_first;
  if (blocking) {
    int n = 0, p;
    complex_t *data_c = (complex_t *) data;
    do {
      size_t rx_samples = handler->rx_nof_samples;
      if (rx_samples > nsamples - n) {
        rx_samples = nsamples - n; 
      }
      p = handler->rx_stream->recv(&data_c[n], rx_samples, n==0?md_first:md);
      if (p == -1) {
        return -1;
      }
      n += p;
#ifdef METADATA_VERBOSE
      if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
        std::cout << "\nError code: " << md.to_pp_string() << "\n\n";
      }
#endif
    } while (n < nsamples                                              && 
             md.error_code == uhd::rx_metadata_t::ERROR_CODE_NONE);
  } else {
    return handler->rx_stream->recv(data, nsamples, md, 0.0);
  }
  if (secs) {
    *secs = md_first.time_spec.get_full_secs();
  }
  if (frac_secs) {
    *frac_secs = md_first.time_spec.get_frac_secs();    
  }
  return nsamples;
}
double cuhd_set_tx_gain(void *h, double gain)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  handler->usrp->set_tx_gain(gain);
  return handler->usrp->get_tx_gain();
}

double cuhd_set_tx_srate(void *h, double freq)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  handler->usrp->set_tx_rate(freq);
  handler->tx_rate = handler->usrp->get_tx_rate();
  return handler->tx_rate; 
}

double cuhd_set_tx_freq(void *h, double freq)
{
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  handler->usrp->set_tx_freq(freq);
  return handler->usrp->get_tx_freq();
}


double cuhd_set_tx_freq_offset(void *h, double freq,  double off) {
  cuhd_handler* handler = static_cast<cuhd_handler*>(h);
  handler->usrp->set_tx_freq(uhd::tune_request_t(freq, off));
  return handler->usrp->get_tx_freq();
}

void cuhd_get_time(void *h, time_t *secs, double *frac_secs) {
  cuhd_handler *handler = static_cast < cuhd_handler * >(h);
  uhd::time_spec_t now = handler->usrp->get_time_now();
  if (secs) {
    *secs = now.get_full_secs();    
  }
  if (frac_secs) {
    *frac_secs = now.get_frac_secs();    
  }
}

                   
int cuhd_send_timed3(void *h,
                     void *data,
                     int nsamples,
                     time_t secs,
                     double frac_secs,                      
                     bool has_time_spec,
                     bool blocking,
                     bool is_start_of_burst,
                     bool is_end_of_burst) 
{
  cuhd_handler* handler = static_cast<cuhd_handler*>(h);
  uhd::tx_metadata_t md;
  md.has_time_spec = has_time_spec;
  if (has_time_spec) {
    md.time_spec = uhd::time_spec_t(secs, frac_secs);
  }

  if (blocking) {
    int n = 0, p;
    complex_t *data_c = (complex_t *) data;
    do {
      size_t tx_samples = handler->tx_nof_samples;
      
      // First packet is start of burst if so defined, others are never 
      if (n == 0) {
        md.start_of_burst = is_start_of_burst;
      } else {
        md.start_of_burst = false; 
      }
      
      // middle packets are never end of burst, last one as defined
      if (nsamples - n > tx_samples) {
        md.end_of_burst = false; 
      } else {
        tx_samples = nsamples - n; 
        md.end_of_burst = is_end_of_burst;
      }
      p = handler->tx_stream->send(&data_c[n], tx_samples, md);
      if (p == -1) {
        return -1;
      }
      // Increase time spec 
      md.time_spec += tx_samples/handler->tx_rate; 
      n += p;
    } while (n < nsamples);
    return nsamples;
  } else {
    return handler->tx_stream->send(data, nsamples, md, 0.0);
  }
}

int cuhd_send(void *h, void *data, uint32_t nsamples, bool blocking)
{
  return cuhd_send2(h, data, nsamples, blocking, true, true); 
}

int cuhd_send2(void *h, void *data, uint32_t nsamples, bool blocking, bool start_of_burst, bool end_of_burst)
{
  return cuhd_send_timed3(h, data, nsamples, 0, 0, false, blocking, start_of_burst, end_of_burst);
}


int cuhd_send_timed(void *h,
                    void *data,
                    int nsamples,
                    time_t secs,
                    double frac_secs) 
{
  return cuhd_send_timed2(h, data, nsamples, secs, frac_secs, true, true);
}

int cuhd_send_timed2(void *h,
                    void *data,
                    int nsamples,
                    time_t secs,
                    double frac_secs,
                    bool is_start_of_burst,
                    bool is_end_of_burst) 
{
  return cuhd_send_timed3(h, data, nsamples, secs, frac_secs, true, true, is_start_of_burst, is_end_of_burst);
}
