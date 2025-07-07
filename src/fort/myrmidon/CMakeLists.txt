set(CMAKE_REQUIRED_LIBRARIES TBB::tbb)
check_cxx_source_compiles(
	"#include <tbb/flow_graph.h>

int main(){
	tbb::flow::graph g;
	tbb::flow::limiter_node<int> limiter {g,4};
	limiter.decrementer();
	return 0;
}
"
	MYRMIDON_TBB_HAVE_DECREMENTER
)

set(MYRMIDON_USE_SYSTEM_CATEGORY 1)

configure_file(config.h.in myrmidon-config.h)
configure_file(version.h.in myrmidon-version.h)

set(PROTO_FILES
	priv/proto/Experiment.proto
	priv/proto/AntDescription.proto
	priv/proto/Vector2d.proto
	priv/proto/Shapes.proto
	priv/proto/Time.proto
	priv/proto/TrackingDataDirectory.proto
	priv/proto/ExperimentFile.proto
	priv/proto/EstimateFile.proto
	priv/proto/TagCloseUpCache.proto
	priv/proto/TagFamily.proto
	priv/proto/Measurement.proto
	priv/proto/Zone.proto
	priv/proto/Space.proto
	priv/proto/AntMetadata.proto
	priv/proto/TagStatisticsCache.proto
)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILES})

foreach(f ${PROTO_SRC})
	set_property(SOURCE ${f} PROPERTY SKIP_AUTOGEN ON)
endforeach(f ${PROTO_SRC})

foreach(f ${PROTO_HDR})
	set_property(SOURCE ${f} PROPERTY SKIP_AUTOGEN ON)
endforeach(f ${PROTO_HDR})

set(HDR_PRIVATE_FILES
	utils/StringManipulation.hpp
	utils/ObjectPool.hpp
	priv/AntTimedValue.hpp
	priv/EigenRefs.hpp
	priv/ForwardDeclaration.hpp
	priv/LocatableTypes.hpp
	priv/TimeUtils.hpp
	priv/DenseMap.hpp
	priv/Space.hpp
	priv/Zone.hpp
	priv/Experiment.hpp
	priv/Identifier.hpp
	priv/Ant.hpp
	priv/AntShapeType.hpp
	priv/Identification.hpp
	priv/SegmentIndexer.hpp
	priv/TrackingDataDirectory.hpp
	priv/TrackingDataDirectoryError.hpp
	priv/RawFrame.hpp
	priv/DeletedReference.hpp
	priv/ExperimentReadWriter.hpp
	utils/NotYetImplemented.hpp
	priv/Isometry2D.hpp
	priv/TimeValid.hpp
	priv/MovieSegment.hpp
	priv/Typedefs.hpp
	priv/FrameReference.hpp
	priv/TagCloseUp.hpp
	priv/AntMetadata.hpp
	priv/proto/FileReadWriter.hpp
	priv/proto/ExperimentReadWriter.hpp
	priv/proto/TDDCache.hpp
	priv/proto/IOUtils.hpp
	priv/proto/TagStatisticsCache.hpp
	priv/proto/TagCloseUpCache.hpp
	utils/Checker.hpp
	utils/Defer.hpp
	priv/AntPoseEstimate.hpp
	priv/Measurement.hpp
	priv/KDTree.hpp
	priv/KDTree.impl.hpp
	priv/CollisionSolver.hpp
	priv/TagStatistics.hpp
	priv/Query.hpp
	priv/Matchers.hpp
	priv/TrackingSolver.hpp
	priv/QueryRunner.hpp
	priv/DataSegmenter.hpp
	handle/HandleInContext.hpp
	handle/AntHandle.hpp
	handle/ExperimentHandle.hpp
	handle/IdentificationHandle.hpp
	handle/SpaceHandle.hpp
	handle/ZoneDefinitionHandle.hpp
	handle/ZoneHandle.hpp
)

set(HDR_PUBLIC_FILES
	types/AntInteraction.hpp
	types/Value.hpp
	types/ValueUtils.hpp
	types/AntTrajectory.hpp
	types/Collision.hpp
	types/Color.hpp
	types/ComputedMeasurement.hpp
	types/ExperimentDataInfo.hpp
	types/FixableError.hpp
	types/ForwardDeclaration.hpp
	types/IdentifiedFrame.hpp
	types/MaybeDeref.hpp
	types/TagStatistics.hpp
	types/Traits.hpp
	types/TraitsCategory.hpp
	types/Typedefs.hpp
	types/Reporter.hpp
	utils/FileSystem.hpp
	Shapes.hpp
	Identification.hpp
	Ant.hpp
	Zone.hpp
	Space.hpp
	TrackingSolver.hpp
	Experiment.hpp
	Query.hpp
	Matchers.hpp
	Video.hpp
)

