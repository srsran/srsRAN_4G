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

include_directories(${PROJECT_SOURCE_DIR}/srsue/test/ttcn3/hdr)

add_executable(rapidjson_test rapidjson_test.cc)
add_test(rapidjson_test rapidjson_test)

add_executable(ttcn3_if_handler_test ttcn3_if_handler_test.cc)
target_link_libraries(ttcn3_if_handler_test sctp srslte_phy srslte_common rrc_asn1)
add_test(ttcn3_if_handler_test ttcn3_if_handler_test)