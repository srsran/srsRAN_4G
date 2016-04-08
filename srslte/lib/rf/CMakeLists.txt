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

IF(RF_FOUND)
  # Include common RF files
  SET(SOURCES_RF "")
  LIST(APPEND SOURCES_RF rf_imp.c rf_utils.c)

  IF (UHD_FOUND)
    add_definitions(-DENABLE_UHD)
    LIST(APPEND SOURCES_RF rf_uhd_imp.c uhd_c_api.cpp)
  ENDIF (UHD_FOUND)

  IF (BLADERF_FOUND)
    add_definitions(-DENABLE_BLADERF)
    LIST(APPEND SOURCES_RF rf_blade_imp.c)
  ENDIF (BLADERF_FOUND)

  ADD_LIBRARY(srslte_rf SHARED ${SOURCES_RF})
  SET_TARGET_PROPERTIES(srslte_rf
    PROPERTIES VERSION ${SRSLTE_VERSION_MAJOR}.${SRSLTE_VERSION_MINOR}
  )

  IF (UHD_FOUND)
    TARGET_LINK_LIBRARIES(srslte_rf ${UHD_LIBRARIES})
  ENDIF (UHD_FOUND)

  IF (BLADERF_FOUND)
    TARGET_LINK_LIBRARIES(srslte_rf ${BLADERF_LIBRARIES})
  ENDIF (BLADERF_FOUND)

  INSTALL(TARGETS srslte_rf DESTINATION ${LIBRARY_DIR})
  SRSLTE_SET_PIC(srslte_rf)

ENDIF(RF_FOUND)
