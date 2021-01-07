#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(FEC_SOURCES ${FEC_SOURCES}
        block/block.c
        PARENT_SCOPE)

add_subdirectory(test)
