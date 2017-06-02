file(GLOB SOURCES "*.cc")
add_library(srsenb_mac SHARED ${SOURCES})
target_link_libraries(srsenb_mac)


