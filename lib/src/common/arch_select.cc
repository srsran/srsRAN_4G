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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef IS_ARM
#include <asm/hwcap.h>
#include <stdio.h>
#include <sys/auxv.h>
#define USER_HWCAP_NEON (1 << 12)
#else
#include <cpuid.h>
#define X86_CPUID_BASIC_LEAF 1
#define X86_CPUID_ADVANCED_LEAF 7
#endif

#define MAX_CMD_LEN (64)

#ifndef IS_ARM
static __inline int __get_cpuid_count_redef(unsigned int  __leaf,
                                            unsigned int  __subleaf,
                                            unsigned int* __eax,
                                            unsigned int* __ebx,
                                            unsigned int* __ecx,
                                            unsigned int* __edx)
{
  unsigned int __max_leaf = __get_cpuid_max(__leaf & 0x80000000, 0);

  if (__max_leaf == 0 || __max_leaf < __leaf)
    return 0;

  __cpuid_count(__leaf, __subleaf, *__eax, *__ebx, *__ecx, *__edx);
  return 1;
}

const char* x86_get_isa()
{
  int          ret       = 0;
  int          has_sse42 = 0, has_avx = 0, has_avx2 = 0;
  unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;

  // query basic features
  ret = __get_cpuid(X86_CPUID_BASIC_LEAF, &eax, &ebx, &ecx, &edx);
  if (ret) {
#ifdef bit_SSE4_2
    has_sse42 = ecx & bit_SSE4_2;
#endif
    has_avx   = ecx & bit_AVX;
  }

  // query advanced features
#ifdef bit_AVX2
  ret = __get_cpuid_count_redef(X86_CPUID_ADVANCED_LEAF, 0, &eax, &ebx, &ecx, &edx);
  if (ret) {
    has_avx2 = ebx & bit_AVX2;
  }
#endif

  if (has_avx2) {
    return "avx2";
  } else if (has_avx) {
    return "avx";
  } else if (has_sse42) {
    return "sse4.2";
  } else {
    return "generic";
  }
}
#endif

#ifdef IS_ARM
const char* arm_get_isa()
{
#ifdef HAVE_NEONv8
  if (getauxval(AT_HWCAP) & USER_HWCAP_NEON) {
#else
  if (getauxval(AT_HWCAP) & HWCAP_NEON) {
#endif
    return "neon";
  } else {
    return "generic";
  }
}
#endif

int main(int argc, char* argv[])
{
  char cmd[MAX_CMD_LEN];
#ifdef IS_ARM
  snprintf(cmd, MAX_CMD_LEN, "%s-%s", argv[0], arm_get_isa());
#else
  snprintf(cmd, MAX_CMD_LEN, "%s-%s", argv[0], x86_get_isa());
#endif

  // execute command with same argument
  if (execvp(cmd, &argv[0]) == -1) {
    fprintf(stderr, "%s: %s\n", cmd, strerror(errno));
    exit(errno);
  }
}
