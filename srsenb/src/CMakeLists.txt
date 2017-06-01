
add_subdirectory(phy)
add_subdirectory(mac)
add_subdirectory(upper)


# Link libstdc++ and libgcc
if(STATIC_LIB)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif(STATIC_LIB)


if (RPATH)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif (RPATH)


add_executable(enb main.cc enb.cc parser.cc enb_cfg_parser.cc metrics_stdout.cc)
target_link_libraries(enb   srsenb_upper
                            srsenb_mac
                            srsenb_phy
                            srslte_common
                            srslte_phy
                            srslte_upper
                            srslte_radio
                            ${CMAKE_THREAD_LIBS_INIT} 
                            ${Boost_LIBRARIES} 
                            ${POLAR_LIBRARIES} 
                            ${LIBCONFIGPP_LIBRARIES}
                            ${SCTP_LIBRARIES})

set_target_properties(enb PROPERTIES INSTALL_RPATH ".")
                            
                            
                            
########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "Added custom post-build command: ${BUILD_CMD}")
  add_custom_command(TARGET enb POST_BUILD COMMAND ${BUILD_CMD})
else(NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "No post-build command defined")
endif (NOT ${BUILD_CMD} STREQUAL "")

