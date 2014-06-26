/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define LIBLTE_IMPORT __declspec(dllimport)
  #define LIBLTE_EXPORT __declspec(dllexport)
  #define LIBLTE_LOCAL
#else
  #if __GNUC__ >= 4
    #define LIBLTE_IMPORT __attribute__ ((visibility ("default")))
    #define LIBLTE_EXPORT __attribute__ ((visibility ("default")))
  #else
    #define LIBLTE_IMPORT
    #define LIBLTE_EXPORT
    #define LIBLTE_LOCAL
  #endif
#endif

// Define LIBLTE_API
// LIBLTE_API is used for the public API symbols.
#ifdef LIBLTE_DLL_EXPORTS // defined if we are building the LIBLTE DLL (instead of using it)
  #define LIBLTE_API LIBLTE_EXPORT
#else
  #define LIBLTE_API LIBLTE_IMPORT
#endif


// Common error codes
#define LIBLTE_SUCCESS                0
#define LIBLTE_ERROR                  -1
#define LIBLTE_ERROR_INVALID_INPUTS   -2

#endif // CONFIG_H
