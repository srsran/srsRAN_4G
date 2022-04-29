#
# Copyright 2013-2022 Software Radio Systems Limited
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

message(STATUS "FINDING ZEROMQ.")
if(NOT ZEROMQ_FOUND)
    pkg_check_modules (ZEROMQ_PKG ZeroMQ)

    find_path(ZEROMQ_INCLUDE_DIRS
            NAMES zmq.h
            PATHS ${ZEROMQ_PKG_INCLUDE_DIRS}
            /usr/include/zmq
            /usr/local/include/zmq
            )

    find_library(ZEROMQ_LIBRARIES
            NAMES zmq
            PATHS ${ZEROMQ_PKG_LIBRARY_DIRS}
            /usr/lib
            /usr/local/lib
            /usr/lib/arm-linux-gnueabihf
            )

    if(ZEROMQ_INCLUDE_DIRS AND ZEROMQ_LIBRARIES)
        set(ZEROMQ_FOUND TRUE CACHE INTERNAL "libZEROMQ found")
        message(STATUS "Found libZEROMQ: ${ZEROMQ_INCLUDE_DIRS}, ${ZEROMQ_LIBRARIES}")
    else(ZEROMQ_INCLUDE_DIRS AND ZEROMQ_LIBRARIES)
        set(ZEROMQ_FOUND FALSE CACHE INTERNAL "libZEROMQ found")
        message(STATUS "libZEROMQ not found.")
    endif(ZEROMQ_INCLUDE_DIRS AND ZEROMQ_LIBRARIES)

    mark_as_advanced(ZEROMQ_LIBRARIES ZEROMQ_INCLUDE_DIRS)

endif(NOT ZEROMQ_FOUND)
