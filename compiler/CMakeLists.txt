cmake_minimum_required(VERSION 3.20)

project(sealc LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/settings.cmake)

file(GLOB_RECURSE SOURCE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)
