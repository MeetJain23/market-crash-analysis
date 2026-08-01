cmake_minimum_required(VERSION 3.16)
project(quantcrash LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(qc
  src/csv_loader.cpp
  src/indicators.cpp
  src/drawdown.cpp
  src/crash.cpp
  src/legs.cpp
  src/volume.cpp
  src/precrash.cpp
  src/backtest.cpp
)
target_include_directories(qc PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(quantcrash src/main.cpp)
target_link_libraries(quantcrash PRIVATE qc)

enable_testing()
add_executable(qc_tests tests/test_indicators.cpp)
target_link_libraries(qc_tests PRIVATE qc)
add_test(NAME qc_tests COMMAND qc_tests)
