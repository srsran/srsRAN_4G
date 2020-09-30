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

#ifndef SRSLOG_SRSLOG_C_H
#define SRSLOG_SRSLOG_C_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Common types.
 */
typedef int srslog_bool;
typedef struct srslog_opaque_sink srslog_sink;
typedef struct srslog_opaque_log_channel srslog_log_channel;
typedef struct srslog_opaque_basic_logger srslog_logger;

/**
 * This function initializes the logging framework. It must be called before
 * any log entry is generated.
 * NOTE: Calling this function more than once has no side effects.
 */
void srslog_init(void);

/**
 * Installs the specified sink to be used as the default one by new log
 * channels and loggers.
 * The initial default sink writes to stdout.
 */
void srslog_set_default_sink(srslog_sink* s);

/**
 * Returns the instance of the default sink being used.
 */
srslog_sink* srslog_get_default_sink(void);

/**
 * Returns an instance of a log_channel with the specified id that writes to
 * the default sink using the default log channel configuration.
 * NOTE: Any '#' characters in the id will get removed.
 */
srslog_log_channel* srslog_fetch_log_channel(const char* id);

/**
 * Finds a log channel with the specified id string in the repository. On
 * success returns a pointer to the requested log channel, otherwise NULL.
 */
srslog_log_channel* srslog_find_log_channel(const char* id);

/**
 * Controls whether the specified channel accepts incoming log entries.
 */
void srslog_set_log_channel_enabled(srslog_log_channel* channel,
                                    srslog_bool enabled);

/**
 * Returns 1 if the specified channel is accepting incoming log entries,
 * otherwise 0.
 */
srslog_bool srslog_is_log_channel_enabled(srslog_log_channel* channel);

/**
 * Returns the id string of the specified channel.
 */
const char* srslog_get_log_channel_id(srslog_log_channel* channel);

/**
 * Logs the provided log entry using the specified log channel. When the channel
 * is disabled the log entry wil be discarded.
 * NOTE: Only printf style formatting is supported when using the C API.
 */
void srslog_log(srslog_log_channel* channel, const char* fmt, ...);

/**
 * Returns an instance of a basic logger (see basic_logger type) with the
 * specified id string. All logger channels will write into the default sink.
 */
srslog_logger* srslog_fetch_default_logger(const char* id);

/**
 * Finds a logger with the specified id string in the repository. On success
 * returns a pointer to the requested log channel, otherwise NULL.
 */
srslog_logger* srslog_find_default_logger(const char* id);

/**
 * These functions log the provided log entry using the specified logger.
 * Entries are automatically discarded depending on the configured level of the
 * logger.
 * NOTE: Only printf style formatting is supported when using the C API.
 */
void srslog_debug(srslog_logger* log, const char* fmt, ...);
void srslog_info(srslog_logger* log, const char* fmt, ...);
void srslog_warning(srslog_logger* log, const char* fmt, ...);
void srslog_error(srslog_logger* log, const char* fmt, ...);

/**
 * Returns the id string of the specified logger.
 */
const char* srslog_get_logger_id(srslog_logger* log);

typedef enum {
  srslog_lvl_error,   /**< error logging level */
  srslog_lvl_warning, /**< warning logging level */
  srslog_lvl_info,    /**< info logging level */
  srslog_lvl_debug    /**< debug logging level */
} srslog_log_levels;

/**
 * Sets the logging level into the specified logger.
 */
void srslog_set_logger_level(srslog_logger* log, srslog_log_levels lvl);

/**
 * Finds a sink with the specified id string in the repository. On
 * success returns a pointer to the requested sink, otherwise NULL.
 */
srslog_sink* srslog_find_sink(const char* id);

/**
 * Returns an instance of a sink that writes to the stdout stream.
 */
srslog_sink* srslog_fetch_stdout_sink(void);

/**
 * Returns an instance of a sink that writes to the stderr stream.
 */
srslog_sink* srslog_fetch_stderr_sink(void);

/**
 * Returns an instance of a sink that writes into a file in the specified path.
 * Specifying a max_size value different to zero will make the sink create a
 * new file each time the current file exceeds this value. The units of
 * max_size are bytes.
 * NOTE: Any '#' characters in the id will get removed.
 */
srslog_sink* srslog_fetch_file_sink(const char* path, size_t max_size);

#ifdef __cplusplus
}
#endif

#endif
