cmake_minimum_required(VERSION 3.21...3.27)

project(
	${SKBUILD_PROJECT_NAME}
	VERSION ${SKBUILD_PROJECT_VERSION}
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_C_STANDARD 11)

find_package(
	Python
	COMPONENTS Interpreter Development.Module
	REQUIRED
)
find_package(pybind11 2.12.0 CONFIG REQUIRED)

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

if(FORT_MYRMIDON_PYTHON_MAIN)
	add_library(
		cmake_git_version_tracking STATIC
		${PROJECT_SOURCE_DIR}/myrmidon/version/git.h
		${PROJECT_SOURCE_DIR}/myrmidon/version/git.cpp
	)
	target_include_directories(
		cmake_git_version_tracking
		INTERFACE ${PROJECT_SOURCE_DIR}/myrmidon/version
	)
	target_compile_options(cmake_git_version_tracking PRIVATE "-fPIC")
endif()

include(FetchContent)
FetchContent_Declare(myrmidon URL ${PROJECT_SOURCE_DIR}/myrmidon)
FetchContent_MakeAvailable(myrmidon)

add_subdirectory(src/fort_myrmidon)

if(FORT_MYRMIDON_PYTHON_MAIN)
	install(SCRIPT ${PROJECT_SOURCE_DIR}/PostInstallWheel.cmake)

endif(FORT_MYRMIDON_PYTHON_MAIN)
