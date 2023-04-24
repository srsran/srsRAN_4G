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

set(SOURCES e2_agent.cc e2ap_ric_subscription.cc e2ap.cc e2sm_kpm_common.cc e2sm_kpm.cc e2sm_kpm_report_service.cc)
add_library(srsgnb_ric STATIC ${SOURCES})
target_link_libraries(srsgnb_ric srsran_asn1 ric_e2)

add_subdirectory(test)
