#
# Copyright 2013-2022 Software Radio Systems Limited
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

set(SOURCES arch_select.cc
            enb_events.cc
            backtrace.c
            byte_buffer.cc
            band_helper.cc
            bearer_manager.cc
            buffer_pool.cc
            crash_handler.cc
            gen_mch_tables.c
            liblte_security.cc
            mac_pcap.cc
            mac_pcap_base.cc
            nas_pcap.cc
            network_utils.cc
            mac_pcap_net.cc
            pcap.c
            phy_cfg_nr.cc
            phy_cfg_nr_default.cc
            rrc_common.cc
            rlc_pcap.cc
            s1ap_pcap.cc
            ngap_pcap.cc
            security.cc
            standard_streams.cc
            thread_pool.cc
            threads.c
            tti_sync_cv.cc
            time_prof.cc
            version.c
            zuc.cc
            s3g.cc)

# Avoid warnings caused by libmbedtls about deprecated functions
set_source_files_properties(security.cc PROPERTIES COMPILE_FLAGS -Wno-deprecated-declarations)

add_library(srsran_common STATIC ${SOURCES})
add_custom_target(gen_build_info COMMAND cmake -P ${CMAKE_BINARY_DIR}/SRSRANbuildinfo.cmake)
add_dependencies(srsran_common gen_build_info)

add_executable(arch_select arch_select.cc)

target_include_directories(srsran_common PUBLIC ${SEC_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${BACKWARD_INCLUDE_DIRS})
target_link_libraries(srsran_common srsran_phy support srslog ${SEC_LIBRARIES} ${BACKWARD_LIBRARIES} ${SCTP_LIBRARIES})
target_compile_definitions(srsran_common PRIVATE ${BACKWARD_DEFINITIONS})

install(TARGETS srsran_common DESTINATION ${LIBRARY_DIR} OPTIONAL)

add_subdirectory(test)
