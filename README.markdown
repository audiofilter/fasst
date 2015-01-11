FASST Documentation
===

This is a copy of FASST toolbox modified to build with QT5 on Mac OS X

# License
This software is released under the Q Public License Version 1.0. Please find a copy of the license at http://opensource.org/licenses/QPL-1.0.

## Install dependencies
FASST has the following dependencies:

* [CMake](http://www.cmake.org/) to build
* [Qt](https://qt-project.org/) for the XML I/Os and the future GUI
* [libsndfile](http://www.mega-nerd.com/libsndfile/) for the WAV file I/Os
* [Eigen](http://eigen.tuxfamily.org/) for the linear algebra and the FFT
* (optional) [numpy](http://www.numpy.org/) for python scripts

### On OS X 
This guide assumes that you use Apple's developer tools and [the brew package manager](http://brew.sh/) (which is trivial to install and simplifies a lot of things for developers on OS X).

Install all the dependencies with the following commands:

    brew install cmake qt libsndfile eigen python
    pip install numpy

## Build

    mkdir build
    cd build
    cmake ..
    make

# Run the examples 
Example scripts will be located in the build/examples directory.

The typical workflow is to use some scripts to initialize the source model, write it to an XML file and call FASST binaries.

## Run the first example
The first example script is provided in Python and MATLAB. It separates three musical sources from an instantaneous mixture.

### With Python 

    python example.py

### With MATLAB 
To run the MATLAB example, simply `cd` to the example directory and type `example`

## Run the second example 
The second example script is provided in MATLAB. It performs source separation on noisy speech data from the first track of the 2nd CHiME challenge (http://spandh.dcs.shef.ac.uk/chime_challenge/). To run this script, you will need:
* The development and/or the test dataset downloadable at http://spandh.dcs.shef.ac.uk/chime_challenge/chime2_task1.html
* The following archive containing additional annotation files and models: [tar](files/CHiME_baseline.tar.gz) and [zip](files/CHiME_baseline.zip)

In addition, you need to edit the script to define the directories where the data and the models are.

The script takes 3 arguments:
1. The dataset ('devel' or 'test')
2. The SNR level ('m6dB', 'm3dB', '0dB', '3dB', '6dB', '9dB')
3. The filename (something like 's1_bgaa9a.wav')

# Write your own script {#write-your-own-script}
As a prerequisite, you need to be familiar with the FASST source model. If not, you can refer to [this paper](http://hal.inria.fr/hal-00626962/) where it is fully described.

You have to be aware that some features that are present in the MATLAB version of FASST are still not implemented, here is a quick list:
* ERB transform
* GMM and HMM spectral models
* Partially-adaptative spectral parameters
* Only one additive noise model

FASST is decomposed in 3 different executables. Here are their descriptions and usages:

## Compute mixture covariance matrix

    Usage:  comp-rx input-wav-file input-xml-file output-bin-file

## Estimate source parameters

    Usage:  model-estimation input-xml-file input-bin-file output-xml-file

## Separate sources

    Usage:  source-estimation input-wav-file input-xml-file output-wav-dir
