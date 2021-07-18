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

set(SOURCES
        lte/cc_worker.cc
        lte/sf_worker.cc
        lte/worker_pool.cc
        nr/slot_worker.cc
        nr/worker_pool.cc
        phy.cc
        phy_common.cc
        phy_ue_db.cc
        prach_worker.cc
        txrx.cc)
add_library(srsenb_phy STATIC ${SOURCES})

if (ENABLE_GUI AND SRSGUI_FOUND)
    target_link_libraries(srsenb_phy ${SRSGUI_LIBRARIES})
endif ()
