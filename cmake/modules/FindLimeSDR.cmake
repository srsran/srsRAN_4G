#
# Copyright 2013-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

if(NOT LIMESDR_FOUND)
  pkg_check_modules (LIMESDR_PKG LimeSuite)

  find_path(LIMESDR_INCLUDE_DIRS 
    NAMES LimeSuite.h
    HINTS $ENV{LIMESUITE_DIR}/include
    PATHS ${LIMESDR_PKG_INCLUDE_DIRS}
          /usr/include/lime
          /usr/local/include/lime
          $ENV{LIMESUITE_DIR}/include/lime
  )

  find_library(LIMESDR_LIBRARIES 
    NAMES LimeSuite
    HINTS $ENV{LIMESUITE_DIR}/lib
    PATHS ${LIMESDR_PKG_LIBRARY_DIRS}
          /usr/lib
          /usr/local/lib
  )

if(LIMESDR_INCLUDE_DIRS AND LIMESDR_LIBRARIES)
  set(LIMESDR_FOUND TRUE CACHE INTERNAL "libLimeSuite found")
  message(STATUS "Found libLimeSuite: ${LIMESDR_INCLUDE_DIRS}, ${LIMESDR_LIBRARIES}")
else(LIMESDR_INCLUDE_DIRS AND LIMESDR_LIBRARIES)
  set(LIMESDR_FOUND FALSE CACHE INTERNAL "libLimeSuite found")
  message(STATUS "libLimeSuite not found.")
endif(LIMESDR_INCLUDE_DIRS AND LIMESDR_LIBRARIES)

mark_as_advanced(LIMESDR_LIBRARIES LIMESDR_INCLUDE_DIRS)

endif(NOT LIMESDR_FOUND)
