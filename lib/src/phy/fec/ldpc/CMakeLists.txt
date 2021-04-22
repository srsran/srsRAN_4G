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

if (HAVE_AVX2)
    set(AVX2_SOURCES
            ldpc/ldpc_dec_c_avx2.c
            ldpc/ldpc_dec_c_avx2long.c
            ldpc/ldpc_dec_c_avx2_flood.c
            ldpc/ldpc_dec_c_avx2long_flood.c
            ldpc/ldpc_enc_avx2.c
            ldpc/ldpc_enc_avx2long.c
            )
endif (HAVE_AVX2)

if (HAVE_AVX512)
    set(AVX512_SOURCES
           ldpc/ldpc_dec_c_avx512.c
            ldpc/ldpc_dec_c_avx512long.c
            ldpc/ldpc_dec_c_avx512long_flood.c
           ldpc/ldpc_enc_avx512.c
            ldpc/ldpc_enc_avx512long.c
            )
endif (HAVE_AVX512)

set(FEC_SOURCES ${FEC_SOURCES} ${AVX2_SOURCES} ${AVX512_SOURCES}
        ldpc/base_graph.c
        ldpc/ldpc_dec_f.c
        ldpc/ldpc_dec_s.c
        ldpc/ldpc_dec_c.c
        ldpc/ldpc_dec_c_flood.c
        ldpc/ldpc_decoder.c
        ldpc/ldpc_enc_c.c
        ldpc/ldpc_encoder.c
        ldpc/ldpc_rm.c
        PARENT_SCOPE)

add_subdirectory(test)
