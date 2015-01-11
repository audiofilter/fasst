#include <iostream>
#include "MixCovMatrix.h"
#include "Audio.h"
#include "gtest/gtest.h"

using namespace std;
using namespace Eigen;

TEST(MixCovMatrix, allZeros) {
  // input: 8 samples, 1 channel, x=0, wlen=4
  // assert: every covariance matrices are zero
  fasst::Audio x(ArrayXXd::Zero(8, 1));
  int wlen = 4;

  fasst::MixCovMatrix Rx(x, "STFT", wlen, 0);
  ASSERT_EQ(Rx.channels(), 1);
  ASSERT_EQ(Rx.bins(), 3);
  ASSERT_EQ(Rx.frames(), 4);
  for (int n = 0; n < Rx.frames(); n++) {
    for (int f = 0; f < Rx.bins(); f++) {
      ASSERT_EQ(Rx(f, n)(0, 0).real(), 0.);
    }
  }
}

TEST(MixCovMatrix, allOnes) {
  // input: 8 samples, 1 channel, x=1, wlen=4
  // assert: every covariance matrices are equals except the edges
  fasst::Audio x(ArrayXXd::Ones(8, 1));
  int wlen = 4;

  fasst::MixCovMatrix Rx(x, "STFT", wlen, 0);
  for (int f = 0; f < Rx.bins(); f++) {
    ASSERT_EQ(Rx(f, 1)(0, 0).real(), Rx(f, 2)(0, 0).real());
  }
}

TEST(MixCovMatrix, firstSampleIsOne) {
  // input: 8 samples, 1 channel, (x=0, x(0)=1), wlen=4
  // assert: every covariance matrices are zero except the first one
  ArrayXXd array = ArrayXXd::Zero(8, 1);
  array(0, 0) = 1.;
  fasst::Audio x(array);
  int wlen = 4;

  fasst::MixCovMatrix Rx(x, "STFT", wlen, 0);
  for (int n = 1; n < Rx.frames(); n++) {
    for (int f = 0; f < Rx.bins(); f++) {
      ASSERT_EQ(Rx(f, n)(0, 0).real(), 0.);
    }
  }
}

TEST(MixCovMatrix, isHermitian) {
  // input: 8 samples, 2 channels, x=rand, wlen=4
  // assert: diagonal elements are real
  // assert: upper triangular is lower triangular conjugate
  ArrayXXd array = ArrayXXd::Random(8, 2);
  fasst::Audio x(array);
  int wlen = 4;

  fasst::MixCovMatrix Rx(x, "STFT", wlen, 0);
  for (int f = 0; f < Rx.bins(); f++) {
    for (int n = 0; n < Rx.frames(); n++) {
      ASSERT_EQ(Rx(f, n)(0, 0).imag(), 0.);
      ASSERT_EQ(Rx(f, n)(1, 1).imag(), 0.);
      ASSERT_EQ(Rx(f, n)(0, 1).real(), Rx(f, n)(1, 0).real());
      ASSERT_EQ(Rx(f, n)(0, 1).imag(), -Rx(f, n)(1, 0).imag());
    }
  }
}

TEST(MixCovMatrix, twoIdenticalChannels) {
  // input: 8 samples, 2 channels, x(0)=x(1)=rand, wlen=4
  // assert: every elements should be real
  ArrayXXd random = ArrayXXd::Random(8, 1);
  ArrayXXd array(8, 2);
  array.col(0) = random;
  array.col(1) = random;
  fasst::Audio x(array);
  int wlen = 4;

  fasst::MixCovMatrix Rx(x, "STFT", wlen, 0);
  for (int n = 1; n < Rx.frames(); n++) {
    for (int f = 0; f < Rx.bins(); f++) {
      ASSERT_DOUBLE_EQ(Rx(f, n)(1, 0).imag(), 0.);
      ASSERT_DOUBLE_EQ(Rx(f, n)(0, 1).imag(), 0.);
    }
  }
}

TEST(MixCovMatrix, readwrite) {
  ArrayXXd array = ArrayXXd::Ones(8, 2);
  fasst::Audio x(array);
  int wlen = 4;

  fasst::MixCovMatrix Rx1(x, "STFT", wlen, 0);
  Rx1.write("tmp.bin");

  fasst::MixCovMatrix Rx2("tmp.bin");
  //Rx2.write("read.bin");
  ASSERT_EQ(Rx1.channels(), Rx2.channels());
  ASSERT_EQ(Rx1.bins(), Rx2.bins());
  ASSERT_EQ(Rx1.frames(), Rx2.frames());
  for (int n = 0; n < Rx1.frames(); n++) {
    for (int f = 0; f < Rx1.bins(); f++) {
      /*
      cout << "Rx(" << f << "," << n << "):\n";
      cout << Rx1(f,n) << "\n\n";
      */
      for (int i1 = 0; i1 < Rx1.channels(); i1++) {
        for (int i2 = 0; i1 < Rx1.channels(); i1++) {
          ASSERT_FLOAT_EQ(static_cast<float>(Rx1(f, n)(i1, i2).real()),
                          static_cast<float>(Rx2(f, n)(i1, i2).real()));
          ASSERT_FLOAT_EQ(static_cast<float>(Rx1(f, n)(i1, i2).imag()),
                          static_cast<float>(Rx2(f, n)(i1, i2).imag()));
        }
      }
    }
  }
}
