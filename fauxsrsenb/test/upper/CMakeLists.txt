# IP tx/rx program test
add_executable(faux_ip_test_enb ip_test.cc)
target_link_libraries(faux_ip_test_enb faux_srsenb_upper
                                  faux_srsenb_mac
                                  faux_srsenb_phy
                                  srslte_common
                                  srslte_phy
                                  srslte_upper
                                  srslte_radio
                                  ${CMAKE_THREAD_LIBS_INIT}
                                  ${Boost_LIBRARIES}
                                  ${SEC_LIBRARIES}
                                  ${FFTW3F_LIBRARIES})

# Simple PLMN -> MCC/MNC test
add_executable(faux_plmn_test plmn_test.cc)
target_link_libraries(faux_plmn_test faux_srsenb_upper srslte_asn1 )

