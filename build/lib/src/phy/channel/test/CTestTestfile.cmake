# CMake generated Testfile for 
# Source directory: /home/pi/srsRAN/lib/src/phy/channel/test
# Build directory: /home/pi/srsRAN/build/lib/src/phy/channel/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(fading_channel_test_epa5 "fading_channel_test" "-m" "epa5" "-s" "26.04e6" "-t" "100")
set_tests_properties(fading_channel_test_epa5 PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/channel/test/CMakeLists.txt;28;add_test;/home/pi/srsRAN/lib/src/phy/channel/test/CMakeLists.txt;0;")
add_test(fading_channel_test_eva70 "fading_channel_test" "-m" "eva70" "-s" "23.04e6" "-t" "100")
set_tests_properties(fading_channel_test_eva70 PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/channel/test/CMakeLists.txt;29;add_test;/home/pi/srsRAN/lib/src/phy/channel/test/CMakeLists.txt;0;")
add_test(fading_channel_test_etu300 "fading_channel_test" "-m" "etu70" "-s" "23.04e6" "-t" "100")
set_tests_properties(fading_channel_test_etu300 PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/channel/test/CMakeLists.txt;30;add_test;/home/pi/srsRAN/lib/src/phy/channel/test/CMakeLists.txt;0;")
add_test(delay_channel_test "delay_channel_test" "-m" "10" "-M" "100" "-t" "1000" "-T" "1" "-s" "1.92e6")
set_tests_properties(delay_channel_test PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/channel/test/CMakeLists.txt;34;add_test;/home/pi/srsRAN/lib/src/phy/channel/test/CMakeLists.txt;0;")
add_test(hst_channel_test "hst_channel_test" "-f" "750" "-t" "7.2" "-i" "0" "-T" "1" "-s" "1.92e6")
set_tests_properties(hst_channel_test PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/channel/test/CMakeLists.txt;38;add_test;/home/pi/srsRAN/lib/src/phy/channel/test/CMakeLists.txt;0;")
