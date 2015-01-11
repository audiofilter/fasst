CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)
PROJECT(fasst)

# Set version
SET(CPACK_PACKAGE_VERSION 2.1.0)

# Specify output directory for every platform
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
IF(MSVC)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
ENDIF()

# Build Release by default (it needs to be manually selected on MSCV)
IF(NOT MSVC AND NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release"
        CACHE STRING "[Debug|Release|RelWithDebInfo|MixSizeRel]"
        FORCE)
ENDIF()

# Make sure Microsoft Visual Studio knows about M_PI
IF(MSVC)
    ADD_DEFINITIONS(-D_USE_MATH_DEFINES)
ENDIF()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

FIND_PACKAGE(Qt5Core) # REQUIRED COMPONENTS Widgets Core Gui)
#FIND_PACKAGE(Qt5Gui) # REQUIRED COMPONENTS Widgets Core Gui)

LINK_DIRECTORIES("/usr/local/lib")

# Try to find OpenMP
FIND_PACKAGE(OpenMP)
IF(OPENMP_FOUND)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF()

# Find Qt (Qt5 on Windows, Qt4 on GNU/Linux)
IF(WIN32)
    SET(QTVERSION "5.1.1")
    IF(MSVC10)
        SET(QTSUBDIR "msvc2010")
    ELSEIF(MSVC11)
        IF(CMAKE_CL_64)
            SET(QTSUBDIR "msvc2012_64")
        ELSE()
            SET(QTSUBDIR "msvc2012")
        ENDIF()
    ENDIF()
    SET(CMAKE_PREFIX_PATH "C:/Qt/${QTVERSION}/${QTSUBDIR}")
    FIND_PACKAGE(Qt5Xml)
ELSE()
    FIND_PACKAGE(Qt5 COMPONENTS Core Xml)
#    INCLUDE(${QT_USE_FILE})
ENDIF()

# Find libsndfile
IF(UNIX)
    SET(SNDFILE_LIB sndfile)
ELSEIF(WIN32)
    IF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64" AND NOT ${CMAKE_CL_64})
        SET(SNDFILE_PATH "C:/Program\ Files\ (x86)/Mega-Nerd/libsndfile")
    ELSE()
        SET(SNDFILE_PATH "C:/Program\ Files/Mega-Nerd/libsndfile")
    ENDIF()
    INCLUDE_DIRECTORIES(${SNDFILE_PATH}/include)
    SET(SNDFILE_LIB "${SNDFILE_PATH}/lib/libsndfile-1.lib")
    SET(SNDFILE_DLL "${SNDFILE_PATH}/bin/libsndfile-1.dll")
ENDIF()

# Find eigen
IF(UNIX)
    SET(EIGEN_PATH /usr/local/include/eigen3 /usr/include/eigen3)
		INCLUDE_DIRECTORIES(${EIGEN_PATH} "/usr/local/include" "/usr/local/opt/qt5/include" ${QT5_INCLUDES})
ELSEIF(WIN32)
    SET(EIGEN_PATH "C:/Program\ Files/Eigen/include/eigen3")
ENDIF()

# Don't use Eigen's parallelization because it slows down the program
ADD_DEFINITIONS( -DEIGEN_DONT_PARALLELIZE )

# Try to find Doxygen
FIND_PACKAGE(Doxygen)
IF(DOXYGEN_FOUND)
    CONFIGURE_FILE(doc/Doxyfile.in
        doc/Doxyfile
        @ONLY)
    ADD_CUSTOM_TARGET(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doc/Doxyfile
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc
        VERBATIM)
ENDIF(DOXYGEN_FOUND)

# If the TEST variable is set to true, activate tests and build test framework
IF(TEST)
    ENABLE_TESTING()

    # Build googletest framework
    IF(MSVC)
        ADD_DEFINITIONS(-D_VARIADIC_MAX=10)
    ENDIF()
    ADD_SUBDIRECTORY(gtest-1.6.0)
    INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    IF(MSVC)
        # Be sure to correctly link against googletest framework
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /MTd")
        SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /MT")
    ENDIF()
ENDIF()

# Add strict warnings
IF(MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /WX /wd4244;4267")
ELSE()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

    # clang compiler doesn't know #pragma omp
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unknown-pragmas")

    # if gcc version > 4.7, another warning is triggered by Eigen
    EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -dumpversion
      OUTPUT_VARIABLE GCC_VERSION)
    IF(GCC_VERSION VERSION_GREATER 4.7)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=aggressive-loop-optimizations")
    ENDIF()
ENDIF()

# Build fasst source tree
ADD_SUBDIRECTORY(src)

# Configure scripts
ADD_SUBDIRECTORY(scripts)

# Configure examples
ADD_SUBDIRECTORY(examples)

# Build a Windows installer
IF(MSVC11)
    SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")

    # Add visual studio dll
    INCLUDE(InstallRequiredSystemLibraries)

    # Add openmp dll
    IF(OPENMP_FOUND)
        IF(CMAKE_CL_64)
            SET(MSVCSUBDIR "x64")
        ELSE()
            SET(MSVCSUBDIR "x86")
        ENDIF()
        INSTALL(FILES
            "${MSVC11_REDIST_DIR}/${MSVCSUBDIR}/Microsoft.VC110.OPENMP/vcomp110.dll"
            DESTINATION bin)
    ENDIF()

    # Force the install path to be C:\Program Files
    SET(CPACK_NSIS_INSTALL_ROOT "C:\\\\Program Files")

    # Create an installer
    INCLUDE(CPack)
ENDIF()
