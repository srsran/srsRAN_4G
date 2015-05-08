#
# Copyright 2013-2015 The srsLTE Developers. See the
# COPYRIGHT file at the top-level directory of this distribution.
#
# This file is part of the srsLTE library.
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

INSTALL(DIRECTORY include/ 
        DESTINATION "${INCLUDE_DIR}" 
        FILES_MATCHING PATTERN "*.h" 
        PATTERN ".svn" EXCLUDE
)

FILE(GLOB SOURCES "src/*.cc")
ADD_LIBRARY(srsapps_ue_mac SHARED ${SOURCES})
INSTALL(TARGETS srsapps_ue_mac DESTINATION ${LIBRARY_DIR})
SRSLTE_SET_PIC(srsapps_ue_mac)

FILE(GLOB HEADERS_ALL "include/srsapps/ue/mac/*.h")
ADD_CUSTOM_TARGET (add_ue_mac_headers SOURCES ${HEADERS_ALL})

ADD_SUBDIRECTORY(test)
