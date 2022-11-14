set(SOURCES ric_client.cc e2ap.cc)
add_library(srsgnb_ric STATIC ${SOURCES})
#target_link_libraries(srsgnb_ric srsran_asn1)

add_subdirectory(test)
