
add_subdirectory(mme)
add_subdirectory(hss)
add_subdirectory(spgw)

# Link libstdc++ and libgcc
if(BUILD_STATIC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif(BUILD_STATIC)


if (RPATH)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif (RPATH)


add_executable(srsepc main.cc )
target_link_libraries(srsepc  srsepc_mme
                              srsepc_hss
                              srsepc_sgw
                              srslte_upper
                              srslte_common
                              ${CMAKE_THREAD_LIBS_INIT} 
                              ${Boost_LIBRARIES} 
                              ${SEC_LIBRARIES}
                              ${LIBCONFIGPP_LIBRARIES}
                              ${SCTP_LIBRARIES})

if (RPATH)
  set_target_properties(srsepc PROPERTIES INSTALL_RPATH ".")
endif (RPATH)

install(TARGETS srsepc DESTINATION ${RUNTIME_DIR})

########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILDEPC_CMD} STREQUAL "")
  message(STATUS "Added custom post-build-EPC command: ${BUILDENB_CMD}")
  add_custom_command(TARGET srsenb POST_BUILD COMMAND ${BUILDENB_CMD})
else(NOT ${BUILDEPC_CMD} STREQUAL "")
  message(STATUS "No post-build-EPC command defined")
endif (NOT ${BUILDEPC_CMD} STREQUAL "")

install(TARGETS srsepc DESTINATION ${RUNTIME_DIR})
