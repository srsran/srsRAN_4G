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

set(SOURCES mac_nr.cc
            ue_nr.cc
            sched_nr.cc
            sched_nr_ue.cc
            sched_ue/ue_cfg_manager.cc
            sched_nr_worker.cc
            sched_nr_grant_allocator.cc
            sched_nr_harq.cc
            sched_nr_pdcch.cc
            sched_nr_sch.cc
            sched_nr_cfg.cc
            sched_nr_helpers.cc
            sched_nr_bwp.cc
            sched_nr_rb.cc
            sched_nr_time_rr.cc
            harq_softbuffer.cc
            sched_nr_signalling.cc
            sched_nr_interface_utils.cc)

add_library(srsgnb_mac STATIC ${SOURCES})
target_link_libraries(srsgnb_mac srsenb_mac_common srsran_mac rrc_nr_asn1)
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(test)
