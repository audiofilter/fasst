#include "Sources.h"
#include "TFRepr.h"
#include "ERBRepr.h"
#include "Audio.h"
#include "NaturalStatistics.h"
#include <Eigen/Dense>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace Eigen;

namespace fasst {
Sources::Sources(QDomNodeList nodeList) {
  // Load sources
  int R = 0;
  for (size_t j = 0; j < static_cast<size_t>(nodeList.length()); j++) {
    Source source(nodeList.item(j));
    m_sources.push_back(source);
    R += source.rank();
  }

  m_bins = m_sources[0].bins();
  m_frames = m_sources[0].frames();
  m_channels = m_sources[0].A(0).rows();

  // Check if dimensions are consistent for each source
  for (size_t j = 1; j < m_sources.size(); j++) {
    if (m_channels != m_sources[j].A(0).rows()) {
      throw runtime_error(
          "Check your source parameters: number of channels should be the same "
          "for each source mixing parameter");
    }
    if (m_bins != m_sources[j].bins()) {
      throw runtime_error(
          "Check your source parameters: number of rows in Wex and Wft "
          "parameter should be the same for each source");
    }
    if (m_frames != m_sources[j].frames()) {
      throw runtime_error("Check your source parameters: number of columns in "
                          "Hex and Hft should be the same for each source");
    }
  }

  // Compute A from each source mixing parameter
  m_A = VectorMatrixXcd(m_bins);
  for (int f = 0; f < m_bins; f++) {
    m_A(f) = MatrixXcd::Zero(m_channels, R);
    int sum = 0;
    for (size_t j = 0; j < m_sources.size(); j++) {
      if (m_sources[j].isInst()) {
        m_A(f).block(0, sum, m_channels, m_sources[j].rank()) =
            m_sources[j].A(0);
      } else {
        m_A(f).block(0, sum, m_channels, m_sources[j].rank()) =
            m_sources[j].A(f);
      }
      sum += m_sources[j].rank();
    }
  }
}

void Sources::replace(QDomDocument doc, QDomNodeList nodeList) {
  // Update each source mixing parameter with A
  int sum = 0;
  for (size_t j = 0; j < m_sources.size(); j++) {
    if (m_sources[j].isInst()) {
      m_sources[j].A(0) = m_A(0).block(0, sum, m_channels, m_sources[j].rank());
    } else {
      for (int f = 0; f < m_bins; f++) {
        m_sources[j].A(f) =
            m_A(f).block(0, sum, m_channels, m_sources[j].rank());
      }
    }
    sum += m_sources[j].rank();
  }

  // Replace the old source parameters with the new ones
  for (size_t j = 0; j < m_sources.size(); j++) {
    m_sources[j].replace(doc, nodeList.item(j));
  }
}

void Sources::updateMixingParameter(const NaturalStatistics &stats) {
  // Indices
  vector<int> ind_C, ind_Ccomp, ind_I, ind_Icomp;
  int current_index = 0;
  for (size_t j = 0; j < m_sources.size(); j++) {
    vector<int> ind_j(m_sources[j].rank());
    for (int i = 0; i < m_sources[j].rank(); i++) {
      ind_j[i] = current_index;
      current_index++;
    }

    // Here we concatenate the C (or I) subset of indices with the j-th source's
    // indices
    if (m_sources[j].A().isFree() && m_sources[j].isInst()) {
      ind_I.insert(ind_I.end(), ind_j.begin(), ind_j.end());
    } else {
      ind_Icomp.insert(ind_Icomp.end(), ind_j.begin(), ind_j.end());
    }
    if (m_sources[j].A().isFree() && m_sources[j].isConv()) {
      ind_C.insert(ind_C.end(), ind_j.begin(), ind_j.end());
    } else {
      ind_Ccomp.insert(ind_Ccomp.end(), ind_j.begin(), ind_j.end());
    }
  }

  // Eq. 26
  for (int f = 0; f < m_bins; f++) {
    MatrixXcd A_Ccomp(m_channels, ind_Ccomp.size());
    for (size_t i = 0; i < ind_Ccomp.size(); i++) {
      A_Ccomp.col(i) = m_A(f).col(ind_Ccomp[i]);
    }
    MatrixXcd sum = MatrixXcd::Zero(m_channels, ind_C.size());
    for (int n = 0; n < m_frames; n++) {
      MatrixXcd hat_Rxs_C(m_channels, ind_C.size());
      for (size_t i = 0; i < ind_C.size(); i++) {
        hat_Rxs_C.col(i) = stats.hatRxs(f, n).col(ind_C[i]);
      }
      MatrixXcd hat_Rs_Ccomp(ind_Ccomp.size(), ind_C.size());
      for (size_t i = 0; i < ind_Ccomp.size(); i++) {
        for (size_t j = 0; j < ind_C.size(); j++) {
          hat_Rs_Ccomp(i, j) = stats.hatRs(f, n)(ind_Ccomp[i], ind_C[j]);
        }
      }
      sum += hat_Rxs_C - A_Ccomp * hat_Rs_Ccomp;
    }
    MatrixXcd sum_hat_Rs_C = MatrixXcd::Zero(ind_C.size(), ind_C.size());
    for (int n = 0; n < m_frames; n++) {
      for (size_t i = 0; i < ind_C.size(); i++) {
        for (size_t j = 0; j < ind_C.size(); j++) {
          sum_hat_Rs_C(i, j) += stats.hatRs(f, n)(ind_C[i], ind_C[j]);
        }
      }
    }
    MatrixXcd rhs = sum * sum_hat_Rs_C.inverse();
    for (size_t i = 0; i < ind_C.size(); i++) {
      m_A(f).col(ind_C[i]) = rhs.col(i);
    }
  }

  // Eq. 27
  MatrixXcd sum = MatrixXcd::Zero(m_channels, ind_I.size());
  MatrixXcd sum_hat_Rs_I = MatrixXcd::Zero(ind_I.size(), ind_I.size());
  for (int f = 0; f < m_bins; f++) {
    MatrixXcd A_Icomp(m_channels, ind_Icomp.size());
    for (size_t i = 0; i < ind_Icomp.size(); i++) {
      A_Icomp.col(i) = m_A(f).col(ind_Icomp[i]);
    }
    for (int n = 0; n < m_frames; n++) {
      MatrixXcd hat_Rxs_I(m_channels, ind_I.size());
      for (size_t i = 0; i < ind_I.size(); i++) {
        hat_Rxs_I.col(i) = stats.hatRxs(f, n).col(ind_I[i]);
      }
      MatrixXcd hat_Rs_Icomp(ind_Icomp.size(), ind_I.size());
      for (size_t i = 0; i < ind_Icomp.size(); i++) {
        for (size_t j = 0; j < ind_I.size(); j++) {
          hat_Rs_Icomp(i, j) = stats.hatRs(f, n)(ind_Icomp[i], ind_I[j]);
        }
      }
      sum += hat_Rxs_I - A_Icomp * hat_Rs_Icomp;
    }
    for (int n = 0; n < m_frames; n++) {
      for (size_t i = 0; i < ind_I.size(); i++) {
        for (size_t j = 0; j < ind_I.size(); j++) {
          sum_hat_Rs_I(i, j) += stats.hatRs(f, n)(ind_I[i], ind_I[j]);
        }
      }
    }
  }
  MatrixXd rhs = sum.real() * sum_hat_Rs_I.real().inverse();
  for (int f = 0; f < m_bins; f++) {
    for (size_t i = 0; i < ind_I.size(); i++) {
      m_A(f).col(ind_I[i]).real() = rhs.col(i);
    }
  }
}

void Sources::updateSpectralPower(const NaturalStatistics &stats) {
  int J = m_sources.size();

  int first = 0;
  for (int j = 0; j < J; j++) {
    // Compute Xi(f,n): Eq. 29
    ArrayXXd Xi = ArrayXXd::Zero(m_bins, m_frames);
    int last = first + m_sources[j].rank();
    for (int r = first; r < last; r++) {
      for (int f = 0; f < m_bins; f++) {
        for (int n = 0; n < m_frames; n++) {
          Xi(f, n) += stats.hatRs(f, n)(r, r).real();
        }
      }
    }
    Xi /= m_sources[j].rank();
    first = last;

    // Update spectral parameters
    m_sources[j].updateSpectralPower(Xi);
  }
}

