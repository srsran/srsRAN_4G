# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(FEC_SOURCES ${FEC_SOURCES}
        turbo/rm_conv.c
        turbo/rm_turbo.c
        turbo/tc_interl_lte.c
        turbo/tc_interl_umts.c
        turbo/turbocoder.c
        turbo/turbodecoder.c
        turbo/turbodecoder_gen.c
        turbo/turbodecoder_sse.c
        PARENT_SCOPE)

add_subdirectory(test)
