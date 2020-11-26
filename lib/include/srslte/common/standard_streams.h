/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_STANDARD_STREAMS_H
#define SRSLTE_STANDARD_STREAMS_H

namespace srslte {

/// Writes the formatted string to the standard output stream.
void console(const char* str, ...);

/// Writes the formatted string to the standard error stream.
void console_stderr(const char* str, ...);

} // namespace srslte

#endif // SRSLTE_STANDARD_STREAMS_H