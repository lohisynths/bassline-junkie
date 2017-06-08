################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/synth/dsp/PolyBLEPOscillator/Oscillator.cpp \
../src/synth/dsp/PolyBLEPOscillator/PolyBLEPOscillator.cpp 

OBJS += \
./src/synth/dsp/PolyBLEPOscillator/Oscillator.o \
./src/synth/dsp/PolyBLEPOscillator/PolyBLEPOscillator.o 

CPP_DEPS += \
./src/synth/dsp/PolyBLEPOscillator/Oscillator.d \
./src/synth/dsp/PolyBLEPOscillator/PolyBLEPOscillator.d 


# Each subdirectory must supply rules for building sources it contributes
src/synth/dsp/PolyBLEPOscillator/%.o: ../src/synth/dsp/PolyBLEPOscillator/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++1y -I../../stk/include -O1 -mfpu=neon-vfpv4 -mfloat-abi=hard -funsafe-math-optimizations -ftree-vectorize -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


