# Set cross compilation information
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# GCC toolchain prefix
set(TOOLCHAIN_PREFIX "arm-none-eabi")

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-as)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}-ar)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN_PREFIX}-objdump)

enable_language(ASM)
set(XMC -DXMC${XMC_SERIES}_${XMC_PACKAGE}x${XMC_SIZE})
set(CPU "-mcpu=cortex-m4")
set(FPU "-mfpu=fpv4-sp-d16 -mfloat-abi=softfp")

set(CMAKE_ASM_FLAGS 
    "${CMAKE_ASM_FLAGS} ${XMC} -mthumb ${CPU} ${FPU} -g3 -gdwarf-2 -c Wa,-adhlns='$@.lst' -Wall -x assembler-with-cpp"
    )
set(CMAKE_C_FLAGS 
    "${CMAKE_C_FLAGS} ${XMC} -mthumb ${CPU} ${FPU} -std=gnu99 -Os -g3 -gdwarf-2 -c -Wa -adhlns='$@.lst' -Wall -ffunction-sections"
    )

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-specs=nano.specs -specs=nosys.specs")
set(CMAKE_EXE_LINKER_FLAGS 
    "-T${PROJECT_SOURCE_DIR}/build/main.ld -nostartfiles -L${XMC_LIB_DIR}/CMSIS/Lib/GCC/ -Wl -Map '$@.map' ${CPU} -mthumb -g3 -gdwarf-2"
    )

# Processor specific definitions
add_definitions(-Dgcc)

