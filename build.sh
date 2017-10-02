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
		make clean -C stk
		make -j4 lib -C stk

		make -j4 clean -C bassline-junkie/arm_debug
		make -j4 -C bassline-junkie/arm_debug

		break


	    elif [[ $i == "x86" ]] ; then
		echo "Is set and compiling!"
		make clean -C stk
		make -j4 lib -C stk

		make clean -C benchmark
		make -j4 -C benchmark

		make -j4 clean -C bassline-junkie/x86_debug
		make -j4 -C bassline-junkie/x86_debug

		break
	    fi
	done

echo -e ${BGreen} 'build done' ${NC}

}


main "$@"


#make -j4 clean -C bassline-junkie/x86_debug
#make -j4 clean -C bassline-junkie/x86_release
#make -j4 clean -C bassline-junkie/x86_tests

#make -j4 -C bassline-junkie/x86_debug
#make -j4 -C bassline-junkie/x86_release
#make -j4 -C bassline-junkie/x86_tests




