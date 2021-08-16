#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES phy_common.c phy_common_sl.c  phy_common_nr.c sequence.c timestamp.c zc_sequence.c sliv.c)
add_library(srsran_phy_common OBJECT ${SOURCES})

add_subdirectory(test)