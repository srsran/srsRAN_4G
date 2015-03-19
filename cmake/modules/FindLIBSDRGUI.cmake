# - Try to find LIBSDRGUI
# Once done this will define
#  LIBSDRGUI_FOUND        - System has libsdrgui
#  LIBSDRGUI_INCLUDE_DIRS - The libsdrgui include directories
#  LIBSDRGUI_LIBRARIES    - The libsdrgui library

find_package(PkgConfig)
pkg_check_modules(PC_LIBSDRGUI QUIET libsdrgui)
set(LIBSDRGUI_DEFINITIONS ${PC_LIBSDRGUI_CFLAGS_OTHER})

FIND_PATH(
    LIBSDRGUI_INCLUDE_DIRS
    NAMES libsdrgui/libsdrgui.h
    HINTS ${PC_LIBSDRGUI_INCLUDEDIR}
          ${PC_LIBSDRGUI_INCLUDE_DIRS}
          $ENV{LIBSDRGUI_DIR}/include
    PATHS /usr/local/include 
          /usr/include 
)

FIND_LIBRARY(
    LIBSDRGUI_LIBRARIES
    NAMES sdrgui
    HINTS ${PC_LIBSDRGUI_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          $ENV{LIBSDRGUI_DIR}/lib
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

message(STATUS "LIBSDRGUI LIBRARIES " ${LIBSDRGUI_LIBRARIES})
message(STATUS "LIBSDRGUI INCLUDE DIRS " ${LIBSDRGUI_INCLUDE_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBSDRGUI DEFAULT_MSG LIBSDRGUI_LIBRARIES LIBSDRGUI_INCLUDE_DIRS)
MARK_AS_ADVANCED(LIBSDRGUI_LIBRARIES LIBSDRGUI_INCLUDE_DIRS)

