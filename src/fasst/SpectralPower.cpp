#include "SpectralPower.h"
using namespace std;
using namespace Eigen;

namespace fasst {
SpectralPower::SpectralPower(QDomNode node, string suffix)
    : m_W(node.firstChildElement(QString::fromStdString("W" + suffix))),
      m_U(node.firstChildElement(QString::fromStdString("U" + suffix))),
      m_G(node.firstChildElement(QString::fromStdString("G" + suffix))),
      m_H(node.firstChildElement(QString::fromStdString("H" + suffix))) {}

void SpectralPower::replace(QDomDocument doc, QDomNode node,
                            string suffix) const {
  if (m_W.isFree()) {
    m_W.replace(doc,
                node.firstChildElement(QString::fromStdString("W" + suffix)));
  }
  if (m_U.isFree()) {
    m_U.replace(doc,
                node.firstChildElement(QString::fromStdString("U" + suffix)));
  }
  if (m_G.isFree()) {
    m_G.replace(doc,
                node.firstChildElement(QString::fromStdString("G" + suffix)));
  }
  if (m_H.isFree()) {
    m_H.replace(doc,
                node.firstChildElement(QString::fromStdString("H" + suffix)));
  }
}

void SpectralPower::update(const ArrayXXd &Xi) {
  if (m_W.isFree()) {
    NonNegMatrix UGH = m_U * m_G * m_H;
    m_W.update(Xi, UGH);
  }

  if (m_U.isFree()) {
    NonNegMatrix GH = m_G * m_H;
    m_U.update(Xi, m_W, GH);
  }

  if (m_G.isFree()) {
    NonNegMatrix WU = m_W * m_U;
    m_G.update(Xi, WU, m_H);
  }

  if (m_H.isFree()) {
    NonNegMatrix WUG = m_W * m_U * m_G;
    m_H.update(Xi, WUG);
  }
}

void SpectralPower::update(const ArrayXXd &Xi, const ArrayXXd &E) {
  if (m_W.isFree()) {
    NonNegMatrix UGH = m_U * m_G * m_H;
    m_W.update(Xi, UGH, E);
  }

  if (m_U.isFree()) {
    NonNegMatrix GH = m_G * m_H;
    m_U.update(Xi, m_W, GH, E);
  }

  if (m_G.isFree()) {
    NonNegMatrix WU = m_W * m_U;
    m_G.update(Xi, WU, m_H, E);
  }

  if (m_H.isFree()) {
    NonNegMatrix WUG = m_W * m_U * m_G;
    m_H.update(Xi, WUG, E);
  }

}
}
