# Build libfasst
ADD_SUBDIRECTORY(fasst)

# Build the three main executables
ADD_EXECUTABLE(comp-rx comp-rx.cpp)
TARGET_LINK_LIBRARIES(comp-rx fasst)
ADD_EXECUTABLE(model-estimation model-estimation.cpp)
TARGET_LINK_LIBRARIES (model-estimation fasst)
ADD_EXECUTABLE(source-estimation source-estimation.cpp)
TARGET_LINK_LIBRARIES(source-estimation fasst)

QT5_USE_Modules(comp-rx Xml)
QT5_USE_Modules(model-estimation Xml)
QT5_USE_Modules(source-estimation Xml)

IF(MSVC)
    # Copy Qt and libsndfile dll in executables directory
    FOREACH(QT_DLL Qt5Core.dll Qt5Xml.dll icuin51.dll icuuc51.dll icudt51.dll)
        ADD_CUSTOM_COMMAND(TARGET comp-rx POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            C:/Qt/${QTVERSION}/${QTSUBDIR}/bin/${QT_DLL}
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${QT_DLL})
    ENDFOREACH()
    ADD_CUSTOM_COMMAND(TARGET comp-rx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SNDFILE_DLL}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libsndfile-1.dll)

    # Add executables and dll to Windows installer
    INSTALL(TARGETS comp-rx model-estimation source-estimation
        RUNTIME DESTINATION bin)
    INSTALL(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        DESTINATION bin/
        FILES_MATCHING PATTERN "*.dll")
ENDIF()
