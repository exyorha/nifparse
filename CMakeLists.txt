cmake_minimum_required(VERSION 3.12)
project(nifparse)

find_program(RUBY_EXECUTABLE ruby)
if(NOT RUBY_EXECUTABLE)
	message(FATAL_ERROR "Ruby is required")
endif()

if(NOT halffloat_SOURCE_DIR)
	add_subdirectory(halffloat)
endif()

add_subdirectory(nifparse)
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
	add_subdirectory(nifparse-test)
endif()

