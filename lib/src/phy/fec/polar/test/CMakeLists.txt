#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(CTEST_LABELS "lib;phy;fec;polar")

add_library(polar_test_utils polar_sets.c subchannel_allocation.c)

add_executable(polar_chain_test polar_chain_test.c)

target_link_libraries(polar_chain_test srsran_phy polar_test_utils)

### Test polar libs
function(polar_tests_lite)
    set(S ${ARGV0})  #101 means no noise, 100 scan
    set(listN 10 10  9  9   9   9   9   9  10   9   10)
    set(listE 32 64 64 64 128 128 256 256 512 864 1024)
    set(listK 31 31 36 63  36  64  36 128 256  56  512)
    set(listI  0  0  0  0   0   0   0   0   1   0    1)
    list(LENGTH listN len)
    math(EXPR lenr "${len} - 1")
    foreach(num RANGE ${lenr})
        list(GET listN ${num} nval)
        list(GET listE ${num} eval)
        list(GET listK ${num} kval)
        list(GET listI ${num} ival)
        add_nr_advanced_test(NAME ${test_name}-s${S}-n${nval}-e${eval}-k${kval}-i${ival}
                COMMAND ${test_command} -s${S} -n${nval} -e${eval} -k${kval} -i${ival}
                )
    endforeach()
endfunction()

### Test polar libs
function(polar_tests)
    set(S ${ARGV0})  #101 means no noise, 100 scan
    #Downlink tests
    set(nval 9)
    foreach(Kval RANGE 36 164 32)
        math(EXPR Emin "${Kval} + 1")
        foreach(Eval RANGE ${Emin} 8192 128)
            add_nr_advanced_test(NAME ${test_name}-s${S}-k${Kval}-e${Eval}-n${nval}-i0
                    COMMAND ${test_command} -s${S} -k${Kval} -e${Eval} -n${nval} -i0
                    )
        endforeach()
    endforeach()


    #Uplink tests
    set(nval 10)
    foreach(Kval RANGE 18 25)
        math(EXPR Emin "${Kval} + 3 + 1")
        foreach(Eval RANGE ${Emin} 8192 128)
            add_nr_advanced_test(NAME ${test_name}-s${S}-k${Kval}-e${Eval}-n${nval}-i1
                    COMMAND ${test_command} -s${S} -k${Kval} -e${Eval} -n${nval} -i1
                    )
        endforeach()
    endforeach()

    foreach(Kval RANGE 32 1023 32)
        math(EXPR Emin "${Kval} + 1")
        foreach(Eval RANGE ${Emin} 8192 128)
            add_nr_advanced_test(NAME ${test_name}-s${S}-k${Kval}-e${Eval}-n${nval}-i1
                    COMMAND ${test_command} -s${S} -k${Kval} -e${Eval} -n${nval} -i1
                    )
        endforeach()
    endforeach()


endfunction()



# Unit tests
set(test_name POLAR-UNIT-TEST-LITE)
set(test_command polar_chain_test)
polar_tests_lite(101)

# WER (performance) tests
set(test_name POLAR-PERF-TEST)
set(test_command polar_chain_test)
polar_tests_lite(-3)

# Unit tests full
set(test_name POLAR-UNIT-TEST)
set(test_command polar_chain_test)
polar_tests(101)

# Polar inter-leaver test
add_executable(polar_interleaver_test polar_interleaver_test.c)
target_link_libraries(polar_interleaver_test srsran_phy)
add_nr_test(polar_interleaver_test polar_interleaver_test)
