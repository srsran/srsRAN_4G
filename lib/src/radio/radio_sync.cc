/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <pthread.h>
#include <semaphore.h>

#include "srslte/radio/radio_sync.h"
#include "srslte/srslte.h"

namespace srslte {

radio_sync::radio_sync()
{
  thread_args = NULL;
}

typedef struct {
  pthread_t           pthread;
  sem_t               start;
  sem_t               finish;
  int                 status;
  srslte_rf_t*        device;
  uint32_t            nsamples;
  cf_t**              data;
  srslte_timestamp_t* timestamp;
  bool                running;
  bool                start_streaming;
  bool                receive;
  bool                synch;
  bool                exit;
} radio_synch_thread_t;

static void* radio_synch_thread(void* ptr)
{
  radio_synch_thread_t* h = (radio_synch_thread_t*)ptr;

  do {
    sem_wait(&h->start);

    if (!h->exit) {
      if (h->synch) {
        h->status = srslte_rf_sync(h->device);
      }

      if (h->start_streaming) {
        srslte_rf_start_rx_stream(h->device, false);
      }

      if (h->receive) {
        srslte_rf_recv_with_time_multi(
            h->device, (void**)h->data, h->nsamples, true, &h->timestamp->full_secs, &h->timestamp->frac_secs);
      }
    }

    sem_post(&h->finish);
  } while (!h->exit);

  return NULL;
}

bool radio_sync::init(srslte_rf_t* device)
{
  bool                  ret = false;
  radio_synch_thread_t* h   = (radio_synch_thread_t*)calloc(sizeof(radio_synch_thread_t), 1);
  if (!h) {
    ERROR("Error: Allocating multiple radio synch thread parameters\n");
    goto clean_exit;
  }

  thread_args        = h;
  h->device          = device;
  h->start_streaming = false;
  h->synch           = false;
  h->running         = false;
  h->exit            = false;

  /* Initialise semaphores */
  if (sem_init(&h->start, 0, 0)) {
    ERROR("Error: creating start semaphore\n");
    goto clean_exit;
  }

  if (sem_init(&h->finish, 0, 0)) {
    ERROR("Error: creating start semaphore\n");
    goto clean_exit;
  }

  /* Create thread */
  if (pthread_create(&h->pthread, NULL, &radio_synch_thread, h)) {
    ERROR("Error: Allocating multiple radio synch thread parameters\n");
    goto clean_exit;
  }

  ret = true;

clean_exit:
  if (!ret) {
    delete this;
  }

  return ret;
}

radio_sync::~radio_sync()
{
  radio_synch_thread_t* h = (radio_synch_thread_t*)thread_args;

  if (h) {
    h->exit = true;
    sem_post(&h->start);

    pthread_join(h->pthread, NULL);
    pthread_detach(h->pthread);

    sem_destroy(&h->start);
    sem_destroy(&h->finish);

    free(h);
    thread_args = NULL;
  }
}

void radio_sync::issue_sync()
{
  radio_synch_thread_t* h = (radio_synch_thread_t*)thread_args;

  if (h) {
    h->running = true;
    ZERO_OBJECT(h->data);
    h->nsamples        = 0;
    h->timestamp       = NULL;
    h->synch           = true;
    h->start_streaming = false;
    h->receive         = false;
    h->exit            = false;
    sem_post(&h->start);
  }
}

void radio_sync::issue_rx(cf_t*               data[SRSLTE_MAX_PORTS],
                          uint32_t            nsamples,
                          srslte_timestamp_t* timestamp,
                          bool                start_streaming)
{
  radio_synch_thread_t* h = (radio_synch_thread_t*)thread_args;

  if (h) {
    h->running         = true;
    h->data            = data;
    h->nsamples        = nsamples;
    h->timestamp       = timestamp;
    h->synch           = start_streaming;
    h->start_streaming = start_streaming;
    h->receive         = true;
    h->exit            = false;
    sem_post(&h->start);
  }
}

int radio_sync::wait()
{
  int                   ret = SRSLTE_ERROR;
  radio_synch_thread_t* h   = (radio_synch_thread_t*)thread_args;

  if (h) {
    if (h->running) {
      sem_wait(&h->finish);
      h->running = false;
      ret        = h->status;
    } else {
      ret = SRSLTE_SUCCESS;
    }
  }

  return ret;
}

}
