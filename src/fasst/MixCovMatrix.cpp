#include "MixCovMatrix.h"
#include "TFRepr.h"
#include "ERBRepr.h"
#include <fstream>
#include <stdexcept>

using namespace std;
using namespace Eigen;

namespace fasst {
MixCovMatrix::MixCovMatrix(const Audio &x, std::string tfr_type, int wlen, int nbin) {
  if (tfr_type == "STFT") {
    // Compute time-frequency representation
    TFRepr X(x, wlen);
    int F = X.bins();
    int N = X.frames();

    // Compute covariance matrix
    _set(ArrayMatrixXcd(F, N));
    for (int f = 0; f < F; f++) {
      for (int n = 0; n < N; n++) {
	(*this)(f, n) = X(f, n) * X(f, n).adjoint();
      }
    }
  } else if (tfr_type == "ERB") {
    _set(ERBRepr(x, wlen, nbin));
  } else {
    stringstream s;
    s << "Wrong TFR type" << tfr_type << ".";
    throw runtime_error(s.str());
  }
}

MixCovMatrix::MixCovMatrix(const char *fname) {
  // Open fname
  ifstream in(fname, ios_base::binary);
  if (!in.good()) {
    stringstream s;
    s << "Can not open " << fname << ". ";
    s << "File probably doesn't exist or isn't readable.";
    throw runtime_error(s.str());
  }

  // Read ndim
  int ndim = 0;
  in.read(reinterpret_cast<char *>(&ndim), sizeof(int));

  // Read dim
  vector<int> dim(ndim);
  in.read(reinterpret_cast<char *>(&dim[0]), sizeof(int) * ndim);

  int ndata = 1;
  for (int i = 0; i < ndim; i++) {
    ndata *= dim[i];
  }

  // Read data to a buffer
  vector<float> data(ndata);
  in.read(reinterpret_cast<char *>(&data[0]), sizeof(float) * ndata);
  in.close();

  int I = static_cast<int>(std::sqrt(static_cast<double>(dim[0])));
  int F = dim[1];
  int N = dim[2];

  // Load buffer
  _set(ArrayMatrixXcd(F, N));
  for (int n = 0; n < N; n++) {
    for (int f = 0; f < F; f++) {
      MatrixXcd Rx_fn(I, I);

      // Real diagonal elements
      int ind1 = f * I * I + n * F * I * I;
      for (int i = 0; i < I; i++) {
        Rx_fn(i, i) = data[i + ind1];
      }

      // Complex elements
      int sum = 0;
      for (int i1 = 0; i1 < I - 1; i1++) {
        for (int i2 = i1 + 1; i2 < I; i2++) {
          int ind2 = ind1 + (i2 - i1 + sum) * 2 - 2 + I;
          Rx_fn(i1, i2) = complex<double>(data[ind2], data[ind2 + 1]);
          Rx_fn(i2, i1) = conj(Rx_fn(i1, i2));
        }
        sum += I - 1 - i1;
      }
      (*this)(f, n) = Rx_fn;
    }
  }
}

void MixCovMatrix::write(const char *fname) {
  int F = bins();
  int N = frames();
  int I = channels();

  // Open fname
  ofstream out(fname, ios_base::binary);
  if (!out.good()) {
    stringstream s;
    s << "Can not open " << fname << ". ";
    s << "You probably don't have write access to this location.";
    throw runtime_error(s.str());
  }

  // Write ndim
  int ndim = 3;
  out.write(reinterpret_cast<char *>(&ndim), sizeof(int));

  // Write dim
  vector<int> dim(ndim);
  dim[0] = I * I;
  dim[1] = F;
  dim[2] = N;
  out.write(reinterpret_cast<char *>(&dim[0]), sizeof(int) * ndim);

  // Load to buffer
  int ndata = I * I * F * N;
  vector<float> data(ndata);

  for (int n = 0; n < N; n++) {
    for (int f = 0; f < F; f++) {
      int ind1 = (f * I * I) + (n * F * I * I);
      // Real diagonal elements
      for (int i = 0; i < I; i++) {
        data[i + ind1] = static_cast<float>((*this)(f, n)(i, i).real());
      }
      // Complex elements
      int sum = 0;
      for (int i1 = 0; i1 < I - 1; i1++) {
        for (int i2 = i1 + 1; i2 < I; i2++) {
          int ind2 = ind1 + (i2 - i1 + sum) * 2 - 2 + I;
          data[ind2] = static_cast<float>((*this)(f, n)(i1, i2).real());
          data[ind2 + 1] = static_cast<float>((*this)(f, n)(i1, i2).imag());
        }
        sum += I - 1 - i1;
      }
    }
  }

  // Write buffer to file
  out.write(reinterpret_cast<char *>(&data[0]), sizeof(float) * ndata);
}
}
