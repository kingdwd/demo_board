set(STM32f4xx_HAL_Driver_DIR ${third_party_dir}/STM32f4xx_HAL_Driver)

set(STM32f4xx_HAL_Driver_INCLUDE_DIRS
    ${STM32f4xx_HAL_Driver_DIR}/Inc
)

file(GLOB src ${STM32f4xx_HAL_Driver_DIR}/Src/*.c)
add_library(STM32f4xx_HAL_Driver ${src})
target_include_directories(STM32f4xx_HAL_Driver PRIVATE ${STM32f4xx_HAL_Driver_INCLUDE_DIRS})
target_compile_options(STM32f4xx_HAL_Driver PRIVATE -Wno-extra) # There is a lot of unused variable warnings in some hal function-prototypes
