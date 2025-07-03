# fort-myrmidon - TRacking Analysis tools for the FORmicidae Tracker.
#
# Copyright (C) 2019-2023  Alexandre Tuleu
#
# This file is part of fort-myrmidon.
#
# fort-myrmidonis free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# fort-myrmidonis distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# fort-myrmidon.  If not, see <http://www.gnu.org/licenses/>.

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0077 NEW)

cmake_minimum_required(VERSION 3.11)

if(${SKBUILD})
	project(
		fort-myrmidon
		VERSION ${SKBUILD_PROJECT_VERSION}
		LANGUAGES C CXX
	)
else(${SKBUILD})
	project(
		fort-myrmidon
		LANGUAGES C CXX
		VERSION 0.9.0
	)
endif(${SKBUILD})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_C_STANDARD 11)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	set(FORT_MYRMIDON_MAIN TRUE)
endif(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)

if(FORT_MYRMIDON_MAIN)
	option(ENABLE_COVERAGE "Enables coverage generation" Off)
	option(
		UNIT_TEST_CHECK_TIMING
		"Test timing improvements during unit tests. May fails due to kernel scheduling, disable for automation"
		Off
	)
	option(BUILD_DOCS "Builds the documentation" Off)
endif(FORT_MYRMIDON_MAIN)

option(BUILD_PYTHON "Build python bindings" Off)
option(BUILD_R "Build R packages" Off)

include(FetchContent)

if(FORT_MYRMIDON_MAIN)
	set(INSTALL_GTEST Off)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG 76bb2afb8b522d24496ad1c757a49784fbfa2e42
	)
	# For Windows: Prevent overriding the parent project's compiler/linker
	# settings
	set(gtest_force_shared_crt
		ON
		CACHE BOOL "" FORCE
	)
	FetchContent_MakeAvailable(googletest)

	enable_testing()
	add_custom_target(check ${CMAKE_CTEST_COMMAND} ARGS -V)
	include(GoogleTest)

	# find_package(OpenCV REQUIRED highgui imgproc imgcodecs videoio)
