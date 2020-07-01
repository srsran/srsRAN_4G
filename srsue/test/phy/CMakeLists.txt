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
        srsue_stack
        srsue_upper
        srsue_mac
        srsue_rrc
        srslte_common
        srslte_phy
        srslte_radio
        srslte_upper
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
# Test disabled, it is not 100 deterministic.
#add_test(ue_phy_test ue_phy_test)

add_executable(scell_search_test scell_search_test.cc)
target_link_libraries(scell_search_test
        srsue_phy
        srsue_stack
        srsue_upper
        srsue_mac
        srsue_rrc
        srslte_common
        srslte_phy
        srslte_radio
        srslte_upper
        rrc_asn1
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_test(scell_search_test scell_search_test --duration=5 --cell.nof_prb=6 --active_cell_list=2,3,4,5,6 --simulation_cell_list=1,2,3,4,5,6 --channel_period_s=30 --channel.hst.fd=750 --channel.delay_max=10000)
set_tests_properties(scell_search_test PROPERTIES LABELS "long;phy;srsue")