set(SRC_PRIVATE_FILES
	utils/StringManipulation.cpp
	priv/AntTimedValue.cpp
	priv/EigenRefs.cpp
	priv/TimeValid.cpp
	priv/Typedefs.cpp
	priv/LocatableTypes.cpp
	priv/TimeUtils.cpp
	priv/DenseMap.cpp
	priv/Space.cpp
	priv/Zone.cpp
	priv/Experiment.cpp
	priv/Identifier.cpp
	priv/Ant.cpp
	priv/AntShapeType.cpp
	priv/SegmentIndexer.cpp
	priv/Identification.cpp
	priv/TrackingDataDirectory.cpp
	priv/TrackingDataDirectoryError.cpp
	priv/RawFrame.cpp
	priv/DeletedReference.cpp
	priv/ExperimentReadWriter.cpp
	priv/Isometry2D.cpp
	priv/MovieSegment.cpp
	priv/FrameReference.cpp
	priv/TagCloseUp.cpp
	priv/AntMetadata.cpp
	priv/proto/FileReadWriter.cpp
	priv/proto/ExperimentReadWriter.cpp
	priv/proto/TDDCache.cpp
	priv/proto/IOUtils.cpp
	priv/proto/TagStatisticsCache.cpp
	priv/proto/TagCloseUpCache.cpp
	utils/Checker.cpp
	utils/Defer.cpp
	priv/AntPoseEstimate.cpp
	priv/Measurement.cpp
	priv/KDTree.cpp
	priv/CollisionSolver.cpp
	priv/TagStatistics.cpp
	priv/Query.cpp
	priv/Matchers.cpp
	priv/TrackingSolver.cpp
	priv/QueryRunner.cpp
	priv/DataSegmenter.cpp
	handle/AntHandle.cpp
	handle/ExperimentHandle.cpp
	handle/IdentificationHandle.cpp
	handle/SpaceHandle.cpp
	handle/ZoneDefinitionHandle.cpp
	handle/ZoneHandle.cpp
)

set(SRC_FILES
	types/AntInteraction.cpp
	types/Value.cpp
	types/ValueUtils.cpp
	types/AntTrajectory.cpp
	types/Collision.cpp
	types/Color.cpp
	types/ComputedMeasurement.cpp
	types/ExperimentDataInfo.cpp
	types/FixableError.cpp
	types/ForwardDeclaration.cpp
	types/IdentifiedFrame.cpp
	types/TagStatistics.cpp
	types/Traits.cpp
	types/Typedefs.cpp
	Shapes.cpp
	Identification.cpp
	Ant.cpp
	Zone.cpp
	Space.cpp
	TrackingSolver.cpp
	Experiment.cpp
	Query.cpp
	Matchers.cpp
	Video.cpp
)

set(COMMON_UTEST_SRC_FILES TestSetup.cpp UtilsUTest.cpp)

set(COMMON_UTEST_HDR_FILES TestSetup.hpp UtilsUTest.hpp)

set(SRC_UTEST_FILES
	main-check.cpp
	utils/NotYetImplementedUTest.cpp
	utils/ObjectPoolUTest.cpp
	priv/DenseMapUTest.cpp
	priv/TimedMapUTest.cpp
	priv/SpaceUTest.cpp
	priv/ZoneUTest.cpp
	priv/ExperimentUTest.cpp
	priv/ExperimentDataLessUTest.cpp
	priv/TimeUtilsUTest.cpp
	priv/TrackingDataDirectoryUTest.cpp
	priv/AntUTest.cpp
	priv/IdentifierUTest.cpp
	priv/IdentificationUTest.cpp
	priv/RawFrameUTest.cpp
	priv/Isometry2DUTest.cpp
	priv/SegmentIndexerUTest.cpp
	priv/TimeValidUTest.cpp
	priv/MovieSegmentUTest.cpp
	priv/FrameReferenceUTest.cpp
	priv/TagCloseUpUTest.cpp
	priv/proto/IOUtilsUTest.cpp
	priv/proto/FileReadWriterUTest.cpp
	priv/proto/ExperimentReadWriterUTest.cpp
	priv/proto/TDDCacheUTest.cpp
	priv/AntPoseEstimateUTest.cpp
	priv/MeasurementUTest.cpp
	priv/KDTreeUTest.cpp
	priv/AntMetadataUTest.cpp
	priv/CollisionSolverUTest.cpp
	priv/TagStatisticsUTest.cpp
	priv/MatchersUTest.cpp
	priv/LocatableTypesUTest.cpp
	priv/UtilsUTest.cpp
	AntUTest.cpp
	ExperimentUTest.cpp
	IdentificationUTest.cpp
	MatchersUTest.cpp
	QueryUTest.cpp
	ShapesUTest.cpp
	SpaceUTest.cpp
	TrackingSolverUTest.cpp
	TypesUTest.cpp
	ZoneUTest.cpp
	VideoUTest.cpp
	types/TraitsUTest.cpp
	types/ColorUTest.cpp
	types/ValueUTest.cpp
	types/ValueUtilsUTest.cpp
)

set(HDR_UTEST_FILES priv/UtilsUTest.hpp UtilsUTest.hpp)

