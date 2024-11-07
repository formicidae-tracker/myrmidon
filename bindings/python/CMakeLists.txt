cmake_minimum_required(VERSION 3.15...3.27)

project(
	${SK_BUILD_PROJECT_NAME}
	VERSION ${SKBUILD+PROJECT_VERSION}
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

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

include(FetchContent)
FetchContent_Declare(myrmidon URL ${PROJECT_SOURCE_DIR}/myrmidon)
FetchContent_MakeAvailable(myrmidon)

add_subdirectory(src)
