# CMake generated Testfile for 
# Source directory: /home/pi/srsRAN/srsue/test/phy
# Build directory: /home/pi/srsRAN/build/srsue/test/phy
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(scell_search_test "scell_search_test" "--duration=1" "--phy_lib_log_level=0" "--intra_meas_log_level=none" "--nof_enb=2" "--cell.nof_prb=25")
set_tests_properties(scell_search_test PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/srsue/test/phy/CMakeLists.txt;62;add_test;/home/pi/srsRAN/srsue/test/phy/CMakeLists.txt;0;")
