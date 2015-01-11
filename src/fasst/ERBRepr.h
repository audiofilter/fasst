#ifndef FASST_ERBREPR_H
#define FASST_ERBREPR_H

#include "typedefs.h"

namespace fasst {
class Audio;
class Source;

/*!
 This class contains a mixture covariance matrix. The data is stored in an
`ArrayMatrixXcd` object, which can be seen as a \f$F \times N\f$-array of \f$I
\times I\f$-matrices.  \f$F\f$ is the number of frequency bins, \f$N\f$ is the
number of time frames and \f$I\f$ is the number of audio channels. The matrices
are hermitian, _ie._ the diagonal elements are real and the upper triangular
part
is equal to the conjugate of the lower triangular part. We take advantage of
this property when we store the data in a binary file. The binary file format is
documented in the \ref binfileformat page.

\remark It might be possible to take advantage of the hermitian property of the
matrices when we store the data in the object (with help of Eigen
`selfadjointView` template).

\remark In the future, it might be interesting to use the binary file format to
store other data (namely features, uncertainty).
 */
class ERBRepr : public ArrayMatrixXcd {
public:
  /*!
   The main constructor of the class computes the mixture covariance matrices of
   some audio signal (with the ERB transform) and stores it in the object.
   \param x a multichannel audio signal
   \param wlen the window length _ie._ the length (in audio samples) of one time
   frame
   \param nbin the number of frequency bins
   */
  ERBRepr(const Audio &x, int wlen, int nbin);

  /*!
   This method applies the Wiener filter in the ERB domain.
   \param x the mixture audio signal
   \param wlen the window length
   \param srcs the sources structure
   \param Sigma_x_inverse the inverse of the model covariance matrix
   \return the audio signal of each source
   */
  static std::vector<Audio> FilterERB(const Audio &x, int wlen, const std::vector<Source> &srcs, const ArrayMatrixXcd &Sigma_x_inverse);

  /*!
   Filters a signal using FFT-based convolution.
   \param h a complex-valued filter with odd length
   \param x a complex-valued multichannel signal with even length
   The result of the convolution process has the same length and no delay
   compared to the original signal
   */
  static Eigen::ArrayXXcd fftfilt(Eigen::ArrayXcd h, Eigen::ArrayXXcd x);

private:
  /*!
   Downsamples a signal by a factor of 2.
   \param x a complex-valued multichannel signal
   */
  static Eigen::ArrayXXcd downsample(Eigen::ArrayXXcd x);

  /*!
   Upsamples a signal by a power of 2 between 2 and 512.
   \param x a complex-valued multichannel signal
   \param factor the upsampling factor
   */
  static Eigen::ArrayXXcd upsample(Eigen::ArrayXXcd x, int factor);
};
}

#endif
