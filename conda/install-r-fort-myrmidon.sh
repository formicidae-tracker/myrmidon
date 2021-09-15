#!/bin/bash


pushd bindings/R/FortMyrmidon

sed "s/Version: .*/Version: ${PKG_VERSION}/" ../DESCRIPTION.in > DESCRIPTION

${R} CMD check .
${R} CMD INSTALL --preclean --build .

popd
