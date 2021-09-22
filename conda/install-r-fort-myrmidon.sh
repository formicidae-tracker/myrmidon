#!/bin/bash


pushd bindings/R/FortMyrmidon

sed "s/Version: .*/Version: ${PKG_VERSION}/" ../DESCRIPTION.in > DESCRIPTION

export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${PREFIX}/share/pkgconfig

${R} CMD INSTALL --preclean --build .
${R} CMD check --no-manual .

popd
