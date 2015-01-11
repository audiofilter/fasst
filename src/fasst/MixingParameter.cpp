#include "MixingParameter.h"
#include <stdexcept>
#include <QtXml/QDomElement>
#include <QtCore/QStringList>

using namespace std;
using namespace Eigen;

namespace fasst {
MixingParameter::MixingParameter(QDomElement el) {
  // Read attributes
  m_adaptability = el.attribute("adaptability").toLocal8Bit().constData();
  m_mixingType = el.attribute("mixing_type").toLocal8Bit().constData();

  // Read dimensions
  int ndims = el.firstChildElement("ndims").toElement().text().toInt();
  if (ndims != 2 && ndims != 3) {
    throw runtime_error("Check your mixing parameter: ndims should be 2 or 3");
  }
  vector<int> dim(ndims);
  QDomNodeList dimNodeList = el.elementsByTagName("dim");
  for (int i = 0; i < ndims; i++) {
    dim[i] = dimNodeList.item(i).toElement().text().toInt();
  }

  // Read type
  string type = el.firstChildElement("type").toElement().text().trimmed()
      .toLocal8Bit().constData();
  if (type != "real" && type != "complex") {
    throw runtime_error(
        "Check your mixing parameter: type should be real or complex");
  }

  // Check if mixingType, ndims and type are consistent
  if (m_mixingType == "conv" && ndims != 3) {
    throw runtime_error("Check your mixing parameter: if mixing type is conv, "
                        "ndims should be equal to 3");
  } else if (m_mixingType == "inst" && ndims != 2) {
    throw runtime_error("Check your mixing parameter: if mixing type is inst, "
                        "ndims should be equal to 2");
  } else if (m_mixingType == "inst" && type != "real") {
    throw runtime_error("Check your mixing parameter: if mixing type is inst, "
                        "type should be real");
  }

  // Read data and convert to double
  QStringList list =
      el.firstChildElement("data").toElement().text().trimmed().split(' ');
  vector<double> data(list.size());
  for (int i = 0; i < list.size(); i++) {
    data[i] = list.at(i).toDouble();
  }

  if (m_mixingType == "inst") {
    _set(VectorMatrixXcd(1));
    (*this)(0) = MatrixXcd::Zero(dim[0], dim[1]);
    (*this)(0).real() = Eigen::Map<MatrixXd>(&data[0], dim[0], dim[1]);
  } else if (m_mixingType == "conv" && type == "real") {
    _set(VectorMatrixXcd(dim[2]));
    int s = dim[0] * dim[1];
    for (int i = 0; i < size(); i++) {
      (*this)(i) = MatrixXcd::Zero(dim[0], dim[1]);
      (*this)(i).real() = Eigen::Map<MatrixXd>(&data[i * s], dim[0], dim[1]);
    }
  } else if (m_mixingType == "conv" && type == "complex") {
    _set(VectorMatrixXcd(dim[2]));
    int s = dim[0] * dim[1];
    for (int i = 0; i < size(); i++) {
      (*this)(i) = MatrixXcd(dim[0], dim[1]);

      int index = i * 2 * s;
      (*this)(i).real() = Eigen::Map<MatrixXd>(&data[index], dim[0], dim[1]);

      index = (i * 2 + 1) * s;
      (*this)(i).imag() = Eigen::Map<MatrixXd>(&data[index], dim[0], dim[1]);
    }
  }
}

void MixingParameter::replace(QDomDocument doc, QDomElement el) const {
  int I = (*this)(0).rows();
  int R = (*this)(0).cols();

  // Convert data to string
  stringstream s;
  if (isInst()) {
    for (int r = 0; r < R; r++) {
      for (int i = 0; i < I; i++) {
        s << (*this)(0)(i, r).real() << ' ';
      }
    }
  } else {
    for (int f = 0; f < size(); f++) {
      for (int r = 0; r < R; r++) {
        for (int i = 0; i < I; i++) {
          s << (*this)(f)(i, r).real() << ' ';
        }
      }
      for (int r = 0; r < R; r++) {
        for (int i = 0; i < I; i++) {
          s << (*this)(f)(i, r).imag() << ' ';
        }
      }
    }

    // If type was 'real', replace it with 'complex'
    QDomElement oldTypeNode = el.firstChildElement("type");
    string type = oldTypeNode.text().trimmed().toLocal8Bit().constData();
    if (type == "real") {
      QDomElement newTypeNode = doc.createElement("type");
      newTypeNode.appendChild(doc.createTextNode("complex"));
      el.replaceChild(newTypeNode, oldTypeNode);
    }
  }

  // Write string to new node
  QDomElement newNode = doc.createElement("data");
  newNode.appendChild(doc.createTextNode(s.str().c_str()));

  // Replace old node with new node
  QDomElement oldNode = el.firstChildElement("data");
  el.replaceChild(newNode, oldNode);
}
}
