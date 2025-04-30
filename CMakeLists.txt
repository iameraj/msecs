cmake_minimum_required(VERSION 3.14)
project(MSECS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")

enable_testing()

file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
add_executable(main-test ${TEST_SOURCES})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)
find_package(GTest REQUIRED)
target_link_libraries(
    main-test 
    GTest::gtest 
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(main-test)
