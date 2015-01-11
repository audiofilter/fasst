# Configure python module
SET(FASST_EXECUTABLE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
CONFIGURE_FILE(fasst.py.in ${CMAKE_CURRENT_BINARY_DIR}/fasst.py @ONLY)

IF(TEST)
    ADD_TEST(python_test
        python fasst.py -v)
ENDIF()

# Add to Windows installer
IF(MSVC11)
    # Reconfigure for package
    SET(FASST_EXECUTABLE_DIR "${CMAKE_INSTALL_PREFIX} ${CPACK_PACKAGE_VERSION}/bin")
    CONFIGURE_FILE(fasst.py.in package/fasst.py @ONLY)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/package/fasst.py
        DESTINATION scripts/python/)
ENDIF()
