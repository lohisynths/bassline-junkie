#!/bin/bash
set -e 
cross_compiler=arm-linux-gnueabihf-g++

# http://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
BGreen='\033[1;32m'
NC='\033[0m'

# http://stackoverflow.com/questions/592620/check-if-a-program-exists-from-a-bash-script
command -v $cross_compiler >/dev/null 2>&1 || { echo -e >&2 ${RED} $cross_compiler "is requied but it's not installed.  Aborting." ${NC}; exit 1; }
type $cross_compiler >/dev/null 2>&1 || { echo >&2 $cross_compiler "is requied but it's not installed.  Aborting."; exit 1; }
hash $cross_compiler 2>/dev/null || { echo >&2 $cross_compiler "is requied but it's not installed.  Aborting."; exit 1; }

echo -e ${GREEN} 'cross-compiler' $cross_compiler 'found' ${NC}

echo -e ${GREEN} 'submodule init' ${NC}
git submodule init
git submodule update

cd stk

echo -e ${GREEN} 'updating stk subrepo' ${NC}
git checkout master

echo -e ${GREEN} 'configuring stk subrepo' ${NC}
autoreconf
#./configure --enable-debug --with-alsa --disable-shared --enable-static
#./configure --with-alsa --disable-shared --enable-static
./configure --host=arm-linux-gnueabihf --build=x86_64-linux-gnu --enable-debug --with-alsa --disable-shared --enable-static


echo -e ${BGreen} 'done' ${NC}
