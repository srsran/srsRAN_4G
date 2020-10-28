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

                                              {100, 1, 1000, true, 0.0, 13208},
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
