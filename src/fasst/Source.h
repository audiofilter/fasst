#ifndef FASST_SOURCE_H
#define FASST_SOURCE_H

#include "MixingParameter.h"
#include "SpectralPower.h"

namespace fasst {
/*!
 This class represents a source with all of its 9 parameters: one
 MixingParameter and eight NonNegMatrix. In addition, it has an attribute for V
 which models the entire spectral power of the source and is stored in a
 `Eigen::MatrixXd` object which can be seen as a \f$F \times N\f$-matrix, and an
 attribute for R which models the spatial covariance matrix and is stored in a
 `VectorMatrixXcd` object which can be seen as a \f$F\f$-vector of \f$I \times
 I\f$-complex-matrices.
 */
class Source {
public:
  /*!
   The main constructor of the class. It loads each parameter from an XML node,
   and then computes V and R from the parameters.
   \param node The XML node containing the source parameters.
   */
  Source(QDomNode node);

  /*!
   This method call the replace method on each parameter which degree of
   adaptability is free.
   \param doc The entire DOM document
   \param node The old XML node
   */
  void replace(QDomDocument doc, QDomNode node) const;

  /*!
   This method is called during the M-step of the EM algorithm. It calls the
   update method either on both SpectralPower, or only on the excitation part if
   the filter part is null. Then the method computes V from the new value of the
   parameters.
   */
  void updateSpectralPower(const Eigen::ArrayXXd &Xi);

  /*!
   \return `true` is the mixing type of the source is instantaneous, `false` if
   it is convolutive.
   */
  inline bool isInst() const { return m_A.isInst(); }
  /*!
   \return `true` is the mixing type of the source is convolutive, `false` if it
   is instantaneous.
   */
  inline bool isConv() const { return m_A.isConv(); }

  /*!
   \return the rank of the spatial covariance of the source.
   */
  inline int rank() const { return m_A.rank(); }

  /*!
   \return the name of the source.
   */
  inline const std::string &name() const { return m_name; }

    /*!
   \return wiener parameter 'a' 'b' 'c1' 'c2' 'd' of the source.
   */
  inline double wiener_qa() const { return m_wiener_qa; }
  inline double wiener_b() const { return m_wiener_b; }
  inline int wiener_c1() const { return m_wiener_c1; }
  inline int wiener_c2() const { return m_wiener_c2; }
  inline double wiener_qd() const { return m_wiener_qd; }

  /*!
   \return the mixing parameter of the source.
   */
  inline const MixingParameter &A() const { return m_A; }

  /*!
   This method is used to get the value of the mixing parameter of the source at
   a given frequency bin.
   \param bin the frequency bin index
   \return the mixing parameter of the source at the given frequency bin.
   */
  inline const Eigen::MatrixXcd &A(int bin) const { return m_A(bin); }

  /*!
   This method is used to set the value of the mixing parameter of the source at
   a given frequency bin.
   \param bin the frequency bin index
   \return the mixing parameter of the source at the given frequency bin.
   */
  inline Eigen::MatrixXcd &A(int bin) { return m_A(bin); }

  /*!
   This method is used to get the value of the spectral power `V` in one TF
   point.
   \param bin the frequency bin index
   \param frame the time frame index
   \return the value of `V` corresponding to the indexes.
   */
  inline double V(int bin, int frame) const { return m_V(bin, frame); }

  /*!
   This method is used to get the spatial covariance matrix `R` in one frequency
   bin.
   \param bin the frequency bin index
   \return the matrix \f$R_{f}\f$ corresponding to the index.
   */
  inline const Eigen::MatrixXcd &R(int bin) const { return m_R(bin); }

  /*!
   This method is used to get the covariance matrix `Sigma_y` in one time-frequency
   bin.
   */
  inline Eigen::MatrixXcd &Sigma_y(int bin, int frame) { return m_Sigma_y(bin,frame); }

  /*!
   This method computes V. It is an implementation of \ref eq "Eq. 9". It is
   called when the object is constructed and after each M-step.
   */
  void compV();

  /*!
   This method computes R. It is an implementation of \ref eq "Eq. 5". It is
   called only when the object is constructed.
   */
  void compR();

  /*!
   This method applied a 2D filter on the spectral power parameter (m_V). It is called optionally before Wiener Filtering.
   */
  void spectralPowerSmoothing();

  /*!
   This method fill the member m_Sigma_y. It is called before Wiener Filtering.
   */
  void initSigmaY();

  /*!
   This method compute the Wiener filter in one time-frequency bin.
   */
  Eigen::MatrixXcd WienerFilter(int f, int n, const Eigen::MatrixXcd &Sigma_x_inverse) const;

  /*!
   \return the number of frequency bins
   */
  inline int bins() const { return m_V.rows(); }

  /*!
   \return the number of time frames
   */
  inline int frames() const { return m_V.cols(); }

private:
  std::string m_name;
  MixingParameter m_A;
  SpectralPower m_ex, m_ft;
  ArrayMatrixXcd m_Sigma_y;

  bool m_excitationOnly;

  int m_bins;
  
  double m_wiener_qa;
  double m_wiener_b;
  int m_wiener_c1;
  int m_wiener_c2;
  double m_wiener_qd;
 
  Eigen::ArrayXXd m_V;
  VectorMatrixXcd m_R;
};
}

#endif
