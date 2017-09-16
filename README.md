# bassline-junkie

sudo apt install libasound2-dev autoconf


init 
	- export toolchain path (toolchain from raspi tool respo)
	in ~/.bashrc add
		export PATH=/home/alax/rpi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin:$PATH
		* replace /home/alax/* with your path (sybolic links and '~/file*' won't work)
	- initialize subrepos and look check toolchain
		./init.sh
	- crosscompile application and static version of stklib
		./build
	- binary is located in bassline-junkie/arm_debug folder; copy to 'home' raspi and run
	- create plots folder in bassline junkie
	- copy '*.bin' from 'home' raspi to bassline/plots
	- run python script
		python plot.py
	- results will be located in plots/* newly created folders 

	- in eclipse > window > preferences > C/C++ > Build > Environment add
		/home/alax/rpi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin
		* replace /home/alax/* with your path ( sybolic links and '~/file*' won't work)
	  and make sure that 'Append variables to native environment' is selected below
	
libstk

	- due to libstk build system limitations it isn't possible to build both release and debug versions of library.
	- eclipse project files added with separate build configuration for debug/release
	- 'lib' recipe added to makefile, sample applications ignored
