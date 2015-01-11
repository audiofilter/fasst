# Configure example script
SET(FASST_MATLAB_DIR ${CMAKE_BINARY_DIR}/scripts/MATLAB)
CONFIGURE_FILE(example.m.in ${CMAKE_CURRENT_BINARY_DIR}/example.m @ONLY)
SET(FASST_PYTHON_DIR ${CMAKE_BINARY_DIR}/scripts/python)
CONFIGURE_FILE(example.py.in ${CMAKE_CURRENT_BINARY_DIR}/example.py @ONLY)

# Copy input data
CONFIGURE_FILE(Shannon_Hurley__Sunrise__inst__mix.wav ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# Add to Windows installer
IF(MSVC11)
    SET(FASST_MATLAB_DIR "${CMAKE_INSTALL_PREFIX} ${CPACK_PACKAGE_VERSION}/scripts/MATLAB")
    CONFIGURE_FILE(example.m.in ${CMAKE_BINARY_DIR}/package/example1/example.m @ONLY)

    SET(FASST_PYTHON_DIR "${CMAKE_INSTALL_PREFIX} ${CPACK_PACKAGE_VERSION}/scripts/python")
    CONFIGURE_FILE(example.py.in ${CMAKE_BINARY_DIR}/package/example1/example.py @ONLY)

    FILE(COPY Shannon_Hurley__Sunrise__inst__mix.wav DESTINATION ${CMAKE_BINARY_DIR}/package/example1/)
ENDIF()
