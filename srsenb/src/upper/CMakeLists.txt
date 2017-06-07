file(GLOB SOURCES "*.cc")
add_library(srsenb_upper SHARED ${SOURCES})
target_link_libraries(srsenb_upper)
