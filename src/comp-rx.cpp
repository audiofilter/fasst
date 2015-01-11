#include "fasst/Audio.h"
#include "fasst/XMLDoc.h"
#include "fasst/MixCovMatrix.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  // Read command line args
  if (argc != 4) {
    cout << "Usage:\t" << argv[0]
         << " input-wav-file input-xml-file output-bin-file\n";
    return 1;
  }

  // Read audio
  fasst::Audio x(argv[1]);

  // Read TFR parameters from XML
  fasst::XMLDoc doc(argv[2]);
  std::string tfr_type = doc.getTFRType();
  int wlen = doc.getWlen();
  int nbin = doc.getNbin();

  // Compute Rx
  fasst::MixCovMatrix Rx(x, tfr_type, wlen, nbin);

  // Export Rx
  Rx.write(argv[3]);

  return 0;
}
