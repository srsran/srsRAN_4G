// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef TESTING_HELPERS_H
#define TESTING_HELPERS_H

#include <cstdio>

/// Invokes the given test function and printing test results to stdout.
#define TEST_FUNCTION(func, ...)                                                                                       \
  do {                                                                                                                 \
    if (!func(__VA_ARGS__)) {                                                                                          \
      std::printf("Test \"%s\" FAILED! - %s:%u\n", #func, __FILE__, __LINE__);                                         \
      return -1;                                                                                                       \
    } else {                                                                                                           \
      std::printf("Test \"%s\" PASSED!\n", #func);                                                                     \
    }                                                                                                                  \
  } while (0)

/// Asserts for equality between a and b. The == operator must exist for the
/// input types.
#define ASSERT_EQ(a, b)                                                                                                \
  do {                                                                                                                 \
    if ((a) == (b)) {                                                                                                  \
      ;                                                                                                                \
    } else {                                                                                                           \
      std::printf("EQ assertion failed in Test \"%s\" - %s:%u\n  Condition "                                           \
                  "\"%s\" not met\n",                                                                                  \
                  __FUNCTION__,                                                                                        \
                  __FILE__,                                                                                            \
                  __LINE__,                                                                                            \
                  #a " == " #b);                                                                                       \
      return false;                                                                                                    \
    }                                                                                                                  \
  } while (0)

/// Asserts for non equality between a and b. The != operator must exist for the
/// input types.
#define ASSERT_NE(a, b)                                                                                                \
  do {                                                                                                                 \
    if ((a) != (b)) {                                                                                                  \
      ;                                                                                                                \
    } else {                                                                                                           \
      std::printf("NE assertion failed in Test \"%s\" - %s:%u\n  Condition "                                           \
                  "\"%s\" not met\n",                                                                                  \
                  __FUNCTION__,                                                                                        \
                  __FILE__,                                                                                            \
                  __LINE__,                                                                                            \
                  #a " != " #b);                                                                                       \
      return false;                                                                                                    \
    }                                                                                                                  \
  } while (0)

#endif // TESTING_HELPERS_H
