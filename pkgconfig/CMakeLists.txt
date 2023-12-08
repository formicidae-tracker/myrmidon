set(PKG_FILE fort-myrmidon.pc.in)
if(USE_CXX17_FS_LIBRARY)
	set(PKG_FS_LIBRARY "-lstdc++fs")
else(USE_CXX17_FS_LIBRARY)
	set(PKG_FS_LIBRTARY "-lboost_filesystem")
endif(USE_CXX17_FS_LIBRARY)

configure_file(${PKG_FILE} fort-myrmidon.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fort-myrmidon.pc
		DESTINATION lib/pkgconfig
)
