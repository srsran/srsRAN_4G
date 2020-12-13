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

add_subdirectory(phy)
add_subdirectory(stack)


# Link libstdc++ and libgcc
if(BUILD_STATIC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif(BUILD_STATIC)


if (RPATH)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif (RPATH)

add_library(enb_cfg_parser STATIC parser.cc enb_cfg_parser.cc)
target_link_libraries(enb_cfg_parser ${LIBCONFIGPP_LIBRARIES})

add_executable(srsenb main.cc enb.cc metrics_stdout.cc metrics_csv.cc)

set(SRSENB_SOURCES srsenb_phy srsenb_stack srsenb_upper srsenb_mac srsenb_rrc srslog)
set(SRSLTE_SOURCES srslte_common srslte_mac srslte_phy srslte_upper srslte_radio rrc_asn1 s1ap_asn1 enb_cfg_parser srslog)

if(ENABLE_5GNR)
  set(SRSENB_SOURCES ${SRSENB_SOURCES} srsgnb_phy srsgnb_stack srsgnb_upper srsgnb_mac srsgnb_rrc)
  set(SRSLTE_SOURCES ${SRSLTE_SOURCES} rrc_nr_asn1 ngap_nr_asn1)
endif()

target_link_libraries(srsenb  ${SRSENB_SOURCES}
                              ${SRSLTE_SOURCES}
                              ${CMAKE_THREAD_LIBS_INIT}
                              ${Boost_LIBRARIES} 
                              ${SEC_LIBRARIES}
                              ${LIBCONFIGPP_LIBRARIES}
                              ${SCTP_LIBRARIES})

if (RPATH)
  set_target_properties(srsenb PROPERTIES INSTALL_RPATH ".")
endif (RPATH)

########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILDENB_CMD} STREQUAL "")
  message(STATUS "Added custom post-build-ENB command: ${BUILDENB_CMD}")
  add_custom_command(TARGET srsenb POST_BUILD COMMAND ${BUILDENB_CMD})
else(NOT ${BUILDENB_CMD} STREQUAL "")
  message(STATUS "No post-build-ENB command defined")
endif (NOT ${BUILDENB_CMD} STREQUAL "")

install(TARGETS srsenb DESTINATION ${RUNTIME_DIR})
