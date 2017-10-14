################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/synth/dsp/Lfo.cpp \
../src/synth/dsp/MoogFilter.cpp \
../src/synth/dsp/Osc.cpp \
../src/synth/dsp/VAStateVariableFilter.cpp 

OBJS += \
./src/synth/dsp/Lfo.o \
./src/synth/dsp/MoogFilter.o \
./src/synth/dsp/Osc.o \
./src/synth/dsp/VAStateVariableFilter.o 

CPP_DEPS += \
./src/synth/dsp/Lfo.d \
./src/synth/dsp/MoogFilter.d \
./src/synth/dsp/Osc.d \
./src/synth/dsp/VAStateVariableFilter.d 


# Each subdirectory must supply rules for building sources it contributes
src/synth/dsp/%.o: ../src/synth/dsp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++1y -D__LINUX_ALSA__ -I../../stk/include -O3 -mfpu=neon-vfpv4 -mfloat-abi=hard -funsafe-math-optimizations -ftree-vectorize -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


