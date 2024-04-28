/**
  @page CustomUC application example for STEVAL-ASTRA1B board

  @verbatim
  ******************** (C) COPYRIGHT 2022 STMicroelectronics *******************
  * @file    readme.txt
  * @author  System Research & Applications Team - Catania Lab
  * @version V2.0.0
  * @brief   Function pack Application FW. CustomUC - Default_configuration
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim
     
@par Example Description

This application shows how to use a STEVAL-ASTRA1B board to implement a CustomUC application firmware. 

The FP-ATR-ASTRA1 is an STM32Cube function pack that implements a complete asset tracking application.

Before to start: 
Please, be sure to have the correct bootloader version. Please read the "important" section below.
 
The application supports long range connectivity and short-range connectivity. It reads the data from
environmental and motion sensors, retrieves the geo-position from GNSS and sends them to the cloud.
The FP-ATR-ASTRA1 package supports low-power profiles and the related transitions to ensure long battery autonomy.
Moreover, it provides key features such as secure element management, data storing, the possibility 
to add custom algorithms, debugging interfaces, and expansion capability.


Multiple asset tracking use cases are implemented; the customer can choose the one that best fits 
his own application.

Use case implemented: CustomUC
Configuration: Default_configuration

This firmware package includes Components Device Drivers, Board Support Package and example application 
for the STMicroelectronics STEVAL-ASTRA1B evaluation board which supports following devices:
Processing, security, long-range and short-range connectivity:
STM32WB5MMG, STM32WL55JC, STSAFE-A110, Teseo-LIV3F, ST25DV64K
Environmental and motion Sensors:
STTS22H, LPS22HH, HTS221, LIS2DTW12, LSM6DSO32X
Power management and protections:
ST1PS02, STBC03, TCPP01-M12, ESDALC6V1-1U2, ESDA7P60-1U1M, ECMF02-2AMX6
Diodes and transistors:
STPS120MF, STPS1L30MF, BAT30F4, STL4P3LLH6, STL6N3LLH6, STG3692

                              ----------------------
                              |     IMPORTANT!!    |
                              ----------------------

The implementation allow the Firmware-Upgrade-Over-The-Air (FUOTA).

*** WITH FUOTA *** 
The board must be flashed with the latest release of the bootloader. StevalAstra1B_BLE_Ota_1.15_LSE.hex can be found 
in Projects\STEVAL-ASTRA1B\Applications\AssetTracking\Binaries directory.
This example must be flashed starting at address 0x08007000.
Please ensure that your toolchain is correctly configured. The linker files and instructions can be found
in Projects\STEVAL-ASTRA1B\Applications\AssetTracking\Binaries directory (see readme.txt and "How to use it" paragraph 
below). 
The application works ONLY if the BootLoader is saved at the beginning of the FLASH (address 0x08000000).
The FUOTA feature can be activated by setting the USE_BLE_OTA define to 1 in Astra_conf.h file
The Firmware-Upgrade-Over-The-Air is done using the ST BLE Sensor Android/iOS application (Version 4.13.0 and above)

*** WITHOUT FUOTA *** 
This example must run starting at address 0x08000000 in memory (at the beginning of the FLASH).
The FUOTA feature is not available

Please refer to quickstart guide or UM3019 for further details.

@par Keywords

asset tracking, ASTRA, LoRaWAN, BLE, STM32WB5MMG, STM32WL55JC, STSAFE-A110, Teseo-LIV3F, ST25DV64K, STTS22H, LPS22HH, HTS221, LIS2DTW12, LSM6DSO32X, 
ST1PS02, STBC03, TCPP01-M12, ESDALC6V1-1U2, ESDA7P60-1U1M, ECMF02-2AMX6, STPS120MF, STPS1L30MF, BAT30F4, STL4P3LLH6, STL6N3LLH6, STG3692

@par Hardware and Software environment

  - This example runs on STEVAL-ASTRA1B and can be easily tailored to any other supported device and development board.
    
  - This example must be used with the related ST Asset tracking Android/iOS application available on Play/itune store,
    in order to read the sent information by Bluetooth Low Energy protocol
	
  - Inside the Binary Directory there are the following binaries:
    - StevalAstra1B_BLE_Ota_1.15_LSE.hex (BootLoader. Use ST-Link or USB DFU to program this file. Do not load this program using FUOTA)
    - CustomUC_0x08007000.bin	(Application program without BootLoader. Can be programmed via FUOTA)
    - CustomUC.hex 		(Application program without BootLoader. Can be programmed via ST-Link or USB DFU)
    
STM32Cube packages:
  - STM32WBxx drivers from STM32CubeWB V1.15.0
X-CUBE packages:
  - X-CUBE-GNSS1 V6.0.0
  - X-CUBE-MEMS1 V9.4.0
  - X-CUBE-NFC4 V3.0.0

@par How to use it ? 

This package contains projects for 3 IDEs: IAR, Keil µVision 5 and Integrated Development Environment for STM32. 
In order to make the  program work, you must do the following:
 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.

For IAR:
 - Open IAR toolchain (this firmware has been successfully tested with Embedded Workbench V9.20.1).
 - Open the IAR project file EWARM\Project.eww
 - TO ENABLE FUOTA
   - set linker file to stm32wb5mxx_flash_cm4_ota.icf 
 - Rebuild all files, load the application into the target memory and run 

For Keil µVision 5:
 - Open Keil µVision 5 toolchain (this firmware has been successfully tested with MDK-ARM Professional Version: 5.37.0).
 - TO ENABLE FUOTA
   - 'Project->Options->Target' and set IROM1 as 0x8007000 for Start and 0x79000 for Size.
   - 'Project->Options->Target' and set IRAM1 as 0x2000004 for Start and 0x2FC00 for Size.
   - 'Project->Options->Linker' and use "stm32wb5mxx_flash_cm4_ota.sct" as scatter file
   - 'Project->Options->Linker' and add two lines in misc controls:
       --keep *.o(TAG_OTA_START)
       --keep *.o(TAG_OTA_END)
 - Open the µVision project file MDK-ARM\CustomUC.uvprojx
 - Rebuild all files, load the application into the target memory and run 
 
For Integrated Development Environment for STM32:
 - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.10.1).
 - Set the default workspace proposed by the IDE (please be sure that there are not spaces in the workspace path).
 - Press "File" -> "Import" -> "Existing Projects into Workspace"; press "Browse" in the "Select root directory" and choose the path where the System
   Workbench project is located (inside STM32CubeIDE folder).
 - TO ENABLE FUOTA
   - Open 'Properties->C/C++ Build->Settings->Tool Settings->MCU GCC Linker->General'
     and use "STM32WB5MMGHX_FLASH_ota.ld" as linker script.
 - Rebuild all files, load the application into the target memory and run 

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */