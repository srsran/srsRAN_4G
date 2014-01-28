if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. This is bad practice.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

cmake_minimum_required (VERSION 2.6)
project (osldlib)


# The version number.
set (OSLDLIB_VERSION_MAJOR 0)
set (OSLDLIB_VERSION_MINOR 0)


set(CPACK_PACKAGE_VERSION_MAJOR ${OSLDLIB_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${OSLDLIB_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES
  "${CMAKE_CURRENT_BINARY_DIR};/.bzr/;~$;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

option(DEBUG "Compiles with debugging symbols and no optimizations" OFF)

if(DEBUG)
	message("-- Configuring debugging CFLAGS")
	set(CFDEB "-O0 -g -rdynamic")
else()
	set(CFDEB "-O2")
endif()

set(CMAKE_C_FLAGS "${CFDEB} -Wall -Wno-format-extra-args -Winline -Wno-unused-result -Wno-format -std=c99 -D_GNU_SOURCE")
set(CMAKE_BINARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

### INCLUDES 
include_directories("{CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(examples)
add_subdirectory(lib)

