# IP tx/rx program test
add_executable(sock_ip_test_enb ip_test.cc)
target_link_libraries(sock_ip_test_enb sock_srsenb_upper
                                  sock_srsenb_mac
                                  sock_srsenb_phy
                                  srslte_common
                                  srslte_phy
                                  srslte_upper
                                  srslte_radio
                                  ${CMAKE_THREAD_LIBS_INIT} 
                                  ${Boost_LIBRARIES} 
                                  ${SEC_LIBRARIES})

# Simple PLMN -> MCC/MNC test
add_executable(sock_plmn_test plmn_test.cc)
target_link_libraries(sock_plmn_test sock_srsenb_upper srslte_asn1 )

