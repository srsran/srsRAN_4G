# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

set(SRCS dft_fftw.c dft_precoding.c ofdm.c)
add_library(srsran_dft OBJECT ${SRCS})
add_subdirectory(test)
