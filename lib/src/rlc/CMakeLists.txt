#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES rlc.cc
            rlc_tm.cc
            rlc_um_base.cc
            rlc_um_lte.cc
            rlc_um_nr.cc
            rlc_am_base.cc
            rlc_am_lte.cc
            rlc_am_nr.cc
            rlc_am_lte_packing.cc
            rlc_am_nr_packing.cc
            bearer_mem_pool.cc)

add_library(srsran_rlc STATIC ${SOURCES})
target_link_libraries(srsran_rlc srsran_common ${ATOMIC_LIBS})
install(TARGETS srsran_rlc DESTINATION ${LIBRARY_DIR} OPTIONAL)
