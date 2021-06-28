#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(GLOB SOURCES "*.c")
add_library(srsran_gnb OBJECT ${SOURCES})
