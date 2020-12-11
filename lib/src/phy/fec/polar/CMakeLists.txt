#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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
