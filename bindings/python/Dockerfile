FROM quay.io/pypa/manylinux_2_28:2025.07.02-1

RUN dnf install -y --nogpgcheck https://download1.rpmfusion.org/free/el/rpmfusion-free-release-8.noarch.rpm
RUN dnf install -y ffmpeg ffmpeg-devel ffmpeg-libs wget
RUN mkdir /deps
WORKDIR /deps
RUN wget https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.zip
RUN unzip eigen-3.4.0.zip
WORKDIR eigen-3.4.0
RUN cmake -B build -S .
RUN cmake --build build
RUN cmake --install build
WORKDIR /deps
RUN wget https://github.com/protocolbuffers/protobuf/releases/download/v3.20.1/protobuf-cpp-3.20.1.zip
RUN unzip protobuf-cpp-3.20.1.zip
WORKDIR protobuf-3.20.1
RUN ./configure
RUN make -j 12
RUN make install
RUN ldconfig
WORKDIR /deps
RUN wget https://github.com/oneapi-src/oneTBB/archive/refs/tags/v2021.5.0.tar.gz
RUN tar -xvf v2021.5.0.tar.gz
WORKDIR oneTBB-2021.5.0
RUN cmake -B build -S . -DTBB_TEST=Off -DCMAKE_POLICY_VERSION_MINIMUM=3.5
RUN cmake --build build -j 12
RUN cmake --install build
WORKDIR /deps
RUN wget https://downloads.sourceforge.net/project/asio/asio/1.18.1%20%28Stable%29/asio-1.18.1.tar.bz2
RUN tar -xvf asio-1.18.1.tar.bz2
WORKDIR asio-1.18.1
RUN ./configure
RUN make -j 12
RUN make install
WORKDIR /project
LABEL org.opencontainers.image.source=https://github.com/formicidae-tracker/myrmidon/bindings/python
LABEL org.opencontainers.image.description="Python manylinux image for myrmidon"
LABEL org.opencontainers.image.licenses=lgpl-3+
