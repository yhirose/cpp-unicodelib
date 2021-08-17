cmake_minimum_required(VERSION 3.1.0)
project(test)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)
FetchContent_MakeAvailable(Catch2)

add_executable(test-main test.cpp test2.cpp)

target_include_directories(test-main PRIVATE ..)
target_link_libraries(test-main PRIVATE Catch2::Catch2WithMain)

enable_testing()

add_test(
  NAME TestMain
  COMMAND test-main
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
