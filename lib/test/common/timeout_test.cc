/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include <pthread.h>
#include <stdio.h>
#include "srslte/common/timeout.h"

using namespace srslte;

class callback
    : public timeout_callback
{
public:
  callback() {
    finished = false; 
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cvar, NULL);
    bzero(&start_time, sizeof(start_time));
  }
  
  void timeout_expired(uint32_t timeout_id)
  {
    pthread_mutex_lock(&mutex);
    finished = true; 
    pthread_cond_signal(&cvar);
    pthread_mutex_unlock(&mutex);
  }
  void wait()
  {
    pthread_mutex_lock(&mutex);
    while(!finished) {
      pthread_cond_wait(&cvar, &mutex);
    }
    pthread_mutex_unlock(&mutex);
  }
  struct timeval start_time[3];
private:
  bool              finished;
  pthread_cond_t    cvar; 
  pthread_mutex_t   mutex; 
};


int timer_thread_test()
{
  bool result;
  uint32_t id       = 0;
  uint32_t duration_msec = 5;
  uint32_t result_tolerance = 1;

  callback c;
  timeout t;

  gettimeofday(&c.start_time[1], NULL);
  t.start(duration_msec);

  while (t.is_running() && !t.expired()) {
    printf("time to expire=%dms\n", t.get_msec_to_expire());
    usleep(1000);
  }

  gettimeofday(&c.start_time[2], NULL);
  get_time_interval(c.start_time);
  uint32_t diff_ms = c.start_time[0].tv_usec*1e-3;
  printf("Target duration: %dms, started: %ld:%ld, ended: %ld:%ld, actual duration %dms\n",
         duration_msec, c.start_time[1].tv_sec, c.start_time[1].tv_usec, c.start_time[2].tv_sec, c.start_time[2].tv_usec, diff_ms);

  result = ((duration_msec - result_tolerance) < diff_ms || diff_ms < (duration_msec + result_tolerance));

  if(result) {
    printf("Timer thread test passed\n");
    return 0;
  }else{
    return -1;
  }
}

int single_thread_test()
{
  bool result;
  uint32_t id       = 0;
  uint32_t duration_msec = 5;

  callback c;
  timeout t;

  gettimeofday(&c.start_time[1], NULL);
  t.start(duration_msec, 0, &c);
  c.wait();
  gettimeofday(&c.start_time[2], NULL);
  get_time_interval(c.start_time);
  uint32_t diff_ms = c.start_time[0].tv_usec*1e-3;
  printf("Target duration: %dms, started: %ld:%ld, ended: %ld:%ld, actual duration %dms\n",
         duration_msec, c.start_time[1].tv_sec, c.start_time[1].tv_usec, c.start_time[2].tv_sec, c.start_time[2].tv_usec, diff_ms);

  result = (diff_ms == duration_msec);

  if(result) {
    printf("Single thread test passed\n");
    return 0;
  }else{
    return -1;
  }
}


int main(int argc, char **argv)
{
  if (single_thread_test()) {
    printf("Single thread test failed.\n");
    return -1;
  }

  if (timer_thread_test()) {
    printf("Timer thread test failed.\n");
    return -1;
  }

  return 0;
}
