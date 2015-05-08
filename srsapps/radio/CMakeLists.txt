#
# Copyright 2012-2013 The srsLTE Developers. See the
# COPYRIGHT file at the top-level directory of this distribution.
#
# This file is part of the srsLTE library.
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# A copy of the GNU Lesser General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

FIND_PACKAGE(UHD)

INSTALL(DIRECTORY include/ 
        DESTINATION "${INCLUDE_DIR}" 
        FILES_MATCHING PATTERN "*.h" 
        PATTERN ".svn" EXCLUDE
)

IF(UHD_FOUND)
  FILE(GLOB SOURCES "src/*.cc")
  ADD_LIBRARY(srsapps_radio SHARED ${SOURCES})
  INSTALL(TARGETS srsapps_radio DESTINATION ${LIBRARY_DIR})
  LIBLTE_SET_PIC(srsapps_radio)
ENDIF(UHD_FOUND)

FILE(GLOB HEADERS_ALL "include/srsapps/radio/*.h")
ADD_CUSTOM_TARGET (add_srsapps_radio_headers SOURCES ${HEADERS_ALL})
