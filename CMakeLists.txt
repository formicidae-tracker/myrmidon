cmake_policy(SET CMP0048 NEW)

project(fort-studio LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.11)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	set(IS_MAIN TRUE)
else(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	set(IS_MAIN FALSE)
endif(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)


option(ENABLE_COVERAGE "Enables coverage generation" Off)
option(UNIT_TEST_CHECK_TIMING "Test timing improvements during unit tests. May fails due to kernel scheduling, disable for automation" Off)
option(BUILD_DOCS "Builds the documentation" Off)
option(BUILD_PYTHON "Build python bindings" Off)
option(BUILD_R "Build R packages" Off)

if(IS_MAIN)
	include(GoogleTest)
	enable_testing()
	set(MAKE_CHECK_TEST_COMMAND ${CMAKE_CTEST_COMMAND} -V)
	add_custom_target(check COMMAND ${MAKE_CHECK_TEST_COMMAND})
endif(IS_MAIN)


list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(VersionFromGit)
version_from_git()

find_package(Eigen3 3.3.4 REQUIRED NO_MODULE)
find_package(Protobuf 3.3.0 REQUIRED)
find_package(TBB REQUIRED)
find_package(OpenCV REQUIRED core imgcodecs imgproc highgui videoio)
find_package(Qt5 COMPONENTS Widgets Concurrent Test Charts REQUIRED)
find_package(yaml-cpp REQUIRED)

include(FetchFortTags)
include(FetchFortHermes)
include(FetchFortTime)
include(FetchSemver)
include(FetchGoogleTest)

fetch_google_test(VERSION release-1.10.0)
fetch_fort_tags(VERSION v1.4.1)
fetch_fort_hermes(VERSION v0.5.3)
fetch_fort_time(VERSION v1.0.7)
fetch_semver(VERSION v0.2.2)



include_directories(${EIGEN3_INCLUDE_DIRS}
                    ${PROTOBUF_INCLUDE_DIRS}
                    ${FORT_HERMES_INCLUDE_DIRS}
                    ${APRILTAG_INCLUDE_DIRS}
                    ${FORT_TAGS_INCLUDE_DIRS}
                    ${FORT_TIME_INCLUDE_DIRS}
                    ${HERMES_INCLUDE_DIRS}
                    ${SEMVER_INCLUDE_DIRS}
                    )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)


include(CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_FLAGS "-lstdc++fs")

check_cxx_source_compiles("#include <filesystem>

int main(){
	std::filesystem::path p;
	return 0;
}
"
	USE_CXX17_FS_LIBRARY
)

if(USE_CXX17_FS_LIBRARY)
	set(CXXFS_LIBRARY "-lstdc++fs")
else(USE_CXX17_FS_LIBRARY)

	find_package(Boost 1.60.0 REQUIRED COMPONENTS filesystem)
	include_directories(${Boost_filesystem_INCLUDE_DIRS})
	set(CXXFS_LIBRARY Boost::filesystem)
	set(MYRMIDON_USE_BOOST_FILESYSTEM 1)
endif(USE_CXX17_FS_LIBRARY)


check_cxx_source_compiles("#include <variant>

int main() {
	std::variant<int, double> v, w;
	v = 0;
	w = 12.0;
	return std::get<int>(v);
}
"
	HAS_STD_VARIANT)
if ( NOT HAS_STD_VARIANT )
	message(FATAL_ERROR "fort-myrmidon needs std::variant support: ${HAS_STD_VARIANT}")
endif ( NOT HAS_STD_VARIANT )



if(IS_MAIN AND ENABLE_COVERAGE)
	include(CodeCoverage)
	enable_coverage()
endif(IS_MAIN AND ENABLE_COVERAGE)



if(UNIT_TEST_CHECK_TIMING)
	add_definitions("-DMYRMIDON_TEST_TIMING")
endif(UNIT_TEST_CHECK_TIMING)

set(INCLUDE_PATH include)
set(INCLUDE_INSTALL_DIR ${INCLUDE_PATH}/fort/myrmidon)
set(LIB_INSTALL_DIR lib )

include(CMakePackageConfigHelpers)
configure_package_config_file(FortMyrmidonConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfig.cmake
	INSTALL_DESTINATION ${LIB_INSTALL_DIR}/FortMyrmidon/cmake
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR INCLUDE_PATH)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/FortMyrmidonConfigVersion.cmake
        DESTINATION ${LIB_INSTALL_DIR}/FortMyrmidon/cmake )

add_subdirectory(pkgconfig)

add_subdirectory(src)

if(IS_MAIN AND ENABLE_COVERAGE)
	setup_target_for_coverage(NAME coverage
	                          DEPENDENCIES check
	                          LCOV_OPTIONS --exclude "'*UTest.*'"
	                                       --exclude "main-check.cpp"
										   --include "'${PROJECT_SOURCE_DIR}/src/fort/myrmidon/*'"
	                          )
endif(IS_MAIN AND ENABLE_COVERAGE)


add_subdirectory(misc)

if(BUILD_PYTHON OR BUILD_DOCS)
	find_package(pybind11 REQUIRED)
	include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
	set(FORT_MYRMIDON_LIBRARIES fort-myrmidon)
	set(FORT_MYRMIDON_UTESTDATA_LIBRARIES fort-myrmidon-utestdata)

	add_subdirectory(bindings/python/src)
	if(IS_MAIN)
		add_test(NAME py_fort_myrmidon-tests
                 COMMAND ${CMAKE_COMMAND} -E env
                         PYTHONPATH=${PROJECT_BINARY_DIR}/bindings/python/src
                         ${PYTHON_EXECUTABLE} -m unittest discover -s tests -v
                 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/bindings/python
                 )

	add_dependencies(check py_fort_myrmidon)
	endif(IS_MAIN)
endif(BUILD_PYTHON OR BUILD_DOCS)

if(BUILD_R)
	add_subdirectory(bindings/R)
endif(BUILD_R)



if(BUILD_DOCS)
	add_subdirectory(docs)
endif(BUILD_DOCS)
