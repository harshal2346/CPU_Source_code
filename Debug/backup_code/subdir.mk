################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../backup_code/app.c 

OBJS += \
./backup_code/app.o 

C_DEPS += \
./backup_code/app.d 


# Each subdirectory must supply rules for building sources it contributes
backup_code/%.o: ../backup_code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 Linux gcc compiler'
	arm-linux-gnueabihf-gcc -lrt -lpthread -Wall -O0 -g3 -I"/home/scope/Desktop/hdmi/InAS_custom_Uart/vivado_proj/Zybo-Z7-20-HDMI.sdk/Linux_InAS/Peripherals/Custom_uart" -I"/home/scope/Desktop/hdmi/InAS_custom_Uart/vivado_proj/Zybo-Z7-20-HDMI.sdk/Linux_InAS/Peripherals/Timer" -I"/home/scope/Desktop/hdmi/InAS_custom_Uart/vivado_proj/Zybo-Z7-20-HDMI.sdk/Linux_InAS/Peripherals/Gpio" -I"/home/scope/Desktop/hdmi/InAS_custom_Uart/vivado_proj/Zybo-Z7-20-HDMI.sdk/Linux_InAS/Application" -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


