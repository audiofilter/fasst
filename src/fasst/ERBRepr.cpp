#include "ERBRepr.h"
#include "Audio.h"
#include "Sources.h"
#include <stdexcept>
#include <unsupported/Eigen/FFT>
#include <unsupported/Eigen/MatrixFunctions>

using namespace Eigen;
using namespace std;

namespace fasst {

// round is not available in MSVC so we reimplent it here
// See: https://stackoverflow.com/questions/19884536/error-c3861-roundf-identifier-not-found
double round(double x) {
  return x >= 0. ? floor(x+0.5) : ceil(x-0.5);
}

ERBRepr::ERBRepr(const Audio &x, int wlen, int nbin) {
  int samples = x.samples();
  double fs = x.samplerate();
  int I = x.channels();
  int F = nbin;
  const std::complex<double> M_I(0, 1);

  // Checking window length
  if (wlen % 2 != 0 || wlen == 0) {
    stringstream s;
    s << "Error:\twlen is " << wlen << " and should be multiple of 2.\n";
    throw runtime_error(s.str());
  }

  // Determining frequency and window length scales
  double emax = 9.26 * std::log(0.00437 * fs / 2. + 1.);
  ArrayXd e = ArrayXd::LinSpaced(F, 0., emax);
  ArrayXd fre = ((e/9.26).exp() - 1.) / 0.00437;
  ArrayXd a = 0.5 * (F-1.) / emax * 9.26 * 0.00437 * fs * (-e/9.26).exp() - .5;

  // Determining dyadic downsampling factors (for fast computation)
  ArrayXd fup = fre + 1.5 * fs / (2 * a + 1.);
  ArrayXd logsubs = (-(2 * fup / fs).log()).min(std::log(wlen/2.));
  for (int f = 0; f < F; f++) {
    logsubs(f) = std::floor(logsubs(f) / std::log(2.));
  }
  ArrayXd subs = (logsubs.max(0) * std::log(2.)).exp();
  double submax = 1.;
  double dwlen = wlen / 2.;
  while (std::floor(dwlen / 2.) == dwlen / 2.) {
    submax = submax * 2.;
    dwlen = dwlen / 2.;
  }
  subs = subs.min(submax);
  for (int f = 0; f < F; f++) {
    subs(f) = round(subs(f));
  }
  ArrayXd subs_shift(F);
  subs_shift.segment(0, F-1) = subs.tail(F-1);
  subs_shift(F-1) = 1.;

  // Defining sine window
  ArrayXd win = Eigen::sin(ArrayXd::LinSpaced(wlen, 0.5, wlen - 0.5) / wlen * M_PI);

  // Zero-padding and Hilbert transform
  int N = static_cast<int>(ceil(static_cast<double>(samples) / wlen * 2));
  ArrayXXcd xx((N + 1) * wlen / 2, I);
  FFT<double> fft;
  for (int i = 0; i < I; i++) {
    VectorXd xchan = VectorXd::Zero((N + 1) * wlen / 2);
    xchan.head(samples) = x.col(i);
    VectorXcd fchan;
    fft.fwd(fchan, xchan);
    int index = std::ceil((N+1) * wlen/4.) - 1;
    fchan.segment(1, index) = 2. * fchan.segment(1, index);
    fchan.tail(index) = VectorXcd::Zero(index);
    VectorXcd xxchan;
    fft.inv(xxchan, fchan);
    xx.col(i) = xxchan;
  }

  // Pre-processing for edges
  ArrayXd swin = ArrayXd::Zero((N + 1) * wlen / 2);
  for (int n = 0; n < N; n++) {
    swin.segment(n * wlen / 2, wlen) += (win * win);
  }
  swin = Eigen::sqrt(swin);

  // Loop over frequency bins
  _set(ArrayMatrixXcd(F, N));
  for (int f = F - 1; f >= 0; f--) {

    // Dyadic downsampling
    if (subs(f) != subs_shift(f)) {
      xx = downsample(xx);
      wlen = wlen / 2;
      win = Eigen::sin(ArrayXd::LinSpaced(wlen, 0.5, wlen - 0.5) / wlen * M_PI);
      swin = ArrayXd::Zero((N + 1) * wlen / 2);
      for (int n = 0; n < N; n++) {
        swin.segment(n * wlen / 2, wlen) += (win * win);
      }
      swin = Eigen::sqrt(swin);
    }

    // Bandpass filter
    int hwlen = round(a(f) / subs(f));
    ArrayXd hann = 0.5 - Eigen::cos(ArrayXd::LinSpaced(2 * hwlen + 1, 1., 2. * hwlen + 1.) / (hwlen + 1.) * M_PI) * 0.5;
    ArrayXcd h = Eigen::exp(ArrayXd::LinSpaced(2 * hwlen + 1, -hwlen, hwlen) * 2 * M_I * M_PI * fre(f) / fs * subs(f)) * hann;
    ArrayXXcd xxband = fftfilt(h, xx);

    // Time integration
    for (int n = 0; n < N; n++) {
      MatrixXcd xxfram(wlen, I);
      for (int i = 0; i < I; i++) {
        xxfram.col(i) = xxband.block(n * wlen / 2, i, wlen, 1) * win / swin.segment(n * wlen / 2, wlen);
      }
      (*this)(f, n) = (xxfram.adjoint() * xxfram).conjugate() * subs(f) / std::pow(hwlen + 1., 2.);
    }
  }
}

