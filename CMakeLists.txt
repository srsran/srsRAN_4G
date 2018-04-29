#
# Copyright 2013-2017 Software Radio Systems Limited
#
# This file is part of srsLTE
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#


########################################################################
# Prevent in-tree builds
########################################################################
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. This is bad practice.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})


########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.6)
project( SRSLTE )
message( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )
message( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR} )
message( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} )

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")
include(SRSLTEVersion) #sets version information
include(SRSLTEPackage) #setup cpack

include(CTest)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake"
    IMMEDIATE @ONLY)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
   message(STATUS "Build type not specified: defaulting to Release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

# Generate CMake to include build information
configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/modules/SRSLTEbuildinfo.cmake.in
  ${CMAKE_BINARY_DIR}/SRSLTEbuildinfo.cmake
)

########################################################################
# Options
########################################################################
option(ENABLE_SRSUE    "Build srsUE application"                  ON)
option(ENABLE_SRSENB   "Build srsENB application"                 ON)
option(ENABLE_SRSEPC   "Build srsEPC application"                 ON)
option(DISABLE_SIMD    "disable simd instructions"                OFF)

option(ENABLE_GUI      "Enable GUI (using srsGUI)"                ON)
option(ENABLE_BLADERF  "Enable BladeRF"                           ON)

option(BUILD_STATIC    "Attempt to statically link external deps" OFF)
option(RPATH           "Enable RPATH"                             OFF)
option(ENABLE_ASAN     "Enable gcc address sanitizer"             OFF)

option(USE_LTE_RATES   "Use standard LTE sampling rates"          OFF)

set(GCC_ARCH native CACHE STRING "GCC compile for specific architecture.")


########################################################################
# Find dependencies
########################################################################

# Threads
find_package(Threads REQUIRED)

# FFT
find_package(MKL)
if(MKL_FOUND)
  include_directories(${MKL_INCLUDE_DIRS})
  link_directories(${MKL_LIBRARY_DIRS})
  set(FFT_LIBRARIES "${MKL_STATIC_LIBRARIES}") # Static by default
else(MKL_FOUND)
  find_package(FFTW3F REQUIRED)
  if(FFTW3F_FOUND)
    include_directories(${FFTW3F_INCLUDE_DIRS})
    link_directories(${FFTW3F_LIBRARY_DIRS})
    if(BUILD_STATIC)
      set(FFT_LIBRARIES "${FFTW3F_STATIC_LIBRARIES}")
    else(BUILD_STATIC)
      set(FFT_LIBRARIES "${FFTW3F_LIBRARIES}")
    endif(BUILD_STATIC)
    message(STATUS "FFT_LIBRARIES: " ${FFT_LIBRARIES})
  endif(FFTW3F_FOUND)
endif(MKL_FOUND)

# Crypto
find_package(Polarssl)
if (POLARSSL_FOUND)
  set(SEC_INCLUDE_DIRS "${POLARSSL_INCLUDE_DIRS}")
  if(BUILD_STATIC)
    set(SEC_LIBRARIES    "${POLARSSL_STATIC_LIBRARIES}")
  else(BUILD_STATIC)
    set(SEC_LIBRARIES    "${POLARSSL_LIBRARIES}")
  endif(BUILD_STATIC)
  add_definitions(-DHAVE_POLARSSL)
else(POLARSSL_FOUND)
  find_package(MbedTLS REQUIRED)
  if (MBEDTLS_FOUND)
    set(SEC_INCLUDE_DIRS "${MBEDTLS_INCLUDE_DIRS}")
    if(BUILD_STATIC)
      set(SEC_LIBRARIES    "${MBEDTLS_STATIC_LIBRARIES}")
    else(BUILD_STATIC)
      set(SEC_LIBRARIES    "${MBEDTLS_LIBRARIES}")
    endif(BUILD_STATIC)
    add_definitions(-DHAVE_MBEDTLS)
  else(MBEDTLS_FOUND)
    message(FATAL_ERROR "Either PolarSSL or mbedTLS are required to build srsLTE")
  endif (MBEDTLS_FOUND)
endif(POLARSSL_FOUND)

# UHD
find_package(UHD)
if(UHD_FOUND)
  include_directories(${UHD_INCLUDE_DIRS})
  link_directories(${UHD_LIBRARY_DIRS})
endif(UHD_FOUND)

# BladeRF
if(ENABLE_BLADERF)
  find_package(bladeRF)
  if(BLADERF_FOUND)
    include_directories(${BLADERF_INCLUDE_DIRS})
    link_directories(${BLADERF_LIBRARY_DIRS})
  endif(BLADERF_FOUND)
