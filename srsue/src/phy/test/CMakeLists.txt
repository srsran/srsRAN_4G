#
# Copyright 2013-2023 Software Radio Systems Limited
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

set(CTEST_LABELS "srsue;phy")

include_directories(
        ${Boost_INCLUDE_DIRS}
        ${SEC_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}
)

link_directories(
        ${Boost_LIBRARY_DIRS}
        ${SEC_LIBRARY_DIRS}
)

add_executable(ue_phy_test ue_phy_test.cc)
target_link_libraries(ue_phy_test
        srsue_phy
        srsran_common
        srsran_phy
        srsran_radio
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
# Test disabled, it is not 100 deterministic.
#add_test(ue_phy_test ue_phy_test)

add_executable(scell_search_test scell_search_test.cc)
target_link_libraries(scell_search_test
        srsue_phy
        srsran_common
        srsran_phy
        srsran_radio
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})

# Test LTE cell search with a complex environment and an odd measurement period
add_lte_test(scell_search_test scell_search_test --duration=5 --cell.nof_prb=6 --active_cell_list=2,3,4,5,6 --simulation_cell_list=1,2,3,4,5,6 --channel_period_s=30 --channel.hst.fd=750 --channel.delay_max=10000 --intra_freq_meas_period_ms=199)

add_executable(nr_cell_search_test nr_cell_search_test.cc)
target_link_libraries(nr_cell_search_test
        srsue_phy
        srsran_common
        srsran_phy
        srsran_radio
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})

# Test NR cell search without delay
# This test checks the search starts in the configured TTI and the NR PSS is detected correctly inside the SF
add_nr_test(nr_cell_search_test nr_cell_search_test --duration=1 --ssb_period=20 --meas_period_ms=20 --meas_len_ms=1 --simulation_cell_list=500)

# Test NR cell search with up 1000us delay
# This test checks the search is capable to find a cell with a broad delay
add_nr_test(nr_cell_search_test_delay nr_cell_search_test --duration=1 --ssb_period=20 --meas_period_ms=100 --meas_len_ms=30 --channel.delay_min=0 --channel.delay_max=1000 --simulation_cell_list=500)

# File test of 10ms captured NR carrier
# Captured using: lib/examples/usrp_capture -a type=b200,master_clock_rate=61.44e6 -g 80 -r 61.44e6 -n 614400  -f 3682.5e6 -o ../srsue/test/phy/n78.fo3675360k.fs6144.data
#add_nr_test(nr_cell_search_test_file nr_cell_search_test --duration=1 --srate=61.44e6 --ssb_arfcn=645024 --carrier_arfcn=645500 --meas_period_ms=10 --meas_len_ms=10 --file.name=${CMAKE_SOURCE_DIR}/n78.fo3675360k.fs6144.data)

add_executable(nr_cell_search_rf nr_cell_search_rf.cc)
target_link_libraries(nr_cell_search_rf
        srsue_phy
        srsran_common
        srsran_phy
        srsran_radio
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})

# RF based Usage example: nr_sa_cell_search_test --phy.log.level=info --stack.log.level=info --duration=10000 --freq_dl=3.67536e9 --rf.freq_offset=10e3 --rf.rx_gain=90
add_executable(nr_sa_cell_search_test nr_sa_cell_search_test.cc)
target_link_libraries(nr_sa_cell_search_test
        srsue_phy
        srsran_common
        srsran_phy
        srsran_radio
        rrc_nr_asn1
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})