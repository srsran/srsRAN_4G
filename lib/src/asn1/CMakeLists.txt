set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch -Wno-unused-but-set-variable -Wno-unused-variable -Wno-return-type -Wno-sign-compare -Wno-reorder -Wno-parantheses")
add_library(srslte_asn1 SHARED
  liblte_common.cc
  liblte_rrc.cc
  liblte_mme.cc
)
INSTALL(TARGETS srslte_asn1 DESTINATION ${LIBRARY_DIR})
