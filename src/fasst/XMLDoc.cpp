#include "XMLDoc.h"
#include "Sources.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace fasst {
XMLDoc::XMLDoc(const char *fname) {
  // Open fname
  QFile file(fname);
  if (!file.open(QIODevice::ReadOnly)) {
    stringstream s;
    s << "Can not open " << fname << ". ";
    s << "File probably doesn't exist or isn't readable.";
    throw runtime_error(s.str());
  }

  // Parse XML
  if (!m_doc.setContent(&file)) {
    file.close();
    stringstream s;
    s << "Can not set content from " << fname << ". ";
    s << "File is probably not well-formed XML.";
    throw runtime_error(s.str());
  }
  file.close();
}

int XMLDoc::getIterations() const {
  if (m_doc.elementsByTagName("iterations").isEmpty()) {
    return 0;
  } else {
    return m_doc.elementsByTagName("iterations").item(0).toElement().text()
        .toInt();
  }
}

std::string XMLDoc::getTFRType() const {
  if (m_doc.elementsByTagName("tfr_type").isEmpty()) {
    return "STFT";
  } else {
    return m_doc.elementsByTagName("tfr_type").item(0).toElement().text().trimmed().toStdString();
  }
}

int XMLDoc::getWlen() const {
  return m_doc.elementsByTagName("wlen").item(0).toElement().text().toInt();
}

int XMLDoc::getNbin() const {
  if (m_doc.elementsByTagName("nbin").isEmpty()) {
    return 0;
  } else {
    return m_doc.elementsByTagName("nbin").item(0).toElement().text()
        .toInt();
  }
}

Sources XMLDoc::getSources() const {
  return Sources(m_doc.elementsByTagName("source"));
}

void XMLDoc::replaceSources(Sources &sources) {

  QDomNodeList oldNodeList = m_doc.elementsByTagName("source");
  sources.replace(m_doc, oldNodeList);
}

void XMLDoc::write(const char *fname) const {
  // Open fname
  QFile file(fname);
  if (!file.open(QIODevice::WriteOnly)) {
    stringstream s;
    s << "Can not open " << fname << ". ";
    s << "You probably don't have write access to this location.";
    throw runtime_error(s.str());
  }

  // Write m_doc to file
  QTextStream stream(&file);
  stream << m_doc.toString();
  file.close();
}
}
