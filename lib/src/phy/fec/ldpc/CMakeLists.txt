#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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

set(FEC_SOURCES ${FEC_SOURCES} ${AVX2_SOURCES}
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
