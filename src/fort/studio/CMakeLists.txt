set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SRC_FILES Format.cpp
              bridge/Bridge.cpp
              bridge/UniverseBridge.cpp
			  bridge/MeasurementBridge.cpp
			  bridge/GlobalPropertyBridge.cpp
			  bridge/IdentifierBridge.cpp
			  bridge/SelectedAntBridge.cpp
			  bridge/SelectedIdentificationBridge.cpp
			  bridge/ExperimentBridge.cpp
			  bridge/IdentifiedFrameConcurrentLoader.cpp
			  bridge/AntShapeTypeBridge.cpp
			  widget/base/TagFamilyComboBox.cpp
			  widget/base/ColorComboBox.cpp
			  widget/base/TimeEditorWidget.cpp
			  widget/base/QLongSpinBox.cpp
			  widget/vectorgraphics/VectorialView.cpp
			  widget/vectorgraphics/VectorialScene.cpp
			  widget/vectorgraphics/Handle.cpp
			  widget/vectorgraphics/Shape.cpp
			  widget/vectorgraphics/Vector.cpp
			  widget/vectorgraphics/Capsule.cpp
			  widget/vectorgraphics/Polygon.cpp
			  widget/vectorgraphics/Circle.cpp
			  widget/Logger.cpp
			  widget/GlobalPropertyWidget.cpp
			  widget/UniverseEditorWidget.cpp
			  widget/SpaceChoiceDialog.cpp
			  widget/MeasurementTypeWidget.cpp
			  widget/AntListWidget.cpp
			  widget/TaggingWidget.cpp
			  widget/AntIdentificationWidget.cpp
			  widget/IdentificationEditorWidget.cpp
			  widget/AntEditorWidget.cpp
			  widget/AntShapeTypeEditorWidget.cpp
			  widget/FrameFinderDialog.cpp
			  # QLongSpinBox.cpp
			  widget/MainWindow.cpp
			  )

set(HDR_FILES Format.hpp
              MyrmidonTypes.hpp
              Utils.hpp
			  bridge/Bridge.hpp
			  bridge/UniverseBridge.hpp
			  bridge/MeasurementBridge.hpp
			  bridge/GlobalPropertyBridge.hpp
			  bridge/IdentifierBridge.hpp
			  bridge/SelectedAntBridge.hpp
			  bridge/SelectedIdentificationBridge.hpp
			  bridge/ExperimentBridge.hpp
			  bridge/IdentifiedFrameConcurrentLoader.hpp
			  bridge/AntShapeTypeBridge.hpp
			  widget/base/TagFamilyComboBox.hpp
			  widget/base/ColorComboBox.hpp
			  widget/base/TimeEditorWidget.hpp
			  widget/base/QLongSpinBox.hpp
			  widget/vectorgraphics/VectorialView.hpp
			  widget/vectorgraphics/VectorialScene.hpp
			  widget/vectorgraphics/Handle.hpp
			  widget/vectorgraphics/Shape.hpp
			  widget/vectorgraphics/Vector.hpp
			  widget/vectorgraphics/Capsule.hpp
			  widget/vectorgraphics/Polygon.hpp
			  widget/vectorgraphics/Circle.hpp
			  widget/Logger.hpp
			  widget/GlobalPropertyWidget.hpp
			  widget/UniverseEditorWidget.hpp
			  widget/SpaceChoiceDialog.hpp
			  widget/MeasurementTypeWidget.hpp
			  widget/AntListWidget.hpp
			  widget/TaggingWidget.hpp
			  widget/AntIdentificationWidget.hpp
			  widget/IdentificationEditorWidget.hpp
			  widget/AntEditorWidget.hpp
			  widget/AntShapeTypeEditorWidget.hpp
			  widget/FrameFinderDialog.hpp
			  widget/MainWindow.hpp
			  )


set(UI_FILES widget/GlobalPropertyWidget.ui
             widget/UniverseEditorWidget.ui
             widget/MainWindow.ui
             widget/LoggerWidget.ui
             widget/SpaceChoiceDialog.ui
             widget/MeasurementTypeWidget.ui
             widget/AntListWidget.ui
             widget/TaggingWidget.ui
             widget/AntIdentificationWidget.ui
             widget/IdentificationEditorWidget.ui
             widget/base/TimeEditorWidget.ui
             widget/AntEditorWidget.ui
             widget/AntShapeTypeEditorWidget.ui
             widget/FrameFinderDialog.ui
			 )

set(RCC_FILES )


set(SRC_UTEST_FILES utest/main.cpp
                    utest/ExperimentBridgeUTest.cpp
                    utest/GlobalPropertyUTest.cpp
                    utest/UniverseUTest.cpp
                    utest/MeasurementUTest.cpp
                    utest/IdentifierUTest.cpp
                    utest/AntShapeTypeUTest.cpp
                    )

set(HDR_UTEST_FILES utest/ExperimentBridgeUTest.hpp
                    utest/GlobalPropertyUTest.hpp
                    utest/UniverseUTest.hpp
                    utest/MeasurementUTest.hpp
                    utest/IdentifierUTest.hpp
                    utest/AntShapeTypeUTest.hpp
                    )

include_directories(${PROJECT_BINARY_DIR}/src
                    ${PROJECT_BINARY_DIR}/src/myrmidon
                    ${PROJECT_BINARY_DIR}/src/fort-studio
                    ${PROJECT_SOURCE_DIR}/src
                    ${GLOG_INCLUDE_DIRS}
                    ${apriltag_INCLUDE_DIR})

add_library(fort-studio-common STATIC ${SRC_FILES}
                                      ${HDR_FILES}
                                      ${UI_FILES}
                                      ${RCC_FILES}
                                      )

target_link_libraries(fort-studio-common Qt5::Widgets Qt5::Concurrent fort-myrmidon fort-tags)

add_executable(fort-studio main.cpp)

target_link_libraries(fort-studio fort-studio-common)

set_target_properties(fort-studio PROPERTIES VERSION ${PROJECT_VERSION}
                                             SOVERSION ${VERSION_ABI})

install(TARGETS fort-studio DESTINATION bin)

add_executable(fort-studio-tests ${SRC_UTEST_FILES}
                                 ${HDR_UTEST_FILES}
                                 )

set_target_properties(fort-studio-tests PROPERTIES AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/src/fort/studio/widget)

target_link_libraries(fort-studio-tests fort-studio-common
                                        myrmidon-utest-common
                                        Qt5::Test
                                        gtest
                                        gmock
                                        )

add_test(NAME fort-studio-tests COMMAND fort-studio-tests)
add_dependencies(check fort-studio-tests)