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


#################################################################
# EXAMPLES shown in WinnForum 2015 Tutorial
#################################################################


FIND_PACKAGE(SRSGUI)

IF(SRSGUI_FOUND)
  include_directories(${SRSGUI_INCLUDE_DIRS})

  add_executable(pss pss.c)
  target_link_libraries(pss srslte cuhd ${SRSGUI_LIBRARIES})

  include_directories("../examples")
  add_executable(ue_rx ue_rx.c ../examples/cuhd_utils.c)
  target_link_libraries(ue_rx srslte cuhd ${SRSGUI_LIBRARIES} pthread)
ENDIF(SRSGUI_FOUND)

add_executable(simple_tx simple_tx.c)
target_link_libraries(simple_tx srslte cuhd)
