# CMake generated Testfile for 
# Source directory: /home/pi/srsRAN/srsue/src
# Build directory: /home/pi/srsRAN/build/srsue/src
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(srsue_version "srsue" "--version")
set_tests_properties(srsue_version PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/srsue/src/CMakeLists.txt;57;add_test;/home/pi/srsRAN/srsue/src/CMakeLists.txt;0;")
add_test(srsue_help "srsue" "--help")
set_tests_properties(srsue_help PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/srsue/src/CMakeLists.txt;58;add_test;/home/pi/srsRAN/srsue/src/CMakeLists.txt;0;")
add_test(ue_rf_failure "srsue" "/home/pi/srsRAN/srsue/ue.conf.example" "--rf.device_name=zmq")
set_tests_properties(ue_rf_failure PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/srsue/src/CMakeLists.txt;63;add_test;/home/pi/srsRAN/srsue/src/CMakeLists.txt;0;")
subdirs("phy")
subdirs("stack")
