cmake_minimum_required(VERSION 2.8.8)

set(CMAKE_SYSTEM_NAME Generic)

project(DemoBoard C ASM)

set(gc_flags "-ffunction-sections -fdata-sections -Wl,--gc-sections") # These flags helps with dead code elimination. More info can found at http://stackoverflow.com/a/10809541
set(mcu_flags "-mcpu=cortex-m4 -mtune=cortex-m4 -mthumb -mlittle-endian -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb-interwork")
set(linker_flags "-Wl,-T${CMAKE_SOURCE_DIR}/STM32F407VE_FLASH.ld -Wl,-gc-sections -Wl,-LTO -nostdlib")

set(WARNINGS "-Wall -Wpedantic -Wextra")
set(CFLAGS "-O0 -g -LTO -std=c99 ${WARNINGS} ${gc_flags} ${mcu_flags} ${linker_flags}")

add_definitions(-DHSE_VALUE=24000000 -DSTM32F407xx)

set(CMAKE_C_FLAGS "${CFLAGS} ${CMAKE_C_FLAGS}")
link_libraries(c gcc) # because we compile with '-nostdlib' we have to link these manually

set(FATFS_DIR ${CMAKE_SOURCE_DIR}/third_party/FATFS)
set(CMSIS_DIR ${CMAKE_SOURCE_DIR}/third_party/CMSIS)
set(STM32f4xx_HAL_Driver_DIR ${CMAKE_SOURCE_DIR}/third_party/STM32f4xx_HAL_Driver)

include_directories(
    # The project source
    ${CMAKE_SOURCE_DIR}/src

    # CMSIS
    ${CMSIS_DIR}/Include
    ${CMSIS_DIR}/Device/ST/STM32F4xx/Include

    # STM32 HAL
    ${STM32f4xx_HAL_Driver_DIR}/Inc

    # FatFs
    ${FATFS_DIR}/src
    ${FATFS_DIR}/src/drivers
)

# Make it possible to Flash using openOCD
find_program(OPEN_OCD openocd)
get_filename_component(open_ocd_path ${OPEN_OCD} DIRECTORY)
set(OPEN_OCD_CONFIG "${open_ocd_path}/../share/openocd/scripts/board/stm32f4discovery.cfg")
function(open_ocd_write_flash elf_file)
	add_custom_target(${elf_file}_writeflash
		COMMAND ${OPEN_OCD}
					-f ${OPEN_OCD_CONFIG}
					-c "init"
					-c "reset halt"
					-c "flash write_image erase ${elf_file}"
					-c "reset run"
		DEPENDS ${elf_file}
		VERBATIM
	)
endfunction()

add_subdirectory(third_party/CMSIS)
add_subdirectory(third_party/STM32F4xx_HAL_Driver)
add_subdirectory(third_party/FatFs)

add_subdirectory(src)
