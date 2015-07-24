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


IF(UHD_FOUND)
  ADD_EXECUTABLE(ue_itf_test_sib1 ue_itf_test_sib1.cc)
  TARGET_LINK_LIBRARIES(ue_itf_test_sib1 srsapps_common srsapps_ue_phy srsapps_radio srslte srslte_uhd)

  ADD_EXECUTABLE(ue_itf_test_prach ue_itf_test_prach.cc)
  TARGET_LINK_LIBRARIES(ue_itf_test_prach srsapps_common srsapps_ue_phy srsapps_radio srslte srslte_uhd)
ENDIF(UHD_FOUND)
