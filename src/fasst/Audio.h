#ifndef FASST_AUDIO_H
#define FASST_AUDIO_H

#include <Eigen/Core>

namespace fasst {

/*!
 This class contains audio data. The audio data is stored in an
 `Eigen::ArrayXXd` object. Rows of the array are audio channels and columns of
 the array are audio samples.
 */
class Audio : public Eigen::ArrayXXd {
public:
  /*!
   The main constructor of the class reads audio data from a WAV file. Please
   note that if the file is not readable, the constructor will throw a
   `runtime_error` exception.
   \param fname the name of the WAV file to be read
   */
  Audio(const char *fname);

  Audio(){};

  /*!
   This constructor build an Audio object from audio data stored in an
   `Eigen::ArrayXXd` object.
   \param x the audio data to be copied
   */
  Audio(const Eigen::ArrayXXd &x) : Eigen::ArrayXXd(x) {}

  /*!
   This method writes the audio data to a file, at a given sample rate. Please note that if the file is not writable, the method will throw a `runtime_error` exception.

   \param fname the name of the WAV file to be written
   \param samplerate the samplerate to be written
   */
  void write(const std::string &fname, int samplerate);

  /*!
   This method writes the audio data to a file. Please note that if the file is
   not writable, the method will throw a `runtime_error` exception.

   \param fname the name of the WAV file to be written
   */
  void write(const std::string &fname);

  /*!
   \return the number of audio samples
   */
  inline int samples() const { return rows(); }

  /*!
   \return the number of audio channels
   */
  inline int channels() const { return cols(); }

  /*!
   \return the samplerate
   */
  inline int samplerate() const { return m_samplerate; }

private:
  int m_samplerate;
};
}

#endif
