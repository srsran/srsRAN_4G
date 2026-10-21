# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

find_package(LibConfig REQUIRED)
find_package(SCTP REQUIRED)

if(BUILD_STATIC)
  set(LIBCONFIGPP_LIBRARIES "${LIBCONFIGPP_STATIC_LIBRARY_PATH}")
endif(BUILD_STATIC)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost required to compile srsENB")
endif()

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${SEC_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}
)

link_directories(
    ${Boost_LIBRARY_DIRS}
    ${SEC_LIBRARY_DIRS}
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(src)
add_subdirectory(test)

########################################################################
# Default configuration files
########################################################################
install(FILES enb.conf.example DESTINATION ${DATA_DIR})
install(FILES rb.conf.example  DESTINATION ${DATA_DIR})
install(FILES rr.conf.example  DESTINATION ${DATA_DIR})
install(FILES sib.conf.example DESTINATION ${DATA_DIR})
