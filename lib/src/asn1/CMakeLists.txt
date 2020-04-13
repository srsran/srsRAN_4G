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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch -Wno-unused-variable -Wno-return-type -Wno-sign-compare -Wno-reorder -Wno-parentheses -fno-exceptions")
add_library(srslte_asn1 STATIC
  liblte_common.cc
  liblte_mme.cc
  liblte_m2ap.cc
  gtpc.cc
)

# ASN1 utils
add_library(asn1_utils STATIC asn1_utils.cc)
target_link_libraries(asn1_utils srslte_common)

add_library(rrc_asn1 STATIC
        rrc_asn1.cc
        rrc_asn1_nbiot.cc
        rrc_asn1_enum.cc
        rrc_asn1_utils.cc
        )
# Compile RRC ASN1 optimized for size
target_compile_options(rrc_asn1 PRIVATE "-Os")
target_link_libraries(rrc_asn1 asn1_utils srslte_common)

add_library(s1ap_asn1 STATIC
        s1ap_asn1.cc)
target_compile_options(s1ap_asn1 PRIVATE "-Os")
target_link_libraries(s1ap_asn1 asn1_utils srslte_common)

if (ENABLE_5GNR)
    # RRC NR ASN1
    add_library(rrc_nr_asn1 STATIC rrc_nr_asn1.cc)
    target_compile_options(rrc_nr_asn1 PRIVATE "-Os")
    target_link_libraries(rrc_nr_asn1 asn1_utils srslte_common)

    # NGAP ASN1
    add_library(ngap_nr_asn1 STATIC ngap_nr_asn1.cc)
    target_compile_options(ngap_nr_asn1 PRIVATE "-Os")
    target_link_libraries(ngap_nr_asn1 asn1_utils srslte_common)
endif(ENABLE_5GNR)


