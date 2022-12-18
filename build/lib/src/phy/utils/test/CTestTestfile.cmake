# CMake generated Testfile for 
# Source directory: /home/pi/srsRAN/lib/src/phy/utils/test
# Build directory: /home/pi/srsRAN/build/lib/src/phy/utils/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(dft_test "dft_test")
set_tests_properties(dft_test PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;28;add_test;/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;0;")
add_test(dft_reverse "dft_test" "-b")
set_tests_properties(dft_reverse PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;29;add_test;/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;0;")
add_test(dft_mirror "dft_test" "-m")
set_tests_properties(dft_mirror PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;30;add_test;/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;0;")
add_test(dft_norm "dft_test" "-n")
set_tests_properties(dft_norm PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;31;add_test;/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;0;")
add_test(dft_dc "dft_test" "-b" "-d")
set_tests_properties(dft_dc PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;32;add_test;/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;0;")
add_test(dft_odd "dft_test" "-N" "255")
set_tests_properties(dft_odd PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;33;add_test;/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;0;")
add_test(dft_odd_dc "dft_test" "-N" "255" "-b" "-d")
set_tests_properties(dft_odd_dc PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;34;add_test;/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;0;")
add_test(algebra_2x2_zf_solver_test "algebra_test" "-z")
set_tests_properties(algebra_2x2_zf_solver_test PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;43;add_test;/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;0;")
add_test(algebra_2x2_mmse_solver_test "algebra_test" "-m")
set_tests_properties(algebra_2x2_mmse_solver_test PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;44;add_test;/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;0;")
add_test(vector_test "vector_test")
set_tests_properties(vector_test PROPERTIES  _BACKTRACE_TRIPLES "/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;48;add_test;/home/pi/srsRAN/lib/src/phy/utils/test/CMakeLists.txt;0;")
