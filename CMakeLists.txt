################################################################################
# CMake system settings and includes
################################################################################
cmake_minimum_required(VERSION 2.8)
enable_testing()
set(CMAKE_MODULE_PATH ${CMAKE_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

################################################################################
# Project definition
################################################################################
project(libtnetstring)

################################################################################
# Eclipse CDT parsing
################################################################################
SET(CMAKE_VERBOSE_MAKEFILE ON)
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

################################################################################
# Options
################################################################################
set(CMAKE_COLOR_MAKEFILE True)

################################################################################
# find dependencies
################################################################################
find_package(GoogleTest REQUIRED)
find_package(Pthread REQUIRED)

################################################################################
# include paths
################################################################################
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${GTEST_INCLUDE_DIRS})

################################################################################
# compile definitions
################################################################################
add_definitions("-g -Wall -Wextra -pedantic -std=c++0x")
add_definitions(-DBOOST_VARIANT_NO_FULL_RECURSIVE_VARIANT_SUPPORT)
add_definitions(-DGTEST)

################################################################################
# gtest runtime arguments
################################################################################
set(GTEST_FLAGS "--gtest_color")

################################################################################
# test executables
################################################################################
add_executable(test_tnetstring tests/test_tnetstring.cpp)
target_link_libraries(test_tnetstring ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${PTHREAD_LIBRARIES})
add_test(test_tnetstring test_tnetstring)

# todo: installation
