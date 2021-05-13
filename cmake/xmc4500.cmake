# XMC Model Specific
set (XMC_SERIES 4500)
set (XMC_PACKAGE F100)
set (XMC_SIZE 1024)
# Set cross compilation information
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# GCC toolchain prefix
set(TOOLCHAIN_PREFIX "arm-none-eabi")

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)

# Need to set to gcc and not as due to options in ASM_FLAGS
# that apparently are not readable by the assembler directly. 
# Have done so as this is a port of a makefile toolchain to
# cmake, so there are grey areas in my understanding.
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)

set(CMAKE_AR ${TOOLCHAIN_PREFIX}-ar)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN_PREFIX}-objdump)

enable_language(ASM)
set(XMC -DXMC${XMC_SERIES}_${XMC_PACKAGE}x${XMC_SIZE})
set(CPU "-mcpu=cortex-m4")
set(FPU "-mfloat-abi=softfp -mfpu=fpv4-sp-d16")
set(DBG "-g3 -gdwarf-2 -Wall")
set(OPT "-O0 -ffunction-sections")

set(CMAKE_ASM_FLAGS 
    "${CMAKE_ASM_FLAGS} ${XMC} ${CPU} ${FPU} -mthumb ${DBG}"
    )

set(CMAKE_C_FLAGS 
    "${CMAKE_C_FLAGS} ${XMC} ${CPU} ${FPU} -mthumb ${DBG} -std=gnu99 ${OPT}"
    )

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-specs=nosys.specs")
set(CMAKE_EXE_LINKER_FLAGS 
    "-T${PROJECT_SOURCE_DIR}/link/XMC${XMC_SERIES}x${XMC_SIZE}.ld -L${XMC_LIB_DIR}/CMSIS/Lib/GCC -nostartfiles ${CPU} -mthumb -g3 -gdwarf-2"
    )
