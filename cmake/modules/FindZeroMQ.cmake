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
