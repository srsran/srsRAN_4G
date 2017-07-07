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

#define NTHREADS 100
#define NMSGS    100

#include <stdio.h>
#include "srslte/common/log_filter.h"
#include "srslte/common/logger_file.h"

using namespace srslte;

typedef struct {
  logger_file *l;
  int thread_id;
}args_t;

void* thread_loop(void *a) {
  args_t *args = (args_t*)a;
  char buf[100];

  sprintf(buf, "LAYER%d", args->thread_id);
  log_filter filter(buf, args->l);
  filter.set_level(LOG_LEVEL_INFO);

  for(int i=0;i<NMSGS;i++)
  {
    filter.error("Thread %d: %d", args->thread_id, i);
    filter.warning("Thread %d: %d", args->thread_id, i);
    filter.info("Thread %d: %d", args->thread_id, i);
    filter.debug("Thread %d: %d", args->thread_id, i);
  }
  return NULL;
}

void* thread_loop_hex(void *a) {
  args_t *args = (args_t*)a;
  char    buf[100];
  uint8_t hex[100];

  for(int i=0;i<100;i++)
    hex[i] = i & 0xFF;
  sprintf(buf, "LAYER%d", args->thread_id);
  log_filter filter(buf, args->l);
  filter.set_level(LOG_LEVEL_DEBUG);
  filter.set_hex_limit(32);

  for(int i=0;i<NMSGS;i++)
  {
    filter.error_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
    filter.warning_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
    filter.info_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
    filter.debug_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
  }
  return NULL;
}

void write(std::string filename) {
  logger_file l;
  l.init(filename);
  pthread_t threads[NTHREADS];
  args_t    args[NTHREADS];
  for(int i=0;i<NTHREADS;i++) {
    args[i].l = &l;
    args[i].thread_id = i;
    pthread_create(&threads[i], NULL, &thread_loop_hex, &args[i]);
  }
  for(int i=0;i<NTHREADS;i++) {
    pthread_join(threads[i], NULL);
  }
}

bool read(std::string filename) {
  bool pass = true;
  bool written[NTHREADS][NMSGS];
  int thread, msg;
  int r;

  for(int i=0;i<NTHREADS;i++) {
    for(int j=0;j<NMSGS;j++) {
      written[i][j] = false;
    }
  }
  FILE *f = fopen(filename.c_str(), "r");
  if(f!=NULL) {
    while(fscanf(f, "Thread %d: %d\n", &thread, &msg)) {
      written[thread][msg] = true;
    }
    fclose(f);
  }
  for(int i=0;i<NTHREADS;i++) {
    for(int j=0;j<NMSGS;j++) {
      if(!written[i][j]) pass = false;
    }
  }
  return pass;
}

int main(int argc, char **argv) {
  bool result;
  std::string f("log.txt");
  write(f);
//  result = read(f);
//  remove(f.c_str());
//  if(result) {
//    printf("Passed\n");
//    exit(0);
//  }else{
//    printf("Failed\n;");
//    exit(1);
//  }
}
