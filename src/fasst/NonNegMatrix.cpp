#include "NonNegMatrix.h"
#include <QtCore/QStringList>
#include <vector>

using namespace std;
using namespace Eigen;

namespace fasst {
NonNegMatrix::NonNegMatrix(QDomElement el) {
  // Read attributes
  m_adaptability = el.attribute("adaptability").toLocal8Bit().constData();

  // Read dimensions
  int rows = el.firstChildElement("rows").toElement().text().toInt();
  int cols = el.firstChildElement("cols").toElement().text().toInt();

  // Read data
  QString string = el.firstChildElement("data").toElement().text().trimmed();
  if (string == "eye") {
    m_eye = true;
  } else {
    _set(MatrixXd(rows, cols));
    QStringList col_list = string.split('\n');
    for (int i = 0; i < cols; i++) {
      QStringList row_list = col_list.at(i).split(' ');
      for (int j = 0; j < rows; j++) {
        (*this)(j, i) = row_list.at(j).toDouble();
      }
    }
    m_eye = false;
  }
}

void NonNegMatrix::replace(QDomDocument doc, QDomElement el) const {
  // Convert data to string
  stringstream s;
  for (int i = 0; i < cols(); i++) {
    for (int j = 0; j < rows(); j++) {
      s << (*this)(j, i) << ' ';
    }
    s << '\n';
  }

  // Write string to new node
  QDomElement newNode = doc.createElement("data");
  newNode.appendChild(doc.createTextNode(s.str().c_str()));

  // Replace old node with new node
  QDomElement oldNode = el.firstChildElement("data");
  el.replaceChild(newNode, oldNode);
}

void W::update(const ArrayXXd &Xi, const NonNegMatrix &D) {
  NonNegMatrix Dtranspose = D.transpose();
  ArrayXXd CD = (*this) * D;
  ArrayXXd num = NonNegMatrix(Xi / (CD * CD)) * Dtranspose;
  ArrayXXd denom = NonNegMatrix(1 / CD) * Dtranspose;
  _set(this->array() * (num / denom));
}

void W::update(const ArrayXXd &Xi, const NonNegMatrix &D, const ArrayXXd &E) {
  NonNegMatrix Dtranspose = D.transpose();
  ArrayXXd CDE = ((*this) * D).array() * E;
  ArrayXXd num = NonNegMatrix(Xi * E / (CDE * CDE)) * Dtranspose;
  ArrayXXd denom = NonNegMatrix(E / CDE) * Dtranspose;
  _set(this->array() * (num / denom));
}

void UG::update(const ArrayXXd &Xi, const NonNegMatrix &B, const NonNegMatrix &D) {
  NonNegMatrix Btranspose = B.transpose();
  NonNegMatrix Dtranspose = D.transpose();
  ArrayXXd BCD = B * (*this) * D;
  ArrayXXd num = Btranspose * NonNegMatrix(Xi / (BCD * BCD)) * Dtranspose;
  ArrayXXd denom = Btranspose * NonNegMatrix(1 / BCD) * Dtranspose;
  _set(this->array() * (num / denom));
}

void UG::update(const ArrayXXd &Xi, const NonNegMatrix &B, const NonNegMatrix &D,
                const ArrayXXd &E) {
  NonNegMatrix Btranspose = B.transpose();
  NonNegMatrix Dtranspose = D.transpose();
  ArrayXXd BCDE = (B * (*this) * D).array() * E;
  ArrayXXd num = Btranspose * NonNegMatrix(Xi * E / (BCDE * BCDE)) * Dtranspose;
  ArrayXXd denom = Btranspose * NonNegMatrix(E / BCDE) * Dtranspose;
  _set(this->array() * (num / denom));
}

void H::update(const ArrayXXd &Xi, const NonNegMatrix &B) {
  NonNegMatrix Btranspose = B.transpose();
  ArrayXXd BC = B * (*this);
  ArrayXXd num = Btranspose * NonNegMatrix(Xi / (BC * BC));
  ArrayXXd denom = Btranspose * NonNegMatrix(1 / BC);
  _set(this->array() * (num / denom));
}

void H::update(const ArrayXXd &Xi, const NonNegMatrix &B, const ArrayXXd &E) {
  NonNegMatrix Btranspose = B.transpose();
  ArrayXXd BCE = (B * (*this)).array() * E;
  ArrayXXd num = Btranspose * NonNegMatrix(Xi * E / (BCE * BCE));
  ArrayXXd denom = Btranspose * NonNegMatrix(E / BCE);
  _set(this->array() * (num / denom));
}
}
