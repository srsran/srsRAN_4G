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


#################################################################
# EXAMPLES
#################################################################

add_executable(hl_example hl_example.c)
target_link_libraries(hl_example lte)

add_executable(ll_example ll_example.c)
target_link_libraries(ll_example lte)

add_executable(synch_file synch_file.c)
target_link_libraries(synch_file lte)


#################################################################
# TO BE MOVED TO UNIT TESTS
#################################################################

add_executable(mib_test mib_test.c)
target_link_libraries(mib_test lte)


#################################################################
# Check if UHD C-API and Graphics library are available
#################################################################

LIST(FIND OPTIONAL_LIBS cuhd CUHD_FIND)
LIST(FIND OPTIONAL_LIBS graphics GRAPHICS_FIND)

#################################################################
# These two can be compiled without UHD or graphics support
#################################################################

add_executable(pbch_ue pbch_ue.c)
target_link_libraries(pbch_ue lte)

add_executable(pbch_enodeb pbch_enodeb.c)
target_link_libraries(pbch_enodeb lte)

IF(${CUHD_FIND} EQUAL -1)
	SET_TARGET_PROPERTIES(pbch_ue PROPERTIES COMPILE_DEFINITIONS "DISABLE_UHD")
	SET_TARGET_PROPERTIES(pbch_enodeb PROPERTIES COMPILE_DEFINITIONS "DISABLE_UHD")
ELSE(${CUHD_FIND} EQUAL -1)	
	target_link_libraries(pbch_ue cuhd)
	target_link_libraries(pbch_enodeb cuhd)
ENDIF(${CUHD_FIND} EQUAL -1)

IF(${GRAPHICS_FIND} EQUAL -1)
	SET_TARGET_PROPERTIES(pbch_ue PROPERTIES COMPILE_DEFINITIONS "DISABLE_GRAPHICS")
	SET_TARGET_PROPERTIES(pbch_enodeb PROPERTIES COMPILE_DEFINITIONS "DISABLE_GRAPHICS")
ELSE(${GRAPHICS_FIND} EQUAL -1)
	target_link_libraries(pbch_ue graphics)
	target_link_libraries(pbch_enodeb graphics)
ENDIF(${GRAPHICS_FIND} EQUAL -1)



#################################################################
# These examples need the UHD driver
#################################################################

IF(${CUHD_FIND} GREATER -1)
	
	add_executable(scan_rssi scan_rssi.c)
	target_link_libraries(scan_rssi lte cuhd )
	
	add_executable(scan_pss scan_pss.c)
	target_link_libraries(scan_pss lte cuhd )
	
	add_executable(scan_mib scan_mib.c)
	target_link_libraries(scan_mib lte cuhd )

	MESSAGE(STATUS "   UHD examples will be installed.")

ELSE(${CUHD_FIND} GREATER -1)
	MESSAGE(STATUS "   UHD examples NOT INSTALLED: CUHD library not compiled.")
ENDIF(${CUHD_FIND} GREATER -1)	

