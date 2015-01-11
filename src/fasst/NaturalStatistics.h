#ifndef FASST_NATURALSTATISTICS
#define FASST_NATURALSTATISTICS

#include "typedefs.h"

namespace fasst {
class Sources;
class MixCovMatrix;

/*!
 This class contains the natural sufficient statistics of the EM algorithm. It
 is used for the E-step of the algorithm and for the computation of the
 log-likelihood.
 */
class NaturalStatistics {
public:
  /*!
   The main constructor of the class contains the computation for the E-step of
   the algorithm (\ref eq "Eq. 21 to 25") and the computation of the
   log-likelihood
   (\ref eq "Eq. 16").
   \param sources
   \param hatRx
   \param Sigma_b
   */
  NaturalStatistics(const Sources &sources, const MixCovMatrix &hatRx,
                    const VectorMatrixXcd &Sigma_b);

  /*!
   This function is used to get the value of the log-likelihood.
   \return the value of the log-likelihood.
   */
  inline double logLikelihood() const { return m_logLikelihood; }

  /*!
   This method is used to get the natural statistic \f$\hat{R_{xs}}\f$ at a
   given TF point.
   \param bin the frequency bin index
   \param frame the time frame index
   \return the value of \f$\hat{R_{xs}}\f$ corresponding to the indexes.
   */
  inline const Eigen::MatrixXcd &hatRxs(int bin, int frame) const {
    return m_hatRxs(bin, frame);
  }

  /*!
   This method is used to get the natural statistic \f$\hat{R_{s}}\f$ at a given
   TF point.
   \param bin the frequency bin index
   \param frame the time frame index
   \return the value of \f$\hat{R_{s}}\f$ corresponding to the indexes.
   */
  inline const Eigen::MatrixXcd &hatRs(int bin, int frame) const {
    return m_hatRs(bin, frame);
  }

private:
  ArrayMatrixXcd m_hatRxs;
  ArrayMatrixXcd m_hatRs;
  double m_logLikelihood;
};
}

#endif
