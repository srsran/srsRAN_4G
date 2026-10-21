# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(FEC_SOURCES
        cbsegm.c
        crc.c
        softbuffer.c)

add_subdirectory(block)
add_subdirectory(convolutional)
add_subdirectory(ldpc)
add_subdirectory(polar)
add_subdirectory(test)
add_subdirectory(turbo)

add_library(srsran_fec OBJECT ${FEC_SOURCES})
