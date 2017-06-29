
#          Copyright Marc Bodmer 2011-2012.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

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
# Options
################################################################################
set(CMAKE_COLOR_MAKEFILE True)

################################################################################
# find dependencies
################################################################################
find_package(GTest REQUIRED)
find_package(Pthread REQUIRED)
find_package(Boost REQUIRED)

################################################################################
# include paths
################################################################################
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${GTEST_INCLUDE_DIRS})

################################################################################
# compile definitions
################################################################################
add_definitions("-g -Wall -Wextra -pedantic -std=c++11")
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

