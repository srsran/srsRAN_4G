#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


set(SOURCES arch_select.cc
            enb_events.cc
            backtrace.c
            band_helper.cc
            buffer_pool.cc
            crash_handler.c
            gen_mch_tables.c
            liblte_security.cc
            log_filter.cc
            logmap.cc
            logger_srslog_wrapper.cc
            mac_pcap.cc
            nas_pcap.cc
            network_utils.cc
            pcap.c
            rlc_pcap.cc
            s1ap_pcap.cc
            security.cc
            standard_streams.cc
            thread_pool.cc
            threads.c
            tti_sync_cv.cc
            time_prof.cc
            version.c
            zuc.cc
            s3g.cc)

if (ENABLE_5GNR)
  set(SOURCES ${SOURCES} mac_nr_pcap.cc basic_vnf.cc)
endif(ENABLE_5GNR)

# Avoid warnings caused by libmbedtls about deprecated functions
set_source_files_properties(security.cc PROPERTIES COMPILE_FLAGS -Wno-deprecated-declarations)

add_library(srslte_common STATIC ${SOURCES})
add_custom_target(gen_build_info COMMAND cmake -P ${CMAKE_BINARY_DIR}/SRSLTEbuildinfo.cmake)
add_dependencies(srslte_common gen_build_info)

add_executable(arch_select arch_select.cc)

target_include_directories(srslte_common PUBLIC ${SEC_INCLUDE_DIRS})
target_link_libraries(srslte_common srslte_phy srslog ${SEC_LIBRARIES})

INSTALL(TARGETS srslte_common DESTINATION ${LIBRARY_DIR})

add_subdirectory(test)
