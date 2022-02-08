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

#include <stdbool.h>

typedef struct {
  int      n;
  uint32_t s;
  int      len;
  bool     tail;
  float    ebno;
  int      errors;
} expected_errors_t;

/* The SSE implementation uses 5-bit metrics and has 0.75 dB loss approximation */
#ifdef LV_HAVE_SSE

static expected_errors_t expected_errors[] = {{1000, 1, 40, true, 0.0, 7282},
                                              {1000, 1, 40, true, 2.0, 725},
                                              {1000, 1, 40, true, 3.0, 176},
                                              {1000, 1, 40, true, 4.5, 24},

                                              {1000, 1, 56, true, 0.0, 7282},
                                              {1000, 1, 56, true, 2.0, 725},
                                              {1000, 1, 56, true, 3.0, 176},
                                              {1000, 1, 56, true, 4.5, 24},

                                              {100, 1, 1000, true, 0.0, 13208},
                                              {100, 1, 1000, true, 2.0, 939},
                                              {100, 1, 1000, true, 3.0, 110},
                                              {100, 1, 1000, true, 4.5, 5},

                                              {-1, -1, -1, true, -1.0, -1}};

#elif HAVE_NEON

static expected_errors_t expected_errors[] = {{1000, 1, 40, true, 0.0, 7282},
                                              {1000, 1, 40, true, 2.0, 725},
                                              {1000, 1, 40, true, 3.0, 176},
                                              {1000, 1, 40, true, 4.5, 24},

                                              {1000, 1, 56, true, 0.0, 7282},
                                              {1000, 1, 56, true, 2.0, 725},
                                              {1000, 1, 56, true, 3.0, 176},
                                              {1000, 1, 56, true, 4.5, 24},

                                              {100, 1, 1000, true, 0.0, 16000},
                                              {100, 1, 1000, true, 2.0, 939},
                                              {100, 1, 1000, true, 3.0, 110},
                                              {100, 1, 1000, true, 4.5, 5},

                                              {-1, -1, -1, true, -1.0, -1}};

#else

static expected_errors_t expected_errors[] = {{1000, 1, 40, true, 0.0, 5363},
                                              {1000, 1, 40, true, 2.0, 356},
                                              {1000, 1, 40, true, 3.0, 48},
                                              {1000, 1, 40, true, 4.5, 0},

                                              {100, 1, 1000, true, 0.0, 8753},
                                              {100, 1, 1000, true, 2.0, 350},
                                              {100, 1, 1000, true, 3.0, 33},
                                              {100, 1, 1000, true, 4.5, 0},

                                              {-1, -1, -1, true, -1.0, -1}};

#endif
int get_expected_errors(int n, uint32_t s, int len, bool tail, float ebno)
{
  int i;
  i = 0;
  while (expected_errors[i].n != -1) {
    if (expected_errors[i].n == n && expected_errors[i].s == s && expected_errors[i].len == len &&
        expected_errors[i].tail == tail && expected_errors[i].ebno == ebno) {
      break;
    } else {
      i++;
    }
  }
  return expected_errors[i].errors;
}
