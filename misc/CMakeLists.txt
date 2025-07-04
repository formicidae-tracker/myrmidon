option(XDESKTOP_APPSTREAM_SUPPORT
	   "Build and install the Freedesktop appstream data" OFF
)

if(XDESKTOP_APPSTREAM_SUPPORT)
	configure_file(
		${PROJECT_SOURCE_DIR}/misc/io.github.formicidae_tracker.Studio.desktop.in
		${PROJECT_BINARY_DIR}/misc/io.github.formicidae_tracker.Studio.desktop
		@ONLY
	)

	install(
		FILES
			${PROJECT_BINARY_DIR}/misc/io.github.formicidae_tracker.Studio.desktop
		DESTINATION share/applications/
	)

	install(
		FILES ${PROJECT_SOURCE_DIR}/resources/icons/fort-studio.svg
		DESTINATION share/icons/hicolor/scalable/apps
		RENAME io.github.formicidae_tracker.Studio.svg
	)

	install(
		FILES
			${PROJECT_SOURCE_DIR}/misc/io.github.formicidae_tracker.Studio.metainfo.xml
		DESTINATION share/metainfo/
	)

endif(XDESKTOP_APPSTREAM_SUPPORT)
