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

#include "srsapps/common/thread_pool.h"

namespace srslte {
 
  
void thread_pool::worker::setup(uint32_t id, thread_pool *parent)
{
  my_id = id; 
  my_parent = parent;   
  start();
}

void thread_pool::worker::run_thread()
{
  running = true;   
  while(running)  {
    wait_to_start();
    if (running) {
      work_imp();
    }
    finished();
  }
}

void thread_pool::worker::stop()
{
  running = false; 
}

void thread_pool::worker::wait_to_start()
{
  pthread_mutex_lock(&my_parent->mutex_start); 
  while(!my_parent->begin[my_id]) {
    pthread_cond_wait(&my_parent->cvar_start, &my_parent->mutex_start);
  }
  my_parent->begin[my_id] = false; 
  pthread_mutex_unlock(&my_parent->mutex_start);
}

void thread_pool::worker::finished()
{
  pthread_mutex_lock(&my_parent->mutex_stop);   
  my_parent->available_workers.push(this); 
  pthread_cond_signal(&my_parent->cvar_stop);
  pthread_mutex_unlock(&my_parent->mutex_stop);
}

thread_pool::thread_pool(uint32_t nof_workers_)  : workers(nof_workers_), begin(nof_workers_) {
  nof_workers = nof_workers_;
  for (int i=0;i<nof_workers;i++) {
    workers[i] = NULL;
  }
  pthread_mutex_init(&mutex_start, NULL);
  pthread_mutex_init(&mutex_stop, NULL);
  pthread_cond_init(&cvar_start, NULL);
  pthread_cond_init(&cvar_stop, NULL);
}

void thread_pool::init_worker(uint32_t id, worker *obj)
{
  if (id < nof_workers) {
    begin[id] = false; 
    workers[id] = obj; 
    available_workers.push(obj);    
    obj->setup(id, this);
  }
}

void thread_pool::stop()
{
  for (uint32_t i=0;i<nof_workers;i++) {
    if (workers[i]) {
      workers[i]->stop(); 
      start_worker(i);
      workers[i]->wait_thread_finish();
    }
  }
  pthread_mutex_destroy(&mutex_start);
  pthread_mutex_destroy(&mutex_stop);
  pthread_cond_destroy(&cvar_start);
  pthread_cond_destroy(&cvar_stop);
}

void thread_pool::start_worker(uint32_t id) {
  if (workers[id]) {
    pthread_mutex_lock(&mutex_start); 
    begin[id%nof_workers] = true; 
    pthread_cond_signal(&cvar_start);
    pthread_mutex_unlock(&mutex_start);
  }
}

void thread_pool::start_worker(worker* x)
{
  for (uint32_t i=0;i<nof_workers;i++) {
    if (x == workers[i]) {
      start_worker(i);
      return;
    }
  }
}

thread_pool::worker* thread_pool::get_worker(uint32_t id)
{
  if (id < nof_workers) {
    return workers[id];
  }
}

uint32_t thread_pool::get_nof_workers()
{
  return nof_workers;
}

thread_pool::worker* thread_pool::wait_worker()
{
  thread_pool::worker *x; 
  pthread_mutex_lock(&mutex_stop); 
  while(!available_workers.empty()) {
    pthread_cond_wait(&cvar_stop, &mutex_stop);    
  }
  x = (worker*) available_workers.top();
  available_workers.pop();
  pthread_mutex_unlock(&mutex_stop);
  return x; 
}

}


