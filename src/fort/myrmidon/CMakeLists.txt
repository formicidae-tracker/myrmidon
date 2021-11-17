include_directories(${OPENCV_INCLUDE_DIRS}
                    ${YAML_CPP_INCLUDE_DIR})

include(CheckCXXSourceRuns)

set(MYRMIDON_USE_SYSTEM_CATEGORY 1)

configure_file(config.h.in myrmidon-config.h)
configure_file(version.h.in myrmidon-version.h)


set(PROTO_FILES priv/proto/Experiment.proto
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

include_directories(${PROJECT_BINARY_DIR}/src
                    ${PROJECT_SOURCE_DIR}/src)

set(HDR_PRIVATE_FILES utils/StringManipulation.hpp
                      utils/ObjectPool.hpp
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
                      priv/RawFrame.hpp
                      priv/DeletedReference.hpp
                      priv/ExperimentReadWriter.hpp
                      utils/NotYetImplemented.hpp
                      priv/Isometry2D.hpp
                      priv/TimeValid.hpp
                      priv/MovieSegment.hpp
                      priv/Types.hpp
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
					  handle/ZoneDefinitionHandle.hpp
					  handle/ZoneHandle.hpp
					  handle/SpaceHandle.hpp
                      )


set(HDR_PUBLIC_FILES ForwardDeclaration.hpp
                     Typedefs.hpp
                     MaybeDeref.hpp
                     TraitCategory.hpp
                     Traits.hpp
                     Types.hpp
                     Color.hpp
                     Shapes.hpp
                     Identification.hpp
                     Ant.hpp
                     Zone.hpp
                     Space.hpp
                     TrackingSolver.hpp
                     Experiment.hpp
                     Query.hpp
                     Matchers.hpp
                     MovieSegmentData.hpp
                     utils/FileSystem.hpp)

set(SRC_PRIVATE_FILES utils/StringManipulation.cpp
                      priv/TimeValid.cpp
                      priv/Types.cpp
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
                      )

set(SRC_FILES ForwardDeclaration.cpp
              Types.cpp
              Color.cpp
              Shapes.cpp
              Identification.cpp
              Ant.cpp
              Zone.cpp
              Space.cpp
              TrackingSolver.cpp
              Experiment.cpp
              Query.cpp
              Matchers.cpp
	          )

set(COMMON_UTEST_SRC_FILES TestSetup.cpp
                           UtilsUTest.cpp
                           )

set(COMMON_UTEST_HDR_FILES TestSetup.hpp
                           UtilsUTest.hpp
                           )


set(SRC_UTEST_FILES main-check.cpp
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
                    ColorUTest.cpp
                    ExperimentUTest.cpp
                    IdentificationUTest.cpp
                    MatchersUTest.cpp
                    QueryUTest.cpp
                    ShapesUTest.cpp
                    SpaceUTest.cpp
                    TrackingSolverUTest.cpp
                    TypesUTest.cpp
                    ZoneUTest.cpp
                    )


set(HDR_UTEST_FILES priv/UtilsUTest.hpp
                    UtilsUTest.hpp
                    )

set(UTESTDATA_SRC_FILES utest-data/UTestData.hpp
                        utest-data/Config.hpp
                        utest-data/GeneratedData.hpp
                        utest-data/SegmentedDataWriter.hpp
                        utest-data/HermesFileWriter.hpp
                        utest-data/CloseUpWriter.hpp
                        utest-data/MovieWriter.hpp
                        utest-data/FrameDrawer.hpp
                       )
set(UTESTDATA_HDR_FILES utest-data/UTestData.cpp
                        utest-data/Config.cpp
                        utest-data/GeneratedData.cpp
                        utest-data/HermesFileWriter.cpp
                        utest-data/CloseUpWriter.cpp
                        utest-data/MovieWriter.cpp
                        utest-data/FrameDrawer.cpp
                        )

add_library(fort-myrmidon SHARED ${SRC_FILES}
                                 ${SRC_PRIVATE_FILES}
                                 ${HDR_PRIVATE_FILES}
                                 ${HDR_PUBLIC_FILES}
                                 ${PROTO_HDR}
                                 ${PROTO_SRC})

target_link_libraries(fort-myrmidon ${PROTOBUF_LIBRARIES}
                                    fort-hermes-cpp
                                    ${CXXFS_LIBRARY}
                                    fort-tags
                                    fort-time
                                    ${OpenCV_LIBS}
                                    TBB::tbb
                                    Threads::Threads
                                    ${YAML_CPP_LIBRARIES}
                                    )

set_target_properties(fort-myrmidon PROPERTIES
                                    VERSION ${PROJECT_VERSION_API}
                                    SOVERSION ${PROJECT_VERSION_ABI})

install(TARGETS fort-myrmidon DESTINATION lib)
install(FILES ${HDR_PUBLIC_FILES} DESTINATION include/fort/myrmidon)

add_library(fort-myrmidon-utestdata SHARED ${UTESTDATA_SRC_FILES}
	                                       ${UTESTDATA_HDR_FILES})

target_link_libraries(fort-myrmidon-utestdata fort-myrmidon)

install(TARGETS fort-myrmidon-utestdata DESTINATION lib)
install(FILES utest-data/UTestData.hpp utest-data/Config.hpp
        DESTINATION include/fort/myrmidon/utest-data/)

add_library(fort-myrmidon-utest-common STATIC ${COMMON_UTEST_SRC_FILES}
                                              ${COMMON_UTEST_HDR_FILES}
                                              )
target_link_libraries(fort-myrmidon-utest-common gtest gmock fort-myrmidon-utestdata)

add_executable(myrmidon-tests ${SRC_UTEST_FILES}
                              ${HDR_UTEST_FILES}
                              )
target_link_libraries(myrmidon-tests fort-myrmidon
                                     fort-myrmidon-utest-common
                                     gtest)

add_test(NAME myrmidon-tests COMMAND myrmidon-tests)
add_dependencies(check myrmidon-tests)

foreach(h ${HDR_PUBLIC_FILES})
	list(APPEND MYRMIDON_API_DOC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${h})
endforeach(h ${HDR_PUBLIC_FILES})
set(MYRMIDON_API_DOC_FILES ${MYRMIDON_API_DOC_FILES} PARENT_SCOPE)

foreach(h ${HDR_PRIVATE_FILES})
	list(APPEND MYRMIDON_PRIVATE_DOC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${h})
endforeach(h ${HDR_PRIVATE_FILES})
set(MYRMIDON_PRIVATE_DOC_FILES ${MYRMIDON_PRIVATE_DOC_FILES} PARENT_SCOPE)

add_executable(benchmarks benchmark/main.cpp)
target_link_libraries(benchmarks fort-myrmidon)
