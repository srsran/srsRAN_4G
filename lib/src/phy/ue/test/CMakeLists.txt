#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# PROGRAM TO DEBUG PSS FROM USRP  
########################################################################

add_executable(gen_ack_test gen_ack_test.c)
target_link_libraries(gen_ack_test srsran_phy)
add_test(gen_ack_test gen_ack_test)

add_executable(gen_ack_nr_test gen_ack_nr_test.c)
target_link_libraries(gen_ack_nr_test srsran_phy)
add_test(gen_ack_nr_test gen_ack_nr_test)

add_executable(pucch_resource_test pucch_resource_test.c)
target_link_libraries(pucch_resource_test srsran_phy)
add_test(pucch_resource_test pucch_resource_test)

add_executable(ue_dl_nbiot_test ue_dl_nbiot_test.c)
target_link_libraries(ue_dl_nbiot_test srsran_phy pthread)
add_test(ue_dl_nbiot_test ue_dl_nbiot_test)

add_executable(ue_sync_nr_test ue_sync_nr_test.c)
target_link_libraries(ue_sync_nr_test srsran_phy pthread)
add_test(ue_sync_nr_test ue_sync_nr_test)

if(RF_FOUND)
    add_executable(ue_mib_sync_test_nbiot_usrp ue_mib_sync_test_nbiot_usrp.c)
    target_link_libraries(ue_mib_sync_test_nbiot_usrp srsran_phy srsran_rf pthread)

    add_executable(ue_sync_test_nbiot_usrp ue_sync_test_nbiot_usrp.c)
    target_link_libraries(ue_sync_test_nbiot_usrp srsran_rf srsran_phy pthread)
    if(SRSGUI_FOUND)
        include_directories(${SRSGUI_INCLUDE_DIRS})
        target_link_libraries(ue_sync_test_nbiot_usrp ${SRSGUI_LIBRARIES})
    else(SRSGUI_FOUND)
        add_definitions(-DDISABLE_GRAPHICS)
    endif(SRSGUI_FOUND)
endif(RF_FOUND)

add_executable(ue_dl_nr_file_test ue_dl_nr_file_test.c)
target_link_libraries(ue_dl_nr_file_test srsran_phy pthread)
foreach (n RANGE 0 9)
    add_test(ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0 ue_dl_nr_file_test -f ${CMAKE_CURRENT_SOURCE_DIR}/ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0.dat -i 1 -P 25 -n ${n} -R 1234)
    add_test(ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0 ue_dl_nr_file_test -f ${CMAKE_CURRENT_SOURCE_DIR}/ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0.dat -i 1 -P 25 -n ${n} -R 1234)
    add_test(ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0 ue_dl_nr_file_test -f ${CMAKE_CURRENT_SOURCE_DIR}/ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0.dat -i 1 -P 25 -n ${n} -R 1234)
    add_test(ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0 ue_dl_nr_file_test -f ${CMAKE_CURRENT_SOURCE_DIR}/ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0.dat -i 1 -P 25 -n ${n} -R 1234)
    add_test(ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0 ue_dl_nr_file_test -f ${CMAKE_CURRENT_SOURCE_DIR}/ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0.dat -i 1 -P 25 -n ${n} -R 1234)
    add_test(ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0 ue_dl_nr_file_test -f ${CMAKE_CURRENT_SOURCE_DIR}/ue_dl_nr_pci1_rb25_n${n}_common_L1_ncce0.dat -i 1 -P 25 -n ${n} -R 1234)
endforeach ()