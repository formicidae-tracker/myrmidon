message(STATUS "Removing ${CMAKE_INSTALL_PREFIX}/include")
file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/include)
file(GLOB LIBRARIES ${CMAKE_INSTALL_PREFIX}/lib/*.so*
	 ${CMAKE_INSTALL_PREFIX}/lib64/*.so*
)
foreach(l ${LIBRARIES})
	message(STATUS "Moving ${l} to ${CMAKE_INSTALL_PREFIX}/py_fort_myrmidon")
	file(COPY ${l} DESTINATION ${CMAKE_INSTALL_PREFIX}/py_fort_myrmidon)
endforeach(l ${LIBRARIES})
message(STATUS "Removing ${CMAKE_INSTALL_PREFIX}/lib")
file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/lib)
message(STATUS "Removing ${CMAKE_INSTALL_PREFIX}/lib64")
file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/lib64)
