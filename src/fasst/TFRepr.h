#ifndef FASST_TFREPR_H
#define FASST_TFREPR_H

#include "typedefs.h"

namespace fasst {
class Audio;
class Source;

/*!
 This class contains the time-frequency representation of some audio signal. The
data is stored in an `ArrayVectorXcd` structure which actually is a \f$F \times
N\f$-array of \f$I\f$-vectors. \f$F\f$ is the number of frequency bins, \f$N\f$
is the number of time frames and \f$I\f$ is the number of audio channels.

\todo At the moment, only the STFT transform is implemented, but it is planned
that this class will be also used for the ERB transform in the near future.
 */
class TFRepr : public ArrayVectorXcd {
public:
  /*!
   The main constructor of the class computes the STFT transform of some audio
   signal and stores it in the object.
   \param x a multichannel audio signal
   \param wlen the window length _ie._ the length (in audio samples) of one time
   frame
   */
  TFRepr(const Audio &x, int wlen);

  /*!
   This constructor is used to initialize the storage of the data.
   \param bins the number of frequency bins
   \param frames the number of time frames
   */
  TFRepr(int bins, int frames) : ArrayVectorXcd(bins, frames) {}

  /*!
   This method computes the STFT inverse of the internal data.
   \param wlen the window length
   \param samples the number of samples in the audio signal
   \return the audio signal
   */
  Audio inverse(int wlen, int samples);

  /*!
   This method computes the STFT, then applies the Wiener filter, then computes the inverse STFT.
   \param x the mixture audio signal
   \param wlen the window length
   \param srcs the sources structure
   \param Sigma_x_inverse the inverse of the model covariance matrix
   \return the audio signal of each source
   */
  static std::vector<Audio> FilterSTFT(const Audio &x, int wlen, const std::vector<Source> &srcs, const ArrayMatrixXcd &Sigma_x_inverse);

  /*!
   \return the number of frequency bins
   */
  inline int bins() const { return rows(); }

  /*!
   \return the number of time frames
   */
  inline int frames() const { return cols(); }

  /*!
   \return the number of channels
   */
  inline int channels() const { return (*this)(0, 0).size(); }
};
}

#endif
