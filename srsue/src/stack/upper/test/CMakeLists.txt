#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(usim_test usim_test.cc)
target_link_libraries(usim_test srsue_upper srsran_phy rrc_asn1 srslog)
add_test(usim_test usim_test)

if(HAVE_PCSC)
  add_executable(pcsc_usim_test pcsc_usim_test.cc)
  target_link_libraries(pcsc_usim_test srsue_upper srsran_phy srsran_common srslog)
endif(HAVE_PCSC)

add_executable(nas_test nas_test.cc)
target_link_libraries(nas_test srsue_upper srsran_common srsran_phy rrc_asn1 srsran_asn1)
add_test(nas_test nas_test)

add_executable(nas_5g_test nas_5g_test.cc)
target_link_libraries(nas_5g_test srsue_upper srsran_phy rrc_asn1)
add_test(nas_5g_test nas_5g_test)

add_executable(gw_test gw_test.cc)
target_link_libraries(gw_test srsue_upper srsran_common srsran_phy)
add_test(gw_test gw_test)

add_executable(tft_test tft_test.cc)
target_link_libraries(tft_test srsue_upper srsran_common srsran_phy)
add_test(tft_test tft_test)

########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "Added custom post-build command: ${BUILD_CMD}")
  add_custom_command(TARGET ip_test POST_BUILD COMMAND ${BUILD_CMD})
else(NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "No post-build command defined")
endif (NOT ${BUILD_CMD} STREQUAL "")

