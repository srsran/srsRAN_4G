# - this module looks for Matlab
# Defines:
#  MATLAB_INCLUDE_DIR: include path for mex.h, engine.h
#  MATLAB_LIBRARIES:   required libraries: libmex, etc
#  MATLAB_MEX_LIBRARY: path to libmex.lib
#  MATLAB_MX_LIBRARY:  path to libmx.lib
#  MATLAB_MAT_LIBRARY:  path to libmat.lib # added
#  MATLAB_ENG_LIBRARY: path to libeng.lib
#  MATLAB_ROOT: path to Matlab's root directory

# This file is part of Gerardus
#
# This is a derivative work of file FindMatlab.cmake released with
# CMake v2.8, because the original seems to be a bit outdated and
# doesn't work with my Windows XP and Visual Studio 10 install
#
# (Note that the original file does work for Ubuntu Natty)
#
# Author: Ramon Casero <rcasero at gmail.com>, Tom Doel
# Version: 0.2.3
# $Rev$
# $Date$
#
# The original file was copied from an Ubuntu Linux install
# /usr/share/cmake-2.8/Modules/FindMatlab.cmake

set(MATLAB_FOUND FALSE)
if(WIN32)
  # Search for a version of Matlab available, starting from the most modern one to older versions
  foreach(MATVER "7.14" "7.11" "7.10" "7.9" "7.8" "7.7" "7.6" "7.5" "7.4")
    if((NOT DEFINED MATLAB_ROOT)
        OR ("${MATLAB_ROOT}" STREQUAL "")
        OR ("${MATLAB_ROOT}" STREQUAL "/registry"))
      get_filename_component(MATLAB_ROOT "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MathWorks\\MATLAB\\${MATVER};MATLABROOT]"
        ABSOLUTE)
      set(MATLAB_VERSION ${MATVER})
    endif()
  endforeach()

  # Directory name depending on whether the Windows architecture is 32
  # bit or 64 bit
  set(CMAKE_SIZEOF_VOID_P 8) # Note: For some wierd reason this variable is undefined in my system...
  if(CMAKE_SIZEOF_VOID_P MATCHES "4")
    set(WINDIR "win32")
  elseif(CMAKE_SIZEOF_VOID_P MATCHES "8")
    set(WINDIR "win64")
  else()
    message(FATAL_ERROR "CMAKE_SIZEOF_VOID_P (${CMAKE_SIZEOF_VOID_P}) doesn't indicate a valid platform")
  endif()

  # Folder where the MEX libraries are, depending of the Windows compiler
  if(${CMAKE_GENERATOR} MATCHES "Visual Studio 6")
    set(MATLAB_LIBRARIES_DIR "${MATLAB_ROOT}/extern/lib/${WINDIR}/microsoft/msvc60")
  elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio 7")
    # Assume people are generally using Visual Studio 7.1,
    # if using 7.0 need to link to: ../extern/lib/${WINDIR}/microsoft/msvc70
    set(MATLAB_LIBRARIES_DIR "${MATLAB_ROOT}/extern/lib/${WINDIR}/microsoft/msvc71")
    # set(MATLAB_LIBRARIES_DIR "${MATLAB_ROOT}/extern/lib/${WINDIR}/microsoft/msvc70")
  elseif(${CMAKE_GENERATOR} MATCHES "Borland")
    # Assume people are generally using Borland 5.4,
    # if using 7.0 need to link to ../extern/lib/${WINDIR}/microsoft/msvc70
    set(MATLAB_LIBRARIES_DIR "${MATLAB_ROOT}/extern/lib/${WINDIR}/microsoft/bcc54")
    # set(MATLAB_LIBRARIES_DIR "${MATLAB_ROOT}/extern/lib/${WINDIR}/microsoft/bcc50")
    # set(MATLAB_LIBRARIES_DIR "${MATLAB_ROOT}/extern/lib/${WINDIR}/microsoft/bcc51")
  elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio*")
    # If the compiler is Visual Studio, but not any of the specific
    # versions above, we try our luck with the microsoft directory
    set(MATLAB_LIBRARIES_DIR "${MATLAB_ROOT}/extern/lib/${WINDIR}/microsoft/")
  else()
    message(FATAL_ERROR "Generator not compatible: ${CMAKE_GENERATOR}")
  endif()

  # Get paths to the Matlab MEX libraries
  find_library(MATLAB_MEX_LIBRARY libmex ${MATLAB_LIBRARIES_DIR} )
  find_library(MATLAB_MX_LIBRARY  libm   ${MATLAB_LIBRARIES_DIR} )
  find_library(MATLAB_MAT_LIBRARY libmat ${MATLAB_LIBRARIES_DIR} )
  find_library(MATLAB_ENG_LIBRARY libeng ${MATLAB_LIBRARIES_DIR} )

  # Get path to the include directory
  find_path(MATLAB_INCLUDE_DIR "mex.h" "${MATLAB_ROOT}/extern/include" )

