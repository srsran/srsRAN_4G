/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <pthread.h>
#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif
  bool threads_new_rt(pthread_t *thread, void *(*start_routine) (void*), void *arg);
  bool threads_new_rt_prio(pthread_t *thread, void *(*start_routine) (void*), void *arg, uint32_t prio_offset);
  bool threads_new_rt_cpu(pthread_t *thread, void *(*start_routine) (void*), void *arg, int cpu, uint32_t prio_offset);
  void threads_print_self();

#ifdef __cplusplus
}
  
#ifndef THREADS_
#define THREADS_   
  
class thread
{
public: 
  bool start(uint32_t prio = 0) {
    return threads_new_rt_prio(&_thread, thread_function_entry, this, prio);    
  }
  void print_priority() {
    threads_print_self();
  }
  void wait_thread_finish() {
    pthread_join(_thread, NULL);
  }
protected:
  virtual void run_thread() = 0; 
private:
  static void *thread_function_entry(void *_this)  { ((thread*) _this)->run_thread();}
  pthread_t _thread;
};
  

#endif

#endif