set(UTESTDATA_SRC_FILES
	utest-data/UTestData.hpp
	utest-data/Config.hpp
	utest-data/GeneratedData.hpp
	utest-data/SegmentedDataWriter.hpp
	utest-data/HermesFileWriter.hpp
	utest-data/CloseUpWriter.hpp
	utest-data/MovieWriter.hpp
	utest-data/FrameDrawer.hpp
)
set(UTESTDATA_HDR_FILES
	utest-data/UTestData.cpp
	utest-data/Config.cpp
	utest-data/GeneratedData.cpp
	utest-data/HermesFileWriter.cpp
	utest-data/CloseUpWriter.cpp
	utest-data/MovieWriter.cpp
	utest-data/FrameDrawer.cpp
)

add_library(
	fort-myrmidon SHARED ${SRC_FILES} ${SRC_PRIVATE_FILES} ${HDR_PRIVATE_FILES}
						 ${HDR_PUBLIC_FILES} ${PROTO_HDR} ${PROTO_SRC}
)

add_library(fort-myrmdion::libfort-myrmidon ALIAS fort-myrmidon)

target_include_directories(
	fort-myrmidon PUBLIC ${PROJECT_BINARY_DIR}/src ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
	fort-myrmidon
	protobuf::libprotobuf
	${CXXFS_LIBRARY}
	fort-tags::libfort-tags
	fort-charis::libfort-time
	fort-hermes::libfort-hermes
	fort-charis::libfort-utils
	fort-charis::libfort-video
	TBB::tbb
	Threads::Threads
	yaml-cpp::yaml-cpp
	Eigen3::Eigen
	semver::semver
)

set_target_properties(
	fort-myrmidon PROPERTIES VERSION ${PROJECT_VERSION_API}
							 SOVERSION ${PROJECT_VERSION_ABI}
)

install(TARGETS fort-myrmidon DESTINATION lib)
foreach(f ${HDR_PUBLIC_FILES})
	get_filename_component(dir ${f} DIRECTORY)
	if(dir)
		install(FILES ${f} DESTINATION include/fort/myrmidon/${dir})
	else(dir)
		install(FILES ${f} DESTINATION include/fort/myrmidon)
	endif(dir)
endforeach(f ${HDR_PUBLIC_FILES})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/myrmidon-config.h
			  ${CMAKE_CURRENT_BINARY_DIR}/myrmidon-version.h
		DESTINATION include/fort/myrmidon
)

if(FORT_MYRMIDON_MAIN)
	add_library(
		fort-myrmidon-utestdata SHARED ${UTESTDATA_SRC_FILES}
									   ${UTESTDATA_HDR_FILES}
	)

	target_link_libraries(fort-myrmidon-utestdata fort-myrmidon)

	install(TARGETS fort-myrmidon-utestdata DESTINATION lib)
	install(FILES utest-data/UTestData.hpp utest-data/Config.hpp
			DESTINATION include/fort/myrmidon/utest-data/
	)

	add_library(
		fort-myrmidon-utest-common STATIC ${COMMON_UTEST_SRC_FILES}
										  ${COMMON_UTEST_HDR_FILES}
	)
	target_link_libraries(
		fort-myrmidon-utest-common gtest gmock fort-myrmidon-utestdata
	)

	add_executable(myrmidon-tests ${SRC_UTEST_FILES} ${HDR_UTEST_FILES})
	target_link_libraries(
		myrmidon-tests fort-myrmidon fort-myrmidon-utest-common GTest::gtest
		cpptrace::cpptrace
	)

	add_dependencies(check myrmidon-tests)
	add_test(NAME myrmidon-tests COMMAND myrmidon-tests)
endif(FORT_MYRMIDON_MAIN)

add_library(fort-myrmidon::libfort-myrmidon INTERFACE IMPORTED GLOBAL)
target_link_libraries(fort-myrmidon::libfort-myrmidon INTERFACE fort-myrmidon)
target_include_directories(
	fort-myrmidon::libfort-myrmidon INTERFACE ${PROJECT_SOURCE_DIR}/src
											  ${PROJECT_BINARY_DIR}/src
)

add_library(fort-myrmidon::libfort-myrmidon-utestdata INTERFACE IMPORTED GLOBAL)
target_link_libraries(
	fort-myrmidon::libfort-myrmidon-utestdata INTERFACE fort-myrmidon
														fort-myrmidon-utestdata
)
target_include_directories(
	fort-myrmidon::libfort-myrmidon-utestdata
	INTERFACE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
)

foreach(h ${HDR_PUBLIC_FILES})
	list(APPEND MYRMIDON_API_DOC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${h})
endforeach(h ${HDR_PUBLIC_FILES})
set(MYRMIDON_API_DOC_FILES
	${MYRMIDON_API_DOC_FILES}
	PARENT_SCOPE
)

foreach(h ${HDR_PRIVATE_FILES})
	list(APPEND MYRMIDON_PRIVATE_DOC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${h})
endforeach(h ${HDR_PRIVATE_FILES})
set(MYRMIDON_PRIVATE_DOC_FILES
	${MYRMIDON_PRIVATE_DOC_FILES}
	PARENT_SCOPE
)

add_executable(benchmarks benchmark/main.cpp)
target_link_libraries(benchmarks fort-myrmidon ${OpenCV_LIBS})

install(TARGETS yaml-cpp DESTINATION lib)
