# Build with
#
#     docker build -t fasst .
#
# Run with
#
#     docker run -v $DATADIR:/data fasst [-i ITERATIONS] [-s SOURCES]
#
# where DATADIR contains your mix.wav input file
# DATADIR must be an absolute path

FROM debian

# Install build environment
RUN apt-get update && apt-get install -y wget bzip2 g++ make

# Install FASST dependencies
RUN apt-get install -y cmake libqt4-dev libsndfile1-dev python-numpy
RUN wget http://bitbucket.org/eigen/eigen/get/3.2.0.tar.bz2
RUN tar xjf 3.2.0.tar.bz2
RUN cd eigen-eigen-* && mkdir build && cd build && cmake .. && make install

# Install dependency for the docker-entrypoint script
RUN apt-get install -y python-scipy

# Build FASST
COPY . /src
RUN cd /src && mkdir build && cd build && cmake .. && make -j

# Run FASST
ENTRYPOINT ["/src/docker-entrypoint.py"]
