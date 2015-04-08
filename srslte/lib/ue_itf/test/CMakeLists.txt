#
# Copyright 2012-2013 The srsLTE Developers. See the
# COPYRIGHT file at the top-level directory of this distribution.
#
# This file is part of the srsLTE library.
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# A copy of the GNU Lesser General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#


LIST(FIND OPTIONAL_LIBS cuhd CUHD_FIND)
IF(${CUHD_FIND} GREATER -1)
  ADD_EXECUTABLE(ue_itf_test ue_itf_test.cc)
  TARGET_LINK_LIBRARIES(ue_itf_test srslte++ cuhd)
ENDIF(${CUHD_FIND} GREATER -1)
