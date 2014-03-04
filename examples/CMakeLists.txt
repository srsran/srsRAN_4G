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
# TO BE MOVED TO UNIT TESTS
#################################################################

add_executable(hl_example hl_example.c)
target_link_libraries(hl_example lte)

add_executable(ll_example ll_example.c)
target_link_libraries(ll_example lte)

add_executable(synch_test synch_test.c)
target_link_libraries(synch_test lte)

add_executable(equalizer_test equalizer_test.c)
target_link_libraries(equalizer_test lte)

add_executable(viterbi_test viterbi_test.c)
target_link_libraries(viterbi_test lte)

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

add_executable(mib_track mib_track.c)
target_link_libraries(mib_track lte)

add_executable(enodeb_bch enodeb_bch.c)
target_link_libraries(enodeb_bch lte)

IF(${CUHD_FIND} EQUAL -1)
	SET_TARGET_PROPERTIES(mib_track PROPERTIES COMPILE_DEFINITIONS "DISABLE_UHD")
	SET_TARGET_PROPERTIES(enodeb_bch PROPERTIES COMPILE_DEFINITIONS "DISABLE_UHD")
ELSE(${CUHD_FIND} EQUAL -1)	
	target_link_libraries(mib_track cuhd)
	target_link_libraries(enodeb_bch cuhd)
ENDIF(${CUHD_FIND} EQUAL -1)

IF(${GRAPHICS_FIND} EQUAL -1)
	SET_TARGET_PROPERTIES(mib_track PROPERTIES COMPILE_DEFINITIONS "DISABLE_GRAPHICS")
	SET_TARGET_PROPERTIES(enodeb_bch PROPERTIES COMPILE_DEFINITIONS "DISABLE_GRAPHICS")
ELSE(${GRAPHICS_FIND} EQUAL -1)
	target_link_libraries(mib_track graphics)
	target_link_libraries(enodeb_bch graphics)
ENDIF(${GRAPHICS_FIND} EQUAL -1)



#################################################################
# These examples need the UHD driver
#################################################################

IF(${CUHD_FIND} GREATER -1)
	
	add_executable(rssi_scan_usrp rssi_scan_usrp.c)
	target_link_libraries(rssi_scan_usrp lte cuhd )
	
	add_executable(pss_scan_usrp pss_scan_usrp.c)
	target_link_libraries(pss_scan_usrp lte cuhd )
	
	add_executable(mib_scan_usrp mib_scan_usrp.c)
	target_link_libraries(mib_scan_usrp lte cuhd )

	MESSAGE(STATUS "   UHD examples will be installed.")

ELSE(${CUHD_FIND} GREATER -1)
	MESSAGE(STATUS "   UHD examples NOT INSTALLED: CUHD library not compiled.")
ENDIF(${CUHD_FIND} GREATER -1)	

