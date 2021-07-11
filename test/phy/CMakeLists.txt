#
# Copyright 2013-2021 Software Radio Systems Limited
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

if (RF_FOUND AND ENABLE_SRSUE AND ENABLE_SRSENB)
    add_executable(nr_phy_test nr_phy_test.cc)
    target_link_libraries(nr_phy_test
            srsue_phy_nr
            srsue_phy
            srsran_common
            srsran_phy
            srsran_radio
            srsenb_phy
            ${CMAKE_THREAD_LIBS_INIT}
            ${Boost_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT}
            ${Boost_LIBRARIES}
            ${ATOMIC_LIBS})

    add_nr_test(nr_phy_test nr_phy_test)
endif ()
