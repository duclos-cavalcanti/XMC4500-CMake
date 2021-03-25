project(XMC4500-Cmake)
cmake_minimum_required(VERSION 3.8.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# XMC4500 Cmake
include(cmake/xmc4500.cmake)

# XMC Specific
set (XMC_SERIES 4500)
set (XMC_PACKAGE F100)
set (XMC_SIZE 1024)

set(XMC_LIB_DIR ${PROJECT_SOURCE_DIR}/../XMClib/XMC_Peripheral_Library_v2.1.16)
set(XMC_USB_LIB_DIR ${XMC_LIB_DIR}/ThirdPartyLibraries/USBlib/USB)

set(PROJECT_INCLUDES
    ${PROJECT_SOURCE_DIR}/include
    ${XMC_LIB_DIR}/CMSIS/Include                                    # CMSIS headers
    ${XMC_LIB_DIR}/CMSIS/Infineon/XMC${XMC_SERIES}_series/Include/  # Device Specific headers
    ${XMC_LIB_DIR}/CMSIS/XMCLib/inc                                 # XMClib headers
    ${XMC_USB_LIB_DIR}                                              # USB Libs
    ${XMC_USB_LIB_DIR}/Class
    ${XMC_USB_LIB_DIR}/Class/Common
    ${XMC_USB_LIB_DIR}/Class/Device
    ${XMC_USB_LIB_DIR}/Common
    ${XMC_USB_LIB_DIR}/Core
    ${XMC_USB_LIB_DIR}/Core/XMC4500
)
include_directories(${PROJECT_INCLUDES})

file (GLOB USER_SOURCES src/*.c)
file (GLOB XMC_SOURCES ${XMC_LIB_DIR}/CMSIS/Infineon/XMC4500/src/*.c)
set (PROJECT_SOURCES
    ${USER_SOURCES}
    ${XMC_SOURCES}/CMSIS
    )

add_executable(${CMAKE_PROJECT_NAME}.axf ${PROJECT_SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME}.axf 
    ${TIVAWARE_PATH}/usblib/gcc/libusb.a
    ${TIVAWARE_PATH}/driverlib/gcc/libdriver.a
)

# Flash
set(FLASH_EXECUTABLE "lm4flash")
ADD_CUSTOM_TARGET("flash" DEPENDS ${CMAKE_PROJECT_NAME}.axf 
  COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.axf ${CMAKE_PROJECT_NAME}.bin 
  COMMAND ${FLASH_EXECUTABLE} ${CMAKE_PROJECT_NAME}.bin
)

# Debug
set(OPENOCD_BOARD "/usr/share/openocd/scripts/board/ek-tm4c123gxl.cfg")
ADD_CUSTOM_TARGET(
    "debug"
    COMMAND openocd -f ${OPENOCD_BOARD} >/dev/null 2>&1 & sleep 2
    COMMAND arm-none-eabi-gdb -quiet -tui -command=${PROJECT_SOURCE_DIR}/gdb/GDBCommands -se ${CMAKE_PROJECT_NAME}.axf 
    COMMAND killall -15 openocd
)
