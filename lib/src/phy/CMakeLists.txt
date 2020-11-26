#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(agc)
add_subdirectory(ch_estimation)
add_subdirectory(common)
add_subdirectory(fec)
add_subdirectory(mimo)
add_subdirectory(phch)
add_subdirectory(rf)
add_subdirectory(sync)
add_subdirectory(utils)
add_subdirectory(channel)
add_subdirectory(dft)
add_subdirectory(io)
add_subdirectory(modem)
add_subdirectory(resampling)
add_subdirectory(scrambling)
add_subdirectory(ue)
add_subdirectory(enb)
set(srslte_srcs     $<TARGET_OBJECTS:srslte_agc>
        $<TARGET_OBJECTS:srslte_ch_estimation>
        $<TARGET_OBJECTS:srslte_phy_common>
        $<TARGET_OBJECTS:srslte_fec>
        $<TARGET_OBJECTS:srslte_mimo>
        $<TARGET_OBJECTS:srslte_phch>
        $<TARGET_OBJECTS:srslte_sync>
        $<TARGET_OBJECTS:srslte_utils>
        $<TARGET_OBJECTS:srslte_channel>
        $<TARGET_OBJECTS:srslte_dft>
        $<TARGET_OBJECTS:srslte_io>
        $<TARGET_OBJECTS:srslte_modem>
        $<TARGET_OBJECTS:srslte_resampling>
        $<TARGET_OBJECTS:srslte_scrambling>
        $<TARGET_OBJECTS:srslte_ue>
        $<TARGET_OBJECTS:srslte_enb>
        )

add_library(srslte_phy STATIC ${srslte_srcs})
target_link_libraries(srslte_phy pthread m ${FFT_LIBRARIES})
INSTALL(TARGETS srslte_phy DESTINATION ${LIBRARY_DIR})