cmake_minimum_required(VERSION 3.15...3.27)

project(py-fort-myrmidon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

find_package(pybind11 REQUIRED)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	set(FORT_MYRMIDON_PYTHON_MAIN TRUE)
endif(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)

set(BUILD_PYTHON
	Off
	CACHE BOOL "Build Python Bindings"
)
set(BUILD_R
	Off
	CACHE BOOL "Build Python Bindings"
)
set(BUILD_STUDIO
	Off
	CACHE BOOL "Build Python Bindings"
)

# include parent project
add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/_myrmidon
)

add_subdirectory(src)
