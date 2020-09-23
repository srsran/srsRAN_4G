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
#ifndef SRSLTE_RF_HELPER_H_
#define SRSLTE_RF_HELPER_H_

// A bunch of helper functions to process device arguments

#include "srslte/config.h"
#include "srslte/phy/rf/rf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REMOVE_SUBSTRING_WITHCOMAS(S, TOREMOVE)                                                                        \
  remove_substring(S, TOREMOVE ",");                                                                                   \
  remove_substring(S, TOREMOVE ", ");                                                                                  \
  remove_substring(S, "," TOREMOVE);                                                                                   \
  remove_substring(S, ", " TOREMOVE);                                                                                  \
  remove_substring(S, TOREMOVE)

static inline void remove_substring(char* s, const char* toremove)
{
  while ((s = strstr(s, toremove))) {
    memmove(s, s + strlen(toremove), 1 + strlen(s + strlen(toremove)));
  }
}

static inline void copy_subdev_string(char* dst, char* src)
{
  int n   = 0;
  int len = (int)strlen(src);
  /* Copy until end of string or comma */
  while (n < len && src[n] != '\0' && src[n] != ',') {
    dst[n] = src[n];
    n++;
  }
  dst[n] = '\0';
}

static inline int parse_string(char* args, const char* config_arg_base, int channel_index, char param_dst[RF_PARAM_LEN])
{
  int ret = SRSLTE_ERROR;

  if (args == NULL) {
    return ret;
  }

  char  config_key[RF_PARAM_LEN] = {0};
  char  config_str[RF_PARAM_LEN] = {0};
  char* config_ptr               = NULL;

  // try to parse parameter without index as is
  snprintf(config_key, RF_PARAM_LEN, "%s=", config_arg_base);
  config_ptr = strstr(args, config_key);

  // check if we have a match
  if (!config_ptr) {
    // Couldn't find param, add channel index and parse again
    snprintf(config_key, RF_PARAM_LEN, "%s%d=", config_arg_base, channel_index);
    config_ptr = strstr(args, config_key);
  }

  if (config_ptr) {
    copy_subdev_string(config_str, config_ptr + strlen(config_key));
    if (channel_index >= 0) {
      printf("CH%d %s=%s\n", channel_index, config_arg_base, config_str);
    } else {
      printf("CHx %s=%s\n", config_arg_base, config_str);
    }

    if (snprintf(param_dst, RF_PARAM_LEN, "%s", config_str) < 0) {
      return SRSLTE_ERROR;
    }

    // concatenate key=value and remove both (avoid removing the same value twice if it occurs twice in rf_args)
    char config_pair[RF_PARAM_LEN * 2] = {0};
    snprintf(config_pair, RF_PARAM_LEN * 2, "%s%s", config_key, config_str);
    remove_substring(args, config_pair);

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

static inline int parse_double(char* args, const char* config_arg_base, int channel_index, double* value)
{
  char tmp_value[RF_PARAM_LEN] = {0};
  int  ret                     = parse_string(args, config_arg_base, channel_index, tmp_value);

  // Copy parsed value only if was found, otherwise it keeps the default
  if (ret == SRSLTE_SUCCESS) {
    *value = strtod(tmp_value, NULL);
  }

  return ret;
}

static inline int parse_uint32(char* args, const char* config_arg_base, int channel_index, uint32_t* value)
{
  char tmp_value[RF_PARAM_LEN] = {0};
  int  ret                     = parse_string(args, config_arg_base, channel_index, tmp_value);

  // Copy parsed value only if was found, otherwise it keeps the default
  if (ret == SRSLTE_SUCCESS) {
    *value = (uint32_t)strtof(tmp_value, NULL);
  }

  return ret;
}

#endif /* SRSLTE_RF_HELPER_H_ */
