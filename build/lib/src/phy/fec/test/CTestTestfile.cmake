# CMake generated Testfile for 
# Source directory: /home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test
# Build directory: /home/msudurip1/Documents/srsRAN_modified/srsRAN/build/lib/src/phy/fec/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(crc_24A "crc_test" "-n" "5001" "-l" "24" "-p" "0x1864CFB" "-s" "1")
set_tests_properties(crc_24A PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;28;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;0;")
add_test(crc_24B "crc_test" "-n" "5001" "-l" "24" "-p" "0x1800063" "-s" "1")
set_tests_properties(crc_24B PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;29;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;0;")
add_test(crc_16 "crc_test" "-n" "5001" "-l" "16" "-p" "0x11021" "-s" "1")
set_tests_properties(crc_16 PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;30;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;0;")
add_test(crc_8 "crc_test" "-n" "5001" "-l" "8" "-p" "0x19B" "-s" "1")
set_tests_properties(crc_8 PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;31;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;0;")
add_test(crc_11 "crc_test" "-n" "30" "-l" "11" "-p" "0xE21" "-s" "1")
set_tests_properties(crc_11 PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;32;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;0;")
add_test(crc_6 "crc_test" "-n" "20" "-l" "6" "-p" "0x61" "-s" "1")
set_tests_properties(crc_6 PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;33;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/test/CMakeLists.txt;0;")
