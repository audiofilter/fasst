#ifndef FASST_SOURCES_H
#define FASST_SOURCES_H

#include "Source.h"
#include "TFRepr.h"
#include "Audio.h"

using namespace std;

namespace fasst {
class NaturalStatistics;

/*!
 This class represents a set of sources. In addition, it has an attribute for A
 which models the global mixing parameter containing each source. This parameter
 is stored as a `VectorMatrixXcd` which can be seen as a \f$F\f$-vector of \f$I
 \times R\f$-complex-matrices, where \f$F\f$ is the number of frequency bins,
 \f$I\f$ is the number of audio channels and \f$R\f$ is the sum of the spatial
 covariance rank of each source.
 */
class Sources {
public:
  /*!
   The main constructor of the class load sources from a list of `<source>` XML
   elements and then compute A from them.
   \param nodeList the list of `<source>` XML elements
   */
  Sources(QDomNodeList nodeList);

  /*!
   This method updates each source individual mixing parameter with the global
   mixing parameter, and then replace the XML data with the updated sources.
   \param doc The entire DOM document
   \param nodeList The old XML node list
   */
  void replace(QDomDocument doc, QDomNodeList nodeList);

  /*!
   This method is an implementation of \ref eq "Eq. 26" and \ref eq "Eq. 27". It
   computes an
   update of A with natural statistics.
   */
  void updateMixingParameter(const NaturalStatistics &stats);

  /*!
   This method does two things:
   1. It computes Xi with natural statistics, which corresponds to \ref eq "Eq.
   29"
   2. It calls the Source::updateSpectralPower on each source.
   */
  void updateSpectralPower(const NaturalStatistics &stats);

  /*!
   This method computes each source estimates and write output audio files .
   It is an implementation of \ref eq "Eq. 31" with additional parameters.
   */
  std::vector<Audio> Filter (const Audio &x, std::string tfr_type, int wlen);

  /*!
   This overloaded []-operator gives acces to an individual source.
   \param i the source index
   \return the source corresponding to the index
   */
  inline const Source &operator[](int i) const { return m_sources[i]; }

  /*!
   This method is used to get the value of the global mixing parameter at a
   given frequency bin.
   \param bin the frequency bin index
   \return the \f$I \times R\f$-matrix corresponding to the frequency bin index
   */
  inline const Eigen::MatrixXcd &A(int bin) const { return m_A(bin); }

  /*!
   \return the number of sources
   */
  inline int size() const { return m_sources.size(); }

  /*!
   \return the number of frequency bins
   */
  inline int bins() const { return m_bins; }

  /*!
   \return the number of time frames
   */
  inline int frames() const { return m_frames; }

  /*!
   \return the number of channels
   */
  inline int channels() const { return m_channels; }

private:
  std::vector<Source> m_sources;
  VectorMatrixXcd m_A;
  int m_bins, m_frames, m_channels;
};
}

#endif
