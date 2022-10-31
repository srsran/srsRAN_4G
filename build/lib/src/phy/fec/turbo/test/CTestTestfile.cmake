# CMake generated Testfile for 
# Source directory: /home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test
# Build directory: /home/msudurip1/Documents/srsRAN_modified/srsRAN/build/lib/src/phy/fec/turbo/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(rm_conv_test_1 "rm_conv_test" "-t" "480" "-r" "1920")
set_tests_properties(rm_conv_test_1 PROPERTIES  LABELS "lte;lib;phy;fec;turbo" _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/CMakeLists.txt;625;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;33;add_lte_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;0;")
add_test(rm_conv_test_2 "rm_conv_test" "-t" "1920" "-r" "480")
set_tests_properties(rm_conv_test_2 PROPERTIES  LABELS "lte;lib;phy;fec;turbo" _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/CMakeLists.txt;625;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;34;add_lte_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;0;")
add_test(rm_turbo_test_1 "rm_turbo_test" "-e" "1920")
set_tests_properties(rm_turbo_test_1 PROPERTIES  LABELS "lte;lib;phy;fec;turbo" _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/CMakeLists.txt;625;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;36;add_lte_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;0;")
add_test(rm_turbo_test_2 "rm_turbo_test" "-e" "8192")
set_tests_properties(rm_turbo_test_2 PROPERTIES  LABELS "lte;lib;phy;fec;turbo" _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/CMakeLists.txt;625;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;37;add_lte_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;0;")
add_test(turbodecoder_test_504_1 "turbodecoder_test" "-n" "100" "-s" "1" "-l" "504" "-e" "1.0" "-t")
set_tests_properties(turbodecoder_test_504_1 PROPERTIES  LABELS "lte;lib;phy;fec;turbo" _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/CMakeLists.txt;625;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;45;add_lte_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;0;")
add_test(turbodecoder_test_504_2 "turbodecoder_test" "-n" "100" "-s" "1" "-l" "504" "-e" "2.0" "-t")
set_tests_properties(turbodecoder_test_504_2 PROPERTIES  LABELS "lte;lib;phy;fec;turbo" _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/CMakeLists.txt;625;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;46;add_lte_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;0;")
add_test(turbodecoder_test_6114_1_5 "turbodecoder_test" "-n" "100" "-s" "1" "-l" "6144" "-e" "1.5" "-t")
set_tests_properties(turbodecoder_test_6114_1_5 PROPERTIES  LABELS "lte;lib;phy;fec;turbo" _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/CMakeLists.txt;625;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;47;add_lte_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;0;")
add_test(turbodecoder_test_known "turbodecoder_test" "-n" "1" "-s" "1" "-k" "-e" "0.5")
set_tests_properties(turbodecoder_test_known PROPERTIES  LABELS "lte;lib;phy;fec;turbo" _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/CMakeLists.txt;625;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;48;add_lte_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;0;")
add_test(turbocoder_test_all "turbocoder_test")
set_tests_properties(turbocoder_test_all PROPERTIES  LABELS "lte;lib;phy;fec;turbo" _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/CMakeLists.txt;625;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;52;add_lte_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/fec/turbo/test/CMakeLists.txt;0;")
