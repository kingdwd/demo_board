set(CMSIS_DIR ${third_party_dir}/CMSIS)

set(CMSIS_INCLUDE_DIRS
    ${CMSIS_DIR}/Include
    ${CMSIS_DIR}/Device/ST/STM32F4xx/Include
)

add_library(CMSIS
    ${CMSIS_DIR}/Device/ST/STM32F4xx/Source/startup_stm32f407xx.s
    ${CMSIS_DIR}/Device/ST/STM32F4xx/Source/system_stm32f4xx.c
)
target_include_directories(CMSIS PRIVATE ${CMSIS_INCLUDE_DIRS})
