#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

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
