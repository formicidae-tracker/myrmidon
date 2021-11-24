set(SRC_FILES Time.cpp
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
              )

set(HDR_FILES BindMethods.hpp)

pybind11_add_module(py_fort_myrmidon ${SRC_FILES} ${HDR_FILES})
target_link_libraries(py_fort_myrmidon PRIVATE ${FORT_MYRMIDON_LIBRARIES})

pybind11_add_module(py_fort_myrmidon_utestdata UTestData.cpp)
target_link_libraries(py_fort_myrmidon_utestdata PRIVATE ${FORT_MYRMIDON_UTESTDATA_LIBRARIES})
