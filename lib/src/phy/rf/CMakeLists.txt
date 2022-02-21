#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if(RF_FOUND)
  # This library is only used by the examples 
  add_library(srsran_rf_utils STATIC rf_utils.c)
  target_link_libraries(srsran_rf_utils srsran_phy)

  # Top-level RF library sources
  set(SOURCES_RF "")
  list(APPEND SOURCES_RF rf_imp.c)

  # Lists of static (builtin) and dynamic RF plugins
  set(STATIC_PLUGINS "")
  set(DYNAMIC_PLUGINS "")

  if (ENABLE_RF_PLUGINS)
    add_definitions(-DENABLE_RF_PLUGINS)
  endif (ENABLE_RF_PLUGINS)

  # RF plugins
  if (UHD_FOUND AND ENABLE_UHD)
    add_definitions(-DENABLE_UHD)
    set(SOURCES_UHD rf_uhd_imp.cc)
    if (ENABLE_RF_PLUGINS)
      add_library(srsran_rf_uhd SHARED ${SOURCES_UHD})
      set_target_properties(srsran_rf_uhd PROPERTIES VERSION ${SRSRAN_VERSION_STRING} SOVERSION ${SRSRAN_SOVERSION})
      list(APPEND DYNAMIC_PLUGINS srsran_rf_uhd)
    else (ENABLE_RF_PLUGINS)
      add_library(srsran_rf_uhd STATIC ${SOURCES_UHD})
      list(APPEND STATIC_PLUGINS srsran_rf_uhd)
    endif (ENABLE_RF_PLUGINS)
    target_link_libraries(srsran_rf_uhd srsran_rf_utils srsran_phy ${UHD_LIBRARIES} ${Boost_LIBRARIES})
    install(TARGETS srsran_rf_uhd DESTINATION ${LIBRARY_DIR})

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

  if (BLADERF_FOUND AND ENABLE_BLADERF)
    add_definitions(-DENABLE_BLADERF)
    set(SOURCES_BLADE rf_blade_imp.c)
    if (ENABLE_RF_PLUGINS)
      add_library(srsran_rf_blade SHARED ${SOURCES_BLADE})
      set_target_properties(srsran_rf_blade PROPERTIES VERSION ${SRSRAN_VERSION_STRING} SOVERSION ${SRSRAN_SOVERSION})
      list(APPEND DYNAMIC_PLUGINS srsran_rf_blade)
    else (ENABLE_RF_PLUGINS)
      add_library(srsran_rf_blade STATIC ${SOURCES_BLADE})
      list(APPEND STATIC_PLUGINS srsran_rf_blade)
    endif (ENABLE_RF_PLUGINS)
    target_link_libraries(srsran_rf_blade srsran_rf_utils srsran_phy ${BLADERF_LIBRARIES})
    install(TARGETS srsran_rf_blade DESTINATION ${LIBRARY_DIR})
  endif (BLADERF_FOUND)

  if (SOAPYSDR_FOUND AND ENABLE_SOAPYSDR)
    add_definitions(-DENABLE_SOAPYSDR)
    set(SOURCES_SOAPY rf_soapy_imp.c)
    if (ENABLE_RF_PLUGINS)
      add_library(srsran_rf_soapy SHARED ${SOURCES_SOAPY})
      set_target_properties(srsran_rf_soapy PROPERTIES VERSION ${SRSRAN_VERSION_STRING} SOVERSION ${SRSRAN_SOVERSION})
      list(APPEND DYNAMIC_PLUGINS srsran_rf_soapy)
    else (ENABLE_RF_PLUGINS)
      add_library(srsran_rf_soapy STATIC ${SOURCES_SOAPY})
      list(APPEND STATIC_PLUGINS srsran_rf_soapy)
    endif (ENABLE_RF_PLUGINS)
    target_link_libraries(srsran_rf_soapy srsran_rf_utils srsran_phy ${SOAPYSDR_LIBRARIES})
    install(TARGETS srsran_rf_soapy DESTINATION ${LIBRARY_DIR})
  endif (SOAPYSDR_FOUND AND ENABLE_SOAPYSDR)

  if(SKIQ_FOUND AND ENABLE_SKIQ)
    add_executable(skiq_pps_test skiq_pps_test.c)
    target_link_libraries(skiq_pps_test ${SKIQ_LIBRARIES} rt pthread m)
    add_definitions(-DENABLE_SIDEKIQ)
    set(SOURCES_SKIQ rf_skiq_imp.c rf_skiq_imp_card.c rf_skiq_imp_port.c)
    if (ENABLE_RF_PLUGINS)
      add_library(srsran_rf_skiq SHARED ${SOURCES_SKIQ})
      set_target_properties(srsran_rf_skiq PROPERTIES VERSION ${SRSRAN_VERSION_STRING} SOVERSION ${SRSRAN_SOVERSION})
      list(APPEND DYNAMIC_PLUGINS srsran_rf_skiq)
    else (ENABLE_RF_PLUGINS)
      add_library(srsran_rf_skiq STATIC ${SOURCES_SKIQ})
      list(APPEND STATIC_PLUGINS srsran_rf_skiq)
    endif (ENABLE_RF_PLUGINS)
    target_link_libraries(srsran_rf_skiq srsran_rf_utils srsran_phy ${SKIQ_LIBRARIES} rt)
    install(TARGETS srsran_rf_skiq DESTINATION ${LIBRARY_DIR})
  endif(SKIQ_FOUND)

  if (ZEROMQ_FOUND AND ENABLE_ZEROMQ)
    add_definitions(-DENABLE_ZEROMQ)
    set(SOURCES_ZMQ rf_zmq_imp.c rf_zmq_imp_tx.c rf_zmq_imp_rx.c)
    if (ENABLE_RF_PLUGINS)
      add_library(srsran_rf_zmq SHARED ${SOURCES_ZMQ})
      set_target_properties(srsran_rf_zmq PROPERTIES VERSION ${SRSRAN_VERSION_STRING} SOVERSION ${SRSRAN_SOVERSION})
      list(APPEND DYNAMIC_PLUGINS srsran_rf_zmq)
    else (ENABLE_RF_PLUGINS)
      add_library(srsran_rf_zmq STATIC ${SOURCES_ZMQ})
      list(APPEND STATIC_PLUGINS srsran_rf_zmq)
    endif (ENABLE_RF_PLUGINS)
    target_link_libraries(srsran_rf_zmq srsran_rf_utils srsran_phy ${ZEROMQ_LIBRARIES})
    install(TARGETS srsran_rf_zmq DESTINATION ${LIBRARY_DIR})
  endif (ZEROMQ_FOUND)

  # Add sources of file-based RF directly to the RF library (not as a plugin)
  list(APPEND SOURCES_RF rf_file_imp.c rf_file_imp_tx.c rf_file_imp_rx.c)

  # Top-level RF library
  add_library(srsran_rf_object OBJECT ${SOURCES_RF})
  set_property(TARGET srsran_rf_object PROPERTY POSITION_INDEPENDENT_CODE 1)
  if (ENABLE_RF_PLUGINS)
    add_dependencies(srsran_rf_object ${DYNAMIC_PLUGINS})
    add_library(srsran_rf SHARED $<TARGET_OBJECTS:srsran_rf_object>)
    target_link_libraries(srsran_rf dl)
    # Add $ORIGIN (i.e. current location of this library) to rpath of srsran_rf.
    # This ensures that it will find the plugins that reside in the same directory as the library
    set_target_properties(srsran_rf PROPERTIES BUILD_RPATH "\$ORIGIN/")
    set_target_properties(srsran_rf PROPERTIES INSTALL_RPATH "\$ORIGIN/")
  else (ENABLE_RF_PLUGINS)
    # Without RF plugins, we aggregate everything in a static library (builtin plugins)
    add_library(srsran_rf STATIC $<TARGET_OBJECTS:srsran_rf_object>)
    target_link_libraries(srsran_rf ${STATIC_PLUGINS})
  endif (ENABLE_RF_PLUGINS)

  target_link_libraries(srsran_rf srsran_rf_utils srsran_phy)
  set_target_properties(srsran_rf PROPERTIES VERSION ${SRSRAN_VERSION_STRING} SOVERSION ${SRSRAN_SOVERSION})
  install(TARGETS srsran_rf DESTINATION ${LIBRARY_DIR})

  # Tests
  if (UHD_FOUND AND UHD_ENABLE_CUSTOM_RFNOC)
    add_executable(rfnoc_test rfnoc_test.cc)
    target_link_libraries(rfnoc_test srsran_rf ${UHD_LIBRARIES} ${Boost_LIBRARIES} /usr/lib/x86_64-linux-gnu/libboost_system.so)
    message(info ${Boost_LIBRARIES})
  endif (UHD_FOUND AND UHD_ENABLE_CUSTOM_RFNOC)

  if (ZEROMQ_FOUND)
    add_executable(rf_zmq_test rf_zmq_test.c)
    target_link_libraries(rf_zmq_test srsran_rf)
    #add_test(rf_zmq_test rf_zmq_test)
  endif (ZEROMQ_FOUND)

  add_executable(rf_file_test rf_file_test.c)
  target_link_libraries(rf_file_test srsran_rf)
  add_test(rf_file_test rf_file_test)
endif(RF_FOUND)
