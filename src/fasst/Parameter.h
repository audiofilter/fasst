#ifndef FASST_PARAMETER_H
#define FASST_PARAMETER_H

#include <string>

class QDomDocument;
class QDomElement;

namespace fasst {

/*!
 This class is an abstract base class for every source parameters. It has an
 attribute corresponding to its degree of adaptability, and a pure virtual
 method to replace an old XML element with a new one with current data.
 */
class Parameter {
public:

  /*!
   This pure virtual method should create a new XML element with the object
   data, and replace the old element with the new one.
   \param doc The entire DOM document.
   \param el The XML element corresponding to the parameter.
   */
  virtual void replace(QDomDocument doc, QDomElement el) const = 0;

  /*!
   \return `true` is the degree of adaptability is free, `false` if it is fixed.
   */
  inline bool isFree() const { return m_adaptability == "free"; }

protected:
  std::string m_adaptability;
};
}

#endif
