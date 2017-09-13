INCLUDE(FindPkgConfig)
#PKG_CHECK_MODULES(UHD uhd)
IF(NOT UHD_FOUND)

FIND_PATH(
    UHD_INCLUDE_DIRS
    NAMES uhd.h
    HINTS $ENV{UHD_DIR}/include
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    UHD_LIBRARIES
    NAMES uhd
    HINTS $ENV{UHD_DIR}/lib
    PATHS /usr/local/lib
          /usr/lib
          /usr/lib/x86_64-linux-gnu
          /usr/local/lib64
          /usr/local/lib32
)

message(STATUS "UHD LIBRARIES " ${UHD_LIBRARIES})
message(STATUS "UHD INCLUDE DIRS " ${UHD_INCLUDE_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(UHD DEFAULT_MSG UHD_LIBRARIES UHD_INCLUDE_DIRS)
MARK_AS_ADVANCED(UHD_LIBRARIES UHD_INCLUDE_DIRS)

ENDIF(NOT UHD_FOUND)
