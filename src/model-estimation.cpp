#include "fasst/XMLDoc.h"
#include "fasst/Sources.h"
#include "fasst/MixCovMatrix.h"
#include "fasst/NaturalStatistics.h"
#include <iostream>

using namespace std;
using namespace Eigen;

int main(int argc, char *argv[]) {
  // Read command line args
  if (argc != 4) {
    cout << "Usage:\t" << argv[0]
         << " input-xml-file input-bin-file output-xml-file\n";
    return 1;
  }

  // Load sources
  fasst::XMLDoc doc(argv[1]);
  fasst::Sources sources = doc.getSources();

  // Load hatRx
  fasst::MixCovMatrix hatRx(argv[2]);
  int F = hatRx.bins();
  int N = hatRx.frames();
  int I = hatRx.channels();

  // Check if dimensions are consistent
  if (F != sources.bins()) {
    cout << "Error:\tnumber of bins is not consistent:\n";
    cout << "F = " << F << " in " << argv[2] << '\n';
    cout << "F = " << sources.bins() << " in " << argv[1] << '\n';
    return 1;
  }
  if (N != sources.frames()) {
    cout << "Error:\tnumber of frames is not consistent:\n";
    cout << "N = " << N << " in " << argv[2] << '\n';
    cout << "N = " << sources.frames() << " in " << argv[1] << '\n';
    return 1;
  }
  if (I != sources.channels()) {
    cout << "Error:\tnumber of channels is not consistent:\n";
    cout << "I = " << I << " in " << argv[2] << '\n';
    cout << "I = " << sources.channels() << " in " << argv[1] << '\n';
    return 1;
  }

  // Compute additive noise
  VectorXd noise = VectorXd::Zero(F);
  for (int f = 0; f < F; f++) {
    for (int n = 0; n < N; n++) {
      noise(f) += (hatRx(f, n).real().trace() / I);
    }
    noise(f) /= N;
  }
  VectorXd noise_beg = noise / 100;
  VectorXd noise_end = noise / 10000;

  // Define number of iterations
  int iterations = doc.getIterations();
  if (iterations == 0) {
    iterations = 50;
  }

  // Main loop
  double log_like_prev = 0;
  for (int iter = 0; iter < iterations; iter++) {
    cout << "GEM iteration " << iter + 1 << " of " << iterations << '\t';

    // Compute Sigma_b
    VectorMatrixXcd Sigma_b(F);
    for (int f = 0; f < F; f++) {
      double sigma_f = (sqrt(noise_beg(f)) * (iterations - iter - 1) +
                        sqrt(noise_end(f)) * (iter + 1)) / iterations;
      Sigma_b(f) = MatrixXcd::Identity(I, I) * sigma_f * sigma_f;
    }

    // Conditional expectation of the natural statistics and log-likelihood
    fasst::NaturalStatistics stats(sources, hatRx, Sigma_b);
    double log_like = stats.logLikelihood();
    if (iter == 0)
      cout << "Log-likelihood: " << log_like << '\n';
    else {
      cout << "Log-likelihood: " << log_like << '\t';
      cout << "Improvement: " << log_like - log_like_prev << '\n';
    }
    log_like_prev = log_like;

    // Update A
    sources.updateMixingParameter(stats);

    // Update V
    sources.updateSpectralPower(stats);
  }

  // Save sources
  fasst::XMLDoc new_doc(doc);
  new_doc.replaceSources(sources);
  new_doc.write(argv[3]);
}
