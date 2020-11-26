#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if(RF_FOUND)
  # This library is only used by the examples 
  add_library(srslte_rf_utils STATIC rf_utils.c)
  target_link_libraries(srslte_rf_utils srslte_phy)

  # Include common RF files
  set(SOURCES_RF "")
  list(APPEND SOURCES_RF rf_imp.c)

  if (UHD_FOUND)
    add_definitions(-DENABLE_UHD)
    list(APPEND SOURCES_RF rf_uhd_imp.cc)

    # If found, add a macro to inform the UHD driver about the available feature
    if (UHD_ENABLE_X300_FW_RESET)
      add_definitions(-DENABLE_UHD_X300_FW_RESET)
    endif(UHD_ENABLE_X300_FW_RESET)
    if (UHD_ENABLE_RFNOC)
      add_definitions(-DUHD_ENABLE_RFNOC)
    endif(UHD_ENABLE_RFNOC)
    if (UHD_ENABLE_CUSTOM_RFNOC)
      add_definitions(-DUHD_ENABLE_CUSTOM_RFNOC)
    endif(UHD_ENABLE_CUSTOM_RFNOC)
  endif (UHD_FOUND)

  if (UHD_FOUND AND UHD_ENABLE_CUSTOM_RFNOC)
    add_executable(rfnoc_test rfnoc_test.cc)
    target_link_libraries(rfnoc_test srslte_rf ${UHD_LIBRARIES} ${Boost_LIBRARIES} /usr/lib/x86_64-linux-gnu/libboost_system.so)
    message(info ${Boost_LIBRARIES})
  endif (UHD_FOUND AND UHD_ENABLE_CUSTOM_RFNOC)

  if (BLADERF_FOUND)
    add_definitions(-DENABLE_BLADERF)
    list(APPEND SOURCES_RF rf_blade_imp.c)
  endif (BLADERF_FOUND)

  if (SOAPYSDR_FOUND AND ENABLE_SOAPYSDR)
    add_definitions(-DENABLE_SOAPYSDR)
    list(APPEND SOURCES_RF rf_soapy_imp.c)
  endif (SOAPYSDR_FOUND AND ENABLE_SOAPYSDR)

  if (ZEROMQ_FOUND)
    add_definitions(-DENABLE_ZEROMQ)
    list(APPEND SOURCES_RF rf_zmq_imp.c rf_zmq_imp_tx.c rf_zmq_imp_rx.c)
  endif (ZEROMQ_FOUND)

  add_library(srslte_rf SHARED ${SOURCES_RF})
  target_link_libraries(srslte_rf srslte_rf_utils srslte_phy)
  set_target_properties(srslte_rf PROPERTIES VERSION ${SRSLTE_VERSION_STRING} SOVERSION ${SRSLTE_SOVERSION})
  
  if (UHD_FOUND)
    target_link_libraries(srslte_rf ${UHD_LIBRARIES})
  endif (UHD_FOUND)

  if (BLADERF_FOUND)
    target_link_libraries(srslte_rf ${BLADERF_LIBRARIES})
  endif (BLADERF_FOUND)

  if (SOAPYSDR_FOUND AND ENABLE_SOAPYSDR)
    target_link_libraries(srslte_rf ${SOAPYSDR_LIBRARIES})
  endif (SOAPYSDR_FOUND AND ENABLE_SOAPYSDR)

  if (ZEROMQ_FOUND)
    target_link_libraries(srslte_rf ${ZEROMQ_LIBRARIES})
    add_executable(rf_zmq_test rf_zmq_test.c)
    target_link_libraries(rf_zmq_test srslte_rf)
    #add_test(rf_zmq_test rf_zmq_test)
  endif (ZEROMQ_FOUND)

  INSTALL(TARGETS srslte_rf DESTINATION ${LIBRARY_DIR})
endif(RF_FOUND)
