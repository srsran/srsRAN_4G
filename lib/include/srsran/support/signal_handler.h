// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

/**
 * @file signal_handler.h
 * @brief Common signal handling methods for all srsRAN applications.
 */

#ifndef SRSRAN_SIGNAL_HANDLER_H
#define SRSRAN_SIGNAL_HANDLER_H

using srsran_signal_hanlder = void (*)();

/// Registers the specified function to be called when the user interrupts the program execution (eg: via Ctrl+C).
/// Passing a null function pointer disables the current installed handler.
void srsran_register_signal_handler(srsran_signal_hanlder handler);

#endif // SRSRAN_SIGNAL_HANDLER_H
