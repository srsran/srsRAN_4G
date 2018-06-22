# CMake generated Testfile for 
# Source directory: /home/ubuntu/vEPC/srsLTE/lib/src/phy/sync/test
# Build directory: /home/ubuntu/vEPC/srsLTE/build/lib/src/phy/sync/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(sync_test_100 "sync_test" "-o" "100" "-p" "50" "-c" "501")
add_test(sync_test_400 "sync_test" "-o" "400" "-p" "50" "-c" "500")
add_test(sync_test_100_e "sync_test" "-o" "100" "-e" "-p" "50" "-c" "133")
add_test(sync_test_400_e "sync_test" "-o" "400" "-e" "-p" "50" "-c" "123")
add_test(cfo_test_1 "cfo_test" "-f" "0.12345" "-n" "1000")
add_test(cfo_test_2 "cfo_test" "-f" "0.99849" "-n" "1000")
