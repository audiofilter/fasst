#ifndef FASST_SPECTRALPOWER_H
#define FASST_SPECTRALPOWER_H

#include "NonNegMatrix.h"

namespace fasst {

/*!
 This class represents either the excitation spectral power or the filter
 spectral power of a source. It has 4 NonNegMatrix objects to store W, U, G and
 H.
    */
class SpectralPower {
public:
  /*!
   The main constructor of the class builds the 4 NonNegMatrix attributes from
   an XML node.
   \param node An XML node
   \param suffix Should be either 'ex' or 'ft'
   */
  SpectralPower(QDomNode node, std::string suffix);

  /*!
   This method calls the replace method on each NonNegMatrix parameter which
   degree of adaptability is free.
   \param doc The entire DOM document
   \param node The old XML node
   \param suffix Should be either 'ex' or 'ft'
   */
  void replace(QDomDocument doc, QDomNode node, std::string suffix) const;

  /*!
   This method is called during the M-step of the EM algorithm. It calls the
   update method on each NonNegMatrix which degree of adaptability is free. It
   is optimized for the case where the source doesn't have a filter spectral
   model.
   */
  void update(const Eigen::ArrayXXd &Xi);

  /*!
   This method is called during the M-step of the EM algorithm. It calls the
   update method on each NonNegMatrix which degree of adaptability is free.
   */
  void update(const Eigen::ArrayXXd &Xi, const Eigen::ArrayXXd &E);

  /*!
   This method is an implementation of \ref eq "Eq. 12".
   \return the product of the four NonNegMatrix
   */
  inline Eigen::ArrayXXd V() const {
    return m_W * m_U * m_G * m_H;
  }

private:
  W m_W;
  UG m_U, m_G;
  H m_H;
};
}

#endif
