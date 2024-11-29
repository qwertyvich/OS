# CMake generated Testfile for 
# Source directory: /mnt/c/Users/user/Desktop/Labs/os/lr2
# Build directory: /mnt/c/Users/user/Desktop/Labs/os/lr2/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Lab2Test "/mnt/c/Users/user/Desktop/Labs/os/lr2/build/run_tests")
set_tests_properties(Lab2Test PROPERTIES  _BACKTRACE_TRIPLES "/mnt/c/Users/user/Desktop/Labs/os/lr2/CMakeLists.txt;12;add_test;/mnt/c/Users/user/Desktop/Labs/os/lr2/CMakeLists.txt;0;")
subdirs("googletest")
