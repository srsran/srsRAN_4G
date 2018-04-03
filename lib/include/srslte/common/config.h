/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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
  #define SRSAPPS_IMPORT __declspec(dllimport)
  #define SRSAPPS_EXPORT __declspec(dllexport)
  #define SRSAPPS_LOCAL
#else
  #if __GNUC__ >= 4
    #define SRSAPPS_IMPORT __attribute__ ((visibility ("default")))
    #define SRSAPPS_EXPORT __attribute__ ((visibility ("default")))
  #else
    #define SRSAPPS_IMPORT
    #define SRSAPPS_EXPORT
    #define SRSAPPS_LOCAL
  #endif
#endif

// Define SRSAPPS_API
// is used for the public API symbols.
#ifdef SRSAPPS_DLL_EXPORTS // defined if we are building the SRSAPPS DLL (instead of using it)
  #define SRSAPPS_EXPORT
#else
  #define SRSAPPS_IMPORT
#endif

// cf_t definition
typedef _Complex float cf_t;

#endif // SRSLTE_CONFIG_H
