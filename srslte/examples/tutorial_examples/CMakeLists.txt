#
# Copyright 2013-2015 Software Radio Systems Limited
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


#################################################################
# EXAMPLES shown in WinnForum 2015 Tutorial
#################################################################

if(SRSGUI_FOUND AND UHD_FOUND)

  add_executable(pss pss.c)
  target_link_libraries(pss srslte ${SRSGUI_LIBRARIES})

  add_executable(simple_tx simple_tx.c)
  target_link_libraries(simple_tx srslte)

endif(SRSGUI_FOUND AND UHD_FOUND)

