#
# Copyright 2012-2013 The libLTE Developers. See the
# COPYRIGHT file at the top-level directory of this distribution.
#
# This file is part of the libLTE library.
#
# libLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# libLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# A copy of the GNU Lesser General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

########################################################################
# BCCH packing TEST  
########################################################################

ADD_EXECUTABLE(bcch_bch_test bcch_bch_test.c)
TARGET_LINK_LIBRARIES(bcch_bch_test lte_rrc rrc_asn )

ADD_TEST(bcch_bch_test_1 bcch_bch_test) 

ADD_EXECUTABLE(bcch_dlsch_test bcch_dlsch_test.c)
TARGET_LINK_LIBRARIES(bcch_dlsch_test lte_rrc rrc_asn )

ADD_TEST(bcch_dlsch_test_1 bcch_dlsch_test) 
