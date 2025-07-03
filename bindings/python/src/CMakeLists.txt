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
		fort_myrmidon MODULE ${SRC_FILES} ${HDR_FILES} WITH_SOABI
	)
	target_link_libraries(
		fort_myrmidon
		PRIVATE pybind11::headers
		PUBLIC fort-myrmidon::libfort-myrmidon
	)
	target_compile_definitions(
		fort_myrmidon PRIVATE VERSION_INFO=${PROJECT_VERSION}
	)

	install(TARGETS fort_myrmidon DESTINATION fort_myrmidon)

else(FORT_MYRMIDON_PYTHON_MAIN)
	pybind11_add_module(fort_myrmidon ${SRC_FILES} ${HDR_FILES})
	target_link_libraries(
		fort_myrmidon PRIVATE fort-myrmidon::libfort-myrmidon
	)

	# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code
	# as a define (VERSION_INFO) here.
	target_compile_definitions(
		fort_myrmidon PRIVATE VERSION_INFO=${fort-myrmidon_SEMVER}
	)

	pybind11_add_module(fort_myrmidon_utestdata UTestData.cpp)
	target_link_libraries(
		fort_myrmidon_utestdata
		PRIVATE fort-myrmidon::libfort-myrmidon-utestdata
	)

	target_compile_definitions(
		fort_myrmidon_utestdata PRIVATE VERSION_INFO=${fort-myrmidon_SEMVER}
	)
endif(FORT_MYRMIDON_PYTHON_MAIN)
