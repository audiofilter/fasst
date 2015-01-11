#include "NonNegMatrix.h"
#include <stdexcept>
#include <QDomDocument>
#include "gtest/gtest.h"

using namespace std;
using namespace fasst;

TEST(NonNegMatrix, ReadWrite) {
  QString str = "<mat adaptability=\"free\">"
                "<rows>2</rows>"
                "<cols>3</cols>"
                "<data>1. 2.\n3. 4.\n5. 6. </data>"
                "</mat>";

  QDomDocument doc;
  ASSERT_TRUE(doc.setContent(str));
  QDomElement el = doc.firstChild().toElement();
  NonNegMatrix mat(el);

  // Replace mat
  mat.replace(doc, el);
  NonNegMatrix new_mat(el);

  // Compare mat and new_mat
  for (int i = 0; i < mat.rows(); i++) {
    for (int j = 0; j < mat.cols(); j++) {
      ASSERT_EQ(mat(i, j), new_mat(i, j));
    }
  }
}
