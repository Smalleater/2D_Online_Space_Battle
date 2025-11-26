cmake_minimum_required(VERSION 3.28)

project(2D_Online_Space_Battle VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(COMMON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/CommonInclude)
set(COMMON_LIB_DIR ${CMAKE_SOURCE_DIR}/CommonLib)

add_subdirectory(Client)
add_subdirectory(Server)