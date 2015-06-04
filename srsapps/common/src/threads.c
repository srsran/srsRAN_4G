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
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

#include "srsapps/common/threads.h"

bool threads_new_rt(pthread_t *thread, void *(*start_routine) (void*), void *arg) {
  return threads_new_rt_cpu(thread, start_routine, arg, -1);
}

bool threads_new_rt_cpu(pthread_t *thread, void *(*start_routine) (void*), void *arg, int cpu) {
  bool ret = false; 
  
  pthread_attr_t attr;
  struct sched_param param;
  param.sched_priority = sched_get_priority_max(SCHED_FIFO);  

  pthread_attr_init(&attr);
  if (pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
    perror("pthread_attr_setinheritsched");
  }
  if (pthread_attr_setschedpolicy(&attr, SCHED_FIFO)) {
    perror("pthread_attr_setschedpolicy");
  }
  if (pthread_attr_setschedparam(&attr, &param)) {
    perror("pthread_attr_setschedparam");
    fprintf(stderr, "Error not enough privileges to set Scheduling priority\n");
  }
  if (cpu != -1) {
    cpu_set_t cpuset; 
    CPU_ZERO(&cpuset);
    CPU_SET((size_t) cpu, &cpuset);
    if (pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset)) {
      perror("pthread_attr_setaffinity_np");
    }
  } 
  int err = pthread_create(thread, &attr, start_routine, arg);
  if (err) {
    if (1 == err) {
      perror("Failed to create thread - permission error. Running with root permissions?");
    }
    perror("pthread_create");
  } else {
    ret = true; 
  }
  pthread_attr_destroy(&attr);
  return ret; 
}

void threads_print_self() {
  pthread_t thread;
  cpu_set_t cpuset;
  struct sched_param param;
  int policy;
  const char *p;
  int s,j;

  thread = pthread_self();

  s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
  if (s != 0) {
    printf("error pthread_getaffinity_np: %s\n",strerror(s));
  }

  printf("Set returned by pthread_getaffinity_np() contained:\n");
  for (j = 0; j < CPU_SETSIZE; j++) {
    if (CPU_ISSET(j, &cpuset)) {
      printf("    CPU %d\n", j);
    }
  }

  s = pthread_getschedparam(thread, &policy, &param);
  if (s != 0) {
    printf("error pthread_getaffinity_np: %s\n", strerror(s));
  }

  switch(policy) {
  case SCHED_FIFO:
    p = "SCHED_FIFO";
    break;
  case SCHED_RR:
    p = "SCHED_RR";
    break;
  default:
    p = "Other";
    break;
  }

  printf("Sched policy is %s. Priority is %d\n",p,param.sched_priority);
}
