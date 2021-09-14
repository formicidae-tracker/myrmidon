find_program(R_EXECUTABLE R)
if(NOT R_EXECUTABLE)
	message(FATAL_ERROR "Could not find R")
endif(NOT R_EXECUTABLE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon/DESCRIPTION.in
               ${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon/DESCRIPTION
               @ONLY)

set(R_FORT_MYRMIDON_PACKAGE ${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon_${PROJECT_VERSION}.tar.gz)


add_custom_command(OUTPUT ${R_FORT_MYRMIDON_PACKAGE}
                   COMMAND  ${CMAKE_COMMAND} -E env FORT_MYRMIDON_CXXFLAGS='-I${PROJECT_BINARY_DIR}/src -I${PROJECT_SOURCE_DIR}/src -I${FORT_TIME_INCLUDE_DIRS}'
                                                    FORT_MYRMIDON_LDFLAGS='-L${PROJECT_BINARY_DIR}/src/fort/myrmidon -lfort-myrmidon'
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
	                  COMMAND ${CMAKE_COMMAND} -E env FORT_MYRMIDON_CXXFLAGS="-I${PROJECT_BINARY_DIR}/src -I${PROJECT_SOURCE_DIR}/src"
	                                                  FORT_MYRMIDON_LDFLAGS="-L${PROJECT_BINARY_DIR}/src/fort/myrmidon -lfort-myrmidon"
	                                                  ${R_EXECUTABLE} CMD check FortMyrmidon
	                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	                  DEPENDS fort-myrmidon fort-myrmidon-utestdata
	                  )
    add_dependencies(check r-fort-myrmidon-check)
endif(IS_MAIN)
