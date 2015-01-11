#include "Audio.h"
#include <sndfile.hh>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace Eigen;

namespace fasst {
Audio::Audio(const char *fname) {
  // Open fname
  SndfileHandle wavFile(fname);
  if (wavFile.error()) {
    stringstream s;
    s << "Can not open " << fname << ". ";
    s << wavFile.strError();
    throw runtime_error(s.str());
  }

  // Read fname to a buffer
  int samples = static_cast<int>(wavFile.frames());
  int channels = wavFile.channels();
  unsigned int buffer_size = samples * channels;
  vector<double> buffer(buffer_size);
  wavFile.read(&buffer[0], buffer_size);

  // Load audio data
  _set(ArrayXXd(samples, channels));
  for (int i = 0; i < samples; i++) {
    for (int j = 0; j < channels; j++) {
      (*this)(i, j) = buffer[i * channels + j];
    }
  }
  m_samplerate = wavFile.samplerate();
}

void Audio::write(const string &fname, int samplerate) {
    m_samplerate = samplerate;
    write(fname);
}

void Audio::write(const string &fname) {
  // Open fname
  SndfileHandle wavFile = SndfileHandle(
      fname, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, channels(), m_samplerate);
  if (wavFile.error()) {
    stringstream s;
    s << "Can not open " << fname << ". ";
    s << "You probably don't have write access to this location.";
    throw runtime_error(s.str());
  }

  // Load audio data to a buffer
  unsigned int buffer_size = samples() * channels();
  vector<double> buffer(buffer_size);
  for (int i = 0; i < samples(); i++) {
    for (int j = 0; j < channels(); j++) {
      buffer[i * channels() + j] = (*this)(i, j);
    }
  }

  // Write buffer
  wavFile.write(&buffer[0], buffer_size);
}
}
