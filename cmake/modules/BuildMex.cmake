# BuildMex.cmake
# Author: Kent Williams norman-k-williams at uiowa.edu
# Modified by Ismael Gomez, 2014

include(CMakeParseArguments)

if(NOT DisableMEX)
  if(NOT MATLAB_FOUND)
    find_package(MATLAB)
  endif()

  if(NOT OCTAVE_FOUND)
    find_package(OCTAVE)
  endif()

  # CMake 2.8.12 & earlier apparently don't define the
  # Mex script path, so find it.
  if(NOT MATLAB_MEX_PATH)
    find_program( MATLAB_MEX_PATH mex
      HINTS ${MATLAB_ROOT}/bin
      PATHS ${MATLAB_ROOT}/bin
      DOC "The mex program path"
      )
  endif()
endif()

#
# BuildMex -- arguments
# MEXNAME = root of mex library name
# SOURCE = list of source files
# LIBRARIES = libraries needed to link mex library
FUNCTION(BuildMex)
  if(NOT DisableMEX)
    set(oneValueArgs MEXNAME)
    set(multiValueArgs SOURCES LIBRARIES)
    cmake_parse_arguments(BuildMex "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if (MATLAB_FOUND)
    add_library(${BuildMex_MEXNAME}-mat SHARED ${BuildMex_SOURCES})
    #target_include_directories(${BuildMex_MEXNAME}-mat PUBLIC ${MATLAB_INCLUDE_DIR})
    set_target_properties(${BuildMex_MEXNAME}-mat PROPERTIES
      SUFFIX "${MATLAB_MEX_EXTENSION}"
      PREFIX "srslte_"
      OUTPUT_NAME "${BuildMex_MEXNAME}"
      COMPILE_FLAGS "-fvisibility=default ${MATLAB_MEX_CFLAGS} -DFORCE_STANDARD_RATE -I${MATLAB_INCLUDE_DIR}"
      )
    target_link_libraries(${BuildMex_MEXNAME}-mat ${BuildMex_LIBRARIES} ${MATLAB_MEX_LIBRARY})
    install(TARGETS ${BuildMex_MEXNAME}-mat DESTINATION "${MEX_DIR}/srslte/")
    endif(MATLAB_FOUND)
    if (OCTAVE_FOUND)
    add_library(${BuildMex_MEXNAME}-oct SHARED ${BuildMex_SOURCES})
    #target_include_directories(${BuildMex_MEXNAME}-oct PUBLIC ${OCTAVE_INCLUDE_DIR})
    set_target_properties(${BuildMex_MEXNAME}-oct PROPERTIES
      SUFFIX ".${OCTAVE_MEXFILE_EXT}"
      PREFIX "srslte_"
      OUTPUT_NAME "${BuildMex_MEXNAME}"
      COMPILE_FLAGS "-fvisibility=default ${OCTAVE_MEX_CFLAGS} -DFORCE_STANDARD_RATE -DUNDEF_BOOL -I${OCTAVE_INCLUDE_DIR}"
      )
    target_link_libraries(${BuildMex_MEXNAME}-oct ${BuildMex_LIBRARIES} ${OCTAVE_LIBRARIES})
    install(TARGETS ${BuildMex_MEXNAME}-oct DESTINATION "${MEX_DIR}/srslte/")
    endif (OCTAVE_FOUND)
  endif (NOT DisableMEX)
ENDFUNCTION(BuildMex)

