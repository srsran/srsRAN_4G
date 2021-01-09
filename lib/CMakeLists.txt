#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(examples)
add_subdirectory(test)

########################################################################
# Install library headers
########################################################################
INSTALL(  DIRECTORY include/
          DESTINATION "${INCLUDE_DIR}"
          FILES_MATCHING PATTERN "*.h" )