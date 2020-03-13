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

#ifndef SRSLTE_CONFIG_H
#define SRSLTE_CONFIG_H

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#define SRSLTE_IMPORT __declspec(dllimport)
#define SRSLTE_EXPORT __declspec(dllexport)
#define SRSLTE_LOCAL
#else
#if __GNUC__ >= 4
#define SRSLTE_IMPORT __attribute__((visibility("default")))
#define SRSLTE_EXPORT __attribute__((visibility("default")))
#else
#define SRSLTE_IMPORT
#define SRSLTE_EXPORT
#define SRSLTE_LOCAL
#endif
#endif

// Define SRSLTE_API
// SRSLTE_API is used for the public API symbols.
#ifdef SRSLTE_DLL_EXPORTS // defined if we are building the SRSLTE DLL (instead of using it)
#define SRSLTE_API SRSLTE_EXPORT
#else
#define SRSLTE_API SRSLTE_IMPORT
#endif

// Useful macros for templates
#define CONCAT(a, b) a##b
#define CONCAT2(a, b) CONCAT(a, b)

#define STRING2(x) #x
#define STRING(x) STRING2(x)

// Common error codes
#define SRSLTE_SUCCESS 0
#define SRSLTE_ERROR -1
#define SRSLTE_ERROR_INVALID_INPUTS -2
#define SRSLTE_ERROR_TIMEOUT -3
#define SRSLTE_ERROR_INVALID_COMMAND -4
#define SRSLTE_ERROR_OUT_OF_BOUNDS -5
#define SRSLTE_ERROR_CANT_START -6
#define SRSLTE_ERROR_ALREADY_STARTED -7

// cf_t definition
typedef _Complex float cf_t;

#ifdef ENABLE_C16
typedef _Complex short int c16_t;
#endif /* ENABLE_C16 */

#endif // SRSLTE_CONFIG_H
