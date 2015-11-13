#
# Copyright 2013-2015 Software Radio Systems Limited
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

IF(UHD_FOUND)
  LINK_DIRECTORIES(${UHD_LIBRARY_DIRS})
ENDIF(UHD_FOUND)


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
  target_link_libraries(pdsch_ue srslte_uhd)
  target_link_libraries(pdsch_enodeb srslte_uhd)
ELSE(UHD_FOUND)
  add_definitions(-DDISABLE_UHD)
ENDIF(UHD_FOUND)

FIND_PACKAGE(SRSGUI)

IF(SRSGUI_FOUND)
  include_directories(${SRSGUI_INCLUDE_DIRS})
  target_link_libraries(pdsch_ue ${SRSGUI_LIBRARIES})
ELSE(SRSGUI_FOUND)
  add_definitions(-DDISABLE_GRAPHICS)
ENDIF(SRSGUI_FOUND)


#################################################################
# These examples need the UHD driver
#################################################################

IF(UHD_FOUND)

  add_executable(cell_search cell_search.c)
  target_link_libraries(cell_search srslte srslte_uhd)

  add_executable(cell_measurement cell_measurement.c)
  target_link_libraries(cell_measurement srslte srslte_uhd)

  add_executable(usrp_capture usrp_capture.c)
  target_link_libraries(usrp_capture srslte srslte_uhd)

  add_executable(usrp_capture_sync usrp_capture_sync.c)
  target_link_libraries(usrp_capture_sync srslte srslte_uhd)

  add_executable(usrp_txrx usrp_txrx.c)
  target_link_libraries(usrp_txrx srslte srslte_uhd)

  MESSAGE(STATUS "   UHD examples will be installed.")

ELSE(UHD_FOUND)
  MESSAGE(STATUS "   UHD examples NOT INSTALLED.")
ENDIF(UHD_FOUND)

# Add subdirectories 
add_subdirectory(tutorial_examples)

