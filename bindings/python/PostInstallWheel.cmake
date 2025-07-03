message(STATUS "Removing ${CMAKE_INSTALL_PREFIX}/include")

file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/include)
file(GLOB LIBRARIES
	 ${CMAKE_INSTALL_PREFIX}/lib/*${CMAKE_SHARED_LIBRARY_SUFFIX}*
	 ${CMAKE_INSTALL_PREFIX}/lib64/*${CMAKE_SHARED_LIBRARY_SUFFIX}*
)

set(PACKAGED_LIB_DIR ${CMAKE_INSTALL_PREFIX}/fort_myrmidon/lib)

if(EXISTS /tmp/cibuildwheel)
	set(PACKAGED_LIB_DIR /tmp/cibuildwheel/lib/fort_myrmidon.libs)
	file(REMOVE_RECURSE ${PACKAGED_LIB_DIR})
	file(MAKE_DIRECTORY ${PACKAGED_LIB_DIR})
else(EXISTS /tmp/cibuildwheel)
	file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/fort_myrmidon/lib)
endif(EXISTS /tmp/cibuildwheel)

foreach(l ${LIBRARIES})
	message(STATUS "Moving ${l} to ${PACKAGED_LIB_DIR}")
	file(COPY ${l} DESTINATION ${PACKAGED_LIB_DIR})
endforeach(l ${LIBRARIES})
message(STATUS "Removing ${CMAKE_INSTALL_PREFIX}/lib")
file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/lib)
message(STATUS "Removing ${CMAKE_INSTALL_PREFIX}/lib64")
file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/lib64)
