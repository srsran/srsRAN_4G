INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_VOLK volk QUIET)

FIND_PATH(
    VOLK_INCLUDE_DIRS
    NAMES volk.h 
    HINTS $ENV{VOLK_DIR}/include/volk
	      ${CMAKE_INSTALL_PREFIX}/include/volk
          ${PC_VOLK_INCLUDE_DIR}
    PATHS /usr/local/include/volk
          /usr/include/volk
)

FIND_LIBRARY(
    VOLK_LIBRARIES
    NAMES volk
    HINTS $ENV{VOLK_DIR}/lib
	${CMAKE_INSTALL_PREFIX}/lib
	${CMAKE_INSTALL_PREFIX}/lib64
        ${PC_VOLK_LIBDIR}
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VOLK DEFAULT_MSG VOLK_LIBRARIES VOLK_INCLUDE_DIRS)
MARK_AS_ADVANCED(VOLK_LIBRARIES VOLK_INCLUDE_DIRS)
