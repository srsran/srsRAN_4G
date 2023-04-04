set(SOURCES ric_client.cc e2ap_ric_subscription.cc e2ap.cc e2sm_kpm_common.cc e2sm_kpm.cc e2sm_kpm_report_service.cc)
add_library(srsgnb_ric STATIC ${SOURCES})
target_link_libraries(srsgnb_ric srsran_asn1 ric_e2)

add_subdirectory(test)
