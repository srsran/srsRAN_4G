#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# Boost is required 
########################################################################
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost required to compile srsUE and ")
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
install(FILES ue.conf.example DESTINATION ${DATA_DIR})