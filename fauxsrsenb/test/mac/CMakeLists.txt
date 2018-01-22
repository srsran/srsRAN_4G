
# Scheduler test
add_executable(faux_scheduler_test scheduler_test.cc)
target_link_libraries(faux_scheduler_test  faux_srsenb_mac 
                                      faux_srsenb_phy 
                                      srslte_common
                                      srslte_phy
                                      ${CMAKE_THREAD_LIBS_INIT} 
                                      ${Boost_LIBRARIES}
                                      ${FFTW3F_LIBRARIES})
