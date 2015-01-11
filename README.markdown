FASST Documentation {#mainpage}
===

[TOC]

This is the home page for the C++ implementation of the FASST toolbox. The content of this page is a quickstart guide for users. The other pages of this documentation describe the internal structure of FASST and are intended for developers.

In case you have any trouble and want to contact a human, you can send an email to fasst-support@inria.fr. We will do our best to answer quickly (we like feedback too).

# Download {#download}
* [Source in tar.gz format](files/fasst-2.1.0.tar.gz)
* [Source in zip format](files/fasst-2.1.0.zip)
* [Windows installer (32-bit)](files/fasst-2.1.0-win32.zip)
* [Windows installer (64-bit)](files/fasst-2.1.0-win64.zip)

# License {#license}
This software is released under the Q Public License Version 1.0. Please find a copy of the license at http://opensource.org/licenses/QPL-1.0.

# Reference {#reference}
If you use FASST and obtain scientific results that you publish, please acknowledge the usage of FASST by referencing the following articles:

\htmlonly
<iframe src="https://haltools.inria.fr/Public/afficheRequetePubli.php?titre_exp=A+General+Flexible+Framework+for+the+Handling+of+Prior+Information+in+Audio+Source+Separation&annee_publideb=2012&CB_auteur=oui&CB_titre=oui&CB_article=oui&langue=Anglais&tri_exp=annee_publi&tri_exp2=typdoc&tri_exp3=date_publi&ordre_aff=TA&Fen=Aff&css=../css/styles_publicationsHAL_frame.css"  frameborder="0" width="800" height="200">
</iframe>
<br>
<iframe src="https://haltools.inria.fr/Public/afficheRequetePubli.php?titre_exp=The+Flexible+Audio+Source+Separation+Toolbox+Version+2.0&CB_auteur=oui&CB_titre=oui&CB_article=oui&langue=Anglais&tri_exp=annee_publi&tri_exp2=typdoc&tri_exp3=date_publi&ordre_aff=TA&Fen=Aff&css=../css/styles_publicationsHAL_frame.css" frameborder="0" width="800" height="220">
</iframe>
\endhtmlonly


# Build FASST {#build-fasst}
The build has been tested and is known to succeed on the following systems:

* Ubuntu 12.04 (gcc, clang and Intel)
* CentOS 6.3 (gcc)
* OS X 10.7
* Windows XP (Visual Studio 2010)
* Windows 7 (Visual Studio 2010 & 2012)

\note If you use Windows, we recommend that you use one of the installers we provide, skip this section and directly [run the first example](\ref run-the-first-example). Nevertheless, if you want to build on Windows anyway, we have [a dedicated page for this](\ref build-windows).

## Install dependencies {#install-dependencies}
FASST has the following dependencies:

* [CMake](http://www.cmake.org/) to build
* [Qt](https://qt-project.org/) for the XML I/Os and the future GUI
* [libsndfile](http://www.mega-nerd.com/libsndfile/) for the WAV file I/Os
* [Eigen](http://eigen.tuxfamily.org/) for the linear algebra and the FFT
* (optional) [numpy](http://www.numpy.org/) for python scripts

### On GNU/Linux {#install-dependencies-linux}
On apt-based GNU/Linux distributions (_eg._ Ubuntu), use the following command:

    sudo apt-get install cmake libqt4-dev libsndfile1-dev python-numpy

On rpm-based GNU/Linux distributions (_eg._ CentOS), use the following command:

    sudo yum install cmake qt-devel libsndfile-devel numpy

You can choose to install Eigen with the package management system, but we recommend to install it manually for two reason: the packages provided by distributions are old, and the manual installation is very quick.

Simply enter the following commands:

    wget http://bitbucket.org/eigen/eigen/get/3.2.0.tar.bz2
    tar xjf 3.2.0.tar.bz2
    cd eigen-eigen-*
    mkdir build
    cd build
    cmake ..
    sudo make install

### On OS X {#install-dependencies-mac}
This guide assumes that you use Apple's developer tools and [the brew package manager](http://brew.sh/) (which is trivial to install and simplifies a lot of things for developers on OS X).

Install all the dependencies with the following commands:

    brew install cmake qt libsndfile eigen python
    pip install numpy

## Build {#build}

    cd fasst-*
    mkdir build
    cd build
    cmake ..
    make

# Run the examples {#run-the-examples}
\note If you built FASST from source, example scripts will be located in the build/examples directory.

The typical workflow is to use some scripts to initialize the source model, write it to an XML file and call FASST binaries.

## Run the first example {#run-the-first-example}
The first example script is provided in Python and MATLAB. It separates three musical sources from an instantaneous mixture.

### With Python {#run-the-python-example}
#### On GNU/Linux and OS X {#run-the-python-example-linux}

    python example.py

#### On Windows {#run-the-python-example-windows}
On Windows, we recommend that you install the pythonxy distribution: https://code.google.com/p/pythonxy/.

Then you can easily run the script by double-clicking it.

### With MATLAB {#run-the-matlab-example}
To run the MATLAB example, simply `cd` to the example directory and type `example`

## Run the second example {#run-the-second-example}
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

\note You have to be aware that some features that are present in the MATLAB version of FASST are still not implemented, here is a quick list:
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
