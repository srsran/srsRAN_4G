#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# PROGRAM TO DEBUG PSS FROM USRP  
########################################################################

add_executable(gen_ack_test gen_ack_test.c)
target_link_libraries(gen_ack_test srslte_phy)
add_test(gen_ack_test gen_ack_test)

add_executable(pucch_resource_test pucch_resource_test.c)
target_link_libraries(pucch_resource_test srslte_phy)
add_test(pucch_resource_test pucch_resource_test)

add_executable(ue_dl_nbiot_test ue_dl_nbiot_test.c)
target_link_libraries(ue_dl_nbiot_test srslte_phy pthread)
add_test(ue_dl_nbiot_test ue_dl_nbiot_test)

if(RF_FOUND)
    add_executable(ue_mib_sync_test_nbiot_usrp ue_mib_sync_test_nbiot_usrp.c)
    target_link_libraries(ue_mib_sync_test_nbiot_usrp srslte_phy srslte_rf pthread)

    add_executable(ue_sync_test_nbiot_usrp ue_sync_test_nbiot_usrp.c)
    target_link_libraries(ue_sync_test_nbiot_usrp srslte_rf srslte_phy pthread)
    if(SRSGUI_FOUND)
        include_directories(${SRSGUI_INCLUDE_DIRS})
        target_link_libraries(ue_sync_test_nbiot_usrp ${SRSGUI_LIBRARIES})
    else(SRSGUI_FOUND)
        add_definitions(-DDISABLE_GRAPHICS)
    endif(SRSGUI_FOUND)
endif(RF_FOUND)