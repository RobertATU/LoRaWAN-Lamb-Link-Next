################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/hts221/hts221.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/hts221/hts221_reg.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lis2dtw12/lis2dtw12.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lis2dtw12/lis2dtw12_reg.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lps22hh/lps22hh.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lps22hh/lps22hh_reg.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lsm6dso32x/lsm6dso32x.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lsm6dso32x/lsm6dso32x_reg.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/ST25DV/st25dv.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/ST25DV/st25dv_reg.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/stts22h/stts22h.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/stts22h/stts22h_reg.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/teseo_liv3f/teseo_liv3f.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/teseo_liv3f/teseo_liv3f_i2c.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/teseo_liv3f/teseo_liv3f_queue.c \
C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/teseo_liv3f/teseo_liv3f_uart.c 

OBJS += \
./Drivers/BSP/Components/hts221.o \
./Drivers/BSP/Components/hts221_reg.o \
./Drivers/BSP/Components/lis2dtw12.o \
./Drivers/BSP/Components/lis2dtw12_reg.o \
./Drivers/BSP/Components/lps22hh.o \
./Drivers/BSP/Components/lps22hh_reg.o \
./Drivers/BSP/Components/lsm6dso32x.o \
./Drivers/BSP/Components/lsm6dso32x_reg.o \
./Drivers/BSP/Components/st25dv.o \
./Drivers/BSP/Components/st25dv_reg.o \
./Drivers/BSP/Components/stts22h.o \
./Drivers/BSP/Components/stts22h_reg.o \
./Drivers/BSP/Components/teseo_liv3f.o \
./Drivers/BSP/Components/teseo_liv3f_i2c.o \
./Drivers/BSP/Components/teseo_liv3f_queue.o \
./Drivers/BSP/Components/teseo_liv3f_uart.o 

C_DEPS += \
./Drivers/BSP/Components/hts221.d \
./Drivers/BSP/Components/hts221_reg.d \
./Drivers/BSP/Components/lis2dtw12.d \
./Drivers/BSP/Components/lis2dtw12_reg.d \
./Drivers/BSP/Components/lps22hh.d \
./Drivers/BSP/Components/lps22hh_reg.d \
./Drivers/BSP/Components/lsm6dso32x.d \
./Drivers/BSP/Components/lsm6dso32x_reg.d \
./Drivers/BSP/Components/st25dv.d \
./Drivers/BSP/Components/st25dv_reg.d \
./Drivers/BSP/Components/stts22h.d \
./Drivers/BSP/Components/stts22h_reg.d \
./Drivers/BSP/Components/teseo_liv3f.d \
./Drivers/BSP/Components/teseo_liv3f_i2c.d \
./Drivers/BSP/Components/teseo_liv3f_queue.d \
./Drivers/BSP/Components/teseo_liv3f_uart.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/hts221.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/hts221/hts221.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/hts221_reg.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/hts221/hts221_reg.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/lis2dtw12.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lis2dtw12/lis2dtw12.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/lis2dtw12_reg.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lis2dtw12/lis2dtw12_reg.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/lps22hh.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lps22hh/lps22hh.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/lps22hh_reg.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lps22hh/lps22hh_reg.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/lsm6dso32x.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lsm6dso32x/lsm6dso32x.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/lsm6dso32x_reg.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/lsm6dso32x/lsm6dso32x_reg.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/st25dv.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/ST25DV/st25dv.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/st25dv_reg.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/ST25DV/st25dv_reg.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/stts22h.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/stts22h/stts22h.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/stts22h_reg.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/stts22h/stts22h_reg.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/teseo_liv3f.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/teseo_liv3f/teseo_liv3f.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/teseo_liv3f_i2c.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/teseo_liv3f/teseo_liv3f_i2c.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/teseo_liv3f_queue.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/teseo_liv3f/teseo_liv3f_queue.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/teseo_liv3f_uart.o: C:/Users/Robert/STM32CubeIDE/FYP/CustomUC/Drivers/BSP/Components/teseo_liv3f/teseo_liv3f_uart.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTSAFE_A100=STSAFE_A100 -DUSE_HAL_DRIVER -DSTM32WB5Mxx -c -I../../Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/lib_gnss/LibGNSS/Inc -I../../Middlewares/ST/lib_gnss/LibNMEA/Inc -I../../Drivers/BSP/Components/teseo_liv3f -I../../Drivers/BSP/Components/lsm6dso32x -I../../Drivers/BSP/Components/hts221 -I../../Drivers/BSP/Components/lps22hh -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/Components/lis2dtw12 -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components

