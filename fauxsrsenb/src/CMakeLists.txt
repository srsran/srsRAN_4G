
add_subdirectory(phy)
add_subdirectory(mac)
add_subdirectory(upper)


# Link libstdc++ and libgcc
if(BUILD_STATIC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif(BUILD_STATIC)


if (RPATH)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif (RPATH)


add_executable(faux_srsenb main.cc enb.cc parser.cc enb_cfg_parser.cc metrics_stdout.cc)
target_link_libraries(faux_srsenb faux_srsenb_upper
                      faux_srsenb_mac
                      faux_srsenb_phy
                      srslte_common
                      srslte_phy
                      srslte_upper
                      srslte_radio
                      ${CMAKE_THREAD_LIBS_INIT} 
                      ${Boost_LIBRARIES} 
                      ${SEC_LIBRARIES}
                      ${LIBCONFIGPP_LIBRARIES}
                      ${SCTP_LIBRARIES}
                      ${FFTW3F_LIBRARIES})

if (RPATH)
  set_target_properties(faux_srsenb PROPERTIES INSTALL_RPATH ".")
endif (RPATH)

install(TARGETS faux_srsenb DESTINATION ${RUNTIME_DIR})

########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILDENB_CMD} STREQUAL "")
  message(STATUS "Added custom post-build-ENB command: ${BUILDENB_CMD}")
  add_custom_command(TARGET faux_srsenb POST_BUILD COMMAND ${BUILDENB_CMD})
else(NOT ${BUILDENB_CMD} STREQUAL "")
  message(STATUS "No post-build-ENB command defined")
endif (NOT ${BUILDENB_CMD} STREQUAL "")

install(TARGETS faux_srsenb DESTINATION ${RUNTIME_DIR})
