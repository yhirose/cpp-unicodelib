cmake_minimum_required(VERSION 3.0)
project("cpp-unicodelib")

include_directories(..)

add_definitions("-std=c++1y -Werror -Wmost -Wnon-virtual-dtor -Woverloaded-virtual -Wfuture-compat -Wunused-lambda-capture -Wsometimes-uninitialized -Winconsistent-missing-destructor-override -Wshadow-uncaptured-local -Wold-style-cast -Wrange-loop-analysis -Wunused-macros")

add_executable(test-main test.cpp test2.cpp)
