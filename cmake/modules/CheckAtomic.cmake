#
# Copyright 2013-2021 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

# Adopted from https://github.com/pothosware/SoapyRTLSDR
# Copyright: 2015, Charles J. Cliffe
# License: MIT

# - Try to find if atomics need -latomic linking
# Once done this will define
#  HAVE_CXX_ATOMICS_WITHOUT_LIB - Wether atomic types work without -latomic
#  HAVE_CXX_ATOMICS64_WITHOUT_LIB - Wether 64 bit atomic types work without -latomic

INCLUDE(CheckCXXSourceCompiles)
INCLUDE(CheckLibraryExists)

# Sometimes linking against libatomic is required for atomic ops, if
# the platform doesn't support lock-free atomics.

function(check_working_cxx_atomics varname)
set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11")
CHECK_CXX_SOURCE_COMPILES("
#include <atomic>
std::atomic<int> x;
int main() {
return std::atomic_is_lock_free(&x);
}
" ${varname})
set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})
endfunction(check_working_cxx_atomics)

function(check_working_cxx_atomics64 varname)
set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
set(CMAKE_REQUIRED_FLAGS "-std=c++11 ${CMAKE_REQUIRED_FLAGS}")
CHECK_CXX_SOURCE_COMPILES("
#include <atomic>
#include <cstdint>
std::atomic<uint64_t> x (0);
int main() {
uint64_t i = x.load(std::memory_order_relaxed);
return std::atomic_is_lock_free(&x);
}
" ${varname})
set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})
endfunction(check_working_cxx_atomics64)

# Check for atomic operations.
if(MSVC)
  # This isn't necessary on MSVC.
  set(HAVE_CXX_ATOMICS_WITHOUT_LIB True)
else()
  # First check if atomics work without the library.
  check_working_cxx_atomics(HAVE_CXX_ATOMICS_WITHOUT_LIB)
endif()

# If not, check if the library exists, and atomics work with it.
if(NOT HAVE_CXX_ATOMICS_WITHOUT_LIB)
  check_library_exists(atomic __atomic_fetch_add_4 "" HAVE_LIBATOMIC)
  if(NOT HAVE_LIBATOMIC)
    message(STATUS "Host compiler appears to require libatomic, but cannot locate it.")
  endif()
  list(APPEND CMAKE_REQUIRED_LIBRARIES "atomic")
  check_working_cxx_atomics(HAVE_CXX_ATOMICS_WITH_LIB)
  if (NOT HAVE_CXX_ATOMICS_WITH_LIB)
    message(FATAL_ERROR "Host compiler must support std::atomic!")
  endif()
endif()

# Check for 64 bit atomic operations.
if(MSVC)
  set(HAVE_CXX_ATOMICS64_WITHOUT_LIB True)
else()
  check_working_cxx_atomics64(HAVE_CXX_ATOMICS64_WITHOUT_LIB)
endif()

# If not, check if the library exists, and atomics work with it.
if(NOT HAVE_CXX_ATOMICS64_WITHOUT_LIB)
  check_library_exists(atomic __atomic_load_8 "" HAVE_LIBATOMIC64)
  if(NOT HAVE_LIBATOMIC64)
    message(STATUS "Host compiler appears to require libatomic, but cannot locate it.")
  endif()
  list(APPEND CMAKE_REQUIRED_LIBRARIES "atomic")
  check_working_cxx_atomics64(HAVE_CXX_ATOMICS64_WITH_LIB)
  if (NOT HAVE_CXX_ATOMICS64_WITH_LIB)
    message(FATAL_ERROR "Host compiler must support std::atomic!")
  endif()
endif()