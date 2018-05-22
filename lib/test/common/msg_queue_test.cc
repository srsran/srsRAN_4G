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

#define NMSGS    1000000

#include <stdio.h>
#include "srslte/upper/rlc_tx_queue.h"

using namespace srslte;

typedef struct {
  rlc_tx_queue   *q;
}args_t;

void* write_thread(void *a) {
  args_t *args = (args_t*)a;
  for(uint32_t i=0;i<NMSGS;i++)
  {
    byte_buffer_t *b = new byte_buffer_t;
    memcpy(b->msg, &i, 4);
    b->N_bytes = 4;
    args->q->write(b);
  }
  return NULL;
}

int main(int argc, char **argv) {
  bool                 result;
  rlc_tx_queue         q;
  byte_buffer_t       *b;
  pthread_t            thread;
  args_t               args;
  u_int32_t            r;

  result = true;
  args.q = &q;

  pthread_create(&thread, NULL, &write_thread, &args);

  for(uint32_t i=0;i<NMSGS;i++)
  {
    q.read(&b);
    memcpy(&r, b->msg, 4);
    delete b;
    if(r != i)
      result = false;
  }

  pthread_join(thread, NULL);

  if (q.size() != 0 || q.size_bytes() != 0) {
    result = false;
  }

  if(result) {
    printf("Passed\n");
    exit(0);
  }else{
    printf("Failed\n;");
    exit(1);
  }
}
