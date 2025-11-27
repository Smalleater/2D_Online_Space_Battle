cmake_minimum_required(VERSION 3.28)

project(2D_Online_Space_Battle_Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(2D_Online_Space_Battle_Client src/main.cpp "src/player.cpp" "src/enemy.cpp")

target_include_directories(2D_Online_Space_Battle_Client 
	PRIVATE 
        ${COMMON_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(2D_Online_Space_Battle_Client PUBLIC ${COMMON_LIB_DIR}/tme_core.lib)
target_link_libraries(2D_Online_Space_Battle_Client PUBLIC ${COMMON_LIB_DIR}/tme_engine.lib)
target_link_libraries(2D_Online_Space_Battle_Client PUBLIC ${COMMON_LIB_DIR}/tme_client.lib)

target_compile_features(2D_Online_Space_Battle_Client PRIVATE cxx_std_17)
target_link_libraries(2D_Online_Space_Battle_Client PRIVATE SFML::Graphics)
