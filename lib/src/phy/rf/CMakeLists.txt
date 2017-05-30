#
# Copyright 2013-2017 Software Radio Systems Limited
#
# This file is part of srsLTE
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

if(RF_FOUND)
  # Include common RF files
  set(SOURCES_RF "")
  list(APPEND SOURCES_RF rf_imp.c rf_utils.c)

  if (UHD_FOUND)
    add_definitions(-DENABLE_UHD)
    list(APPEND SOURCES_RF rf_uhd_imp.c uhd_c_api.cpp)
  endif (UHD_FOUND)

  if (BLADERF_FOUND)
    add_definitions(-DENABLE_BLADERF)
    list(APPEND SOURCES_RF rf_blade_imp.c)
  endif (BLADERF_FOUND)

  if (LIMESDR_FOUND)
    add_definitions(-DENABLE_LIMESDR)
    list(APPEND SOURCES_RF rf_limesdr_imp.c)
  endif (LIMESDR_FOUND)

  if (SOAPYSDR_FOUND)
    add_definitions(-DENABLE_SOAPYSDR)
    list(APPEND SOURCES_RF rf_soapy_imp.c)
  endif (SOAPYSDR_FOUND)


  add_library(srslte_rf SHARED ${SOURCES_RF})
  
  
  if (UHD_FOUND)
    target_link_libraries(srslte_rf ${UHD_LIBRARIES})
  endif (UHD_FOUND)

  if (BLADERF_FOUND)
    target_link_libraries(srslte_rf ${BLADERF_LIBRARIES})
  endif (BLADERF_FOUND)

  if (LIMESDR_FOUND)
    target_link_libraries(srslte_rf ${LIMESDR_LIBRARIES})
  endif (LIMESDR_FOUND)

  if (SOAPYSDR_FOUND)
    target_link_libraries(srslte_rf ${SOAPYSDR_LIBRARIES})
  endif (SOAPYSDR_FOUND)


  INSTALL(TARGETS srslte_rf DESTINATION ${LIBRARY_DIR})
  SRSLTE_SET_PIC(srslte_rf)
endif(RF_FOUND)
