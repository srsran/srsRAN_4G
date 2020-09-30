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

#ifndef TESTING_HELPERS_H
#define TESTING_HELPERS_H

#include <cstdio>

/// Invokes the given test function and printing test results to stdout.
#define TEST_FUNCTION(func, ...)                                               \
  do {                                                                         \
    if (!func(__VA_ARGS__)) {                                                  \
      std::printf("Test \"%s\" FAILED! - %s:%u\n", #func, __FILE__, __LINE__); \
      return -1;                                                               \
    } else {                                                                   \
      std::printf("Test \"%s\" PASSED!\n", #func);                             \
    }                                                                          \
  } while (0)

/// Asserts for equality between a and b. The == operator must exist for the
/// input types.
#define ASSERT_EQ(a, b)                                                        \
  do {                                                                         \
    if ((a) == (b)) {                                                          \
      ;                                                                        \
    } else {                                                                   \
      std::printf("EQ assertion failed in Test \"%s\" - %s:%u\n  Condition "   \
                  "\"%s\" not met\n",                                          \
                  __FUNCTION__,                                                \
                  __FILE__,                                                    \
                  __LINE__,                                                    \
                  #a " == " #b);                                               \
      return false;                                                            \
    }                                                                          \
  } while (0)

/// Asserts for non equality between a and b. The != operator must exist for the
/// input types.
#define ASSERT_NE(a, b)                                                        \
  do {                                                                         \
    if ((a) != (b)) {                                                          \
      ;                                                                        \
    } else {                                                                   \
      std::printf("NE assertion failed in Test \"%s\" - %s:%u\n  Condition "   \
                  "\"%s\" not met\n",                                          \
                  __FUNCTION__,                                                \
                  __FILE__,                                                    \
                  __LINE__,                                                    \
                  #a " != " #b);                                               \
      return false;                                                            \
    }                                                                          \
  } while (0)

#endif // TESTING_HELPERS_H
