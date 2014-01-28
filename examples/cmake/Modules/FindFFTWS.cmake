# - Find FFTW
# Find the native FFTW includes and library
#
#  FFTW_INCLUDES    - where to find fftw3.h
#  FFTW_LIBRARIES   - List of libraries when using FFTW.
#  FFTW_FOUND       - True if FFTW found.

if (FFTWS_INCLUDES)
  # Already in cache, be silent
  set (FFTWS_FIND_QUIETLY TRUE)
endif (FFTWS_INCLUDES)

find_path (FFTWS_INCLUDES fftw3.h)
SET(CMAKE_FIND_LIBRARY_SUFFIXES .a) 
find_library (FFTWfS_LIBRARIES NAMES fftw3f)
find_library (FFTWnS_LIBRARIES NAMES fftw3)
set(FFTWS_LIBRARIES ${FFTWfS_LIBRARIES} ${FFTWnS_LIBRARIES})

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (FFTWS DEFAULT_MSG FFTWS_LIBRARIES FFTWS_INCLUDES)

mark_as_advanced (FFTWS_LIBRARIES FFTWS_INCLUDES)
