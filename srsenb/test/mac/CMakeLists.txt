#
# Copyright 2013-2019 Software Radio Systems Limited
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

# Scheduler test
add_executable(scheduler_test scheduler_test.cc)
target_link_libraries(scheduler_test  srsenb_mac 
                                      srsenb_phy 
                                      srslte_common
                                      srslte_phy
                                      rrc_asn1
                                      ${CMAKE_THREAD_LIBS_INIT} 
                                      ${Boost_LIBRARIES})

# Scheduler test random
add_executable(scheduler_test_rand scheduler_test_rand.cc)
target_link_libraries(scheduler_test_rand  srsenb_mac 
                                           srsenb_phy 
                                           srslte_common
                                           srslte_phy
                                           rrc_asn1
                                           ${CMAKE_THREAD_LIBS_INIT} 
                                           ${Boost_LIBRARIES})
