# Configure 3 main functions
SET(FASST_EXECUTABLE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
FOREACH(FILE_TO_CONFIGURE
        fasst_compute_mixture_covariance_matrix.m
        fasst_estimate_source_parameters.m
        fasst_estimate_sources.m)
    CONFIGURE_FILE(${FILE_TO_CONFIGURE}.in ${CMAKE_CURRENT_BINARY_DIR}/${FILE_TO_CONFIGURE} @ONLY)
ENDFOREACH()

# Copy other helper functions (all *.m files)
FILE(GLOB FILES_TO_COPY *.m)
FOREACH(FILE_TO_COPY ${FILES_TO_COPY})
    CONFIGURE_FILE(${FILE_TO_COPY} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
ENDFOREACH()

# Add to Windows installer
IF(MSVC11)
    # Reconfigure for package
    SET(FASST_EXECUTABLE_DIR "${CMAKE_INSTALL_PREFIX} ${CPACK_PACKAGE_VERSION}/bin")
    FOREACH(FILE_TO_CONFIGURE
            fasst_compute_mixture_covariance_matrix.m
            fasst_estimate_source_parameters.m
            fasst_estimate_sources.m)
        CONFIGURE_FILE(${FILE_TO_CONFIGURE}.in package/${FILE_TO_CONFIGURE} @ONLY)
        INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/package/${FILE_TO_CONFIGURE}
            DESTINATION scripts/MATLAB/)
    ENDFOREACH()
    INSTALL(DIRECTORY .
        DESTINATION scripts/MATLAB/
        FILES_MATCHING PATTERN "*.m")
ENDIF()
