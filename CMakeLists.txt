project(XMC4500-Cmake)
cmake_minimum_required(VERSION 3.8.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(PROJECT_ELF_FILE ${CMAKE_PROJECT_NAME}.elf)
SET(PROJECT_HEX_FILE ${CMAKE_PROJECT_NAME}.hex)
SET(PROJECT_BIN_FILE ${CMAKE_PROJECT_NAME}.bin)

# XMC Specific
set (XMC_SERIES 4500)
set (XMC_PACKAGE F100)
set (XMC_SIZE 1024)

set(XMC_LIB_DIR ${PROJECT_SOURCE_DIR}/../XMClib/XMC_Peripheral_Library_v2.1.16)
set(USB_LIBDIR ${XMC_LIBDIR}/ThirdPartyLibraries/USBlib/USB)

# XMC4500 Cmake
include(cmake/xmc4500.cmake)

set(PROJECT_INCLUDES
    ${XMC_LIB_DIR}/CMSIS/Include                                    # CMSIS headers
    ${XMC_LIB_DIR}/CMSIS/Infineon/XMC${XMC_SERIES}_series/Include/  # Device Specific headers
    ${XMC_LIB_DIR}/XMCLib/inc                                       # XMClib headers
    ${XMC_LIB_DIR}/USBlib/USB                                       # USB Libs
    ${XMC_LIB_DIR}/USBlib/USB/Class
    ${XMC_LIB_DIR}/USBlib/USB/Class/Common
    ${XMC_LIB_DIR}/USBlib/USB/Class/Device
    ${XMC_LIB_DIR}/USBlib/USB/Common
    ${XMC_LIB_DIR}/USBlib/USB/Core
    ${XMC_LIB_DIR}/USBlib/USB/Core/XMC4000
    ${PROJECT_SOURCE_DIR}/include
)
# May well have to include these, they were an array to the VPATH
# variable from the Makefile used to port this cmake project.
# Lets see.

# ${XMC_LIBDIR}/CMSIS/DSP_Lib/Source/BasicMathFunctions
# ${XMC_LIBDIR}/CMSIS/DSP_Lib/Source/CommonTables
# ${XMC_LIBDIR}/CMSIS/DSP_Lib/Source/ComplexMathFunctions
# ${XMC_LIBDIR}/CMSIS/DSP_Lib/Source/ControllerFunctions
# ${XMC_LIBDIR}/CMSIS/DSP_Lib/Source/FastMathFunctions
# ${XMC_LIBDIR}/CMSIS/DSP_Lib/Source/FilteringFunctions
# ${XMC_LIBDIR}/CMSIS/DSP_Lib/Source/MatrixFunctions
# ${XMC_LIBDIR}/CMSIS/DSP_Lib/Source/StatisticsFunctions
# ${XMC_LIBDIR}/CMSIS/DSP_Lib/Source/SupportFunctions
# ${XMC_LIBDIR}/CMSIS/DSP_Lib/Source/TransformFunctions
# ${XMC_LIBDIR}/CMSIS/Infineon/XMC${XMC_SERIES}_series/Source
# ${XMC_LIBDIR}/CMSIS/RTOS/RTX/SRC
# ${XMC_LIBDIR}/CMSIS/RTOS/RTX/SRC/ARM
# ${XMC_LIBDIR}/CMSIS/RTOS/RTX/Templates
# ${XMC_LIBDIR}/CMSIS/RTOS/RTX/UserCodeTemplates
# ${XMC_LIBDIR}/ThirdPartyLibraries/Newlib
# ${XMC_LIBDIR}/XMCLib/src
# ${USB_LIBDIR}/Class/Common
# ${USB_LIBDIR}/Class/Device
# ${USB_LIBDIR}/Common
# ${USB_LIBDIR}/Core
# ${USB_LIBDIR}/Core/XMC4000

include_directories(${PROJECT_INCLUDES})

file (GLOB USER_SOURCES src/*.c)
set (START_SOURCES 
    ${XMC_LIB_DIR}/CMSIS/Infineon/XMC4500_series/Source/GCC/startup_XMC${XMC_SERIES}.S
    ${XMC_LIB_DIR}/CMSIS/Infineon/XMC4500_series/Source/system_XMC${XMC_SERIES}.c
    )
set (XMC_SOURCES 
    ${XMC_LIB_DIR}/XMCLib/src/xmc_gpio.c
    ${XMC_LIB_DIR}/XMCLib/src/xmc4_gpio.c
    )
set (THIRD_SOURCES 
    ${XMC_LIB_DIR}/ThirdPartyLibraries/Newlib/syscalls.c
    )

set (PROJECT_SOURCES
    ${START_SOURCES}
    ${XMC_SOURCES}
    ${THIRD_SOURCES}
    ${USER_SOURCES}
    )

add_executable(${PROJECT_ELF_FILE} ${PROJECT_SOURCES})

file (GLOB PROJECT_LINKERS "")
target_link_libraries(${PROJECT_ELF_FILE}
    ${PROJECT_LINKERS}
)

# Flash
set(FLASH_EXECUTABLE "JLinkExe")
ADD_CUSTOM_TARGET("flash" DEPENDS
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_ELF_FILE} ${PROJECT_BIN_FILE}
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_ELF_FILE} ${PROJECT_HEX_FILE}
    COMMAND ${FLASH_EXECUTABLE} -Device XMC${XMC_SERIES} -If SWD -Speed 1000 -CommanderScript ${PROJECT_SOURCE_DIR}/jlink/JLinkCommands
)
ADD_DEPENDENCIES(flash ${PROJECT_ELF_FILE})

# Debug
ADD_CUSTOM_TARGET("debug"
    COMMAND JLinkGDBServer -device XMC${XMC_SERIES} -endian little -if SWD -speed 1000 -halt -logtofile -log JLinkLog -silent -vd &
	COMMAND sleep 1
	COMMAND sh -ci "arm-none-eabi-gdb -quiet -tui -command=${PROJECT_SOURCE_DIR}/gdb/GDBCommands"
	COMMAND killall JLinkGDBServer
)
ADD_DEPENDENCIES(debug flash)
