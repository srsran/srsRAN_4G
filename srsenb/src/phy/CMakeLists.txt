file(GLOB SOURCES "*.cc")
add_library(srsenb_phy SHARED ${SOURCES})
target_link_libraries(srsenb_phy)

if(ENABLE_GUI AND SRSGUI_FOUND)
    target_link_libraries(srsenb_phy ${SRSGUI_LIBRARIES})
endif(ENABLE_GUI AND SRSGUI_FOUND)

