#!/bin/bash
set -e 


cross_compiler=arm-linux-gnueabihf-g++

# http://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
LIGHT_BLUE='\033[1;34m'
NC='\033[0m'

function main() {
	if [ "$#" -ne 1 ]
	then
	  echo "Usage: ..."
	  exit 1
	fi


	for i in "$@" ; do
	    if [[ $i == "arm" ]] ; then
		echo -e ${LIGHT_GREEN} 'check crosscompiler' ${NC}
		check_crosscompiler
		echo
		echo -e ${LIGHT_GREEN} 'git initialization' ${NC}
		git_init
		echo
		echo -e ${LIGHT_GREEN} 'configuring arm build' ${NC}
		configure arm
		echo
		echo -e ${LIGHT_GREEN} 'building' ${NC}
		./build.sh arm
		break
	    elif [[ $i == "x86" ]] ; then
		echo -e ${LIGHT_BLUE} 'git initialization' ${NC}
		git_init
		echo
		echo -e ${LIGHT_BLUE} 'configuring x86 build' ${NC}
		configure x86
		echo
		echo -e ${LIGHT_BLUE} 'building' ${NC}
		./build.sh x86
		echo
		break
	    fi
	done

echo -e ${BGreen} 'done' ${NC}

	#exit
}

function check_crosscompiler {
	# http://stackoverflow.com/questions/592620/check-if-a-program-exists-from-a-bash-script
	command -v $cross_compiler >/dev/null 2>&1 || { echo -e >&2 ${RED} $cross_compiler "is requied but it's not installed.  Aborting." ${NC}; exit 1; }
	type $cross_compiler >/dev/null 2>&1 || { echo >&2 $cross_compiler "is requied but it's not installed.  Aborting."; exit 1; }
	hash $cross_compiler 2>/dev/null || { echo >&2 $cross_compiler "is requied but it's not installed.  Aborting."; exit 1; }

	echo -e ${GREEN} 'cross-compiler' $cross_compiler 'found' ${NC}
}  

function git_init {
	echo -e ${BLUE} 'submodule init' ${NC}
	git submodule init
	git submodule update

	cd stk
	echo -e ${GREEN} 'updating stk subrepo' ${NC}
	git checkout master
	cd ../

	cd spdlog
	echo -e ${GREEN} 'updating spdlog subrepo' ${NC}
	git checkout master
	cd ../

	cd benchmark
	echo -e ${GREEN} 'updating benchmark subrepo' ${NC}
	git checkout master
	cd ../

}


function configure {


	for i in "$@" ; do
	    if [[ $i == "arm" ]] ; then
		echo -e ${LIGHT_BLUE} "arm build" ${NC}
		echo -e ${GREEN} 'configuring stk library' ${NC}
		cd stk
		autoreconf
		./configure --host=arm-linux-gnueabihf --build=x86_64-linux-gnu --enable-debug --with-alsa --disable-shared --enable-static
		cd ../ 
echo todo cmake arm google bench
exit
		echo -e ${GREEN} 'configuring benchmark library' ${NC}
		cd benchmark
		cmake .
		make -j
		cd ../ 
		break
	   

	    elif [[ $i == "x86" ]] ; then
		echo -e ${BLUE} "x86 build"

		echo -e ${GREEN} 'configuring stk library' ${NC}
		cd stk
		autoreconf
		./configure --enable-debug --with-alsa --disable-shared --enable-static
		cd ../

		echo -e ${GREEN} 'configuring benchmark library' ${NC}
		cd benchmark
		cmake .
		cd ../

		break
	    else
		echo "wrong configure !"
		exit
	    fi
	done
}



main "$@"

