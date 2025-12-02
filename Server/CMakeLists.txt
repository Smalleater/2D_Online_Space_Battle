cmake_minimum_required(VERSION 3.28)

project(2D_Online_Space_Battle_Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(2D_Online_Space_Battle_Server src/main.cpp "src/player.cpp")

target_include_directories(2D_Online_Space_Battle_Server 
	PRIVATE 
        ${COMMON_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(2D_Online_Space_Battle_Server PUBLIC ${COMMON_LIB_DIR}/tra_core.lib)
target_link_libraries(2D_Online_Space_Battle_Server PUBLIC ${COMMON_LIB_DIR}/tra_engine.lib)
target_link_libraries(2D_Online_Space_Battle_Server PUBLIC ${COMMON_LIB_DIR}/tra_server.lib)
