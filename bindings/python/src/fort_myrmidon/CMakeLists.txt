set(SRC_FILES
	Time.cpp
	Types.cpp
	Experiment.cpp
	Identification.cpp
	Shapes.cpp
	Ant.cpp
	Matchers.cpp
	Query.cpp
	Color.cpp
	Zone.cpp
	Space.cpp
	TrackingSolver.cpp
	Video.cpp
	main.cpp
	Progress.cpp
)

set(HDR_FILES BindMethods.hpp Progress.hpp)

if(FORT_MYRMIDON_PYTHON_MAIN)
	python_add_library(
		_fort_myrmidon MODULE ${SRC_FILES} ${HDR_FILES} WITH_SOABI
	)
	target_link_libraries(
		_fort_myrmidon
		PRIVATE pybind11::headers
		PUBLIC fort-myrmidon::libfort-myrmidon
	)
	target_compile_definitions(
		_fort_myrmidon PRIVATE VERSION_INFO=${PROJECT_VERSION}
							   FM_PYTHON_PACKAGE_NAME=_fort_myrmidon
	)
	target_compile_options(_fort_myrmidon PRIVATE -fvisibility=hidden)

	install(TARGETS _fort_myrmidon DESTINATION fort_myrmidon)
	install(FILES __init__.py DESTINATION fort_myrmidon)

	install(FILES _fort_myrmidon.pyi DESTINATION fort_myrmidon)

else(FORT_MYRMIDON_PYTHON_MAIN)
	pybind11_add_module(_fort_myrmidon ${SRC_FILES} ${HDR_FILES})
	target_link_libraries(
		_fort_myrmidon PRIVATE fort-myrmidon::libfort-myrmidon
	)

	# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code
	# as a define (VERSION_INFO) here.
	target_compile_definitions(
		_fort_myrmidon PRIVATE VERSION_INFO=${fort-myrmidon_SEMVER}
							   FM_PYTHON_PACKAGE_NAME=_fort_myrmidon
	)
	target_compile_options(_fort_myrmidon PRIVATE -fvisibility=hidden)

	pybind11_add_module(fort_myrmidon_utestdata UTestData.cpp)
	target_link_libraries(
		fort_myrmidon_utestdata
		PRIVATE fort-myrmidon::libfort-myrmidon-utestdata
	)
	set_target_properties(
		fort_myrmidon_utestdata PROPERTIES LIBRARY_OUTPUT_DIRECTORY
										   ${CMAKE_CURRENT_BINARY_DIR}/..
	)

	target_compile_definitions(
		fort_myrmidon_utestdata PRIVATE VERSION_INFO=${fort-myrmidon_SEMVER}
	)

	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/__init__.py
		COMMAND
			${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
			${CMAKE_CURRENT_BINARY_DIR}/__init__.py
		DEPENDS __init__.py _fort_myrmidon
	)

	add_custom_target(
		setup_fort_myrmidon_py_module ALL
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/__init__.py _fort_myrmidon
	)

	find_program(PYBIND11_STUBGEN_EXECUTABLE pybind11-stubgen REQUIRED)

	add_custom_target(
		fort_myrmidon-python-index ALL
		DEPENDS setup_fort_myrmidon_py_module
		COMMAND
			${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}
			${PYBIND11_STUBGEN_EXECUTABLE} _fort_myrmidon --ignore-all-errors -o
			.
		COMMAND ${CMAKE_COMMAND} -E copy _fort_myrmidon.pyi
				${CMAKE_CURRENT_SOURCE_DIR}/_fort_myrmidon.pyi
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating .pyi for fort_myrmidon"
	)

endif(FORT_MYRMIDON_PYTHON_MAIN)
