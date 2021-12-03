#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(test)

set(SOURCES rrc_nr.cc rrc_nr_procedures.cc ../rrc/rrc_cell.cc)
add_library(srsue_rrc_nr STATIC ${SOURCES})