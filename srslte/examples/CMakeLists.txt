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


#################################################################
# EXAMPLES
#################################################################

add_executable(hl_example hl_example.c)
target_link_libraries(hl_example srslte)

add_executable(ll_example ll_example.c)
target_link_libraries(ll_example srslte)


#################################################################
# Applications
#################################################################

add_executable(synch_file synch_file.c)
target_link_libraries(synch_file srslte)


#################################################################
# These can be compiled without UHD or graphics support
#################################################################

add_executable(pdsch_ue pdsch_ue.c)
target_link_libraries(pdsch_ue srslte pthread)

add_executable(pdsch_enodeb pdsch_enodeb.c)
target_link_libraries(pdsch_enodeb srslte pthread)

IF(UHD_FOUND)
ELSE(UHD_FOUND)
  add_definitions(-DDISABLE_UHD)
  add_definitions(-DDISABLE_UHD)
ENDIF(UHD_FOUND)

IF(SRSGUI_FOUND)
  target_link_libraries(pdsch_ue ${SRSGUI_LIBRARIES})
ELSE(SRSGUI_FOUND)
  add_definitions(-DDISABLE_GRAPHICS)
ENDIF(SRSGUI_FOUND)


#################################################################
# These examples need the UHD driver
#################################################################

IF(UHD_FOUND)

  add_executable(cell_search cell_search.c)
  target_link_libraries(cell_search srslte )

  add_executable(prach_ue prach_ue.c)
  target_link_libraries(prach_ue srslte )

  add_executable(cell_measurement cell_measurement.c)
  target_link_libraries(cell_measurement srslte)

  add_executable(usrp_capture usrp_capture.c)
  target_link_libraries(usrp_capture srslte)

  add_executable(usrp_capture_sync usrp_capture_sync.c)
  target_link_libraries(usrp_capture_sync srslte)

  MESSAGE(STATUS "   UHD examples will be installed.")

ELSE(UHD_FOUND)
  MESSAGE(STATUS "   UHD examples NOT INSTALLED.")
ENDIF(UHD_FOUND)

# Add subdirectories 
add_subdirectory(tutorial_examples)

