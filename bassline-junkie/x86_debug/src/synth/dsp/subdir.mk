################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/synth/dsp/MoogFilter.cpp 

OBJS += \
./src/synth/dsp/MoogFilter.o 

CPP_DEPS += \
./src/synth/dsp/MoogFilter.d 


# Each subdirectory must supply rules for building sources it contributes
src/synth/dsp/%.o: ../src/synth/dsp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -I../../stk/include -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


