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


#include <assert.h>
#include <stdio.h>
#include "srslte/common/thread_pool.h"

#define DEBUG 0
#define debug_thread(fmt, ...) do { if(DEBUG) printf(fmt, __VA_ARGS__); } while(0)

#define USE_QUEUE

namespace srslte {
 
  
void thread_pool::worker::setup(uint32_t id, thread_pool *parent, uint32_t prio, uint32_t mask)
{
  my_id = id; 
  my_parent = parent;
  if(mask == 255)
  {
    start(prio);
  }
  else
  {
    start_cpu_mask(prio,mask);
  }
  
}

void thread_pool::worker::run_thread()
{
  running = true;   
  while(running)  {
    wait_to_start();
    if (running) {
      work_imp();
      finished();
    }
  }
}

uint32_t thread_pool::worker::get_id()
{
  return my_id;
}

void thread_pool::worker::stop()
{
  running = false; 
  pthread_cond_signal(&my_parent->cvar[my_id]);
  wait_thread_finish();
}

thread_pool::thread_pool(uint32_t max_workers_)  : 
                                  workers(max_workers_),
                                  status(max_workers_),
                                  cvar(max_workers_),
                                  mutex(max_workers_)
{
  max_workers = max_workers_;
  for (uint32_t i=0;i<max_workers;i++) {
    workers[i] = NULL;
    status[i] = IDLE; 
    pthread_mutex_init(&mutex[i], NULL);
    pthread_cond_init(&cvar[i], NULL);
  }
  pthread_mutex_init(&mutex_queue, NULL);
  pthread_cond_init(&cvar_queue, NULL);
  running = true; 
  nof_workers = 0; 
}

void thread_pool::init_worker(uint32_t id, worker *obj, uint32_t prio, uint32_t mask)
{
  if (id < max_workers) {
    if (id >= nof_workers) {
      nof_workers = id+1;
    }
    pthread_mutex_lock(&mutex_queue);   
    workers[id] = obj; 
    available_workers.push(obj);    
    obj->setup(id, this, prio, mask);
    pthread_cond_signal(&cvar_queue);
    pthread_mutex_unlock(&mutex_queue);    
  }
}

void thread_pool::stop()
{
  /* Stop any thread waiting for available worker */
  running = false; 
  
  /* Now stop all workers */
  for (uint32_t i=0;i<nof_workers;i++) {
    if (workers[i]) {
      workers[i]->stop(); 
      // Need to call start to wake it up 
      start_worker(i);
      workers[i]->wait_thread_finish();
    }
    pthread_cond_destroy(&cvar[i]);
    pthread_mutex_destroy(&mutex[i]);
  }
  pthread_cond_destroy(&cvar_queue);
  pthread_mutex_destroy(&mutex_queue);
}


void thread_pool::worker::release()
{
  finished();
}

void thread_pool::worker::wait_to_start()
{
  
  debug_thread("wait_to_start() id=%d, status=%d, enter\n", my_id, my_parent->status[my_id]);

  pthread_mutex_lock(&my_parent->mutex[my_id]); 
  while(my_parent->status[my_id] != START_WORK && running) {
    pthread_cond_wait(&my_parent->cvar[my_id], &my_parent->mutex[my_id]);
  }
  my_parent->status[my_id] = WORKING; 
  pthread_mutex_unlock(&my_parent->mutex[my_id]);

  debug_thread("wait_to_start() id=%d, status=%d, exit\n", my_id, my_parent->status[my_id]);
}

void thread_pool::worker::finished()
{
#ifdef USE_QUEUE
  pthread_mutex_lock(&my_parent->mutex[my_id]); 
  my_parent->status[my_id] = IDLE; 
  pthread_mutex_unlock(&my_parent->mutex[my_id]); 

  pthread_mutex_lock(&my_parent->mutex_queue);  
  pthread_cond_signal(&my_parent->cvar_queue);
  pthread_mutex_unlock(&my_parent->mutex_queue); 
#else
  pthread_mutex_lock(&my_parent->mutex[my_id]); 
  my_parent->status[my_id] = IDLE; 
  pthread_cond_signal(&my_parent->cvar[my_id]);
  pthread_mutex_unlock(&my_parent->mutex[my_id]);   
#endif
}


thread_pool::worker* thread_pool::wait_worker()
{
  return wait_worker(0);
}

bool thread_pool::find_finished_worker(uint32_t tti, uint32_t *id) {
  for(uint32_t i=0;i<nof_workers;i++) {
    if (status[i] == IDLE) {
      *id = i; 
      return true; 
    }
  }
  return false; 
}

thread_pool::worker* thread_pool::wait_worker(uint32_t tti)
{
  thread_pool::worker *x; 
  
#ifdef USE_QUEUE
  debug_thread("wait_worker() - enter - tti=%d, state0=%d, state1=%d\n", tti, status[0], status[1]);
  pthread_mutex_lock(&mutex_queue); 
  uint32_t id = 0;
  while(!find_finished_worker(tti, &id) && running) {
    pthread_cond_wait(&cvar_queue, &mutex_queue);    
  }
  pthread_mutex_unlock(&mutex_queue);
  if (running) {
    x = workers[id];
    pthread_mutex_lock(&mutex[id]); 
    status[id] = WORKER_READY;
    pthread_mutex_unlock(&mutex[id]); 
} else {
    x = NULL; 
  }  
  debug_thread("wait_worker() - exit - id=%d\n", id);
#else
  
  uint32_t id = tti%nof_workers;
  pthread_mutex_lock(&mutex[id]); 
  while(status[id] != IDLE && running) {
    pthread_cond_wait(&cvar[id], &mutex[id]);    
  }
  if (running) {
    x = (worker*) workers[id];
    status[id] = WORKER_READY;
  } else {
    x = NULL; 
  }
  pthread_mutex_unlock(&mutex[id]);
#endif  
  return x; 
}

thread_pool::worker* thread_pool::wait_worker_nb(uint32_t tti)
{
  thread_pool::worker *x; 
  
  debug_thread("wait_worker() - enter - tti=%d, state0=%d, state1=%d\n", tti, status[0], status[1]);
  pthread_mutex_lock(&mutex_queue); 
  uint32_t id = 0;
  if (find_finished_worker(tti, &id)) {
    x = workers[id];
  } else {
    x = NULL; 
  }
  pthread_mutex_unlock(&mutex_queue);
  if (running && x) {
    pthread_mutex_lock(&mutex[id]); 
    status[id] = WORKER_READY;
    pthread_mutex_unlock(&mutex[id]); 
} else {
    x = NULL; 
  }  
  debug_thread("wait_worker() - exit - id=%d\n", id);
  return x; 
}


void thread_pool::start_worker(uint32_t id) {
  if (id < nof_workers) {
    pthread_mutex_lock(&mutex[id]); 
    status[id] = START_WORK;
    pthread_cond_signal(&cvar[id]);
    pthread_mutex_unlock(&mutex[id]);
    debug_thread("start_worker() id=%d, status=%d\n", id, status[id]);
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
  return NULL;
}

uint32_t thread_pool::get_nof_workers()
{
  return nof_workers;
}

}


