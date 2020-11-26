#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SRCS dft_fftw.c dft_precoding.c ofdm.c)
add_library(srslte_dft OBJECT ${SRCS})
add_subdirectory(test)
