################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../wsq/decoder.c \
../wsq/encoder.c \
../wsq/globals.c \
../wsq/huff.c \
../wsq/ppi.c \
../wsq/tableio.c \
../wsq/tree.c \
../wsq/util.c 

OBJS += \
./wsq/decoder.o \
./wsq/encoder.o \
./wsq/globals.o \
./wsq/huff.o \
./wsq/ppi.o \
./wsq/tableio.o \
./wsq/tree.o \
./wsq/util.o 

C_DEPS += \
./wsq/decoder.d \
./wsq/encoder.d \
./wsq/globals.d \
./wsq/huff.d \
./wsq/ppi.d \
./wsq/tableio.d \
./wsq/tree.d \
./wsq/util.d 


# Each subdirectory must supply rules for building sources it contributes
wsq/%.o: ../wsq/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c11 -D__NBISLE__ -I../include -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


