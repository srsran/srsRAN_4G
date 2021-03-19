#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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
