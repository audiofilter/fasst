Build on Windows {#build-windows}
===

# Install dependencies
## CMake
1. Visit [http://www.cmake.org/](http://cmake.org/)
2. Download the Win32 installer
3. Run the installer

## Qt
1. Visit [https://qt-project.org/](https://qt-project.org/)
2. Download Qt Online Installer for Windows
3. Run the installer

## libsndfile
1. Visit [http://www.mega-nerd.com/libsndfile/](http://www.mega-nerd.com/libsndfile/)
2. Download the Windows installer that corresponds to your hardware
3. Run the installer

## Eigen
1. Visit [http://eigen.tuxfamily.org/](http://eigen.tuxfamily.org/)
2. Download the last zip and unzip it
3. Run CMake in the eigen source directory
4. Open a Visual Studio command prompt as an admin, cd to eigen build directory and enter the following command: `msbuild INSTALL.vcxproj`

# Build
1. Run CMake in the fasst source directory. It is recommended to build in a new subdirectory to avoid mixing source files and built files. It should not be necessary to manually modify any CMake variable.
\note Choose the CMake generator depending on your system:
  * `Visual Studio 11 Win64` for Visual Studio 2012 and 64-bit hardware
  * `Visual Studio 11` for Visual Studio 2012 and 32-bit hardware
  * `Visual Studio 10` for Visual Studio 2010

2. Build with Visual Studio. Simply open the fasst.sln file in the build directory with Visual Studio. 
\warning We highly recommend that you manually select the Release build instead of the Debug build, as it makes FASST run much faster. In addition, the Debug build might be broken.

To build, simply click Build > Build Solution, or hit F7.
