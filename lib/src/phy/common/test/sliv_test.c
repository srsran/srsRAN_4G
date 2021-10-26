/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "srsran/common/test_common.h"
#include "srsran/phy/common/sliv.h"
#include <srsran/phy/utils/debug.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint32_t N = 48;

static int test()
{
  for (uint32_t s = 0; s < N; s++) {
    for (uint32_t l = 1; l < N - s; l++) {
      uint32_t sliv = srsran_sliv_from_s_and_l(N, s, l);

      uint32_t S = 0;
      uint32_t L = 0;
      srsran_sliv_to_s_and_l(N, sliv, &S, &L);

      if (s != S || l != L) {
        printf("s=%d; l=%d; SLIV=%d; Start: %d; Length: %d;\n", s, l, sliv, S, L);
        return SRSRAN_ERROR;
      }
    }
  }
  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  int ret = SRSRAN_ERROR;

  // Parse N
  if (argc >= 2) {
    N = (uint32_t)strtol(argv[1], NULL, 10);
  }

  // No input arguments are provided
  if (argc <= 1) {
    ERROR("Error: too few arguments");
  }

  // If two arguments, run brute force test
  else if (argc == 2) {
    ret = test();
  }

  // if three arguments, calculate start and length from sliv
  else if (argc == 3) {
    uint32_t sliv = (uint32_t)strtol(argv[2], NULL, 10);
    uint32_t S    = 0;
    uint32_t L    = 0;

    // check that N is not zero to prevent undefined division
    if (N) {
      srsran_sliv_to_s_and_l(N, sliv, &S, &L);
      printf("SLIV=%d; Start: %d; Length: %d;\n", sliv, S, L);
      ret = SRSRAN_SUCCESS;
    }
    else {
      ERROR("Error: N cannot be 0 to prevent an undefined division");
    }
  }

  // if four arguments, calculate sliv from start and length
  else if (argc == 4) {
    uint32_t s    = (uint32_t)strtol(argv[2], NULL, 10);
    uint32_t l    = (uint32_t)strtol(argv[3], NULL, 10);
    uint32_t sliv = srsran_sliv_from_s_and_l(N, s, l);

    printf("SLIV=%d; Start: %d; Length: %d;\n", sliv, s, l);
    ret = SRSRAN_SUCCESS;
  }

  else {
    ERROR("Error: too many arguments");
  }
  return ret;
}