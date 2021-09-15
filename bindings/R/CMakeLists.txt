find_program(R_EXECUTABLE R)
if(NOT R_EXECUTABLE)
	message(FATAL_ERROR "Could not find R")
endif(NOT R_EXECUTABLE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION.in
               ${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon/DESCRIPTION
               @ONLY)

set(R_FORT_MYRMIDON_PACKAGE ${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon_${PROJECT_VERSION}.tar.gz)
set(FORT_MYRMIDON_CXXFLAGS -I${PROJECT_BINARY_DIR}/src -I${PROJECT_SOURCE_DIR}/src -I${FORT_TIME_INCLUDE_DIRS} -I${EIGEN3_INCLUDE_DIRS})
set(FORT_MYRMIDON_LDFLAGS -L$<TARGET_FILE_DIR:fort-myrmidon> -lfort-myrmidon -L$<TARGET_FILE_DIR:fort-time> -lfort-time)
set(FORT_MYRMIDON_LD_PATH $<TARGET_FILE_DIR:fort-myrmidon>:$<TARGET_FILE_DIR:fort-time>)
set(R_FORT_MYRMIDON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon)

set(ENV_VARIABLES FORT_MYRMIDON_CXXFLAGS='${FORT_MYRMIDON_CXXFLAGS}'
	              FORT_MYRMIDON_LDFLAGS='${FORT_MYRMIDON_LDFLAGS}'
	              LD_LIBRARY_PATH='${FORT_MYRMIDON_LD_PATH}'
	              R_FORT_MYRMIDON_PATH=${R_FORT_MYRMIDON_PATH}
	              )


add_custom_command(OUTPUT ${R_FORT_MYRMIDON_PACKAGE}
                   COMMAND  ${CMAKE_COMMAND} -E env ${ENV_VARIABLES}
                                             ${R_EXECUTABLE} CMD INSTALL
                                                             -l ${CMAKE_CURRENT_BINARY_DIR}
                                                             --preclean
                                                             --build
                                                             FortMyrmidon
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   DEPENDS fort-myrmidon fort-myrmidon-utestdata
                   )
add_custom_target(r-fort-myrmidon ALL
                  DEPENDS ${R_FORT_MYRMIDON_PACKAGE})

if(IS_MAIN)
	add_custom_target(r-fort-myrmidon-check
	                  COMMAND ${CMAKE_COMMAND} -E env ${ENV_VARIABLES}
	                                           ${R_EXECUTABLE} CMD check FortMyrmidon --no-manual
	                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	                  DEPENDS fort-myrmidon fort-myrmidon-utestdata
	                  )
    add_dependencies(check r-fort-myrmidon-check)
endif(IS_MAIN)
