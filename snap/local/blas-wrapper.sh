#!/bin/bash


SNAP=`echo $SNAP | sed -e "s|/var/lib/snapd||g"`

if [ "$SNAP_ARCH" == "amd64" ]; then
  ARCH="x86_64-linux-gnu"
elif [ "$SNAP_ARCH" == "armhf" ]; then
  ARCH="arm-linux-gnueabihf"
elif [ "$SNAP_ARCH" == "arm64" ]; then
  ARCH="aarch64-linux-gnu"
elif [ "$SNAP_ARCH" == "ppc64el" ]; then
  ARCH="powerpc64le-linux-gnu"
else
  ARCH="$SNAP_ARCH-linux-gnu"
fi


function append_dir() {
  local var="$1"
  local dir="$2"
  if [ -d "$dir" ]; then
    eval "export $var=\"\${$var:+\$$var:}\$dir\""
  fi
}

append_dir LD_LIBRARY_PATH $SNAP/usr/lib/$ARCH/blas
append_dir LD_LIBRARY_PATH $SNAP/usr/lib/$ARCH/lapack

exec "$@"
