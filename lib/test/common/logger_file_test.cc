/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#define NTHREADS 100
#define NMSGS 100

#include "srslte/common/logger_file.h"
#include <stdio.h>
#include <stdlib.h>

using namespace srslte;

typedef struct {
  logger_file* l;
  int          thread_id;
} args_t;

void* thread_loop(void* a)
{
  args_t* args = (args_t*)a;
  char    buf[100];
  for (int i = 0; i < NMSGS; i++) {
    sprintf(buf, "Thread %d: %d", args->thread_id, i);
    args->l->log_char(buf);
  }
  return NULL;
}

void write(std::string filename)
{
  logger_file l;
  l.init(filename);
  pthread_t threads[NTHREADS];
  args_t    args[NTHREADS];
  for (int i = 0; i < NTHREADS; i++) {
    args[i].l         = &l;
    args[i].thread_id = i;
    pthread_create(&threads[i], NULL, &thread_loop, &args[i]);
  }
  for (int i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }
}

bool read(std::string filename)
{
  bool pass = true;
  bool written[NTHREADS][NMSGS];
  int  thread = 0, msg = 0;
  int  r;

  for (int i = 0; i < NTHREADS; i++) {
    for (int j = 0; j < NMSGS; j++) {
      written[i][j] = false;
    }
  }
  FILE* f = fopen(filename.c_str(), "r");
  if (f != NULL) {
    while (fscanf(f, "Thread %d: %d\n", &thread, &msg)) {
      if (thread < NTHREADS && msg < NMSGS) {
        written[thread][msg] = true;
      } else {
        perror("Wrong thread and/or msg");
        fclose(f);
        return false;
      }
    }
    fclose(f);
  }
  for (int i = 0; i < NTHREADS; i++) {
    for (int j = 0; j < NMSGS; j++) {
      if (!written[i][j])
        pass = false;
    }
  }
  return pass;
}

int main(int argc, char** argv)
{
  bool        result;
  std::string f("log.txt");
  write(f);
  result = read(f);

  if (remove(f.c_str())) {
    perror("Removing file");
  }

  if (result) {
    printf("Passed\n");
    exit(0);
  } else {
    printf("Failed\n;");
    exit(1);
  }
}
