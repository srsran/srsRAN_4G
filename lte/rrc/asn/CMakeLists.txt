#
# Copyright 2012-2013 The libLTE Developers. See the
# COPYRIGHT file at the top-level directory of this distribution.
#
# This file is part of the libLTE library.
#
# libLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# libLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# A copy of the GNU Lesser General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#


FILE(GLOB RRC_ASN_SOURCES *.c) 
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../include/liblte/rrc/asn/")
ADD_LIBRARY(rrc_asn SHARED ${RRC_ASN_SOURCES})
SET(CMAKE_C_FLAGS "-std=c99 -D_GNU_SOURCE -w")
INSTALL(TARGETS rrc_asn DESTINATION ${LIBRARY_DIR})
LIBLTE_SET_PIC(rrc_asn)