#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES rrc.cc rrc_ue.cc rrc_mobility.cc rrc_cell_cfg.cc rrc_bearer_cfg.cc mac_controller.cc ue_rr_cfg.cc ue_meas_cfg.cc rrc_endc.cc)
add_library(srsenb_rrc STATIC ${SOURCES})

set(SOURCES rrc_nr.cc)
add_library(srsgnb_rrc STATIC ${SOURCES})
