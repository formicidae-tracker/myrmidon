find_program(R_EXECUTABLE R)
if(NOT R_EXECUTABLE)
	message(FATAL_ERROR "Could not find R")
endif(NOT R_EXECUTABLE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon/DESCRIPTION.in
               ${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon/DESCRIPTION
               @ONLY)

set(R_FORT_MYRMIDON_PACKAGE ${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon_${PROJECT_VERSION}.tar.gz)

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/localenv/${CMAKE_INSTALL_PREFIX}/lib/libfort-myrmidon.so
                   COMMAND ${CMAKE_COMMAND} --build .
                   COMMAND ${CMAKE_COMMAND} ARGS -E env DESTDIR=${PROJECT_BINARY_DIR}/localenv ${CMAKE_COMMAND} --install .
                   WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                   DEPENDS fort-myrmidon fort-myrmidon-utestdata)

add_custom_command(OUTPUT ${R_FORT_MYRMIDON_PACKAGE}
                   COMMAND ${CMAKE_COMMAND} ARGS -E env PKG_CONFIG_SYSROOT_DIR=${PROJECT_BINARY_DIR}/localenv
                                                        PKG_CONFIG_PATH=${PROJECT_BINARY_DIR}/localenv/${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
                                                        CFLAGS=${EIGEN3_INCLUDE_DIRS}
                                            ${R_EXECUTABLE} CMD INSTALL -l ${CMAKE_CURRENT_BINARY_DIR} --build FortMyrmidon
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   DEPENDS ${PROJECT_BINARY_DIR}/localenv/${CMAKE_INSTALL_PREFIX}/lib/libfort-myrmidon.so
                   )
add_custom_target(r-fort-myrmidon
                  DEPENDS ${R_FORT_MYRMIDON_PACKAGE})

if(IS_MAIN)
	add_custom_target(r-fort-myrmidon-check
	                  COMMAND ${CMAKE_COMMAND} -E env PKG_CONFIG_SYSROOT_DIR=${PROJECT_BINARY_DIR}/localenv
	                                                  PKG_CONFIG_PATH=${PROJECT_BINARY_DIR}/localenv/${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
	                                                  ${R_EXECUTABLE} CMD check FortMyrmidon
	                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	                  DEPENDS ${PROJECT_BINARY_DIR}/localenv/${CMAKE_INSTALL_PREFIX}/lib/libfort-myrmidon.so
	                  )
    add_dependencies(check r-fort-myrmidon-check)
endif(IS_MAIN)
