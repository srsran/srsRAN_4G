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

if (HAVE_AVX2)
    set(AVX2_SOURCES
            polar/polar_encoder_avx2.c
            polar/polar_decoder_ssc_c_avx2.c
            polar/polar_decoder_vector_avx2.c
            )
endif (HAVE_AVX2)

set(FEC_SOURCES ${FEC_SOURCES} ${AVX2_SOURCES}
        polar/polar_chanalloc.c
        polar/polar_code.c
        polar/polar_encoder.c
        polar/polar_encoder_pipelined.c
        polar/polar_decoder.c
        polar/polar_decoder_ssc_all.c
        polar/polar_decoder_ssc_f.c
        polar/polar_decoder_ssc_s.c
        polar/polar_decoder_ssc_c.c
        polar/polar_decoder_vector.c
        polar/polar_interleaver.c
        polar/polar_rm.c
        PARENT_SCOPE)

add_subdirectory(test)