clean-Drivers-2f-BSP-2f-Components:
	-$(RM) ./Drivers/BSP/Components/hts221.cyclo ./Drivers/BSP/Components/hts221.d ./Drivers/BSP/Components/hts221.o ./Drivers/BSP/Components/hts221.su ./Drivers/BSP/Components/hts221_reg.cyclo ./Drivers/BSP/Components/hts221_reg.d ./Drivers/BSP/Components/hts221_reg.o ./Drivers/BSP/Components/hts221_reg.su ./Drivers/BSP/Components/lis2dtw12.cyclo ./Drivers/BSP/Components/lis2dtw12.d ./Drivers/BSP/Components/lis2dtw12.o ./Drivers/BSP/Components/lis2dtw12.su ./Drivers/BSP/Components/lis2dtw12_reg.cyclo ./Drivers/BSP/Components/lis2dtw12_reg.d ./Drivers/BSP/Components/lis2dtw12_reg.o ./Drivers/BSP/Components/lis2dtw12_reg.su ./Drivers/BSP/Components/lps22hh.cyclo ./Drivers/BSP/Components/lps22hh.d ./Drivers/BSP/Components/lps22hh.o ./Drivers/BSP/Components/lps22hh.su ./Drivers/BSP/Components/lps22hh_reg.cyclo ./Drivers/BSP/Components/lps22hh_reg.d ./Drivers/BSP/Components/lps22hh_reg.o ./Drivers/BSP/Components/lps22hh_reg.su ./Drivers/BSP/Components/lsm6dso32x.cyclo ./Drivers/BSP/Components/lsm6dso32x.d ./Drivers/BSP/Components/lsm6dso32x.o ./Drivers/BSP/Components/lsm6dso32x.su ./Drivers/BSP/Components/lsm6dso32x_reg.cyclo ./Drivers/BSP/Components/lsm6dso32x_reg.d ./Drivers/BSP/Components/lsm6dso32x_reg.o ./Drivers/BSP/Components/lsm6dso32x_reg.su ./Drivers/BSP/Components/st25dv.cyclo ./Drivers/BSP/Components/st25dv.d ./Drivers/BSP/Components/st25dv.o ./Drivers/BSP/Components/st25dv.su ./Drivers/BSP/Components/st25dv_reg.cyclo ./Drivers/BSP/Components/st25dv_reg.d ./Drivers/BSP/Components/st25dv_reg.o ./Drivers/BSP/Components/st25dv_reg.su ./Drivers/BSP/Components/stts22h.cyclo ./Drivers/BSP/Components/stts22h.d ./Drivers/BSP/Components/stts22h.o ./Drivers/BSP/Components/stts22h.su ./Drivers/BSP/Components/stts22h_reg.cyclo ./Drivers/BSP/Components/stts22h_reg.d ./Drivers/BSP/Components/stts22h_reg.o ./Drivers/BSP/Components/stts22h_reg.su ./Drivers/BSP/Components/teseo_liv3f.cyclo ./Drivers/BSP/Components/teseo_liv3f.d ./Drivers/BSP/Components/teseo_liv3f.o ./Drivers/BSP/Components/teseo_liv3f.su ./Drivers/BSP/Components/teseo_liv3f_i2c.cyclo ./Drivers/BSP/Components/teseo_liv3f_i2c.d ./Drivers/BSP/Components/teseo_liv3f_i2c.o ./Drivers/BSP/Components/teseo_liv3f_i2c.su ./Drivers/BSP/Components/teseo_liv3f_queue.cyclo ./Drivers/BSP/Components/teseo_liv3f_queue.d ./Drivers/BSP/Components/teseo_liv3f_queue.o ./Drivers/BSP/Components/teseo_liv3f_queue.su ./Drivers/BSP/Components/teseo_liv3f_uart.cyclo ./Drivers/BSP/Components/teseo_liv3f_uart.d ./Drivers/BSP/Components/teseo_liv3f_uart.o ./Drivers/BSP/Components/teseo_liv3f_uart.su

.PHONY: clean-Drivers-2f-BSP-2f-Components
