cd stk
autoreconf
./configure CXX="clang++" CC="clang" --enable-debug --with-alsa --disable-shared --enable-static
cd ..

make clean -C stk
make -j -C stk

make clean -C Plot/Debug
make -j -C Plot/Debug
cd Plot/Debug && ../link.sh && cd ../



make -j -C bassline-junkie/x86_debug
make -j -C bassline-junkie/x86_release
make -j -C bassline-junkie/x86_tests

