cmake_minimum_required(VERSION 3.20)

project(unittest LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/settings.cmake)

file(GLOB_RECURSE SOURCE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*Test.cpp"
)

add_executable(${PROJECT_NAME}
	"${CMAKE_CURRENT_SOURCE_DIR}/src/_a.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp"
	${SOURCE_FILES}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/../compiler/include
)
