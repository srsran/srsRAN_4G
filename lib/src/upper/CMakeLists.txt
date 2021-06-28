#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES gtpu.cc)

add_library(srsran_upper STATIC ${SOURCES})
target_link_libraries(srsran_upper srsran_common srsran_asn1 ${ATOMIC_LIBS})
INSTALL(TARGETS srsran_upper DESTINATION ${LIBRARY_DIR})
