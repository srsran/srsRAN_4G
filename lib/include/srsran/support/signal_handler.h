/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

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
