#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES rrc.cc rrc_procedures.cc rrc_meas.cc rrc_cell.cc phy_controller.cc)

if(ENABLE_5GNR)
    set(SOURCES ${SOURCES} rrc_nr.cc)
endif()

add_library(srsue_rrc STATIC ${SOURCES})

if(ENABLE_5GNR)
    set(SOURCES rrc_nr.cc)
    add_library(srsue_rrc_nr STATIC ${SOURCES})
endif()
