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

#ifndef SRSRAN_CONFIG_H
#define SRSRAN_CONFIG_H

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#define SRSRAN_IMPORT __declspec(dllimport)
#define SRSRAN_EXPORT __declspec(dllexport)
#define SRSRAN_LOCAL
#else
#if __GNUC__ >= 4
#define SRSRAN_IMPORT __attribute__((visibility("default")))
#define SRSRAN_EXPORT __attribute__((visibility("default")))
#else
#define SRSRAN_IMPORT
#define SRSRAN_EXPORT
#define SRSRAN_LOCAL
#endif
#endif

// Define SRSRAN_API
// SRSRAN_API is used for the public API symbols.
#ifdef SRSRAN_DLL_EXPORTS // defined if we are building the SRSRAN DLL (instead of using it)
#define SRSRAN_API SRSRAN_EXPORT
#else
#define SRSRAN_API SRSRAN_IMPORT
#endif

// Useful macros for templates
#define CONCAT(a, b) a##b
#define CONCAT2(a, b) CONCAT(a, b)

#define STRING2(x) #x
#define STRING(x) STRING2(x)

// Common error codes
#define SRSRAN_SUCCESS 0
#define SRSRAN_ERROR -1
#define SRSRAN_ERROR_INVALID_INPUTS -2
#define SRSRAN_ERROR_TIMEOUT -3
#define SRSRAN_ERROR_INVALID_COMMAND -4
#define SRSRAN_ERROR_OUT_OF_BOUNDS -5
#define SRSRAN_ERROR_CANT_START -6
#define SRSRAN_ERROR_ALREADY_STARTED -7
#define SRSRAN_ERROR_RX_EOF -8

// cf_t definition
typedef _Complex float cf_t;

#ifdef ENABLE_C16
typedef _Complex short int c16_t;
#endif /* ENABLE_C16 */

#endif // SRSRAN_CONFIG_H
