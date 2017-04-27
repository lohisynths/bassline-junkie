#!/bin/bash
set -e

# http://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
BGreen='\033[1;32m'
NC='\033[0m'


make clean -C stk
make -j4 lib -C stk

#make -j4 clean -C bassline-junkie/x86_debug
#make -j4 clean -C bassline-junkie/x86_release
#make -j4 clean -C bassline-junkie/x86_tests

#make -j4 -C bassline-junkie/x86_debug
#make -j4 -C bassline-junkie/x86_release
#make -j4 -C bassline-junkie/x86_tests

make -j4 clean -C bassline-junkie/arm_debug

make -j4 -C bassline-junkie/arm_debug

echo -e ${BGreen} 'build done' ${NC}
