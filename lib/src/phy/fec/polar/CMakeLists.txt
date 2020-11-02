#
# Project: 5GCoding-SRS
# Author: Jesus Gomez (CTTC)
# Copyright: Software Radio Systems Limited
#

set(FEC_SOURCES ${FEC_SOURCES}
        polar/polar_encoder.c
        polar/polar_encoder_pipelined.c
        polar/polar_encoder_avx2.c
        polar/polar_decoder.c
        polar/polar_decoder_ssc_all.c
        polar/polar_decoder_ssc_f.c
        polar/polar_decoder_ssc_s.c
        polar/polar_decoder_ssc_c.c
        polar/polar_decoder_ssc_c_avx2.c
        polar/polar_decoder_vector.c
        polar/polar_decoder_vector_avx2.c
        PARENT_SCOPE)

add_subdirectory(test)