endif(ENABLE_BLADERF)

# Soapy
find_package(SoapySDR)
if(SOAPYSDR_FOUND)
  include_directories(${SOAPYSDR_INCLUDE_DIRS})
  link_directories(${SOAPYSDR_LIBRARY_DIRS})
endif(SOAPYSDR_FOUND)

if(BLADERF_FOUND OR UHD_FOUND OR SOAPYSDR_FOUND)
  set(RF_FOUND TRUE CACHE INTERNAL "RF frontend found")
else(BLADERF_FOUND OR UHD_FOUND OR SOAPYSDR_FOUND)
  set(RF_FOUND FALSE CACHE INTERNAL "RF frontend found")
  add_definitions(-DDISABLE_RF)
endif(BLADERF_FOUND OR UHD_FOUND OR SOAPYSDR_FOUND)

# Boost
if(ENABLE_SRSUE OR ENABLE_SRSENB OR ENABLE_SRSEPC)
  if(BUILD_STATIC)
    set(Boost_USE_STATIC_LIBS ON)
  endif(BUILD_STATIC)

  set(BOOST_REQUIRED_COMPONENTS
      program_options
  )
  if(UNIX AND EXISTS "/usr/lib64")
      list(APPEND BOOST_LIBRARYDIR "/usr/lib64") #fedora 64-bit fix
  endif(UNIX AND EXISTS "/usr/lib64")
  set(Boost_ADDITIONAL_VERSIONS
      "1.35.0" "1.35" "1.36.0" "1.36" "1.37.0" "1.37" "1.38.0" "1.38" "1.39.0" "1.39"
      "1.40.0" "1.40" "1.41.0" "1.41" "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44"
      "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47" "1.48.0" "1.48" "1.49.0" "1.49"
      "1.50.0" "1.50" "1.51.0" "1.51" "1.52.0" "1.52" "1.53.0" "1.53" "1.54.0" "1.54"
      "1.55.0" "1.55" "1.56.0" "1.56" "1.57.0" "1.57" "1.58.0" "1.58" "1.59.0" "1.59"
      "1.60.0" "1.60" "1.61.0" "1.61" "1.62.0" "1.62" "1.63.0" "1.63" "1.64.0" "1.64"
      "1.65.0" "1.65" "1.66.0" "1.66" "1.67.0" "1.67" "1.68.0" "1.68" "1.69.0" "1.69"
  )
  find_package(Boost "1.35" COMPONENTS ${BOOST_REQUIRED_COMPONENTS})
endif(ENABLE_SRSUE OR ENABLE_SRSENB OR ENABLE_SRSEPC)

# srsGUI
if(ENABLE_GUI)
  find_package(SRSGUI)
  if(SRSGUI_FOUND)
    add_definitions(-DENABLE_GUI)
    include_directories(${SRSGUI_INCLUDE_DIRS})
    link_directories(${SRSGUI_LIBRARY_DIRS})
  endif(SRSGUI_FOUND)
endif(ENABLE_GUI)

########################################################################
# Install Dirs
########################################################################
if (NOT CMAKE_INSTALL_LIBDIR)
    include(GNUInstallDirs)
endif (NOT CMAKE_INSTALL_LIBDIR)

# Fall back to just "lib" if the item provided by GNUInstallDirs doesn't exist
if (NOT EXISTS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    message(STATUS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR} does not exist. Defaulting install location to ${CMAKE_INSTALL_PREFIX}/lib.")
    set(CMAKE_INSTALL_LIBDIR lib)
endif()

set(RUNTIME_DIR bin)
set(LIBRARY_DIR ${CMAKE_INSTALL_LIBDIR})
set(INCLUDE_DIR include)
set(DOC_DIR "share/doc/${CPACK_PACKAGE_NAME}")
set(DATA_DIR share/${CPACK_PACKAGE_NAME})

########################################################################
# Compiler specific setup
########################################################################
macro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE flag have)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(${flag} ${have})
    if(${have})
        add_definitions(${flag})        
    endif(${have})
