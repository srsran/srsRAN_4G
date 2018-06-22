# CMake generated Testfile for 
# Source directory: /home/ubuntu/vEPC/srsLTE/lib/src/phy/utils/test
# Build directory: /home/ubuntu/vEPC/srsLTE/build/lib/src/phy/utils/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(dft_test "dft_test")
add_test(dft_reverse "dft_test" "-b")
add_test(dft_mirror "dft_test" "-m")
add_test(dft_norm "dft_test" "-n")
add_test(dft_dc "dft_test" "-b" "-d")
add_test(dft_odd "dft_test" "-N" "255")
add_test(dft_odd_dc "dft_test" "-N" "255" "-b" "-d")
add_test(algebra_2x2_zf_solver_test "algebra_test" "-z")
add_test(algebra_2x2_mmse_solver_test "algebra_test" "-m")
add_test(vector_test "vector_test")
