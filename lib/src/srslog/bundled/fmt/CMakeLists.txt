#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
    format.cc
    os.cc)

add_library(fmt STATIC ${SOURCES})
target_include_directories(fmt PUBLIC ../../../../include/srslte/srslog/bundled)
