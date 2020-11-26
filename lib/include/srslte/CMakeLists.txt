#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.h
        DESTINATION "${INCLUDE_DIR}/srslte"
)
