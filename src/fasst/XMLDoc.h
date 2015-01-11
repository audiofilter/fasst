#ifndef FASST_XMLDOC_H
#define FASST_XMLDOC_H

#include <QtXml/QDomDocument>

namespace fasst {
class Sources;

/*!
 This class represents an entire DOM and is used to read and write XML file
 */
class XMLDoc {
public:
  /*!
   The main constructor of the class reads a file and parses the XML data inside
   it. Please note that if the file is not readable or not well-formed XML, the
   constructor will throw a `runtime_error` exception.
   \param fname the name of the XML file to be read
   */
  XMLDoc(const char *fname);

  /*!
   \return the number of iterations, or 0 if the field doesn't exist
   */
  int getIterations() const;

  /*!
   \return the window length in the DOM
   */
  std::string getTFRType() const;

  /*!
   \return the window length in the DOM
   */
  int getWlen() const;

  /*!
   \return the number of frequency bins in the DOM
   */
  int getNbin() const;

  /*!
   \return a Sources instance containing all sources in the DOM
   */
  Sources getSources() const;

  /*!
   This au replaces the sources parameters in the current DOM with updated ones.
   \param sources the updated sources
   */
  void replaceSources(Sources &sources);

  /*!
   This au writes the current DOM to a file. Please note that if the file is not
   writable, the method will throw a `runtime_error` exception.
   \param fname the name of the XML file to be written
   */
  void write(const char *fname) const;

private:
  /*!
   This variable contains the whole DOM
   */
  QDomDocument m_doc;
};
}

#endif
