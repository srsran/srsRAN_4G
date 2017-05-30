include_directories(hdr)
add_library(srslte_asn1 SHARED
  liblte_common.cc
  liblte_rrc.cc
  liblte_mme.cc
)
INSTALL(TARGETS srslte_asn1 DESTINATION ${LIBRARY_DIR})
