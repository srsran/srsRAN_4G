#
# Copyright 2013-2020 Software Radio Systems Limited
#
# This file is part of srsLTE
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

add_library(polar_test_utils polar_sets.c subchannel_allocation.c)

add_executable(polar_chain_test polar_chain_test.c)

target_link_libraries(polar_chain_test srslte_phy polar_test_utils)

set_target_properties(polar_chain_test
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/tests/polar"
        )

add_custom_target(polar_frozen_sets
        DEPENDS ${OUT_FROZEN_SETS}
        )

add_dependencies(polar_chain_test polar_frozen_sets)

### Test polar libs
function(polar_unit_tests)
    set(S ${ARGV0})  #101 means no noise, 100 scan
    set(listC 5   6  6  6   7   7   8   8   9   9   10)
    set(listR 32 64 64 64 128 128 256 256 512 864 1024)
    set(listM 31 31 36 63  36  64  36 128 256  56  512)
    set(listP  0  0  0  0   0   0   0   0   0   0    0)
    set(listW  0  0  0  0   0   0   0   0   0   0    0)
    list(LENGTH listC len)
    math(EXPR lenr "${len} - 1")
    foreach(num RANGE ${lenr})
        list(GET listC ${num} cval)
        list(GET listR ${num} rval)
        list(GET listM ${num} mval)
        list(GET listP ${num} pval)
        list(GET listW ${num} wval)
        add_test(NAME ${test_name}-s${S}-c${cval}-r${rval}-m${mval}-p${pval}-w${wval}
                COMMAND ${test_command} -s${S} -c${cval} -r${rval} -m${mval} -p${pval} -w${wval}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
                )
    endforeach()
endfunction()

# Unit tests
set(test_name POLAR-UNIT-TEST)
set(test_command polar_chain_test)
polar_unit_tests(101)

# WER (performance) tests
# For these tests, run ctest --verbose
set(test_name POLAR-PERF-TEST)
set(test_command polar_chain_test)
polar_unit_tests(-3)
