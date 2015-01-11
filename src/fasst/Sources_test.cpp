#include "Sources.h"
#include <stdexcept>
#include "gtest/gtest.h"

using namespace std;
using namespace fasst;

TEST(Sources, SimpleTest) {
  QString str = "<sources>"
                "<source>"
                "<A adaptability=\"free\" mixing_type=\"inst\">"
                "<ndims>2</ndims>"
                "<dim>2</dim>"
                "<dim>1</dim>"
                "<type>real</type>"
                "<data>1 0 </data>"
                "</A>"
                "</source>"
                "<source>"
                "<A adaptability=\"free\" mixing_type=\"inst\">"
                "<ndims>2</ndims>"
                "<dim>2</dim>"
                "<dim>1</dim>"
                "<type>real</type>"
                "<data>0 1 </data>"
                "</A>"
                "</source>"
                "</sources>";

  QDomDocument doc;
  ASSERT_TRUE(doc.setContent(str));
  QDomNodeList list = doc.elementsByTagName("source");
  Sources src(list);
  ASSERT_EQ(src.size(), 2);
}

TEST(Sources, WrongNumberOfChannels) {
  QString str = "<sources>"
                "<source>"
                "<A adaptability=\"free\" mixing_type=\"inst\">"
                "<ndims>2</ndims>"
                "<dim>1</dim>"
                "<dim>1</dim>"
                "<type>real</type>"
                "<data>1 </data>"
                "</A>"
                "</source>"
                "<source>"
                "<A adaptability=\"free\" mixing_type=\"inst\">"
                "<ndims>2</ndims>"
                "<dim>2</dim>"
                "<dim>1</dim>"
                "<type>real</type>"
                "<data>1 0 </data>"
                "</A>"
                "</source>"
                "</sources>";

  QDomDocument doc;
  ASSERT_TRUE(doc.setContent(str));
  QDomNodeList list = doc.elementsByTagName("source");
  ASSERT_THROW(Sources src(list), runtime_error);
}

TEST(Sources, WrongNumberOfBins) {
  QString str = "<sources>"
                "<source>"
                "<A adaptability=\"free\" mixing_type=\"inst\">"
                "<ndims>2</ndims>"
                "<dim>1</dim>"
                "<dim>1</dim>"
                "<type>real</type>"
                "<data>1 </data>"
                "</A>"
                "<Wex adaptability=\"free\">"
                "<rows>2</rows>"
                "<cols>1</cols>"
                "<data>1 0 </data>"
                "</Wex>"
                "<Uex adaptability=\"fixed\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Uex>"
                "<Gex adaptability=\"free\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Gex>"
                "<Hex adaptability=\"fixed\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Hex>"
                "</source>"
                "<source>"
                "<A adaptability=\"free\" mixing_type=\"inst\">"
                "<ndims>2</ndims>"
                "<dim>1</dim>"
                "<dim>1</dim>"
                "<type>real</type>"
                "<data>0 </data>"
                "</A>"
                "<Wex adaptability=\"free\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Wex>"
                "<Uex adaptability=\"fixed\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Uex>"
                "<Gex adaptability=\"free\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Gex>"
                "<Hex adaptability=\"fixed\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Hex>"
                "</source>"
                "</sources>";

  QDomDocument doc;
  ASSERT_TRUE(doc.setContent(str));
  QDomNodeList list = doc.elementsByTagName("source");
  ASSERT_THROW(Sources src(list), runtime_error);
}

TEST(Sources, WrongNumberOfFrames) {
  QString str = "<sources>"
                "<source>"
                "<A adaptability=\"free\" mixing_type=\"inst\">"
                "<ndims>2</ndims>"
                "<dim>1</dim>"
                "<dim>1</dim>"
                "<type>real</type>"
                "<data>1 </data>"
                "</A>"
                "<Wex adaptability=\"free\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Wex>"
                "<Uex adaptability=\"fixed\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Uex>"
                "<Gex adaptability=\"free\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Gex>"
                "<Hex adaptability=\"fixed\">"
                "<rows>1</rows>"
                "<cols>2</cols>"
                "<data>1\n0 </data>"
                "</Hex>"
                "</source>"
                "<source>"
                "<A adaptability=\"free\" mixing_type=\"inst\">"
                "<ndims>2</ndims>"
                "<dim>1</dim>"
                "<dim>1</dim>"
                "<type>real</type>"
                "<data>0 </data>"
                "</A>"
                "<Wex adaptability=\"free\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Wex>"
                "<Uex adaptability=\"fixed\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Uex>"
                "<Gex adaptability=\"free\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Gex>"
                "<Hex adaptability=\"fixed\">"
                "<rows>1</rows>"
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Hex>"
                "</source>"
                "</sources>";

  QDomDocument doc;
  ASSERT_TRUE(doc.setContent(str));
  QDomNodeList list = doc.elementsByTagName("source");
  ASSERT_THROW(Sources src(list), runtime_error);
}
