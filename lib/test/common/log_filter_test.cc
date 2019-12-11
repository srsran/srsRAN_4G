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

#define NTHREADS 100
#define NMSGS 100

#include "srslte/common/log_filter.h"
#include "srslte/common/logger_file.h"
#include "srslte/common/test_common.h"
#include <stdio.h>

using namespace srslte;

typedef struct {
  logger_file* l;
  int          thread_id;
} args_t;

void* thread_loop(void* a)
{
  args_t* args = (args_t*)a;
  char    buf[100];

  sprintf(buf, "LAYER%d", args->thread_id);
  log_filter filter(buf, args->l);
  filter.set_level(LOG_LEVEL_INFO);

  for (int i = 0; i < NMSGS; i++) {
    filter.error("Thread %d: %d", args->thread_id, i);
    filter.warning("Thread %d: %d", args->thread_id, i);
    filter.info("Thread %d: %d", args->thread_id, i);
    filter.debug("Thread %d: %d", args->thread_id, i);
  }
  return NULL;
}

void* thread_loop_hex(void* a)
{
  args_t* args = (args_t*)a;
  char    buf[100];
  uint8_t hex[100];

  for (int i = 0; i < 100; i++) {
    hex[i] = i & 0xFF;
  }
  sprintf(buf, "LAYER%d", args->thread_id);
  log_filter filter(buf, args->l);
  filter.set_level(LOG_LEVEL_DEBUG);
  filter.set_hex_limit(32);

  for (int i = 0; i < NMSGS; i++) {
    filter.error_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
    filter.warning_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
    filter.info_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
    filter.debug_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
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
    pthread_create(&threads[i], NULL, &thread_loop_hex, &args[i]);
  }
  for (int i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }
}

bool read(std::string filename)
{
  bool pass = true;
  bool written[NTHREADS][NMSGS];
  int  thread, msg;

  for (int i = 0; i < NTHREADS; i++) {
    for (int j = 0; j < NMSGS; j++) {
      written[i][j] = false;
    }
  }
  FILE* f = fopen(filename.c_str(), "r");
  if (f != NULL) {
    while (fscanf(f, "Thread %d: %d\n", &thread, &msg)) {
      written[thread][msg] = true;
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

int basic_hex_test()
{
  logger_stdout l;

  log_filter filter("layer", &l);
  filter.set_level(LOG_LEVEL_DEBUG);
  filter.set_hex_limit(500);

  const uint32_t hex_len = 497;

  uint8_t hex[hex_len];
  for (uint32_t i = 0; i < hex_len; i++) {
    hex[i] = i & 0xFF;
  }

  filter.debug_hex(hex, hex_len, "This is the long hex msg (%d B)\n", hex_len);
  filter.debug("This is a message after the long hex msg that should not be cut\n");

  return SRSLTE_SUCCESS;
}

int test_log_singleton()
{
  {
    log_filter   test_log("LAYER1");
    srslte::log* log_ptr = srslte::log::get("LAYER1");
    TESTASSERT(log_ptr == &test_log);
    TESTASSERT(log_ptr->get_service_name() == "LAYER1");
    test_log.set_service_name("LAYER2");
    // log_ptr should now point to LAYER2
    TESTASSERT(srslte::log::get("LAYER1") == nullptr);
    TESTASSERT(srslte::log::get("LAYER2") == &test_log);
  }
  TESTASSERT(srslte::log::get("LAYER2") == nullptr);
  return SRSLTE_SUCCESS;
}

int full_test()
{
  bool        result;
  std::string f("log.txt");
  write(f);
#if 0
  result = read(f);
  remove(f.c_str());
  if(result) {
    printf("Passed\n");
    exit(0);
  }else{
    printf("Failed\n;");
   exit(1);
  }
#endif

  return SRSLTE_SUCCESS;
}
int main(int argc, char** argv)
{
  TESTASSERT(basic_hex_test() == SRSLTE_SUCCESS);
  TESTASSERT(full_test() == SRSLTE_SUCCESS);
  TESTASSERT(test_log_singleton() == SRSLTE_SUCCESS);

  return SRSLTE_SUCCESS;
}
