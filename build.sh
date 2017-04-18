#!/bin/bash
set -e

#cd stk
#autoreconf
#./configure CXX="clang++" CC="clang" --enable-debug --with-alsa --disable-shared --enable-static
#./configure --enable-debug --with-alsa --disable-shared --enable-static


make clean -C stk
make -j4 lib -C stk


make -j4 clean -C bassline-junkie/x86_debug
make -j4 clean -C bassline-junkie/x86_release
make -j4 clean -C bassline-junkie/x86_tests


make -j4 -C bassline-junkie/x86_debug
make -j4 -C bassline-junkie/x86_release
make -j4 -C bassline-junkie/x86_tests



echo "TADAAAAAAA  "
