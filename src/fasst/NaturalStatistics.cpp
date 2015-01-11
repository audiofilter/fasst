#include "NaturalStatistics.h"
#include "MixCovMatrix.h"
#include "Sources.h"
#include <Eigen/Dense>

using namespace Eigen;

namespace fasst {
NaturalStatistics::NaturalStatistics(const Sources &sources,
                                     const MixCovMatrix &hatRx,
                                     const VectorMatrixXcd &Sigma_b) {
  int F = hatRx.bins();
  int N = hatRx.frames();

  m_hatRxs = ArrayMatrixXcd(F, N);
  m_hatRs = ArrayMatrixXcd(F, N);

  int R = sources.A(0).cols();

  double log_like = 0;

#pragma omp parallel for reduction(+ : log_like)
  for (int n = 0; n < N; n++) {
    for (int f = 0; f < F; f++) {
      // Eq. 25 
      VectorXd Phi(R);
      int j = 0;
      int sum = 0;
      for (int r = 0; r < R; r++) {
        if (r == sources[j].rank() + sum) {
          sum += sources[j].rank();
          j++;
        }
        Phi(r) = sources[j].V(f, n);
      }
      DiagonalMatrixXd Sigma_s(R);
      Sigma_s.diagonal() = Phi;

      // Eq. 24
      MatrixXcd Sigma_x =
          sources.A(f) * Sigma_s * sources.A(f).adjoint() + Sigma_b(f);
      MatrixXcd Sigma_x_inverse = Sigma_x.inverse();

      // Eq. 23
      MatrixXcd Omega_s = Sigma_s * sources.A(f).adjoint() * Sigma_x_inverse;

      // Eq. 22
      m_hatRs(f, n) =
          Omega_s * hatRx(f, n) * Omega_s.adjoint() +
          (MatrixXcd::Identity(R, R) - Omega_s * sources.A(f)) * Sigma_s;

      // Eq. 21
      m_hatRxs(f, n) = hatRx(f, n) * Omega_s.adjoint();

      // Log-likelihood: Eq. 16
      log_like -= (Sigma_x_inverse * hatRx(f, n)).real().trace() +
                  log(Sigma_x.determinant().real() * M_PI);
    }
  }
  log_like /= (F * N);
  m_logLikelihood = log_like;
}
}
