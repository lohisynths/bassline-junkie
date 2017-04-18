################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/synth/engines/serial.cpp \
../src/synth/engines/serial_sleep.cpp \
../src/synth/engines/sharedfutures.cpp \
../src/synth/engines/sharedfutures_sleep.cpp 

OBJS += \
./src/synth/engines/serial.o \
./src/synth/engines/serial_sleep.o \
./src/synth/engines/sharedfutures.o \
./src/synth/engines/sharedfutures_sleep.o 

CPP_DEPS += \
./src/synth/engines/serial.d \
./src/synth/engines/serial_sleep.d \
./src/synth/engines/sharedfutures.d \
./src/synth/engines/sharedfutures_sleep.d 


# Each subdirectory must supply rules for building sources it contributes
src/synth/engines/%.o: ../src/synth/engines/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++1y -I../../stk/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


