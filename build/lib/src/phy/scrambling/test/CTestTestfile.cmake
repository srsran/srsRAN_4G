# CMake generated Testfile for 
# Source directory: /home/ubuntu/vEPC/srsLTE/lib/src/phy/scrambling/test
# Build directory: /home/ubuntu/vEPC/srsLTE/build/lib/src/phy/scrambling/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(scrambling_pbch_bit "scrambling_test" "-s" "PBCH" "-c" "50")
add_test(scrambling_pbch_float "scrambling_test" "-s" "PBCH" "-c" "50" "-f")
add_test(scrambling_pbch_e_bit "scrambling_test" "-s" "PBCH" "-c" "50" "-e")
add_test(scrambling_pbch_e_float "scrambling_test" "-s" "PBCH" "-c" "50" "-f" "-e")
