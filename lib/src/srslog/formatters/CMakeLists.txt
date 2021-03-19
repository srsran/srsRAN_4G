#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
    json_formatter.cpp
    text_formatter.cpp)

add_library(formatters STATIC ${SOURCES})
