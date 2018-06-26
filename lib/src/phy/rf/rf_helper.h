/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

// A bunch of helper functions to process device arguments


#define REMOVE_SUBSTRING_WITHCOMAS(S, TOREMOVE) \
  remove_substring(S, TOREMOVE ",");\
  remove_substring(S, TOREMOVE ", ");\
  remove_substring(S, "," TOREMOVE);\
  remove_substring(S, ", " TOREMOVE);\
  remove_substring(S, TOREMOVE)

static void remove_substring(char *s,const char *toremove) {
  while((s=strstr(s,toremove))) {
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
  }
}

static void copy_subdev_string(char *dst, char *src) {
  int n = 0;
  size_t len = strlen(src);
  /* Copy until end of string or comma */
  while (n < len && src[n] != '\0' && src[n] != ',') {
    dst[n] = src[n];
    n++;
  }
  dst[n] = '\0';
}