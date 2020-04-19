#
# Copyright 2013-2020 Software Radio Systems Limited
#
# This file is part of srsLTE
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

#################################################################
# Applications
#################################################################

add_executable(synch_file synch_file.c)
target_link_libraries(synch_file srslte_phy)

#################################################################
# These can be compiled without UHD or graphics support
#################################################################

if(RF_FOUND)
  add_executable(pdsch_ue pdsch_ue.c)
  target_link_libraries(pdsch_ue srslte_phy srslte_common srslte_rf pthread)

  add_executable(pdsch_enodeb pdsch_enodeb.c)
  target_link_libraries(pdsch_enodeb srslte_phy srslte_common srslte_rf pthread)

  add_executable(npdsch_enodeb npdsch_enodeb.c)
  target_link_libraries(npdsch_enodeb srslte_phy srslte_rf pthread)

  add_executable(npdsch_ue npdsch_ue.c npdsch_ue_helper.cc)
  target_link_libraries(npdsch_ue srslte_common srslte_phy srslte_rf pthread rrc_asn1)
else(RF_FOUND)
  add_definitions(-DDISABLE_RF)

  add_executable(pdsch_ue pdsch_ue.c)
  target_link_libraries(pdsch_ue srslte_common srslte_phy pthread)

  add_executable(pdsch_enodeb pdsch_enodeb.c)
  target_link_libraries(pdsch_enodeb srslte_common srslte_phy pthread)

  add_executable(npdsch_enodeb npdsch_enodeb.c)
  target_link_libraries(npdsch_enodeb srslte_phy pthread)

  add_executable(npdsch_ue npdsch_ue.c npdsch_ue_helper.cc)
  target_link_libraries(npdsch_ue srslte_common srslte_phy pthread rrc_asn1)
endif(RF_FOUND)

if(SRSGUI_FOUND)
  include_directories(${SRSGUI_INCLUDE_DIRS})
  target_link_libraries(pdsch_ue ${SRSGUI_LIBRARIES})
  target_link_libraries(npdsch_ue ${SRSGUI_LIBRARIES})
  add_definitions(-DENABLE_GUI)
endif(SRSGUI_FOUND)

if (ZEROMQ_FOUND)
  add_executable(zmq_remote_rx zmq_remote_rx.c)
  target_link_libraries(zmq_remote_rx srslte_phy srslte_rf)
endif (ZEROMQ_FOUND)

#################################################################
# These examples need the UHD driver
#################################################################

if(RF_FOUND)

  add_executable(cell_search cell_search.c)
  target_link_libraries(cell_search srslte_phy srslte_common srslte_rf)

  add_executable(usrp_capture usrp_capture.c)
  target_link_libraries(usrp_capture srslte_phy srslte_rf)

  add_executable(usrp_capture_sync usrp_capture_sync.c)
  target_link_libraries(usrp_capture_sync srslte_phy srslte_rf)

  add_executable(usrp_capture_sync_nbiot usrp_capture_sync_nbiot.c)
  target_link_libraries(usrp_capture_sync_nbiot srslte_rf srslte_phy)

  add_executable(cell_search_nbiot cell_search_nbiot.c)
  target_link_libraries(cell_search_nbiot srslte_rf srslte_phy)

  add_executable(usrp_txrx usrp_txrx.c)
  target_link_libraries(usrp_txrx srslte_phy srslte_rf)

  add_executable(pssch_ue pssch_ue.c)
  target_link_libraries(pssch_ue srslte_phy srslte_common srslte_rf pthread)

  if(SRSGUI_FOUND)
    target_link_libraries(pssch_ue ${SRSGUI_LIBRARIES})
  endif(SRSGUI_FOUND)

  message(STATUS "   examples will be installed.")

else(RF_FOUND)
  message(STATUS "   examples will NOT BE INSTALLED.")
endif(RF_FOUND)

# Add eNB/UE IQ test
add_subdirectory(test)
