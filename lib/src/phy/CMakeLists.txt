#
# Copyright 2013-2021 Software Radio Systems Limited
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
add_subdirectory(gnb)
add_subdirectory(cfr)
set(srsran_srcs     $<TARGET_OBJECTS:srsran_agc>
        $<TARGET_OBJECTS:srsran_ch_estimation>
        $<TARGET_OBJECTS:srsran_phy_common>
        $<TARGET_OBJECTS:srsran_fec>
        $<TARGET_OBJECTS:srsran_mimo>
        $<TARGET_OBJECTS:srsran_phch>
        $<TARGET_OBJECTS:srsran_sync>
        $<TARGET_OBJECTS:srsran_utils>
        $<TARGET_OBJECTS:srsran_channel>
        $<TARGET_OBJECTS:srsran_dft>
        $<TARGET_OBJECTS:srsran_io>
        $<TARGET_OBJECTS:srsran_modem>
        $<TARGET_OBJECTS:srsran_resampling>
        $<TARGET_OBJECTS:srsran_scrambling>
        $<TARGET_OBJECTS:srsran_ue>
        $<TARGET_OBJECTS:srsran_enb>
        $<TARGET_OBJECTS:srsran_gnb>
        $<TARGET_OBJECTS:srsran_cfr>
        )

add_library(srsran_phy STATIC ${srsran_srcs} )
target_link_libraries(srsran_phy pthread m ${FFT_LIBRARIES})
INSTALL(TARGETS srsran_phy DESTINATION ${LIBRARY_DIR})
