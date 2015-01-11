ADD_LIBRARY(fasst
    Audio.cpp
    TFRepr.cpp
    ERBRepr.cpp
    MixCovMatrix.cpp
    MixingParameter.cpp
    NonNegMatrix.cpp
    SpectralPower.cpp
    Source.cpp
    Sources.cpp
    NaturalStatistics.cpp
    XMLDoc.cpp)

# Link with Qt
IF(WIN32)
    qt5_use_modules(fasst Xml)
ELSE()
    TARGET_LINK_LIBRARIES(fasst ${QT_LIBRARIES})
ENDIF()

# Link with libsndfile
TARGET_LINK_LIBRARIES(fasst ${SNDFILE_LIB})

IF(TEST)
    # Find input data during tests
    SET_SOURCE_FILES_PROPERTIES(Audio_test.cpp
        PROPERTIES COMPILE_DEFINITIONS INPUT_DATA_DIR=\"${CMAKE_BINARY_DIR}/examples/example1\")

    MACRO(unit_test class)
        ADD_EXECUTABLE(${class}_test ${class}_test.cpp)
        TARGET_LINK_LIBRARIES(${class}_test gtest gtest_main fasst)
        ADD_TEST(${class}_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${class}_test)
    ENDMACRO()

    unit_test(Audio)
    unit_test(TFRepr)
    unit_test(NonNegMatrix)
    unit_test(MixCovMatrix)
    unit_test(MixingParameter)
    unit_test(Sources)
    unit_test(Source)
ENDIF(TEST)
