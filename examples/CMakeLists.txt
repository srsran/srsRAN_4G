#include fftw3 directories

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/")

find_package(FFTWS REQUIRED)
include_directories(${FFTWS_INCLUDE_DIRS})

find_package(UHD)


set(LIBRARIES osld m ${FFTWS_LIBRARIES})


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(hl_example hl_example.c)
target_link_libraries(hl_example ${LIBRARIES})

add_executable(ll_example ll_example.c)
target_link_libraries(ll_example ${LIBRARIES})

add_executable(synch_test synch_test.c)
target_link_libraries(synch_test ${LIBRARIES})

add_executable(equalizer_test equalizer_test.c)
target_link_libraries(equalizer_test ${LIBRARIES})

add_executable(viterbi_test viterbi_test.c)
target_link_libraries(viterbi_test ${LIBRARIES})

add_executable(cell_search cell_search.c)
target_link_libraries(cell_search ${LIBRARIES})


include_directories(${UHD_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../uhd)
add_executable(rssi_scan_usrp rssi_scan_usrp.c ../uhd/uhd_imp.cpp ../uhd/uhd_utils.c)
target_link_libraries(rssi_scan_usrp ${LIBRARIES} ${UHD_LIBRARIES})


include_directories(${UHD_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../uhd)
add_executable(pss_scan_usrp pss_scan_usrp.c ../uhd/uhd_imp.cpp ../uhd/uhd_utils.c)
target_link_libraries(pss_scan_usrp ${LIBRARIES} ${UHD_LIBRARIES})


include_directories(${UHD_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../uhd)
add_executable(mib_scan_usrp mib_scan_usrp.c ../uhd/uhd_imp.cpp ../uhd/uhd_utils.c)
target_link_libraries(mib_scan_usrp ${LIBRARIES} ${UHD_LIBRARIES})



