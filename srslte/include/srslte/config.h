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

#ifndef CONFIG_H
#define CONFIG_H

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define SRSLTE_IMPORT __declspec(dllimport)
  #define SRSLTE_EXPORT __declspec(dllexport)
  #define SRSLTE_LOCAL
#else
  #if __GNUC__ >= 4
    #define SRSLTE_IMPORT __attribute__ ((visibility ("default")))
    #define SRSLTE_EXPORT __attribute__ ((visibility ("default")))
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


// Common error codes
#define SRSLTE_SUCCESS                0
#define SRSLTE_ERROR                  -1
#define SRSLTE_ERROR_INVALID_INPUTS   -2

// cf_t definition
typedef _Complex float cf_t;

#endif // CONFIG_H
