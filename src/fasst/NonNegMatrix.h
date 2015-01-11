#ifndef FASST_NONNEGMATRIX_H
#define FASST_NONNEGMATRIX_H

#include "Parameter.h"
#include <Eigen/Core>
#include <QtXml/QDomElement>

namespace fasst {

/*!
 This class represents a nonnegative matrix which is a part of the spectral
 power of a source. It is inherited by the W, UG and H classes. All of its
 subclass are able to update themself during the M-step of the EM algorithm.

 In case the matrix is not defined in XML, the m_eye flag is set to true and the matrix behaves like an identity matrix. We implement multiplication and transpose operations to avoid wasting time doing operations on identity matrices.
     */
class NonNegMatrix : public Parameter, public Eigen::MatrixXd {
public:
  /*!
   The main constructor of the class loads data and other attributes from an XML
   element.
   \param el An XML element
   */
  NonNegMatrix(QDomElement el);

  /*!
   Kind of copy constructor to create NonNegMatrix from the result of an operation on MatrixXd.
   */
  NonNegMatrix(const Eigen::MatrixXd &m) : Eigen::MatrixXd(m), m_eye(false) {}

  /*!
   This method creates a new XML element with the object data and replaces the
   old element with the new one.
   \param doc The entire DOM document
   \param el The old XML element
   */
  void replace(QDomDocument doc, QDomElement el) const;

  inline NonNegMatrix operator*(const NonNegMatrix &rhs) const {
    if (isEye()) {
      return rhs;
    } else if (rhs.isEye()) {
      return *this;
    } else {
      return NonNegMatrix(Eigen::MatrixXd::operator*(rhs));
    }
  }

  inline NonNegMatrix transpose() const {
    if (isEye()) {
      return *this;
    } else {
      return NonNegMatrix(Eigen::MatrixXd::transpose());
    }
  }

  inline bool isEye() const { return m_eye; }

private:
  bool m_eye;
};

/*!
 This class represent a W parameter of a source.
 */
class W : public NonNegMatrix {
public:
  /*!
   The main constructor of the class just calls NonNegMatrix::NonNegMatrix
   constructor
   */
  W(QDomElement el) : NonNegMatrix(el) {}

  /*!
   This method updates the data during the EM algorithm. It is a implementation
   of \ref eq "Eq. 30" optimized for W and for the case where the source is
   excitation-only (_ie._ `E == ones(F, N)`).
   */
  void update(const Eigen::ArrayXXd &Xi, const NonNegMatrix &D);

  /*!
   This method updates the data during the EM algorithm. It is a implementation
   of \ref eq "Eq. 30" optimized for W.
   */
  void update(const Eigen::ArrayXXd &Xi, const NonNegMatrix &D,
              const Eigen::ArrayXXd &E);
};

/*!
 This class represent either a U or a G parameter of a source.
 */
class UG : public NonNegMatrix {
public:
  /*!
   The main constructor of the class just calls NonNegMatrix::NonNegMatrix
   constructor
   */
  UG(QDomElement el) : NonNegMatrix(el) {}

  /*!
   This method updates the data during the EM algorithm. It is a implementation
   of \ref eq "Eq. 30" optimized for U and G and for the case where the source
   is
   excitation-only (_ie._ `E == ones(F, N)`).
   */
  void update(const Eigen::ArrayXXd &Xi, const NonNegMatrix &B,
              const NonNegMatrix &D);

  /*!
   This method updates the data during the EM algorithm. It is a implementation
   of \ref eq "Eq. 30" optimized for U and G.
   */
  void update(const Eigen::ArrayXXd &Xi, const NonNegMatrix &B,
              const NonNegMatrix &D, const Eigen::ArrayXXd &E);
};

/*!
 This class represent a H parameter of a source.
 */
class H : public NonNegMatrix {
public:
  /*!
   The main constructor of the class just calls NonNegMatrix::NonNegMatrix
   constructor
   */
  H(QDomElement el) : NonNegMatrix(el) {}

  /*!
   This method updates the data during the EM algorithm. It is a implementation
   of \ref eq "Eq. 30" optimized for H and for the case where the source is
   excitation-only (_ie._ `E == ones(F, N)`).
   */
  void update(const Eigen::ArrayXXd &Xi, const NonNegMatrix &B);

  /*!
   This method updates the data during the EM algorithm. It is a implementation
   of \ref eq "Eq. 30" optimized for H.
   */
  void update(const Eigen::ArrayXXd &Xi, const NonNegMatrix &B,
              const Eigen::ArrayXXd &E);
};
}

#endif
