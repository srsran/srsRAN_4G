# Install script for directory: /home/pi/srsRAN/lib/src/phy

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/pi/srsRAN/build/lib/src/phy/libsrslte_phy.a")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/pi/srsRAN/build/lib/src/phy/agc/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/ch_estimation/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/common/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/fec/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/mimo/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/phch/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/rf/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/sync/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/utils/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/channel/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/dft/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/io/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/modem/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/resampling/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/scrambling/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/ue/cmake_install.cmake")
  include("/home/pi/srsRAN/build/lib/src/phy/enb/cmake_install.cmake")

endif()

