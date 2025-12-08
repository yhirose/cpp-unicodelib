cmake_minimum_required(VERSION 3.14)
project("cpp-unicodelib")

set(CMAKE_CXX_STANDARD 17)
option(UNICODELIB_BUILD_MODULES "Build with modules support" OFF)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /utf-8")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow")
endif()

enable_testing()

add_subdirectory(test)

install(FILES unicodelib.h unicodelib_encodings.h aaa.h DESTINATION include)

if(UNICODELIB_BUILD_MODULES)
  add_subdirectory(modules)
endif()
