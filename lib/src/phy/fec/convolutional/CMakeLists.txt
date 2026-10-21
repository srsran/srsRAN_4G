# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

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