else()

  if((NOT DEFINED MATLAB_ROOT)
      OR ("${MATLAB_ROOT}" STREQUAL ""))
    # get path to the Matlab root directory
    
    execute_process(
      COMMAND which matlab
      OUTPUT_VARIABLE MATLAB_BIN_EXISTS
    )
    
    IF (MATLAB_BIN_EXISTS)
      execute_process(
        COMMAND which matlab
        COMMAND xargs realpath
        COMMAND xargs dirname
        COMMAND xargs dirname
        COMMAND xargs echo -n
        OUTPUT_VARIABLE MATLAB_ROOT
        )
    ENDIF (MATLAB_BIN_EXISTS)     
  endif()

  # Check if this is a Mac
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    set(LIBRARY_EXTENSION .dylib)

    # If this is a Mac and the attempts to find MATLAB_ROOT have so far failed,
    # we look in the applications folder
    if((NOT DEFINED MATLAB_ROOT) OR ("${MATLAB_ROOT}" STREQUAL ""))

    # Search for a version of Matlab available, starting from the most modern one to older versions
      foreach(MATVER "R2013b" "R2013a" "R2012b" "R2012a" "R2011b" "R2011a" "R2010b" "R2010a" "R2009b" "R2009a" "R2008b")
        if((NOT DEFINED MATLAB_ROOT) OR ("${MATLAB_ROOT}" STREQUAL ""))
          if(EXISTS /Applications/MATLAB_${MATVER}.app)
            set(MATLAB_ROOT /Applications/MATLAB_${MATVER}.app)
          endif()
        endif()
      endforeach()
    endif()

  else()
    set(LIBRARY_EXTENSION .so)
  endif()

  # Get path to the MEX libraries
  execute_process(
    #COMMAND find "${MATLAB_ROOT}/extern/lib" -name libmex${LIBRARY_EXTENSION} # Peter
    COMMAND find "${MATLAB_ROOT}/bin" -name libmex${LIBRARY_EXTENSION} # standard
    COMMAND xargs echo -n
    OUTPUT_VARIABLE MATLAB_MEX_LIBRARY
    )
  execute_process(
    #COMMAND find "${MATLAB_ROOT}/extern/lib" -name libmx${LIBRARY_EXTENSION} # Peter
    COMMAND find "${MATLAB_ROOT}/bin" -name libmx${LIBRARY_EXTENSION} # Standard
    COMMAND xargs echo -n
    OUTPUT_VARIABLE MATLAB_MX_LIBRARY
    )
  execute_process(
    #COMMAND find "${MATLAB_ROOT}/extern/lib" -name libmat${LIBRARY_EXTENSION} # Peter
    COMMAND find "${MATLAB_ROOT}/bin" -name libmat${LIBRARY_EXTENSION} # Standard
    COMMAND xargs echo -n
    OUTPUT_VARIABLE MATLAB_MAT_LIBRARY
    )
  execute_process(
    #COMMAND find "${MATLAB_ROOT}/extern/lib" -name libeng${LIBRARY_EXTENSION} # Peter
    COMMAND find "${MATLAB_ROOT}/bin" -name libeng${LIBRARY_EXTENSION} # Standard
    COMMAND xargs echo -n
    OUTPUT_VARIABLE MATLAB_ENG_LIBRARY
    )

  # Get path to the include directory
  find_path(MATLAB_INCLUDE_DIR
    "mex.h"
    PATHS "${MATLAB_ROOT}/extern/include"
    )

  find_program( MATLAB_MEX_PATH mex
             HINTS ${MATLAB_ROOT}/bin
             PATHS ${MATLAB_ROOT}/bin
             DOC "The mex program path"
            )

  find_program( MATLAB_MEXEXT_PATH mexext
             HINTS ${MATLAB_ROOT}/bin
             PATHS ${MATLAB_ROOT}/bin
             DOC "The mexext program path"
            )

  execute_process(
        COMMAND ${MATLAB_MEXEXT_PATH}
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE MATLAB_MEX_EXT
    )

endif()

# This is common to UNIX and Win32:
set(MATLAB_LIBRARIES
  ${MATLAB_MEX_LIBRARY}
  ${MATLAB_MX_LIBRARY}
  ${MATLAB_ENG_LIBRARY}
)

if(MATLAB_INCLUDE_DIR AND MATLAB_LIBRARIES)
  set(MATLAB_FOUND TRUE)
endif()

# 32-bit or 64-bit mex
if(WIN32)
  if (CMAKE_CL_64)
      SET(MATLAB_MEX_EXTENSION .mexw64)
  else(CMAKE_CL_64)
      SET(MATLAB_MEX_EXTENSION .mexw32)
  endif(CMAKE_CL_64)
else(WIN32)
  if (CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET(MATLAB_MEX_EXTENSION .mexa64)
  else(CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET(MATLAB_MEX_EXTENSION .mexglx)
  endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
endif(WIN32)

SET(MATLAB_MEX_CFLAGS "-DMATLAB_MEX_FILE -DMX_COMPAT_32")

mark_as_advanced(
  MATLAB_LIBRARIES
  MATLAB_MEX_LIBRARY
  MATLAB_MX_LIBRARY
  MATLAB_ENG_LIBRARY
  MATLAB_INCLUDE_DIR
  MATLAB_FOUND
  MATLAB_ROOT
  MATLAB_MEX_PATH
  MATLAB_MEXEXT_PATH
  MATLAB_MEX_EXT
)

