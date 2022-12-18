# CMake generated Testfile for 
# Source directory: /home/pi/srsRAN/lib/src/phy/dft/test
# Build directory: /home/pi/srsRAN/build/lib/src/phy/dft/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ofdm_normal "ofdm_test")
set_tests_properties(ofdm_normal PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/dft/test/CMakeLists.txt;28;add_test;/home/pi/srsRAN/lib/src/phy/dft/test/CMakeLists.txt;0;")
add_test(ofdm_extended "ofdm_test" "-e")
set_tests_properties(ofdm_extended PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/dft/test/CMakeLists.txt;29;add_test;/home/pi/srsRAN/lib/src/phy/dft/test/CMakeLists.txt;0;")
add_test(ofdm_normal_single "ofdm_test" "-n" "6")
set_tests_properties(ofdm_normal_single PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/dft/test/CMakeLists.txt;31;add_test;/home/pi/srsRAN/lib/src/phy/dft/test/CMakeLists.txt;0;")
add_test(ofdm_extended_single "ofdm_test" "-e" "-n" "6")
set_tests_properties(ofdm_extended_single PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/dft/test/CMakeLists.txt;32;add_test;/home/pi/srsRAN/lib/src/phy/dft/test/CMakeLists.txt;0;")
