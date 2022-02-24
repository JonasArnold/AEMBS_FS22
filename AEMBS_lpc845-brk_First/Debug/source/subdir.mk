################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/main.c \
../source/mtb.c \
../source/semihost_hardfault.c 

OBJS += \
./source/main.o \
./source/mtb.o \
./source/semihost_hardfault.o 

C_DEPS += \
./source/main.d \
./source/mtb.d \
./source/semihost_hardfault.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_LPC845M301JBD48 -DCPU_LPC845M301JBD48_cm0plus -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\jonas\Documents\MCUXpressoIDE_11.5.0_7232\workspace\AEMBS_lpc845-brk_First\board" -I"C:\Users\jonas\Documents\MCUXpressoIDE_11.5.0_7232\workspace\AEMBS_lpc845-brk_First\source" -I"C:\Users\jonas\Documents\MCUXpressoIDE_11.5.0_7232\workspace\AEMBS_lpc845-brk_First\drivers" -I"C:\Users\jonas\Documents\MCUXpressoIDE_11.5.0_7232\workspace\AEMBS_lpc845-brk_First\component\uart" -I"C:\Users\jonas\Documents\MCUXpressoIDE_11.5.0_7232\workspace\AEMBS_lpc845-brk_First\utilities" -I"C:\Users\jonas\Documents\MCUXpressoIDE_11.5.0_7232\workspace\AEMBS_lpc845-brk_First\CMSIS" -I"C:\Users\jonas\Documents\MCUXpressoIDE_11.5.0_7232\workspace\AEMBS_lpc845-brk_First\device" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


