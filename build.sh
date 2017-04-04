#!/bin/bash
set -e

cd stk
autoreconf
#./configure CXX="clang++" CC="clang" --enable-debug --with-alsa --disable-shared --enable-static
./configure --enable-debug --with-alsa --disable-shared --enable-static
cd ..

make clean -C stk
make -j -C stk

make -j clean -C bassline-junkie/x86_debug
make -j clean -C bassline-junkie/x86_release
make -j clean -C bassline-junkie/x86_tests


make -j -C bassline-junkie/x86_debug
make -j -C bassline-junkie/x86_release
make -j -C bassline-junkie/x86_tests



echo "TADAAAAAAA  "
