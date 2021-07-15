#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(CTEST_PROJECT_NAME "srsRAN")
set(CTEST_NIGHTLY_START_TIME "00:00:00 GMT")
set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=srsRAN")
set(CTEST_DROP_SITE_CDASH TRUE)
set(VALGRIND_COMMAND_OPTIONS "--error-exitcode=1 --trace-children=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes --vex-guest-max-insns=25")
