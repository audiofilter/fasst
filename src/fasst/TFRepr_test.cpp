#include "TFRepr.h"
#include "Audio.h"
#include "gtest/gtest.h"

using namespace std;
using namespace Eigen;

TEST(TFRepr, allZeros) {
  // input: 8 samples, 1 channel, x=0, wlen=4
  // assert: every frames are zero
  fasst::Audio x(ArrayXXd::Zero(8, 1));
  int wlen = 4;

  fasst::TFRepr X(x, wlen);
  ASSERT_EQ(X(0, 0).size(), 1);
  ASSERT_EQ(X.bins(), 3);
  ASSERT_EQ(X.frames(), 4);
  for (int n = 0; n < X.frames(); n++) {
    for (int f = 0; f < X.bins(); f++) {
      ASSERT_EQ(X(f, n)(0).real(), 0.);
      ASSERT_EQ(X(f, n)(0).imag(), 0.);
    }
  }
}

TEST(TFRepr, allOnes) {
  // input: 8 samples, 1 channel, x=1, wlen=4
  // assert: every frames are equals except the edges
  fasst::Audio x(ArrayXXd::Ones(8, 1));
  int wlen = 4;

  fasst::TFRepr X(x, wlen);
  for (int f = 0; f < X.bins(); f++) {
    ASSERT_EQ(X(f, 1)(0).real(), X(f, 2)(0, 0).real());
    ASSERT_EQ(X(f, 1)(0).imag(), X(f, 2)(0, 0).imag());
  }
}

TEST(TFRepr, firstSampleIsOne) {
  // input: 8 samples, 1 channel, (x=0, x(0)=1), wlen=4
  // assert: every frames are zero except the first one
  ArrayXXd array = ArrayXXd::Zero(8, 1);
  array(0, 0) = 1.;
  fasst::Audio x(array);
  int wlen = 4;

  fasst::TFRepr X(x, wlen);
  for (int n = 1; n < X.frames(); n++) {
    for (int f = 0; f < X.bins(); f++) {
      ASSERT_EQ(X(f, n)(0).real(), 0.);
      ASSERT_EQ(X(f, n)(0).imag(), 0.);
    }
  }
}
