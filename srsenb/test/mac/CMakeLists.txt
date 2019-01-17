
# Scheduler test
add_executable(scheduler_test scheduler_test.cc)
target_link_libraries(scheduler_test  srsenb_mac 
                                      srsenb_phy 
                                      srslte_common
                                      srslte_phy
                                      rrc_asn1
                                      ${CMAKE_THREAD_LIBS_INIT} 
                                      ${Boost_LIBRARIES})
