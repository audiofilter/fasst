#ifndef FASST_MIXCOVMATRIX_H
#define FASST_MIXCOVMATRIX_H

#include "typedefs.h"

namespace fasst {
class Audio;

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
class MixCovMatrix : public ArrayMatrixXcd {
public:
  /*!
   The main constructor of the class computes the mixture covariance matrices of
   some audio signal (with the STFT transform) and stores it in the object.
   \param x a multichannel audio signal
   \param tfr_type is either STFT or ERB
   \param wlen the window length _ie._ the length (in audio samples) of one time
   frame
   \param nbin the number of frequency bins
   */
  MixCovMatrix(const Audio &x, std::string tfr_type, int wlen, int nbin);

  /*!
   This constructor reads a binary file and loads the mixture covariance
   matrices from it. Please note that if the file doesn't exist or is not
   readable, this method will throw a `runtime_error` exception.
   \param fname the name of the input binary file
   */
  MixCovMatrix(const char *fname);

  /*!
   This method writes the mixture covariance matrices to a binary file. Please
   note that if the file is not writable, this method will throw a
   `runtime_error` exception.
   \param fname the name of the output binary file
   */
  void write(const char *fname);

  /*!
   \return the number of frequency bins
   */
  inline int bins() const { return rows(); }

  /*!
   \return the number of time frames
   */
  inline int frames() const { return cols(); }

  /*!
   \return the number of audio channels
   */
  inline int channels() const { return (*this)(0, 0).rows(); }
};
}

#endif
