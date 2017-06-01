################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/synth/utils/AudioDevice.cpp \
../src/synth/utils/MidiReceiver.cpp \
../src/synth/utils/array_writer.cpp \
../src/synth/utils/cpucounter.cpp \
../src/synth/utils/wavwriter.cpp 

OBJS += \
./src/synth/utils/AudioDevice.o \
./src/synth/utils/MidiReceiver.o \
./src/synth/utils/array_writer.o \
./src/synth/utils/cpucounter.o \
./src/synth/utils/wavwriter.o 

CPP_DEPS += \
./src/synth/utils/AudioDevice.d \
./src/synth/utils/MidiReceiver.d \
./src/synth/utils/array_writer.d \
./src/synth/utils/cpucounter.d \
./src/synth/utils/wavwriter.d 


# Each subdirectory must supply rules for building sources it contributes
src/synth/utils/%.o: ../src/synth/utils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++1y -I../../stk/include -O0 -mfpu=neon-vfpv4 -mfloat-abi=hard -funsafe-math-optimizations -ftree-vectorize -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


