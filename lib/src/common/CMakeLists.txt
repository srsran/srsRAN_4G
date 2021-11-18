#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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

INSTALL(TARGETS srsran_common DESTINATION ${LIBRARY_DIR})

add_subdirectory(test)