endif(FORT_MYRMIDON_MAIN)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# only infer version not from SKBUILD
if(${SKBUILD})
	set(PROJECT_VERSION_API ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

	if(${PROJECT_VERSION_MAJOR} EQUAL 0)
		set(PROJECT_VERSION_ABI
			${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
		)
	else(${PROJECT_VERSION_MAJOR} EQUAL 0)
		set(PROJECT_VERSION_ABI ${PROJECT_VERSION_MAJOR})
	endif(${PROJECT_VERSION_MAJOR} EQUAL 0)
else(${SKBUILD})
	include(VersionFromGit)
	version_from_git()
endif(${SKBUILD})

find_package(Eigen3 3.3.4 REQUIRED NO_MODULE)
find_package(Protobuf 3.3.0 REQUIRED)
find_package(TBB REQUIRED)

if(BUILD_STUDIO)
	find_package(
		Qt6
		COMPONENTS Widgets Concurrent Test Charts
		REQUIRED
	)
endif(BUILD_STUDIO)

set(YAML_BUILD_SHARED_LIBS
	On
	CACHE BOOL "Compile shared libraries"
)

FetchContent_Declare(
	yaml-cpp
	GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
	GIT_TAG yaml-cpp-0.7.0
)

FetchContent_Declare(
	fort-tags
	GIT_REPOSITORY https://github.com/formicidae-tracker/fort-tags.git
	GIT_TAG v1.4.2
)

FetchContent_Declare(
	fort-hermes
	GIT_REPOSITORY https://github.com/formicidae-tracker/hermes.git
	GIT_TAG v0.5.6
)

FetchContent_Declare(
	semver
	GIT_REPOSITORY https://github.com/Neargye/semver.git
	GIT_TAG v0.3.0
)

set(FORT_CHARIS_BUILD_GL Off)
FetchContent_Declare(
	fort-charis
	GIT_REPOSITORY https://github.com/formicidae-tracker/charis.git
	GIT_TAG 93d378afd3e28ca1e13467a80b89aa67ca523ef9
)

FetchContent_MakeAvailable(yaml-cpp semver fort-tags fort-hermes fort-charis)

include(CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_FLAGS "-lstdc++fs")
check_cxx_source_compiles(
	"#include <filesystem>

int main(){
	std::filesystem::path p;
	return 0;
}
"
	USE_CXX17_FS_LIBRARY
)
set(CMAKE_REQUIRED_FLAGS)

if(USE_CXX17_FS_LIBRARY)
	set(CXXFS_LIBRARY "-lstdc++fs")
else(USE_CXX17_FS_LIBRARY)
	check_cxx_source_compiles(
		"#include <filesystem>

int main(){
	std::filesystem::path p;
	return 0;
}
"
		NO_LINK_CXX17_FS_LIBRARY
	)
	if(NO_LINK_CXX17_FS_LIBRARY)
		set(CXXFS_LIBRARY)
	else(NO_LINK_CXX17_FS_LIBRARY)
		find_package(Boost 1.60.0 REQUIRED COMPONENTS filesystem)
		include_directories(${Boost_filesystem_INCLUDE_DIRS})
		set(CXXFS_LIBRARY Boost::Filesystem)
		set(MYRMIDON_USE_BOOST_FILESYSTEM 1)
	endif(NO_LINK_CXX17_FS_LIBRARY)
endif(USE_CXX17_FS_LIBRARY)

check_cxx_source_compiles(
	"#include <variant>

int main() {
	std::variant<int, double> v, w;
	v = 0;
	w = 12.0;
	return std::get<int>(v);
}
"
	HAS_STD_VARIANT
)
if(NOT HAS_STD_VARIANT)
	message(
		FATAL_ERROR
			"fort-myrmidon needs std::variant support: ${HAS_STD_VARIANT}"
	)
endif(NOT HAS_STD_VARIANT)

if(FORT_MYRMIDON_MAIN AND ENABLE_COVERAGE)
	include(CodeCoverage)
	enable_coverage()
endif(FORT_MYRMIDON_MAIN AND ENABLE_COVERAGE)

if(UNIT_TEST_CHECK_TIMING)
	add_definitions("-DMYRMIDON_TEST_TIMING")
endif(UNIT_TEST_CHECK_TIMING)

set(INCLUDE_PATH include)
set(INCLUDE_INSTALL_DIR ${INCLUDE_PATH}/fort/myrmidon)
set(LIB_INSTALL_DIR lib)

if(FORT_MYRMIDON_MAIN)
	include(CMakePackageConfigHelpers)
	configure_package_config_file(
		FortMyrmidonConfig.cmake.in
		${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfig.cmake
		INSTALL_DESTINATION ${LIB_INSTALL_DIR}/FortMyrmidon/cmake
		PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR INCLUDE_PATH
	)
	write_basic_package_version_file(
		${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfigVersion.cmake
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion
	)

	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfig.cmake
				  ${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfigVersion.cmake
			DESTINATION ${LIB_INSTALL_DIR}/FortMyrmidon/cmake
	)

	add_subdirectory(pkgconfig)

endif(FORT_MYRMIDON_MAIN)

add_subdirectory(src)

if(FORT_MYRMIDON_MAIN AND ENABLE_COVERAGE)
	setup_target_for_coverage(
		NAME
		coverage
		DEPENDENCIES
		check
		LCOV_OPTIONS
		--exclude
		"'*UTest.*'"
		--exclude
		"main-check.cpp"
		--include
		"'${PROJECT_SOURCE_DIR}/src/fort/myrmidon/*'"
		# --include "'${PROJECT_SOURCE_DIR}/src/fort/studio/*'"
	)
endif(FORT_MYRMIDON_MAIN AND ENABLE_COVERAGE)

add_subdirectory(misc)

if(BUILD_PYTHON OR BUILD_DOCS)
	FetchContent_Declare(
		pybind11
		GIT_REPOSITORY https://github.com/pybind/pybind11
		GIT_TAG v2.13.6
	)
	FetchContent_MakeAvailable(pybind11)

	add_subdirectory(bindings/python/src)
	if(FORT_MYRMIDON_MAIN)
		add_custom_target(
			py_fort_myrmidon-tests
			COMMAND
				${CMAKE_COMMAND} -E env
				PYTHONPATH=${PROJECT_BINARY_DIR}/bindings/python/src
				${PYTHON_EXECUTABLE} -m pytest
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/bindings/python
			DEPENDS fort_myrmidon fort_myrmidon_utestdata
		)

		add_dependencies(check py_fort_myrmidon-tests)
		add_dependencies(fort_myrmidon fort-myrmidon)
		add_dependencies(fort_myrmidon_utestdata fort-myrmidon-utestdata)
	endif(FORT_MYRMIDON_MAIN)
endif(BUILD_PYTHON OR BUILD_DOCS)

if(BUILD_R)
	add_subdirectory(bindings/R)
endif(BUILD_R)

if(BUILD_DOCS)
	add_subdirectory(docs)
endif(BUILD_DOCS)
