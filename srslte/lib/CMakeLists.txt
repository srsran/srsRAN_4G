#
# Copyright 2013-2015 Software Radio Systems Limited
#
# This file is part of the srsLTE library.
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

ADD_SUBDIRECTORY(agc)
ADD_SUBDIRECTORY(ch_estimation)
ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(fec)
ADD_SUBDIRECTORY(mimo)
ADD_SUBDIRECTORY(phch)
ADD_SUBDIRECTORY(rf)
ADD_SUBDIRECTORY(sync)
ADD_SUBDIRECTORY(utils)
ADD_SUBDIRECTORY(channel)
ADD_SUBDIRECTORY(dft)
ADD_SUBDIRECTORY(io)
ADD_SUBDIRECTORY(modem)
ADD_SUBDIRECTORY(resampling)
ADD_SUBDIRECTORY(scrambling)
ADD_SUBDIRECTORY(ue)

ADD_LIBRARY(srslte  SHARED  version.c)
TARGET_LINK_LIBRARIES(srslte  srslte_agc
                              srslte_ch_estimation
                              srslte_common
                              srslte_fec
                              srslte_mimo
                              srslte_phch
                              srslte_sync
                              srslte_utils
                              srslte_channel
                              srslte_dft
                              srslte_io
                              srslte_modem
                              srslte_resampling
                              srslte_scrambling
                              srslte_ue
                              srslte_rf
                              pthread)
SET_TARGET_PROPERTIES(srslte  PROPERTIES
                              VERSION ${SRSLTE_VERSION_MAJOR}.${SRSLTE_VERSION_MINOR})

IF(VOLK_FOUND)
  SET_TARGET_PROPERTIES(srslte PROPERTIES COMPILE_DEFINITIONS "${VOLK_DEFINITIONS}")
  TARGET_LINK_LIBRARIES(srslte ${VOLK_LIBRARIES})
ENDIF(VOLK_FOUND)

INSTALL(TARGETS srslte DESTINATION ${LIBRARY_DIR})
SRSLTE_SET_PIC(srslte)

