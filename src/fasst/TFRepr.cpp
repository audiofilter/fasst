#include "TFRepr.h"
#include "Audio.h"
#include "Sources.h"
#include <stdexcept>
#include <unsupported/Eigen/FFT>

using namespace Eigen;
using namespace std;

namespace fasst {
TFRepr::TFRepr(const Audio &x, int wlen) {
  int samples = x.samples();
  int I = x.channels();

  // Checking window length
  if (wlen % 4 != 0 || wlen == 0) {
    stringstream s;
    s << "Error:\twlen is " << wlen << " and should be multiple of 4.\n";
    throw runtime_error(s.str());
  }
  
  // Defining sine window
  ArrayXd win =
      Eigen::sin(ArrayXd::LinSpaced(wlen, 0.5, wlen - 0.5) / wlen * M_PI);

  // Zero-padding
  int N = static_cast<int>(ceil(static_cast<double>(samples) / wlen * 2));
  ArrayXXd xx = ArrayXXd::Zero((N + 1) * wlen / 2, I);
  xx.block(wlen / 4, 0, samples, I) = x;

  // Pre-processing for edges
  ArrayXd swin = ArrayXd::Zero((N + 1) * wlen / 2);
  for (int n = 0; n < N; n++) {
    swin.segment(n * wlen / 2, wlen) += (win * win);
  }
  swin = Eigen::sqrt(wlen * swin);

  int F = wlen / 2 + 1;
  VectorMatrixXcd X(I);
  FFT<double> fft;
  for (int i = 0; i < I; i++) {
    X(i) = ArrayXXcd(F, N);
    for (int n = 0; n < N; n++) {
      // Framing
      VectorXd frame = xx.col(i).segment(n * wlen / 2, wlen) * win /
                       swin.segment(n * wlen / 2, wlen);
      // FFT
      VectorXcd fframe;
      fft.fwd(fframe, frame);
      X(i).col(n) = fframe.segment(0, F);
    }
  }

  // See data as a F-by-N-array of I-vectors
  _set(ArrayVectorXcd(F, N));
  for (int n = 0; n < N; n++) {
    for (int f = 0; f < F; f++) {
      (*this)(f, n) = VectorXcd(I);
      for (int i = 0; i < I; i++) {
        (*this)(f, n)(i) = X(i)(f, n);
      }
    }
  }
}

std::vector<Audio> TFRepr::FilterSTFT(const Audio &x, int wlen, const std::vector<Source> &srcs, const ArrayMatrixXcd &Sigma_x_inverse) {

  // Checking window length
  if (wlen % 4 != 0 || wlen == 0) {
    stringstream s;
    s << "Error:\twlen is " << wlen << " and should be multiple of 4.\n";
    throw runtime_error(s.str());
  }
  
  // Computing TF representation
  fasst::TFRepr X(x, wlen);
  int F = X.bins();
  int N = X.frames();
  int J = srcs.size();
  int samples = x.samples();

  // Checking if dimensions are consistent
  if (F != srcs[0].bins()) {
    stringstream s;
    s << "Error:\tnumber of bins is not consistent:\n";
    s << "F = " << F << " in wavfile\n";
    s << "F = " << srcs[0].bins() << " in xml file\n";
    throw runtime_error(s.str());
  }
  if (N != srcs[0].frames()) {
    stringstream s;
    s << "Error:\tnumber of frames is not consistent:\n";
    s << "N = " << N << " in wavfile\n";
    s << "N = " << srcs[0].frames() << " in xml file\n";
    throw runtime_error(s.str());
  }

  // Source estimation: Eq. 31
  vector<Audio> output(J);
  for (int j = 0; j < J; j++) {
    fasst::TFRepr Y(F, N);
    for (int n = 0; n < N; n++) {
      for (int f = 0; f < F; f++) {
        Y(f, n) = srcs[j].WienerFilter(f,n,Sigma_x_inverse(f, n)) * X(f, n);
      }
    }
    
    // Computing TF inverse
    fasst::Audio y = Y.inverse(wlen, samples);
    output[j] = y;
  }
  return output;
}

Audio TFRepr::inverse(int wlen, int samples) {
  int I = channels();
  int F = bins();
  int N = frames();

  // Defining sine window
  ArrayXd win = Eigen::sin(ArrayXd::LinSpaced(wlen, 0.5, wlen - 0.5) / wlen * M_PI);

  // Pre-processing for edges
  ArrayXd swin = ArrayXd::Zero((N + 1) * wlen / 2);
  for (int n = 0; n < N; n++) {
    swin.segment(n * wlen / 2, wlen) += (win * win);
  }
  swin = Eigen::sqrt(swin / wlen);

  // See data as a I-vector of F-by-N-arrays
  VectorMatrixXcd X(I);
  for (int i = 0; i < I; i++) {
    X(i) = ArrayXXcd(F, N);
    for (int n = 0; n < N; n++) {
      for (int f = 0; f < F; f++) {
        X(i)(f, n) = (*this)(f, n)(i);
      }
    }
  }

  ArrayXXd x = ArrayXXd::Zero((N + 1) * wlen / 2, I);
  FFT<double> fft;
  for (int i = 0; i < I; i++) {
    for (int n = 0; n < N; n++) {
      // IFFT
      VectorXcd fframe(wlen);
      fframe.segment(0, F) = X(i).col(n);
      fframe.segment(F, F - 2) = X(i).col(n).segment(1, F - 2).reverse();
      VectorXd frame;
      fft.inv(frame, fframe);

      // Overlap-add
      x.col(i).segment(n * wlen / 2, wlen) +=
          frame.array() * win / swin.segment(n * wlen / 2, wlen);
    }
  }

  // Truncation
  ArrayXXd xx = x.block(wlen / 4, 0, samples, I);
  return Audio(xx);
}
}
