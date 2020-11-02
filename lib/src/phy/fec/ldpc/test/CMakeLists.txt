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

add_executable(ldpc_enc_test ldpc_enc_test.c)
target_link_libraries(ldpc_enc_test srslte_phy)

add_executable(ldpc_dec_test ldpc_dec_test.c)
target_link_libraries(ldpc_dec_test srslte_phy)

add_executable(ldpc_dec_s_test ldpc_dec_s_test.c)
target_link_libraries(ldpc_dec_s_test srslte_phy)

add_executable(ldpc_dec_c_test ldpc_dec_c_test.c)
target_link_libraries(ldpc_dec_c_test srslte_phy)

add_executable(ldpc_chain_test ldpc_chain_test.c)
target_link_libraries(ldpc_chain_test srslte_phy)

add_executable(ldpc_rm_test ldpc_rm_test.c)
target_link_libraries(ldpc_rm_test srslte_phy)

add_executable(ldpc_rm_chain_test ldpc_rm_chain_test.c)
target_link_libraries(ldpc_rm_chain_test srslte_phy)

set_target_properties(ldpc_enc_test ldpc_dec_test ldpc_dec_s_test ldpc_dec_c_test ldpc_chain_test ldpc_rm_test ldpc_rm_chain_test
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/tests/ldpc"
)

if(NOT DISABLE_SIMD)
  add_executable(ldpc_enc_avx2_test ldpc_enc_avx2_test.c)
  target_link_libraries(ldpc_enc_avx2_test srslte_phy)

  add_executable(ldpc_dec_avx2_test ldpc_dec_avx2_test.c)
  target_link_libraries(ldpc_dec_avx2_test srslte_phy)

  set_target_properties(ldpc_dec_avx2_test ldpc_enc_avx2_test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/tests/ldpc"
  )
endif(NOT DISABLE_SIMD)

add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/tests/ldpc/examplesBG1.dat
         ${PROJECT_SOURCE_DIR}/tests/ldpc/examplesBG2.dat
  COMMAND cp examplesBG?.dat ${PROJECT_SOURCE_DIR}/tests/ldpc
  DEPENDS examplesBG1.dat examplesBG2.dat
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Copying LDPC test reference files"
  VERBATIM
)

add_custom_target(ldpc_test_files
  DEPENDS ${PROJECT_SOURCE_DIR}/tests/ldpc/examplesBG1.dat
          ${PROJECT_SOURCE_DIR}/tests/ldpc/examplesBG2.dat
)

add_dependencies(ldpc_dec_test ldpc_test_files)
add_dependencies(ldpc_enc_test ldpc_test_files)
add_dependencies(ldpc_rm_test ldpc_test_files)

### Test LDPC libs
function(ldpc_unit_tests)
  foreach(i IN LISTS ARGN)
    add_test(NAME ${test_name}-LS${i} COMMAND ${test_command} -l${i}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests/ldpc
            )
  endforeach()
endfunction()

set(lifting_sizes
        2 4 8 16 32 64 128 256
        3 6 12 24 48 96 192 384
        5 10 20 40 80 160 320
        7 14 28 56 112 224
        9 18 36 72 144 288
        11 22 44 88 176 352
        13 26 52 104 208
        15 30 60 120 240
        )

set(test_name LDPC-ENC-BG1)
set(test_command ldpc_enc_test -b1)
ldpc_unit_tests(${lifting_sizes})

set(test_name LDPC-ENC-BG2)
set(test_command ldpc_enc_test -b2)
ldpc_unit_tests(${lifting_sizes})

set(test_name LDPC-DEC-BG1)
set(test_command ldpc_dec_test -b1)
ldpc_unit_tests(${lifting_sizes})

set(test_name LDPC-DEC-BG2)
set(test_command ldpc_dec_test -b2)
ldpc_unit_tests(${lifting_sizes})

add_test(NAME LDPC-chain COMMAND ldpc_chain_test)

### Test LDPC Rate Matching UNIT tests
set(mod_order
        1 2 4 6 8
        )
