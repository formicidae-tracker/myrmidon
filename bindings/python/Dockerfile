FROM quay.io/pypa/manylinux_2_28:2025.07.02-1

RUN dnf install -y wget
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

WORKDIR /deps
RUN dnf install -y libass-devel freetype-devel gnutls-devel libtool libva-devel libvdpau-devel yasm nasm numactl-devel libdav1d-devel
RUN git clone --depth 1 https://code.videolan.org/videolan/x264.git
WORKDIR x264
RUN ./configure --enable-shared --enable-pic
RUN make
RUN make install

WORKDIR /deps
RUN wget https://bitbucket.org/multicoreware/x265_git/downloads/x265_3.6.tar.gz
RUN tar -xvf x265_3.6.tar.gz

WORKDIR x265_3.6/build/linux
RUN sed -i 's/find_package_handle_standard_args(NUMA/find_package_handle_standard_args(Numa/' ../../source/cmake/FindNuma.cmake
RUN sed -i 's/find_package_handle_standard_args(nasm/find_package_handle_standard_args(Nasm/' ../../source/cmake/FindNasm.cmake
RUN sed -i 's/cmake_policy(SET CMP0025 OLD)/cmake_policy(SET CMP0025 NEW)/' ../../source/CMakeLists.txt
RUN sed -i 's/cmake_policy(SET CMP0054 OLD)/cmake_policy(SET CMP0054 NEW)/' ../../source/CMakeLists.txt
RUN cmake -G "Unix Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DENABLE_SHARED=On -DCMAKE_BUILD_TYPE=Release  ../../source
RUN make -j 12
RUN make install


WORKDIR /deps
RUN git clone --depth 1 https://aomedia.googlesource.com/aom
WORKDIR /deps/aom
RUN cmake -DENABLE_TESTS=OFF -DENABLE_NASM=On -DBUILD_SHARED_LIBS=On -DCMAKE_BUILD_TYPE=Release -B build
RUN make -C build -j 12
RUN make -C build install

WORKDIR /deps
RUN wget https://ffmpeg.org/releases/ffmpeg-7.1.tar.gz
RUN tar -xvf ffmpeg-7.1.tar.gz
WORKDIR /deps/ffmpeg-7.1
RUN PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig:/usr/local/lib64/pkgconfig ./configure  --enable-shared --disable-static --extra-libs="-lpthread -lm" --ld="g++" --enable-gpl --disable-gnutls --enable-libaom  --enable-libass --enable-libfreetype --enable-libdav1d --enable-libx264 --enable-libx265 --enable-nonfree
RUN make -j 12
RUN make install

WORKDIR /project

RUN rm -Rf /deps

LABEL org.opencontainers.image.source=https://github.com/formicidae-tracker/myrmidon/bindings/python
LABEL org.opencontainers.image.description="Python manylinux image for myrmidon"
LABEL org.opencontainers.image.licenses=lgpl-3+
