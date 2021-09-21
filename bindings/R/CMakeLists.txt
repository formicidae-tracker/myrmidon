find_program(R_EXECUTABLE R)
if(NOT R_EXECUTABLE)
	message(FATAL_ERROR "Could not find R")
endif(NOT R_EXECUTABLE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION.in
               ${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon/DESCRIPTION
               @ONLY)


set(R_FORT_MYRMIDON_PACKAGE ${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon_${PROJECT_VERSION}.tar.gz)
set(FORT_MYRMIDON_CXXFLAGS_ ${PROJECT_BINARY_DIR}/src
                            ${PROJECT_SOURCE_DIR}/src
                            ${FORT_TIME_INCLUDE_DIRS}
                            ${EIGEN3_INCLUDE_DIRS}
                            ${FORT_TAGS_INCLUDE_DIRS}
                            ${FORT_HERMES_INCLUDE_DIRS})
foreach(F ${FORT_MYRMIDON_CXXFLAGS_})
	list(APPEND FORT_MYRMIDON_CXXFLAGS -I${F})
endforeach(F ${FORT_TAGS_INCLUDE_DIRS_})

set(FORT_MYRMIDON_LDFLAGS -L$<TARGET_FILE_DIR:fort-myrmidon> -lfort-myrmidon -L$<TARGET_FILE_DIR:fort-time> -lfort-time)
set(FORT_MYRMIDON_LD_PATH $<TARGET_FILE_DIR:fort-myrmidon>:$<TARGET_FILE_DIR:fort-time>)
set(R_FORT_MYRMIDON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/FortMyrmidon)

set(ENV_VARIABLES FORT_MYRMIDON_CXXFLAGS='${FORT_MYRMIDON_CXXFLAGS}'
	              FORT_MYRMIDON_LDFLAGS='${FORT_MYRMIDON_LDFLAGS}'
	              LD_LIBRARY_PATH='${FORT_MYRMIDON_LD_PATH}'
	              R_FORT_MYRMIDON_PATH=${R_FORT_MYRMIDON_PATH}
	              )

file(GLOB R_FORT_MYRMIDON_FILES
	      CONFIGURE_DEPENDS
	      RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	      FortMyrmidon/src/*.h
	      FortMyrmidon/src/*.cpp
	      FortMyrmidon/R/*.R
	      FortMyrmidon/inst/include/*.hpp
	      )
list(REMOVE_ITEM R_FORT_MYRMIDON_FILES
	             FortMyrmidon/src/RcppExports.cpp
	             FortMyrmidon/R/RcppExports.R)

add_custom_command(OUTPUT ${R_FORT_MYRMIDON_PACKAGE}
                   COMMAND  ${CMAKE_COMMAND} -E env ${ENV_VARIABLES}
                                             ${R_EXECUTABLE} CMD INSTALL
                                                             -l ${CMAKE_CURRENT_BINARY_DIR}
                                                             --preclean
                                                             --build
                                                             FortMyrmidon
                   COMMAND  ${CMAKE_COMMAND} -E env ${ENV_VARIABLES}
                                             ${R_EXECUTABLE} CMD build
                                                             FortMyrmidon
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   DEPENDS fort-myrmidon fort-myrmidon-utestdata ${R_FORT_MYRMIDON_FILES}
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
