cmake_minimum_required(VERSION 3.1.0)
project("cpp-unicodelib")

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /utf-8")
endif()

add_subdirectory(test)
enable_testing()
