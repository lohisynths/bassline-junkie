mkdir -p ~/arm
cd ~/arm
download and extract
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

add to path in ~/.bashrc

cd ~/git
git clone https://github.com/ARMmbed/mbed-os.git
cd mbed-os

git checkout mbed-os-5.9.0

pip install -r requirements.txt 

python tools/build.py --mcu NUCLEO_F411RE --tool GCC_ARM

export MBED_PATH=/home/alax/git/mbed-os/BUILD/mbed
