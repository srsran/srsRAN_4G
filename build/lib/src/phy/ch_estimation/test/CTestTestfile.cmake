# CMake generated Testfile for 
# Source directory: /home/pi/srsRAN/lib/src/phy/ch_estimation/test
# Build directory: /home/pi/srsRAN/build/lib/src/phy/ch_estimation/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(chest_test_dl_cellid0 "chest_test_dl" "-c" "0")
set_tests_properties(chest_test_dl_cellid0 PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;28;add_test;/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;0;")
add_test(chest_test_dl_cellid1 "chest_test_dl" "-c" "1")
set_tests_properties(chest_test_dl_cellid1 PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;29;add_test;/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;0;")
add_test(chest_test_dl_cellid2 "chest_test_dl" "-c" "2")
set_tests_properties(chest_test_dl_cellid2 PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;30;add_test;/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;0;")
add_test(chest_test_dl_cellid0_50prb "chest_test_dl" "-c" "0" "-r" "50")
set_tests_properties(chest_test_dl_cellid0_50prb PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;32;add_test;/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;0;")
add_test(chest_test_dl_cellid1_50prb "chest_test_dl" "-c" "1" "-r" "50")
set_tests_properties(chest_test_dl_cellid1_50prb PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;33;add_test;/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;0;")
add_test(chest_test_dl_cellid2_50prb "chest_test_dl" "-c" "2" "-r" "50")
set_tests_properties(chest_test_dl_cellid2_50prb PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;34;add_test;/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;0;")
add_test(chest_test_ul_cellid0 "chest_test_ul" "-c" "0" "-r" "50")
set_tests_properties(chest_test_ul_cellid0 PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;47;add_test;/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;0;")
add_test(chest_test_ul_cellid1 "chest_test_ul" "-c" "2" "-r" "50")
set_tests_properties(chest_test_ul_cellid1 PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;48;add_test;/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;0;")
add_test(chest_nbiot_test_dl "chest_nbiot_test_dl")
set_tests_properties(chest_nbiot_test_dl PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;59;add_test;/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;0;")
add_test(chest_test_sl_psbch "chest_test_sl")
set_tests_properties(chest_test_sl_psbch PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;69;add_test;/home/pi/srsRAN/lib/src/phy/ch_estimation/test/CMakeLists.txt;0;")
