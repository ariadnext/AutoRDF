#!/bin/bash
sudo apt-get update
apt download libboost-date-time1.74.0
dpkg -i --force-not-root --root=$PREFIX libboost-date-time1.74.0_1.74.0-9_amd64.deb
cp $PREFIX/usr/lib/x86_64-linux-gnu/* $PREFIX/lib/

mkdir -p $PREFIX/lib
cp $AUTORDF_BUILD_DIR/libautordf.so* $PREFIX/lib/
cp $IRMA_INSTALL_DIR/libsord* $PREFIX/lib/
cp $IRMA_INSTALL_DIR/libserd* $PREFIX/lib/

pip install $AUTORDF_BUILD_DIR/dist/autordf-1.0.0-cp39-cp39-linux_x86_64.whl
