################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Java/OpenEBTSJNI.c \
../Java/OpenEBTSJNIHelpers.c 

OBJS += \
./Java/OpenEBTSJNI.o \
./Java/OpenEBTSJNIHelpers.o 

C_DEPS += \
./Java/OpenEBTSJNI.d \
./Java/OpenEBTSJNIHelpers.d 


# Each subdirectory must supply rules for building sources it contributes
Java/OpenEBTSJNI.o: ../Java/OpenEBTSJNI.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c11 -DUNICODE -D_DEBUG -I../. -I../.. -I../Java -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"Java/OpenEBTSJNI.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Java/%.o: ../Java/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DUNICODE -D_DEBUG -I../. -I../.. -I../Java -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


