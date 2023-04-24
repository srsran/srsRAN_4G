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

set(FEC_SOURCES ${FEC_SOURCES}
        convolutional/convcoder.c
        convolutional/parity.c
        convolutional/viterbi.c
        convolutional/viterbi37_avx2.c
        convolutional/viterbi37_avx2_16bit.c
        convolutional/viterbi37_neon.c
        convolutional/viterbi37_port.c
        convolutional/viterbi37_sse.c
        PARENT_SCOPE)

add_subdirectory(test)
