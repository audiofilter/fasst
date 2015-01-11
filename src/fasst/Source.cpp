#include "Source.h"
#include "ERBRepr.h"
#include <QtCore/QTextStream>
#include <sstream>
#include <stdexcept>
#include <Eigen/Eigenvalues>
#include <iostream>

using namespace Eigen;
using namespace std;

namespace fasst {
Source::Source(QDomNode node)
    : m_A(node.firstChildElement("A")), m_ex(node, "ex"), m_ft(node, "ft") {
  if (node.firstChildElement("Wft").isNull()) {
    m_excitationOnly = true;
  } else {
    m_excitationOnly = false;
  }

  if (node.toElement().hasAttribute("name")) {
    m_name = node.toElement().attribute("name").toLocal8Bit().constData();
  } else {
    m_name = "";
  }

  if (node.firstChildElement("wiener").isNull()) {
    m_wiener_qa = 1;
    m_wiener_b  = 0.;
    m_wiener_c1 = 0;
    m_wiener_c2 = 0;
    m_wiener_qd = 0.;
  } else {
    if (node.firstChildElement("wiener").firstChildElement("a").isNull()) {
      m_wiener_qa = 1;
    } else {
      m_wiener_qa = node.firstChildElement("wiener").firstChildElement("a").toElement().text().toDouble();
      m_wiener_qa = pow(10., m_wiener_qa/10.); // Convert from dB ; qa=1 if a=0 dB (default value with no effect)
    }
    if (node.firstChildElement("wiener").firstChildElement("b").isNull()) {
      m_wiener_b = 0.;
    } else {
      m_wiener_b = node.firstChildElement("wiener").firstChildElement("b").toElement().text().toDouble();
      if(m_wiener_b<0 || m_wiener_b>1) {
        stringstream s;
        s << "wiener.b = " << m_wiener_b << " but should be inside [0;1].";
        throw runtime_error(s.str());
      }
    }
    if (node.firstChildElement("wiener").firstChildElement("c1").isNull()) {
      m_wiener_c1 = 0;
    } else {
      m_wiener_c1 = node.firstChildElement("wiener").firstChildElement("c1").toElement().text().toDouble();
      double c1_test = node.firstChildElement("wiener").firstChildElement("c1").toElement().text().toDouble();
      if(m_wiener_c1<0) {
        stringstream s;
        s << "wiener.c1 = " << m_wiener_c1 << " but should be >=0.";
        throw runtime_error(s.str());
      }
      if(c1_test!=floor(c1_test)) {
        stringstream s;
        s << "wiener.c1 = " << c1_test << " but should be integer";
        throw runtime_error(s.str());
      }
    }
    if (node.firstChildElement("wiener").firstChildElement("c2").isNull()) {
      m_wiener_c2 = 0;
    } else {
      m_wiener_c2 = node.firstChildElement("wiener").firstChildElement("c2").toElement().text().toDouble();
      double c2_test = node.firstChildElement("wiener").firstChildElement("c2").toElement().text().toDouble();
      if(m_wiener_c2<0) {
        stringstream s;
        s << "wiener.c2 = " << m_wiener_c2 << " but should be >=0.";
        throw runtime_error(s.str());
      }
      if(c2_test!=floor(c2_test)) {
        stringstream s;
        s << "wiener.c2 = " << c2_test << " but should be integer";
        throw runtime_error(s.str());
      }
    }
    if (node.firstChildElement("wiener").firstChildElement("d").isNull()) {
      m_wiener_qd = 0.;
    } else {
      stringstream s;
      s << node.firstChildElement("wiener").firstChildElement("d").toElement().text().toStdString();
      if (s.str() == "-Inf" || s.str() == "-inf") {
        m_wiener_qd  = 0.;
      } else {
        m_wiener_qd = node.firstChildElement("wiener").firstChildElement("d").toElement().text().toDouble();
        m_wiener_qd = pow(10., m_wiener_qd/10.); // Convert from dB ; qd=0 if d=-Inf dB (default value with no effect)
      }
    }
  }

  m_bins = node.firstChildElement("Wex").firstChildElement("rows").toElement()
      .text().toInt();

  compV();
  compR();
}

void Source::replace(QDomDocument doc, QDomNode node) const {
  if (m_A.isFree()) {
    m_A.replace(doc, node.firstChildElement("A"));
  }

  m_ex.replace(doc, node, "ex");
  if (!m_excitationOnly) {
    m_ft.replace(doc, node, "ft");
  }
}

void Source::updateSpectralPower(const ArrayXXd &Xi) {
  if (m_excitationOnly) {
    m_ex.update(Xi);
  } else {
    ArrayXXd E;
    E = m_ft.V();
    m_ex.update(Xi, E);

    E = m_ex.V();
    m_ft.update(Xi, E);
  }

  compV();
}

void Source::compV() {
  if (m_excitationOnly) {
    m_V = m_ex.V();
  } else {
    m_V = m_ex.V() * m_ft.V();
  }
}

void Source::compR() {
  m_R = VectorMatrixXcd(m_bins);
  if (isInst()) {
    for (int f = 0; f < m_bins; f++) {
      m_R(f) = m_A(0) * m_A(0).adjoint();
    }
  } else {
    for (int f = 0; f < m_bins; f++) {
      m_R(f) = m_A(f) * m_A(f).adjoint();
    }
  }
}

void Source::spectralPowerSmoothing(){
  ArrayXXcd xx = ArrayXXcd::Zero(m_V.rows(),m_V.cols());
  xx.real()= m_V;

  if(m_wiener_c1!=0){
    int c1 = m_wiener_c1*2 +1;
    ArrayXcd h1 = ArrayXcd::Zero(c1);
    for (int i1 = 0 ; i1 < c1; i1++) {
      h1(i1) = 1 + cos(M_PI/(m_wiener_c1+1)*(i1-m_wiener_c1));
    }
    h1 = h1/ h1.sum();
    xx = ERBRepr::fftfilt(h1, xx);
  }

  if(m_wiener_c2!=0){
    int c2 = m_wiener_c2*2 +1;
    ArrayXcd h2 = ArrayXcd::Zero(c2);
    for (int i2 = 0 ; i2 < c2; i2++) {
      h2(i2) = 1 + cos(M_PI/(m_wiener_c2+1)*(i2-m_wiener_c2));
    }
    h2 = h2/ h2.sum();
    xx.transposeInPlace();
    xx = ERBRepr::fftfilt(h2, xx);
    xx.transposeInPlace();
  }
  m_V=xx.real();
}

void Source::initSigmaY(){
  m_Sigma_y = ArrayMatrixXcd(m_bins, frames());
  int I =m_A(0).rows();
  MatrixXcd b = MatrixXcd::Zero(I, I);
  for (int i1 = 0; i1 < I; i1++) {
    for (int i2 = 0; i2 < I; i2++) {
      if(i1==i2){
        b(i1,i2)=1;
      } else {
        b(i1,i2)=m_wiener_b;
      }
    }
  }

  for (int n = 0; n < frames(); n++) {
    for (int f = 0; f < m_bins; f++) {
      m_Sigma_y(f,n) = m_wiener_qa * m_V(f, n) * b * m_R(f);
    }
  }
}


MatrixXcd Source::WienerFilter(int f, int n, const MatrixXcd &Sigma_x_inverse) const {
  MatrixXcd output = m_Sigma_y(f,n) * Sigma_x_inverse;

  // Optional thresholding
  if (m_wiener_qd>0) {
    int I =m_A(0).rows();
    ComplexEigenSolver<MatrixXcd> ces(output);
    MatrixXcd D = ces.eigenvalues().asDiagonal();
    MatrixXcd V = ces.eigenvectors();
    for (int i = 0; i < I; i++) {
      if(abs(D(i,i))<m_wiener_qd) {
        D(i,i)=m_wiener_qd;
      }
    }
    output = V * D * V.inverse();
  }
  return output;
}

}
