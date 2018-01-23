# IP tx/rx program test
add_executable(ip_test_enb ip_test.cc)
target_link_libraries(ip_test_enb srsenb_upper
                                  srsenb_mac
                                  srsenb_phy
                                  srslte_common
                                  srslte_phy
                                  srslte_upper
                                  srslte_radio
                                  ${CMAKE_THREAD_LIBS_INIT} 
                                  ${Boost_LIBRARIES} 
                                  ${SEC_LIBRARIES})

# Simple PLMN -> MCC/MNC test
add_executable(plmn_test plmn_test.cc)
target_link_libraries(plmn_test srsenb_upper srslte_asn1 )

