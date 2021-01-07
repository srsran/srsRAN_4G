#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

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

add_library(srslte_fec OBJECT ${FEC_SOURCES})
