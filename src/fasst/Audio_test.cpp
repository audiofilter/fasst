#include "Audio.h"
#include "gtest/gtest.h"

using namespace std;
using namespace Eigen;

string g_inputDataDir(INPUT_DATA_DIR);

TEST(Audio, read) {
  string fname(g_inputDataDir + "/Shannon_Hurley__Sunrise__inst__mix.wav");
  fasst::Audio x(fname.c_str());
  ASSERT_EQ(2, x.channels());
  ASSERT_EQ(50000, x.samples());
}

TEST(Audio, write) {
  string fname(g_inputDataDir + "/Shannon_Hurley__Sunrise__inst__mix.wav");
  fasst::Audio x1(fname.c_str());
  x1.write("tmp.wav");

  fasst::Audio x2("tmp.wav");
  ASSERT_EQ(x1.channels(), x2.channels());
  ASSERT_EQ(x1.samples(), x2.samples());

  // Here we assert errormax between the two audio datas is lower than 1/16369
  // The reason for 1/16369 is explained in the following URL:
  // http://www.mega-nerd.com/libsndfile/FAQ.html#Q010
  ASSERT_LT((x1 - x2).abs().maxCoeff(), 1. / 16369);
}
