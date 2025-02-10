################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Peripherals/Timer/timer.c 

OBJS += \
./Peripherals/Timer/timer.o 

C_DEPS += \
./Peripherals/Timer/timer.d 


# Each subdirectory must supply rules for building sources it contributes
Peripherals/Timer/%.o: ../Peripherals/Timer/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 Linux gcc compiler'
	arm-linux-gnueabihf-gcc -lrt -lpthread -Wall -O0 -g3 -I"/home/scope/Desktop/hdmi/InAS_custom_Uart/vivado_proj/Zybo-Z7-20-HDMI.sdk/Linux_InAS/Peripherals/Custom_uart" -I"/home/scope/Desktop/hdmi/InAS_custom_Uart/vivado_proj/Zybo-Z7-20-HDMI.sdk/Linux_InAS/Peripherals/Timer" -I"/home/scope/Desktop/hdmi/InAS_custom_Uart/vivado_proj/Zybo-Z7-20-HDMI.sdk/Linux_InAS/Peripherals/Gpio" -I"/home/scope/Desktop/hdmi/InAS_custom_Uart/vivado_proj/Zybo-Z7-20-HDMI.sdk/Linux_InAS/Application" -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


