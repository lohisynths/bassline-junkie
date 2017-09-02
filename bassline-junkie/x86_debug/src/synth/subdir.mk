################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/synth/bassline-junkie.cpp \
../src/synth/voice.cpp 

OBJS += \
./src/synth/bassline-junkie.o \
./src/synth/voice.o 

CPP_DEPS += \
./src/synth/bassline-junkie.d \
./src/synth/voice.d 


# Each subdirectory must supply rules for building sources it contributes
src/synth/%.o: ../src/synth/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -D__LINUX_ALSA__ -I../../stk/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


