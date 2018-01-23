# - Try to find fftw3f - the single-precision version of FFTW3
# Once done this will define
#  FFTW3F_FOUND - System has fftw3f
#  FFTW3F_INCLUDE_DIRS - The fftw3f include directories
#  FFTW3F_LIBRARIES - The libraries needed to use fftw3f
#  FFTW3F_DEFINITIONS - Compiler switches required for using fftw3f

find_package(PkgConfig)
pkg_check_modules(PC_FFTW3F "fftw3f >= 3.0")
set(FFTW3F_DEFINITIONS ${PC_FFTW3F_CFLAGS_OTHER})

find_path(FFTW3F_INCLUDE_DIR 
            NAMES fftw3.h
            HINTS ${PC_FFTW3F_INCLUDEDIR} ${PC_FFTW3F_INCLUDE_DIRS} $ENV{FFTW3_DIR}/include
            PATHS /usr/local/include 
                  /usr/include )

find_library(FFTW3F_STATIC_LIBRARY
            NAMES fftw3f.a libfftw3f.a libfftw3f-3.a
            HINTS ${PC_FFTW3F_LIBDIR} ${PC_FFTW3F_LIBRARY_DIRS} $ENV{FFTW3_DIR}/lib
            PATHS /usr/local/lib
                  /usr/lib)

find_library(FFTW3F_LIBRARY 
            NAMES fftw3f libfftw3f libfftw3f-3
            HINTS ${PC_FFTW3F_LIBDIR} ${PC_FFTW3F_LIBRARY_DIRS} $ENV{FFTW3_DIR}/lib
            PATHS /usr/local/lib
                  /usr/lib)

set(FFTW3F_LIBRARIES ${FFTW3F_LIBRARY} )
set(FFTW3F_STATIC_LIBRARIES ${FFTW3F_STATIC_LIBRARY} )
set(FFTW3F_INCLUDE_DIRS ${FFTW3F_INCLUDE_DIR} )

message(STATUS "FFTW3F LIBRARIES: " ${FFTW3F_LIBRARIES})
message(STATUS "FFTW3F STATIC LIBRARIES: " ${FFTW3F_STATIC_LIBRARIES})
message(STATUS "FFTW3F INCLUDE DIRS: " ${FFTW3F_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FFTW3F_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(fftw3f  DEFAULT_MSG
                                  FFTW3F_LIBRARY FFTW3F_INCLUDE_DIR)

mark_as_advanced(FFTW3F_INCLUDE_DIR FFTW3F_STATIC_LIBRARY FFTW3F_LIBRARY )
