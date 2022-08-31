# CMake generated Testfile for 
# Source directory: /Users/parkgeonwoo/Documents/document/2022-2/ComputerVision/opencv-3.4.11/modules/ml
# Build directory: /Users/parkgeonwoo/Documents/document/2022-2/ComputerVision/opencv-3.4.11/build/modules/ml
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_ml "/Users/parkgeonwoo/Documents/document/2022-2/ComputerVision/opencv-3.4.11/build/bin/opencv_test_ml" "--gtest_output=xml:opencv_test_ml.xml")
set_tests_properties(opencv_test_ml PROPERTIES  LABELS "Main;opencv_ml;Accuracy" WORKING_DIRECTORY "/Users/parkgeonwoo/Documents/document/2022-2/ComputerVision/opencv-3.4.11/build/test-reports/accuracy" _BACKTRACE_TRIPLES "/Users/parkgeonwoo/Documents/document/2022-2/ComputerVision/opencv-3.4.11/cmake/OpenCVUtils.cmake;1649;add_test;/Users/parkgeonwoo/Documents/document/2022-2/ComputerVision/opencv-3.4.11/cmake/OpenCVModule.cmake;1287;ocv_add_test_from_target;/Users/parkgeonwoo/Documents/document/2022-2/ComputerVision/opencv-3.4.11/cmake/OpenCVModule.cmake;1069;ocv_add_accuracy_tests;/Users/parkgeonwoo/Documents/document/2022-2/ComputerVision/opencv-3.4.11/modules/ml/CMakeLists.txt;2;ocv_define_module;/Users/parkgeonwoo/Documents/document/2022-2/ComputerVision/opencv-3.4.11/modules/ml/CMakeLists.txt;0;")
