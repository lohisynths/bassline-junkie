################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cpucounter.cpp \
../src/main.cpp \
../src/sin.cpp 

OBJS += \
./src/cpucounter.o \
./src/main.o \
./src/sin.o 

CPP_DEPS += \
./src/cpucounter.d \
./src/main.d \
./src/sin.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -I../../benchmark/include -I../../benchmark/third_party/json/src -I../../stk/include -O3 -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard -g3 -fno-omit-frame-pointer -ggdb -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


