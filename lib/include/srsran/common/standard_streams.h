// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_STANDARD_STREAMS_H
#define SRSRAN_STANDARD_STREAMS_H

namespace srsran {

/// Writes the formatted string to the standard output stream.
void console(const char* str, ...);

/// Writes the formatted string to the standard error stream.
void console_stderr(const char* str, ...);

} // namespace srsran

#endif // SRSRAN_STANDARD_STREAMS_H