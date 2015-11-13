#
# Copyright 2013-2015 Software Radio Systems Limited
#
# This file is part of the srsLTE library.
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

if(NOT DisableMEX)
  ########################################################################
  # Install headers
  ########################################################################
  INSTALL(DIRECTORY include/
          DESTINATION "${INCLUDE_DIR}"
          FILES_MATCHING PATTERN "*.h"
          PATTERN ".svn" EXCLUDE
  )

  ########################################################################
  # Add headers to cmake project (useful for IDEs)
  ########################################################################
  SET(HEADERS_ALL "")
  FILE(GLOB headers *)
  FOREACH (_header ${headers})
          IF(IS_DIRECTORY ${_header})
                  FILE(GLOB_RECURSE tmp "${_header}/*.h")
                  LIST(APPEND HEADERS_ALL ${tmp})
          ENDIF(IS_DIRECTORY ${_header})
  ENDFOREACH()

  ADD_CUSTOM_TARGET (add_mex_headers SOURCES ${HEADERS_ALL})

  ########################################################################
  # Add the subdirectories
  ########################################################################
  ADD_SUBDIRECTORY(lib)
endif()
