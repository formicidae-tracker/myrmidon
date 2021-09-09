#!/bin/bash


set -x -e

declare -a CMAKE_PLATFORM_FLAGS

if [[ ${HOST} =~ .*darwin.* ]]; then
	CMAKE_PLATFORM_FLAGS+=(-DCMAKE_OSX_SYSROOT="${CONDA_BUILD_SYSROOT}")
	export LDFLAGS=$(echo "${LDFLAGS}" | sed "s/-Wl,-dead_strip_dylibs//g")
else
	CMAKE_PLATFORM_FLAGS+=(-DCMAKE_TOOLCHAIN_FILE="${RECIPE_DIR}/cross-linux.cmake")
fi

if [[ ${DEBUG_C} == yes ]]; then
  CMAKE_BUILD_TYPE=Debug
else
  CMAKE_BUILD_TYPE=RelWithDebInfo
fi



pushd bindings/python

CMAKE_PLATFORM_FLAGS="${CMAKE_PLATFORM_FLAGS[@]}" CMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE ${PYTHON} setup.py install
${PYTHON} -m unittest discover -v -s tests

popd
