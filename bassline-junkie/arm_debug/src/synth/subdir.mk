################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/synth/AudioDevice.cpp \
../src/synth/MoogFilter.cpp \
../src/synth/bassline-junkie.cpp \
../src/synth/cpucounter.cpp \
../src/synth/synth.cpp \
../src/synth/wavwriter.cpp 

OBJS += \
./src/synth/AudioDevice.o \
./src/synth/MoogFilter.o \
./src/synth/bassline-junkie.o \
./src/synth/cpucounter.o \
./src/synth/synth.o \
./src/synth/wavwriter.o 

CPP_DEPS += \
./src/synth/AudioDevice.d \
./src/synth/MoogFilter.d \
./src/synth/bassline-junkie.d \
./src/synth/cpucounter.d \
./src/synth/synth.d \
./src/synth/wavwriter.d 


# Each subdirectory must supply rules for building sources it contributes
src/synth/%.o: ../src/synth/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++1y -I../../stk/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


