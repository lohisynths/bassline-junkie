#!/bin/bash
set -e 


main() {
	if [ "$#" -ne 1 ]
	then
	  echo "Usage: ..."
	  exit 1
	fi


	for i in "$@" ; do
	    if [[ $i == "arm" ]] ; then
		echo "Is set and compiling!"
		check_crosscompiler
		git_init
		configure arm
		./build.sh arm
		break
	    elif [[ $i == "x86" ]] ; then
		echo "Is set and compiling!"
		git_init
		configure x86
		./build.sh x86
		break
	    fi
	done

echo -e ${BGreen} 'done' ${NC}

	#exit
}



cross_compiler=arm-linux-gnueabihf-g++

# http://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
BGreen='\033[1;32m'
NC='\033[0m'

function check_crosscompiler {
	# http://stackoverflow.com/questions/592620/check-if-a-program-exists-from-a-bash-script
	command -v $cross_compiler >/dev/null 2>&1 || { echo -e >&2 ${RED} $cross_compiler "is requied but it's not installed.  Aborting." ${NC}; exit 1; }
	type $cross_compiler >/dev/null 2>&1 || { echo >&2 $cross_compiler "is requied but it's not installed.  Aborting."; exit 1; }
	hash $cross_compiler 2>/dev/null || { echo >&2 $cross_compiler "is requied but it's not installed.  Aborting."; exit 1; }

	echo -e ${GREEN} 'cross-compiler' $cross_compiler 'found' ${NC}
}  

function git_init {
	echo -e ${GREEN} 'submodule init' ${NC}
	git submodule init
	git submodule update

	cd stk
	echo -e ${GREEN} 'updating stk subrepo' ${NC}
	git checkout v4.6.0
	#git checkout master
	cd ../

	cd benchmark
	echo -e ${GREEN} 'updating google benchmark subrepo' ${NC}
	git pull lohi v2
	git checkout v2
	cd ../
}


function configure {

	echo "cmake arm google benchmark"
	cd benchmark
	mkdir -p build_arm
	cd build_arm
	cmake -DCMAKE_TOOLCHAIN_FILE=/home/alax/git/buildroot/output/host/share/buildroot/toolchainfile.cmake ../
	cd ../../
	echo "done"

	cd stk
	autoreconf

	for i in "$@" ; do
	    if [[ $i == "arm" ]] ; then
		echo "arm configure !"
		export 
		CXXFLAGS="-O3 -mfpu=neon-vfpv4 -mfloat-abi=hard -funsafe-math-optimizations -ftree-vectorize" 
		./configure --host=arm-linux-gnueabihf --build=x86_64-linux-gnu --enable-debug --with-alsa --disable-shared --enable-static
		break
	    elif [[ $i == "x86" ]] ; then
		echo "x86 configure !"
		./configure --enable-debug --with-alsa --disable-shared --enable-static
		break
	    else
		echo "wrong configure !"
		exit
	    fi
	done
	cd ../
}



main "$@"

