################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/synth/dsp/ADSR/ADSR.cpp 

OBJS += \
./src/synth/dsp/ADSR/ADSR.o 

CPP_DEPS += \
./src/synth/dsp/ADSR/ADSR.d 


# Each subdirectory must supply rules for building sources it contributes
src/synth/dsp/ADSR/%.o: ../src/synth/dsp/ADSR/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -D__LINUX_ALSA__ -I../../stk/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


