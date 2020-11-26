#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

find_package(LibConfig REQUIRED)
find_package(SCTP REQUIRED)

if(BUILD_STATIC)
  set(LIBCONFIGPP_LIBRARIES "${LIBCONFIGPP_STATIC_LIBRARY_PATH}")
endif(BUILD_STATIC)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost required to compile srsEPC")
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

########################################################################
# Default configuration files
########################################################################
install(FILES epc.conf.example DESTINATION ${DATA_DIR})
install(FILES mbms.conf.example DESTINATION ${DATA_DIR})
install(FILES user_db.csv.example DESTINATION ${DATA_DIR})
install(PROGRAMS srsepc_if_masq.sh DESTINATION ${RUNTIME_DIR})