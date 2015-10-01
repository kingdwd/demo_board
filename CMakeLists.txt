cmake_minimum_required(VERSION 2.8.8)

set(CMAKE_SYSTEM_NAME Generic)

project(DemoBoard C ASM)

set(gc_flags "-ffunction-sections -fdata-sections -Wl,--gc-sections") # These flags helps with dead code elimination. More info can found at http://stackoverflow.com/a/10809541
set(mcu_flags "-mcpu=cortex-m4 -mtune=cortex-m4 -mthumb -mlittle-endian -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb-interwork")
set(linker_flags "-Wl,-T${CMAKE_SOURCE_DIR}/STM32F407VE_FLASH.ld -Wl,-gc-sections -Wl,-LTO")

set(WARNINGS "-Wall -Wpedantic -Wextra")
set(CFLAGS "-O0 -g -LTO -std=c99 ${WARNINGS} ${gc_flags} ${mcu_flags} ${linker_flags}")

add_definitions(-DHSE_VALUE=24000000 -DSTM32F407xx)

set(CMAKE_C_FLAGS "${CFLAGS} ${CMAKE_C_FLAGS}")

add_subdirectory(src)
