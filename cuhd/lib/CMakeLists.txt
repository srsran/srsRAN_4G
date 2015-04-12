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

IF(UHD_FOUND)

  ADD_LIBRARY(cuhd SHARED cuhd_imp.cpp cuhd_utils.c radio_uhd.cc)
  INCLUDE_DIRECTORIES(${UHD_INCLUDE_DIRS})
  LINK_DIRECTORIES(${UHD_LIBRARY_DIRS})
  TARGET_LINK_LIBRARIES(cuhd ${UHD_LIBRARIES})

  LIBLTE_SET_PIC(cuhd)
  APPEND_INTERNAL_LIST(OPTIONAL_LIBS cuhd)
  INSTALL(TARGETS cuhd DESTINATION ${LIBRARY_DIR})

  MESSAGE(STATUS "   cuhd UHD C wrapper will be installed.")

ELSE(UHD_FOUND)

  MESSAGE(STATUS "   UHD driver not found. CUHD library is not generated")

ENDIF(UHD_FOUND)
