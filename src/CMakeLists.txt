set(SRC_FILES
	main.c
	interrupts.c
	syscalls.c

	BSP/devboard/devboard.c
	BSP/devboard/sd.c
	BSP/devboard/uart.c
	BSP/devboard/can.c
	BSP/devboard/dac.c
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME}
	CMSIS
	STM32f4xx_HAL_Driver
	FATFS
)
open_ocd_write_flash(${PROJECT_NAME})
