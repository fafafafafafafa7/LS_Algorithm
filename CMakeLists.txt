cmake_minimum_required(VERSION 3.4)

project(GrEdPa)

SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++11 -O3 -Wall")

add_subdirectory(Algorithms)
add_subdirectory(LS)
add_subdirectory(src)
