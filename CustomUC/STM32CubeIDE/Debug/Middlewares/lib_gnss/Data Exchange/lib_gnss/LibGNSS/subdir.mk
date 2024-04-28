################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Middlewares/ST/lib_gnss/LibGNSS/Src/gnss_data.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Middlewares/ST/lib_gnss/LibGNSS/Src/gnss_parser.c 

OBJS += \
./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_data.o \
./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_parser.o 

C_DEPS += \
./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_data.d \
./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_parser.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_data.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Middlewares/ST/lib_gnss/LibGNSS/Src/gnss_data.c Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Middlewares/lib_gnss/Data Exchange/lib_gnss/LibGNSS/gnss_data.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_parser.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Middlewares/ST/lib_gnss/LibGNSS/Src/gnss_parser.c Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Middlewares/lib_gnss/Data Exchange/lib_gnss/LibGNSS/gnss_parser.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lib_gnss-2f-Data-20-Exchange-2f-lib_gnss-2f-LibGNSS

clean-Middlewares-2f-lib_gnss-2f-Data-20-Exchange-2f-lib_gnss-2f-LibGNSS:
	-$(RM) ./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_data.cyclo ./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_data.d ./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_data.o ./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_data.su ./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_parser.cyclo ./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_parser.d ./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_parser.o ./Middlewares/lib_gnss/Data\ Exchange/lib_gnss/LibGNSS/gnss_parser.su

.PHONY: clean-Middlewares-2f-lib_gnss-2f-Data-20-Exchange-2f-lib_gnss-2f-LibGNSS