  vector<Audio> Sources::Filter(const Audio &x, std::string tfr_type, int wlen){
  int N = m_frames;
  int F = m_bins;
  int I = m_channels;
  int J = size();
  
  // Optional temporal and frequency smoothing
  for (int j = 0; j < J; j++) {
    if(m_sources[j].wiener_c1()!=0 || m_sources[j].wiener_c2()!=0){
      m_sources[j].spectralPowerSmoothing();
    }
  }

  // Update Sigma_y
  for (int j = 0; j < J; j++) {
    m_sources[j].initSigmaY();
  }

  // Compute Sigma_x inverse
  ArrayMatrixXcd Sigma_x_inverse(F, N);
  for (int n = 0; n < N; n++) {
    for (int f = 0; f < F; f++) {
      MatrixXcd Sigma_x = MatrixXcd::Zero(I, I);
      for (int j = 0; j < J; j++) {
        Sigma_x += m_sources[j].Sigma_y(f,n);
      }
      Sigma_x_inverse(f, n) = Sigma_x.inverse();
    }
  }

  // Switch TFR type
  vector<Audio> output(J);
  if (tfr_type == "STFT") {
    output = TFRepr::FilterSTFT(x, wlen, m_sources, Sigma_x_inverse);
  } else if (tfr_type == "ERB") {
    output = ERBRepr::FilterERB(x, wlen, m_sources, Sigma_x_inverse);
  } else {
    stringstream s;
    s << "Wrong TFR type" << tfr_type << ".";
    throw runtime_error(s.str());
  }
  return output;
}

}