function(ldpc_rm_unit_tests)
  #foreach(j IN LIST ${ARGV0})
  set(listMod  0  1  2  3  4)
  set(listModord 1 2 4 6 8)
  set(listrv 0 1 2 3)
  set(listbg 1 2)
  set(listbaseN 66 50)
  set(listbaseK 22 10)
  list(LENGTH listMod modlen)
  list(LENGTH listrv rvlen)
  math(EXPR modlen "${modlen} - 1")
  math(EXPR rvlen "${rvlen} - 1")
  foreach(i IN LISTS ARGN)

    foreach(numbg RANGE ${bglen}) #bg
      foreach(numrv RANGE ${rvlen}) #rv
        foreach(nummod RANGE ${modlen})
          list(GET listbaseN ${numbg} baseNval)
          list(GET listbaseK ${numbg} baseKval)
          list(GET listbg ${numbg} bgval)
          math(EXPR N "${i} * ${baseNval}")
          math(EXPR K "${i} * ${baseKval}")

          list(GET listMod ${nummod} Modval)
          list(GET listModord ${nummod} Ordval)
          list(GET listrv ${numrv} rvval)

          math(EXPR Div "${Ordval}")
          math(EXPR tmpN "${N} - (${N} % ${Div})")
          math(EXPR E "${Ordval}*(${tmpN})/${Div}") #twice the rate

          add_test(NAME ${test_name}-b${bgval}-l${i}-e${E}-f10-m${Modval}-r${rvval}-M${N} COMMAND ${test_command} -b${bgval} -l${i} -e${E} -f10 -m${Modval} -r${rvval} -M${N}
                  WORKING_DIRECTORY ${PROJECT_S${cval}OURCE_DIR}/tests/ldpc
                  )
          math(EXPR M "${N} / 2")
          # Half size buffer
          add_test(NAME ${test_name}-b${bgval}-l${i}-e${E}-f10-m${Modval}-r${rvval}-M${M} COMMAND ${test_command} -b${bgval} -l${i} -e${E} -f10 -m${Modval} -r${rvval} -M${M}
                  WORKING_DIRECTORY ${PROJECT_S${cval}OURCE_DIR}/tests/ldpc
                  )
          math(EXPR Div "2*${Ordval}")
          math(EXPR tmpN "${N} - (${N} % ${Div})")
          math(EXPR E "${Ordval}*(${tmpN})/${Div}") #twice the rate
          add_test(NAME ${test_name}-b${bgval}-l${i}-e${E}-f10-m${Modval}-r${rvval}-M${N} COMMAND ${test_command} -b${bgval} -l${i} -e${E} -f10 -m${Modval} -r${rvval} -M${N}
                  WORKING_DIRECTORY ${PROJECT_S${cval}OURCE_DIR}/tests/ldpc
                  )
          math(EXPR M "${N}/ 2")
          # Half size buffer
          add_test(NAME ${test_name}-b${bgval}-l${i}-e${E}-f10-m${Modval}-r${rvval}-M${M} COMMAND ${test_command} -b${bgval} -l${i} -e${E} -f10 -m${Modval} -r${rvval} -M${M}
                  WORKING_DIRECTORY ${PROJECT_S${cval}OURCE_DIR}/tests/ldpc
                  )

          math(EXPR Div "${Ordval}")
          math(EXPR tmpN "2*${N} - (2*${N} % ${Div})") #Half the rate
          math(EXPR E "${Ordval}*(${tmpN})/${Div}")
          add_test(NAME ${test_name}-b${bgval}-l${i}-e${E}-f10-m${Modval}-r${rvval}-M${N} COMMAND ${test_command} -b${bgval} -l${i} -e${E} -f10 -m${Modval} -r${rvval} -M${N}

                  WORKING_DIRECTORY ${PROJECT_S${cval}OURCE_DIR}/tests/ldpc
                  )
          math(EXPR M "${N}/ 2")
          # Half size buffer
          add_test(NAME ${test_name}-b${bgval}-l${i}-e${E}-f10-m${Modval}-r${rvval}-M${M} COMMAND ${test_command} -b${bgval} -l${i} -e${E} -f10 -m${Modval} -r${rvval} -M${M}
                  WORKING_DIRECTORY ${PROJECT_S${cval}OURCE_DIR}/tests/ldpc
                  )
        endforeach()
      endforeach()
    endforeach()
  endforeach()
endfunction()
set(test_name LDPC-RM-BG1)
set(test_command ldpc_rm_test)
ldpc_rm_unit_tests(${lifting_sizes})

add_test(NAME LDPC-RM-chain COMMAND ldpc_rm_chain_test)