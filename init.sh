#!/bin/bash
git submodule init
git submodule update

cd stk


git checkout master
autoreconf
#./configure --enable-debug --with-alsa --disable-shared --enable-static
./configure --with-alsa --disable-shared --enable-static
