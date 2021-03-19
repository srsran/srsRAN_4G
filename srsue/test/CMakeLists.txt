#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(phy)
add_subdirectory(upper)
add_subdirectory(mac_nr)

if (ENABLE_TTCN3)
  add_subdirectory(ttcn3)
endif (ENABLE_TTCN3)

add_executable(metrics_test metrics_test.cc ../src/metrics_stdout.cc ../src/metrics_csv.cc)
target_link_libraries(metrics_test srsran_phy srsran_common)
add_test(metrics_test metrics_test -o ${CMAKE_CURRENT_BINARY_DIR}/ue_metrics.csv)

add_executable(mac_test mac_test.cc)
target_link_libraries(mac_test srsue_mac srsue_phy srsran_common srsran_mac srsran_phy srsran_radio srsran_asn1 rrc_asn1 ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
add_test(mac_test mac_test)