endmacro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=${GCC_ARCH} -Wall -Wno-comment -Wno-reorder -Wno-unused-but-set-variable -Wno-unused-variable -Wformat -Wtype-limits -std=c++03")

  find_package(SSE)
  if (HAVE_AVX2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -mavx2 -DLV_HAVE_AVX2 -DLV_HAVE_AVX -DLV_HAVE_SSE")
  else (HAVE_AVX2)
    if(HAVE_AVX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -mavx -DLV_HAVE_AVX -DLV_HAVE_SSE")
    elseif(HAVE_SSE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -msse4.1 -DLV_HAVE_SSE")
    endif(HAVE_AVX)
  endif (HAVE_AVX2)
endif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")

ADD_CXX_COMPILER_FLAG_IF_AVAILABLE("-Werror=incompatible-pointer-types" HAVE_ERROR_INCOMPATIBLE)

if(CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=${GCC_ARCH} -Wall -Wno-comment -Wno-write-strings -Winline -Wno-unused-result -Wformat -Wtype-limits -std=c99 -D_GNU_SOURCE")

  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -O0 -DDEBUG_MODE -DBUILD_TYPE_DEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -O0 -DDEBUG_MODE -DBUILD_TYPE_DEBUG")
  else(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    if(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -DBUILD_TYPE_RELWITHDEBINFO")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -DBUILD_TYPE_RELWITHDEBINFO")
    else(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -DBUILD_TYPE_RELEASE")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DBUILD_TYPE_RELEASE")
    endif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
  endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

  if (USE_LTE_RATES)
    message(STATUS "Using standard LTE sampling rates")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFORCE_STANDARD_RATE")
  endif (USE_LTE_RATES)

  find_package(SSE)
  if (HAVE_AVX2)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpmath=sse -mavx2 -DLV_HAVE_AVX2 -DLV_HAVE_AVX -DLV_HAVE_SSE")
  else (HAVE_AVX2)
    if(HAVE_AVX)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpmath=sse -mavx -DLV_HAVE_AVX -DLV_HAVE_SSE")
    elseif(HAVE_SSE)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpmath=sse -msse4.1 -DLV_HAVE_SSE")
    endif(HAVE_AVX)
  endif (HAVE_AVX2)

  if (HAVE_AVX512)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx512f -mavx512cd -DLV_HAVE_AVX512")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512f -mavx512cd -DLV_HAVE_AVX512")
  endif(HAVE_AVX512)

  if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    if(HAVE_SSE)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast -funroll-loops")
    endif(HAVE_SSE)
  endif(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")

  
  if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -mfpu=neon -march=native -DIS_ARM -DHAVE_NEON")
    message(STATUS "have ARM")
    set(HAVE_NEON "True")
  else(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set(HAVE_NEON "False")
  endif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
  set(CMAKE_REQUIRED_FLAGS ${CMAKE_C_FLAGS})

 if(NOT HAVE_SSE AND NOT HAVE_NEON AND NOT DISABLE_SIMD)
  message(FATAL_ERROR "no SIMD instructions found")
 endif(NOT HAVE_SSE AND NOT HAVE_NEON AND NOT DISABLE_SIMD)

  if(NOT WIN32)
      ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN)
  endif(NOT WIN32)
 if (ENABLE_ASAN)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
 endif (ENABLE_ASAN)
endif(CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   # The following is needed for weak linking to work under OS X
   set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

message(STATUS "CMAKE_C_FLAGS is ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")

########################################################################
# Create uninstall targets
########################################################################
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

########################################################################
# Add -fPIC property to all targets
########################################################################
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION}")

########################################################################
# Add general includes and dependencies
########################################################################
include_directories(${PROJECT_BINARY_DIR}/lib/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/include)

########################################################################
# Add headers to cmake project (useful for IDEs)
########################################################################
set(HEADERS_ALL "")
file(GLOB headers *)
foreach(_header ${headers})
    if(IS_DIRECTORY ${_header})
        file(GLOB_RECURSE tmp "${_header}/*.h")
        list(APPEND HEADERS_ALL ${tmp})
    endif(IS_DIRECTORY ${_header})
endforeach()
add_custom_target(add_srslte_headers SOURCES ${HEADERS_ALL})

########################################################################
# Add the subdirectories
########################################################################
add_subdirectory(lib)

if(RF_FOUND)
  if(ENABLE_SRSUE)
    message(STATUS "Building with srsUE")
    add_subdirectory(srsue)
  else(ENABLE_SRSUE)
    message(STATUS "srsUE build disabled")
  endif(ENABLE_SRSUE)

  if(ENABLE_SRSENB)
    message(STATUS "Building with srsENB")
    add_subdirectory(srsenb)
  else(ENABLE_SRSENB)
    message(STATUS "srsENB build disabled")
  endif(ENABLE_SRSENB)
else(RF_FOUND)
  message(STATUS "srsUE and srsENB builds disabled due to missing RF driver")
endif(RF_FOUND)

if(ENABLE_SRSEPC)
  message(STATUS "Building with srsEPC")
  add_subdirectory(srsepc)
else(ENABLE_SRSEPC)
  message(STATUS "srsEPC build disabled")
endif(ENABLE_SRSEPC)
