#
# Copyright 2013-2021 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

set(CTEST_PROJECT_NAME "srsRAN")
set(CTEST_NIGHTLY_START_TIME "00:00:00 GMT")
set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=srsRAN")
set(CTEST_DROP_SITE_CDASH TRUE)
set(VALGRIND_COMMAND_OPTIONS "--error-exitcode=1 --trace-children=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes --vex-guest-max-insns=25")
