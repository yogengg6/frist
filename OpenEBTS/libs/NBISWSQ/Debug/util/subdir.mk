################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../util/computil.c \
../util/fatalerr.c \
../util/syserr.c 

OBJS += \
./util/computil.o \
./util/fatalerr.o \
./util/syserr.o 

C_DEPS += \
./util/computil.d \
./util/fatalerr.d \
./util/syserr.d 


# Each subdirectory must supply rules for building sources it contributes
util/%.o: ../util/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c11 -D__NBISLE__ -I../include -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


