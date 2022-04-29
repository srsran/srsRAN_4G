#
# Copyright 2013-2022 Software Radio Systems Limited
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

set(FEC_SOURCES ${FEC_SOURCES}
        turbo/rm_conv.c
        turbo/rm_turbo.c
        turbo/tc_interl_lte.c
        turbo/tc_interl_umts.c
        turbo/turbocoder.c
        turbo/turbodecoder.c
        turbo/turbodecoder_gen.c
        turbo/turbodecoder_sse.c
        PARENT_SCOPE)

add_subdirectory(test)
