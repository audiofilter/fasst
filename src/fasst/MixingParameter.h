#ifndef FASST_MIXINGPARAMETER_H
#define FASST_MIXINGPARAMETER_H

#include "typedefs.h"
#include "Parameter.h"

namespace fasst {

/*!
 This class represents the mixing parameter of a source, which models its
 spatial covariance. Data is stored in a `VectorMatrixXcd` object, which can be
 seen as a \f$F\f$-vector of \f$I \times R\f$-matrices. \f$F\f$ is the number of
 frequency bins, \f$I\f$ is the number of audio channels and \f$R\f$ is the rank
 of the spatial covariance.

 \remark Three cases are possible:
   1. The mixing type is instantaneous and the data is real and constant over
 frequency (_ie._ `size() == 1`).
   2. The mixing type is convolutive and the data is real and
 frequency-dependent.
   3. The mixing type is convolutive and the data is complex and
 frequency-dependent.

  If this implementation allows another combination, it's a bug, not a feature.

  \todo Document the format of the data in the XML file: [a
 link](http://stackoverflow.com/questions/16898691/pythonic-way-to-print-a-multidimensional-complex-numpy-array-to-a-string)
     */
class MixingParameter : public Parameter, public VectorMatrixXcd {
public:
  /*!
   The main constructor of the class loads data and other attributes from an XML
   element. If the input is not consistent, this constructor will throw a
   `runtime_error` exception
   \param el An XML element
   */
  MixingParameter(QDomElement el);

  /*!
   This method creates a new XML element with the object data and replaces the
   old element with the new one.
   \param doc The entire DOM document
   \param el The old XML element
   */
  void replace(QDomDocument doc, QDomElement el) const;

  /*!
   \return `true` is the mixing type is instantaneous, `false` if it is
   convolutive.
   */
  inline bool isInst() const { return m_mixingType == "inst"; }

  /*!
   \return `true` is the mixing type is convolutive, `false` if it is
   instantaneous.
   */
  inline bool isConv() const { return m_mixingType == "conv"; }

  /*!
   \return the rank of the spatial covariance.
   */
  inline int rank() const { return (*this)(0).cols(); }

private:
  std::string m_mixingType;
};
}

#endif
