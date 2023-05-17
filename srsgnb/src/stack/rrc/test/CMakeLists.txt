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

add_library(rrc_nr_test_helpers STATIC rrc_nr_test_helpers.cc)

add_executable(rrc_nr_test rrc_nr_test.cc)
target_link_libraries(rrc_nr_test srsgnb_rrc srsgnb_rrc_config_utils srsran_common rrc_nr_asn1 rrc_nr_test_helpers srsgnb_mac ${ATOMIC_LIBS})
add_test(rrc_nr_test rrc_nr_test)

add_executable(rrc_nr_core_test rrc_nr_core_test.cc)
target_link_libraries(rrc_nr_core_test srsgnb_rrc srsgnb_rrc_config_utils srsran_common rrc_nr_asn1 test_helpers
                                       rrc_nr_test_helpers srsgnb_mac srsgnb_ngap ngap_nr_asn1 srsran_gtpu
                                       srsenb_upper ${SCTP_LIBRARIES} ${ATOMIC_LIBS} ${Boost_LIBRARIES})

