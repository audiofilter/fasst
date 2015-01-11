#include "fasst/Audio.h"
#include "fasst/XMLDoc.h"
#include "fasst/TFRepr.h"
#include "fasst/Sources.h"
#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

int main(int argc, char *argv[]) {
  // Read command line args
  if (argc != 4) {
    cout << "Usage:\t" << argv[0] << " input-wav-file input-xml-file output-wav-dir\n";
    return 1;
  }

  // Read audio
  fasst::Audio x(argv[1]);

  // Read wlen and TFR type from XML
  fasst::XMLDoc doc(argv[2]);
  std::string tfr_type = doc.getTFRType();
  int wlen = doc.getWlen();
  
  // Read output dirname
  string dirname = argv[3];
  if (dirname[dirname.length()-1] != '/') {
      dirname.push_back('/');
  }

  // Load sources
  fasst::Sources sources = doc.getSources();
  int J = sources.size();

  // Wiener filter
  vector<fasst::Audio> output =  sources.Filter(x, tfr_type, wlen);


  // Write audio file
  for (int j = 0; j < J; j++) {
    string srcname;
    if (sources[j].name().empty()) {
      stringstream ss;
      ss << j;
      srcname = "y" + ss.str();
    } else {
      srcname = sources[j].name();
    }
    output[j].write(dirname + srcname + ".wav", x.samplerate());
  }
}
