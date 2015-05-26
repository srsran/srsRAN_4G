#
# Copyright 2013-2015 The srsLTE Developers. See the
# COPYRIGHT file at the top-level directory of this distribution.
#
# This file is part of the srsLTE library.
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

FIND_PACKAGE(openLTE)

IF(UHD_FOUND AND OPENLTE_FOUND)  
  INCLUDE_DIRECTORIES(${OPENLTE_INCLUDE_DIRS})
  ADD_EXECUTABLE(mac_test mac_test.cc)
  TARGET_LINK_LIBRARIES(mac_test srsapps_ue_mac srsapps_ue_phy srsapps_radio srslte ${OPENLTE_LIBRARIES} srslte_uhd)
ENDIF(UHD_FOUND AND OPENLTE_FOUND)
