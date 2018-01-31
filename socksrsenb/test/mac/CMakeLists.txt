
# Scheduler test
add_executable(sock_scheduler_test scheduler_test.cc)
target_link_libraries(sock_scheduler_test  sock_srsenb_mac 
                                      sock_srsenb_phy 
                                      srslte_common
                                      srslte_phy
                                      ${CMAKE_THREAD_LIBS_INIT} 
                                      ${Boost_LIBRARIES})
