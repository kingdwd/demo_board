add_subdirectory(third_party)

set(SRC_FILES
	main.c
	interrupts.c

	BSP/devboard/devboard.c
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME}
	CMSIS
	STM32f4xx_HAL_Driver
)
