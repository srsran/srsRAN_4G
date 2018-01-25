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

/******************************************************************************
 *  File:         debug.h
 *
 *  Description:  Debug output utilities.
 *
 *  Reference:
 *****************************************************************************/

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "srslte/config.h"
#include "srslte/phy/common/phy_logger.h"

#define SRSLTE_VERBOSE_DEBUG 2
#define SRSLTE_VERBOSE_INFO  1
#define SRSLTE_VERBOSE_NONE  0

#include <sys/time.h>
SRSLTE_API void get_time_interval(struct timeval * tdata);

#define SRSLTE_DEBUG_ENABLED 1

SRSLTE_API extern int srslte_verbose;
SRSLTE_API extern int handler_registered;

#define SRSLTE_VERBOSE_ISINFO() (srslte_verbose>=SRSLTE_VERBOSE_INFO)
#define SRSLTE_VERBOSE_ISDEBUG() (srslte_verbose>=SRSLTE_VERBOSE_DEBUG)
#define SRSLTE_VERBOSE_ISNONE() (srslte_verbose==SRSLTE_VERBOSE_NONE)

#define PRINT_DEBUG srslte_verbose=SRSLTE_VERBOSE_DEBUG
#define PRINT_INFO srslte_verbose=SRSLTE_VERBOSE_INFO
#define PRINT_NONE srslte_verbose=SRSLTE_VERBOSE_NONE

#define DEBUG(_fmt, ...) if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_DEBUG && !handler_registered)\
        {  fprintf(stdout, "[DEBUG]: " _fmt, ##__VA_ARGS__);  }\
        else{  srslte_phy_log_print(LOG_LEVEL_DEBUG, _fmt, ##__VA_ARGS__); }

#define INFO(_fmt, ...) if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO   && !handler_registered) \
        {  fprintf(stdout, "[INFO]: " _fmt, ##__VA_ARGS__);  }\
        else{  srslte_phy_log_print(LOG_LEVEL_INFO, _fmt, ##__VA_ARGS__); }

#if CMAKE_BUILD_TYPE==Debug
/* In debug mode, it prints out the  */
#define ERROR(_fmt, ...) if (!handler_registered)\
    {   fprintf(stderr, "\e[31m%s.%d: " _fmt "\e[0m\n", __FILE__, __LINE__, ##__VA_ARGS__);}\
        else {srslte_phy_log_print(LOG_LEVEL_ERROR, _fmt, ##__VA_ARGS__);} // 
#else
#define ERROR(_fmt, ...) if (!handler_registered)\
        {   fprintf(stderr, "[ERROR in %s]:" _fmt "\n", __FUNCTION__, ##__VA_ARGS__);}\
        else{srslte_phy_log_print(LOG_LEVEL_ERROR, _fmt, ##__VA_ARGS__);} // 
#endif /* CMAKE_BUILD_TYPE==Debug */

void srslte_debug_handle_crash(int argc, char **argv);

#include<time.h>
#include<stdint.h>
#include<stdarg.h>
#include<stdio.h>
#include<string.h>

extern struct timeval g_tv_next;
extern uint32_t       g_tti;

#define  w_trace true
#define  i_trace true
#define  d_trace false

#define DEBUG_LOG_FMT "%02d:%02d:%02d.%06ld [%s] [%c] [%05hu] %s:%s,  "

#define W_TRACE(_id, _fmt, ...) do {                                                                    \
                             if(w_trace) {                                                              \
                             struct timeval _tv_now;                                                    \
                             struct tm _tm;                                                             \
                             gettimeofday(&_tv_now, NULL);                                              \
                             localtime_r(&_tv_now.tv_sec, &_tm);                                        \
                             const char *_pos = strrchr(__FILE__, '/');                                 \
                             fprintf(stdout, DEBUG_LOG_FMT _fmt "\n",                                   \
                                     _tm.tm_hour,                                                       \
                                     _tm.tm_min,                                                        \
                                     _tm.tm_sec,                                                        \
                                     _tv_now.tv_usec,                                                   \
                                     _id,                                                               \
                                     'W',                                                               \
                                     g_tti,                                                             \
                                     _pos ? _pos+1 : "",                                                \
                                     __func__,                                                          \
                                     ##__VA_ARGS__);                                                    \
                             }                                                                          \
                           } while(0);


#define I_TRACE(_id, _fmt, ...) do {                                                                    \
                             if(i_trace) {                                                              \
                             struct timeval _tv_now;                                                    \
                             struct tm _tm;                                                             \
                             gettimeofday(&_tv_now, NULL);                                              \
                             localtime_r(&_tv_now.tv_sec, &_tm);                                        \
                             const char *_pos = strrchr(__FILE__, '/');                                 \
                             fprintf(stdout, DEBUG_LOG_FMT _fmt "\n",                                   \
                                     _tm.tm_hour,                                                       \
                                     _tm.tm_min,                                                        \
                                     _tm.tm_sec,                                                        \
                                     _tv_now.tv_usec,                                                   \
                                     _id,                                                               \
                                     'I',                                                               \
                                     g_tti,                                                             \
                                     _pos ? _pos+1 : "",                                                \
                                     __func__,                                                          \
                                     ##__VA_ARGS__);                                                    \
                             }                                                                          \
                           } while(0);

#define D_TRACE(_id, _fmt, ...) do {                                                                    \
                             if(d_trace) {                                                              \
                             struct timeval _tv_now;                                                    \
                             struct tm _tm;                                                             \
                             gettimeofday(&_tv_now, NULL);                                              \
                             localtime_r(&_tv_now.tv_sec, &_tm);                                        \
                             const char *_pos = strrchr(__FILE__, '/');                                 \
                             fprintf(stdout, DEBUG_LOG_FMT _fmt "\n",                                   \
                                     _tm.tm_hour,                                                       \
                                     _tm.tm_min,                                                        \
                                     _tm.tm_sec,                                                        \
                                     _tv_now.tv_usec,                                                   \
                                     _id,                                                               \
                                     'D',                                                               \
                                     g_tti,                                                             \
                                     _pos ? _pos+1 : "",                                                \
                                     __func__,                                                          \
                                     ##__VA_ARGS__);                                                    \
                             }                                                                          \
                           } while(0);

#endif // DEBUG_H
