# CMake generated Testfile for 
# Source directory: /home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test
# Build directory: /home/msudurip1/Documents/srsRAN_modified/srsRAN/build/lib/src/phy/ue/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(gen_ack_test "gen_ack_test")
set_tests_properties(gen_ack_test PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;27;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;0;")
add_test(gen_ack_nr_test "gen_ack_nr_test")
set_tests_properties(gen_ack_nr_test PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;31;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;0;")
add_test(pucch_resource_test "pucch_resource_test")
set_tests_properties(pucch_resource_test PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;35;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;0;")
add_test(ue_dl_nbiot_test "ue_dl_nbiot_test")
set_tests_properties(ue_dl_nbiot_test PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;39;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;0;")
add_test(ue_sync_nr_test "ue_sync_nr_test")
set_tests_properties(ue_sync_nr_test PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;43;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;0;")
add_test(ue_dl_nr_pci500_rb52_si_coreset0_idx6 "ue_dl_nr_file_test" "-f" "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/ue_dl_nr_pci500_rb52_si_coreset0_idx6_s15.36e6.dat" "-S" "-i" "500" "-P" "52" "-n" "0" "-R" "ffff" "-T" "si" "-c" "6" "-s" "common0" "-A" "368500" "-a" "368410")
set_tests_properties(ue_dl_nr_pci500_rb52_si_coreset0_idx6 PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;70;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;0;")
add_test(ue_dl_nr_pci500_rb52_pdsch "ue_dl_nr_file_test" "-f" "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/ue_dl_nr_pci500_rb52_rnti0x100_s15.36e6.dat" "-S" "-i" "500" "-P" "52" "-N" "48" "-n" "1" "-R" "0x100" "-T" "c" "-s" "common3" "-o" "1" "-A" "368500" "-a" "368410" "-I" "-t" "1" "13")
set_tests_properties(ue_dl_nr_pci500_rb52_pdsch PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;72;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;0;")
add_test(ue_dl_nr_pci500_rb52_rar "ue_dl_nr_file_test" "-f" "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/ue_dl_nr_pci500_rb52_rar_s15.36e6.dat" "-i" "500" "-P" "52" "-n" "5" "-R" "f" "-T" "ra" "-c" "6" "-S" "-s" "common1" "-A" "368500" "-a" "368410")
set_tests_properties(ue_dl_nr_pci500_rb52_rar PROPERTIES  _BACKTRACE_TRIPLES "/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;73;add_test;/home/msudurip1/Documents/srsRAN_modified/srsRAN/lib/src/phy/ue/test/CMakeLists.txt;0;")
