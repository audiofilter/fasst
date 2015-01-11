#!/usr/bin/python

from __future__ import division
import numpy as np
import os
import sys
import argparse
import scipy.io.wavfile

# Import the fasst package
fasst_python_dir = '/src/build/scripts/python'
if fasst_python_dir not in sys.path:
    sys.path.insert(0, fasst_python_dir)
import fasst

# Command line args
parser = argparse.ArgumentParser()
parser.add_argument('-i', "--iterations", type=int, default=200)
parser.add_argument('-s', "--sources", type=int, default=3)
args = parser.parse_args()
iterations = args.iterations
J = args.sources

audio_fname = '/data/mix.wav'
if not os.path.exists(audio_fname):
    print "Error: No such file or directory: '{}'".format(audio_fname)
    sys.exit(1)

try:
    (fs, x) = scipy.io.wavfile.read(audio_fname)
except IOError, ValueError:
    print "Error: '{}' is not a WAV file".format(audio_fname)
    sys.exit(1)

I = x.shape[1]
if I != 2:
    print "Error: '{}' should have 2 channels (has {})".format(audio_fname, I)
    sys.exit(1)

if fs != 16000:
    # resample
    import scipy.signal
    num = 16000 * x.shape[0] / fs
    x = scipy.signal.resample(x, num).astype(np.int16)
    audio_fname = '/data/mix-16000Hz.wav'
    scipy.io.wavfile.write(audio_fname, 16000, x)

nsampl = x.shape[0]

# Compute STFT spectrogram size
wlen = 1024
F = wlen/2 + 1
N = np.ceil(nsampl/wlen*2)

# Define sources
K = 4  # NMF rank
sources = []
for j in range(J):
    source = {}
    source['A'] = {}
    source['A']['adaptability'] = 'free'
    source['A']['mixingType'] = 'conv'
    source['A']['data'] = np.tile(
        np.array(([np.sin((j+1)*np.pi/(J+1)/2)],
                  [np.cos((j+1)*np.pi/(J+1)/2)]))[:, :, np.newaxis], [1, 1, F])
    source['Wex'] = {}
    source['Wex']['adaptability'] = 'free'
    source['Wex']['data'] = 0.75 * abs(np.random.randn(F, K)) + \
        0.25 * np.ones((F, K))
    source['Hex'] = {}
    source['Hex']['data'] = 0.75 * abs(np.random.randn(K, N)) + \
        0.25 * np.ones((K, N))
    source['Hex']['adaptability'] = 'free'
    sources.append(source)

# Define data structure
data = {}
data['iterations'] = iterations
data['wlen'] = wlen
data['sources'] = sources

# Write to XML
xml_fname = 'sources.xml'
fasst.writeXML(xml_fname, data)

print 'Input time-frequency representation'
fasst.compute_mixture_covariance_matrix(audio_fname, xml_fname, 'Rx.bin')

# Run EM algorithm
fasst.estimate_source_parameters(xml_fname, 'Rx.bin', xml_fname + '.new')

print 'Computation of the spatial source images'
fasst.estimate_sources(audio_fname, xml_fname + '.new', '/data')
