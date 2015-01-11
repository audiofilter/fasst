#include "Source.h"
#include <QDomDocument>
#include "gtest/gtest.h"

using namespace std;
using namespace fasst;

TEST(Source, Simplest) {
  QString str = "<source>"
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
                "<cols>1</cols>"
                "<data>1 </data>"
                "</Hex>"
                "</source>";

  QDomDocument doc;
  ASSERT_TRUE(doc.setContent(str));
  QDomElement el = doc.firstChild().toElement();
  Source src(el);

  ASSERT_TRUE(src.isInst());
  ASSERT_EQ(src.rank(), 1);
}