  std::vector<Audio> ERBRepr::FilterERB(const Audio &x, int wlen, const std::vector<Source> &srcs, const ArrayMatrixXcd &Sigma_x_inverse) {

  int samples = x.samples();
  double fs = x.samplerate();
  int I = x.channels();
  int J = srcs.size();
  int F = srcs[0].bins();
  const std::complex<double> M_I(0, 1);
  ArrayXd ranks(J + 1);
  ranks(0) = 0;
  for (int j = 0; j < J; j++) {
    ranks(j + 1) = srcs[j].rank();
  }
  int R = ranks.sum();
  
  // Checking window length
  if (wlen % 2 != 0 || wlen == 0) {
    stringstream s;
    s << "Error:\twlen is " << wlen << " and should be multiple of 2.\n";
    throw runtime_error(s.str());
  }

  // Determining frequency and window length scales
  double emax = 9.26 * std::log(0.00437 * fs / 2. + 1.);
  ArrayXd e = ArrayXd::LinSpaced(F, 0., emax);
  ArrayXd fre = ((e/9.26).exp() - 1.) / 0.00437;
  ArrayXd a = 0.5 * (F-1.) / emax * 9.26 * 0.00437 * fs * (-e/9.26).exp() - .5;
  
  // Determining dyadic downsampling factors (for fast computation)
  ArrayXd fup = fre + 1.5 * fs / (2 * a + 1.);
  ArrayXd logsubs = (-(2 * fup / fs).log()).min(std::log(wlen/2.));
  for (int f = 0; f < F; f++) {
    logsubs(f) = std::floor(logsubs(f) / std::log(2.));
  }
  ArrayXd subs = (logsubs.max(0) * std::log(2.)).exp();
  double submax = 1.;
  double dwlen = wlen / 2.;
  while (std::floor(dwlen / 2.) == dwlen / 2.) {
    submax = submax * 2.;
    dwlen = dwlen / 2.;
  }
  subs = subs.min(submax);
  subs = subs.min(512.);
  for (int f = 0; f < F; f++) {
    subs(f) = round(subs(f));
  }
  ArrayXd subs_shift(F);
  subs_shift.segment(0, F-1) = subs.tail(F-1);
  subs_shift(F-1) = 1.;

  // Determining filterbank and inverse filterbank magnitude response
  int ngrid = 1000;
  ArrayXd egrid = ArrayXd::LinSpaced(ngrid, 0., emax);
  ArrayXd fgrid = ((egrid/9.26).exp() - 1.) / 0.00437;
  MatrixXd resp(ngrid,F);
  for (int f = 0; f < F; f++) {
    int hwlen = round(a(f) / subs(f));
    double alen = (2. * hwlen + 1.) * subs(f);
    ArrayXd r = (fgrid - fre(f)) * alen / fs;
    for (int g = 0; g < ngrid; g++) {
      if (r(g) == 0) {
	r(g) = 1e-12;
      }
    }
    resp.col(f) = (Eigen::sin(M_PI*r).cwiseQuotient(M_PI*r) + .5 * Eigen::sin(M_PI*(r+1.)).cwiseQuotient(M_PI*(r+1.)) + .5 * Eigen::sin(M_PI*(r-1.)).cwiseQuotient(M_PI*(r-1.))).pow(2);
  }
  ArrayXd wei = (resp.adjoint() * resp).inverse() * resp.adjoint() * VectorXd::Ones(ngrid);

  // Defining sine window
  ArrayXd win = Eigen::sin(ArrayXd::LinSpaced(wlen, 0.5, wlen - 0.5) / wlen * M_PI);

  // Checking if dimensions are consistent
  int N = static_cast<int>(ceil(static_cast<double>(samples) / wlen * 2));
  if (N != srcs[0].frames()) {
    stringstream s;
    s << "Error:\tnumber of frames is not consistent:\n";
    s << "N = " << N << " in wavfile\n";
    s << "N = " << srcs[0].frames() << " in xml file\n";
    throw runtime_error(s.str());
  }

  // Zero-padding and Hilbert transform
  ArrayXXcd xx((N + 1) * wlen / 2, I);
  FFT<double> fft;
  for (int i = 0; i < I; i++) {
    VectorXd xchan = VectorXd::Zero((N + 1) * wlen / 2);
    xchan.head(samples) = x.col(i);
    VectorXcd fchan;
    fft.fwd(fchan, xchan);
    int index = std::ceil((N+1) * wlen/4.) - 1;
    fchan.segment(1, index) = 2. * fchan.segment(1, index);
    fchan.tail(index) = VectorXcd::Zero(index);
    VectorXcd xxchan;
    fft.inv(xxchan, fchan);
    xx.col(i) = xxchan;
  }

  // Pre-processing for edges
  ArrayXd swin = ArrayXd::Zero((N + 1) * wlen / 2);
  for (int n = 0; n < N; n++) {
    swin.segment(n * wlen / 2, wlen) += (win * win);
  }
  swin = Eigen::sqrt(swin);

  // Initialize source signals
  ArrayXXd yy = ArrayXXd::Zero((N + 1) * wlen / 2, R);
  ArrayXXcd yyscale = ArrayXXcd::Zero((N + 1) * wlen / 2, R);
  
  // Loop over frequency bins
  for (int f = F - 1; f >= 0; f--) {

    // Dyadic downsampling
    if (subs(f) != subs_shift(f)) {
      xx = downsample(xx);
      wlen = wlen / 2;
      win = Eigen::sin(ArrayXd::LinSpaced(wlen, 0.5, wlen - 0.5) / wlen * M_PI);
      swin = ArrayXd::Zero((N + 1) * wlen / 2);
      for (int n = 0; n < N; n++) {
        swin.segment(n * wlen / 2, wlen) += (win * win);
      }
      swin = Eigen::sqrt(swin);
      yy += upsample(yyscale, subs(f+1)).real();
      yyscale = ArrayXXcd::Zero((N + 1) * wlen / 2, R);
    }

    // Filterbank
    int hwlen = round(a(f) / subs(f));
    ArrayXd hann = 0.5 - Eigen::cos(ArrayXd::LinSpaced(2 * hwlen + 1, 1., 2. * hwlen + 1.) / (hwlen + 1.) * M_PI) * 0.5;
    ArrayXcd h = Eigen::exp(ArrayXd::LinSpaced(2 * hwlen + 1, -hwlen, hwlen) * 2 * M_I * M_PI * fre(f) / fs * subs(f)) * hann / (hwlen + 1.);
    ArrayXXcd xxband = fftfilt(h, xx);

    // Bandwise filtering
    ArrayXXcd yyband = ArrayXXcd::Zero((N + 1) * wlen / 2, R);
    for (int j = 0; j < J; j++) {
      int rankpos = (ranks.segment(0, j+1)).sum();
      for (int n = 0; n < N; n++) {
	MatrixXcd xxfram(wlen, I);
	for (int i = 0; i < I; i++) {
	  xxfram.col(i) = xxband.block(n * wlen / 2, i, wlen, 1) * (win / swin.segment(n * wlen / 2, wlen)).pow(2);
	}
	MatrixXcd W = srcs[j].WienerFilter(f,n,Sigma_x_inverse(f, n));
	yyband.block(n * wlen / 2, rankpos, wlen, ranks(j+1)) += (xxfram * W.transpose()).array();
      }
    }

    // Inverse filterbank
    yyscale += wei(f) * fftfilt(h, yyband);
  }
  yy += upsample(yyscale, subs(0)).real();
 
  // Seeing output as audio
  vector<Audio> output(J);
  for (int j = 0; j < J; j++) {
    int rankpos = (ranks.segment(0, j+1)).sum();
    ArrayXXd y = yy.block(0, rankpos, samples, ranks(j+1));
    output[j] = Audio(y);
  }
  return output;
}

Eigen::ArrayXXcd ERBRepr::fftfilt(Eigen::ArrayXcd h, Eigen::ArrayXXcd x) {
  int samples = x.rows();
  int I = x.cols();
  int L = h.rows();

  // Checking filter length
  if (L % 2 == 0) {
    stringstream s;
    s << "Error:\filter length is " << L << " and should be odd.\n";
    throw runtime_error(s.str());
  }
  
  // FFT size
  int nfft = std::pow(2, std::ceil(std::log(static_cast<double>(samples + L - 1))/std::log(2.)));
  
  // Zero-padding and FFT
  ArrayXXcd y(samples, I);
  FFT<double> fft;
  VectorXcd hpad = VectorXcd::Zero(nfft);
  hpad.head(L) = h;
  VectorXcd fhpad;
  fft.fwd(fhpad, hpad);
  for (int i = 0; i < I; i++) {
    VectorXcd xchan = VectorXcd::Zero(nfft);
    xchan.head(samples) = x.col(i);
    VectorXcd fxchan;
    fft.fwd(fxchan, xchan);

    // Inverse FFT and truncation
    VectorXcd fychan = fxchan.cwiseProduct(fhpad);
    VectorXcd ychan;
    fft.inv(ychan, fychan);
    y.col(i) = ychan.segment((L - 1)/ 2, samples);
  }
  return y;
}

Eigen::ArrayXXcd ERBRepr::downsample(Eigen::ArrayXXcd x) {
  int samples = x.rows();
  int I = x.cols();
  
  // Lowpass filter
  ArrayXd coeff(50);
  coeff << -0.000133178150, 0.000169248200, -0.000210182991, 0.000256357557, -0.000308162910, 0.000366006936, -0.000430315500, 0.000501533833, -0.000580128236, 0.000666588166, -0.000761428788, 0.000865194071, -0.000978460543, 0.001101841829, -0.001235994132, 0.001381622851, -0.001539490568, 0.001710426705, -0.001895339216, 0.002095228768, -0.002311206013, 0.002544512690, -0.002796547517, 0.003068898145, -0.003363380811, 0.003682089868, -0.004027460137, 0.004402346030, -0.004810122891, 0.005254818118, -0.005741282722, 0.006275418606, -0.006864483808, 0.007517508754, -0.008245873624, 0.009064124553, -0.009991152491, 0.011051937730, -0.012280204408, 0.013722591565, -0.015445458153, 0.017546491099, -0.020175599200, 0.023575092892, -0.028163695506, 0.034732568224, -0.044977376046, 0.063307309465, -0.105890188403, 0.318238799405;
  ArrayXcd h = ArrayXcd::Zero(201);
  h(100) = 0.5;
  for (int t = 0; t < 50; t++) {
    h(2 * t + 1) = coeff(t);
    h(199 - 2 * t) = coeff(t);
  }
  
  // Filtering
  ArrayXXcd y = fftfilt(h, x);

  // Downsampling
  ArrayXXcd xr(samples / 2, I);
  for (int t = 0; t < samples / 2; t++) {
    xr.row(t) = y.row(2 * t);
  }
  return xr;
}

Eigen::ArrayXXcd ERBRepr::upsample(Eigen::ArrayXXcd x, int factor) {
  int samples = x.rows();
  int I = x.cols();

  // Here we define the double array "coeff"
  #include "lowpass.h"

  // Lowpass filter
  ArrayXcd h = ArrayXcd::Zero(100 * factor + 1);
  h(50 * factor) = 1.;
  for (int t = 0; t < 50 * factor; t++) {
    h(t) = coeff[t * 512 / factor];
    h(100 * factor - t) = coeff[t * 512 / factor];
  }

  // Upsampling
  ArrayXXcd y = ArrayXXcd::Zero(samples * factor, I);
  for (int t = 0; t < samples; t++) {
    y.row(factor * t) = x.row(t);
  }

  // Filtering
  ArrayXXcd xr = fftfilt(h, y);
  return xr;
}
}
