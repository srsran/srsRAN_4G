#
# Copyright 2013-2017 Software Radio Systems Limited
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

########################################################################
# LAYER MAPPING TEST  
########################################################################

add_executable(layermap_test layermap_test.c)
target_link_libraries(layermap_test srslte_phy)

add_test(layermap_single layermap_test -n 1000 -m single -c 1 -l 1) 

add_test(layermap_diversity_2 layermap_test -n 1000 -m diversity -c 1 -l 2) 
add_test(layermap_diversity_4 layermap_test -n 1000 -m diversity -c 1 -l 4) 

add_test(layermap_multiplex_11 layermap_test -n 1000 -m multiplex -c 1 -l 1) 
add_test(layermap_multiplex_12 layermap_test -n 1000 -m multiplex -c 1 -l 2) 
add_test(layermap_multiplex_13 layermap_test -n 1002 -m multiplex -c 1 -l 3) 
add_test(layermap_multiplex_14 layermap_test -n 1000 -m multiplex -c 1 -l 4) 


add_test(layermap_multiplex_22 layermap_test -n 1000 -m multiplex -c 2 -l 2) 
add_test(layermap_multiplex_23 layermap_test -n 1002 -m multiplex -c 2 -l 3) 
add_test(layermap_multiplex_24 layermap_test -n 1000 -m multiplex -c 2 -l 4)
 
 
########################################################################
# PRECODING MAPPING TEST
########################################################################
 
add_executable(precoding_test precoder_test.c)
target_link_libraries(precoding_test srslte_phy)

add_test(precoding_single precoding_test -n 1000 -m single) 
add_test(precoding_diversity2 precoding_test -n 1000 -m diversity -l 2 -p 2) 
add_test(precoding_diversity4 precoding_test -n 1024 -m diversity -l 4 -p 4) 

add_test(precoding_cdd_2x2_zf precoding_test -m cdd -l 2 -p 2 -r 2 -n 14000 -d zf)
add_test(precoding_cdd_2x2_mmse precoding_test -m cdd -l 2 -p 2 -r 2 -n 14000 -d mmse)

add_test(precoding_multiplex_1l_cb0 precoding_test -m multiplex -l 1 -p 2 -r 2 -n 14000 -c 0)
add_test(precoding_multiplex_1l_cb1 precoding_test -m multiplex -l 1 -p 2 -r 2 -n 14000 -c 1)
add_test(precoding_multiplex_1l_cb2 precoding_test -m multiplex -l 1 -p 2 -r 2 -n 14000 -c 2)
add_test(precoding_multiplex_1l_cb3 precoding_test -m multiplex -l 1 -p 2 -r 2 -n 14000 -c 3)

add_test(precoding_multiplex_2l_cb0_zf precoding_test -m multiplex -l 2 -p 2 -r 2 -n 14000 -c 0 -d zf)
add_test(precoding_multiplex_2l_cb1_zf precoding_test -m multiplex -l 2 -p 2 -r 2 -n 14000 -c 1 -d zf)
add_test(precoding_multiplex_2l_cb2_zf precoding_test -m multiplex -l 2 -p 2 -r 2 -n 14000 -c 2 -d zf)


add_test(precoding_multiplex_2l_cb0_mmse precoding_test -m multiplex -l 2 -p 2 -r 2 -n 14000 -c 0 -d mmse)
add_test(precoding_multiplex_2l_cb1_mmse precoding_test -m multiplex -l 2 -p 2 -r 2 -n 14000 -c 1 -d mmse)
add_test(precoding_multiplex_2l_cb2_mmse precoding_test -m multiplex -l 2 -p 2 -r 2 -n 14000 -c 2 -d mmse)

########################################################################
# PMI SELECT TEST
########################################################################

add_executable(pmi_select_test pmi_select_test.c)
target_link_libraries(pmi_select_test srslte_phy)

add_test(pmi_select_test pmi_select_test)


