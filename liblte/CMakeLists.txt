include_directories(hdr)
add_library(lte SHARED
  src/liblte_common.cc
  src/liblte_rrc.cc
  src/liblte_mme.cc
  src/liblte_security.cc
)
target_link_libraries(lte ${POLAR_LIBRARIES})
INSTALL(TARGETS lte DESTINATION ${LIBRARY_DIR})
