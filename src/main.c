#include <stm32f4xx_hal.h>

#include <ff_gen_drv.h>
#include <sd_diskio.h>

#include "BSP/devboard/devboard.h"
#include "BSP/devboard/uart.h"
#include "BSP/devboard/can.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch(GPIO_Pin) {
		case BUTTON1_PIN: BSP_LED_toggle(LED1); break;
		case BUTTON2_PIN: BSP_LED_toggle(LED2); break;
		case BUTTON3_PIN: BSP_LED_toggle(LED3); break;
		case BUTTON4_PIN: BSP_LED_toggle(LED4); break;
		case BUTTON5_PIN: BSP_LED_toggle(LED5); break;
		case BUTTON6_PIN: BSP_LED_toggle(LED6); break;
		case BUTTON7_PIN: BSP_LED_toggle(LED7); break;
		case BUTTON8_PIN: BSP_LED_toggle(LED8); break;
	}
}

void set_system_clock_168mhz(void) {
	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	const uint32_t HSE_mhz = HSE_VALUE/10e5;
    const bool HSE_is_odd  = (HSE_mhz & 1) == 1;

	RCC_OscInitTypeDef RCC_OscInit = {
		.OscillatorType = RCC_OSCILLATORTYPE_HSE,
		.HSEState       = RCC_HSE_ON,
		.PLL = {
			.PLLState   = RCC_PLL_ON,
			.PLLSource  = RCC_PLLSOURCE_HSE,
			.PLLM       = HSE_is_odd ? HSE_mhz : (HSE_mhz / 2), // should match HSE freq in mhz so HSE/PLLM == 1
			.PLLN       = HSE_is_odd ? (168*2) : 168, // 336 / 2 == 168 == max core clk
			.PLLP       = RCC_PLLP_DIV2, // ((HSE/pllm) * plln) / pllp == 168 (max core clk)
			.PLLQ       = 7, // ((HSE/pllm) * plln) / pllq == 48 (USB needs 48 and sdio needs 48 or lower)
		},
	};
	if (HAL_RCC_OscConfig(&RCC_OscInit) != HAL_OK) {
		while (1);
	}

	RCC_ClkInitTypeDef RCC_ClkInit = {
		.ClockType      = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2),
		.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK,
		.AHBCLKDivider  = RCC_SYSCLK_DIV1,
		.APB1CLKDivider = RCC_HCLK_DIV4,
		.APB2CLKDivider = RCC_HCLK_DIV2,
	};
	// We set FLASH_LATENCY_5 as we are in vcc range 2.7-3.6 at 168mhz
	// see datasheet table 10 at page 80.
	if (HAL_RCC_ClockConfig(&RCC_ClkInit, FLASH_LATENCY_5) != HAL_OK) {
		while (1);
	}

	  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
	if (HAL_GetREVID() == 0x1001) {
		__HAL_FLASH_PREFETCH_BUFFER_ENABLE();
	}
}

static int init_fatfs(FATFS* fs) {
	char SDPath[4];

	// SD_Driver is exported from sd_diskio.h
	if (FATFS_LinkDriver(&SD_Driver, SDPath) != 0) return -1;
	if (f_mount(fs, SDPath, 1) != FR_OK) return -1;
	return 0;
}

int main(void) {
    HAL_Init();
    set_system_clock_168mhz();

    // init all leds
    for (enum led led = 0; led < LEDn; led++) {
        BSP_LED_init(led);
    }

	for (enum button button = 0; button < BUTTONn; button++) {
		BSP_BUTTON_init(button, BUTTON_MODE_EXTI);
	}

	seven_seg_init();

	// Flash all LEDS 3 times to signal ON
	for (int i = 0; i < 3; i++) {
		for (enum led l = 0; l < LEDn; l++) {
			BSP_LED_on(l);
		}
		HAL_Delay(250);
		for (enum led l = 0; l < LEDn; l++) {
			BSP_LED_off(l);
		}
		HAL_Delay(250);
	}

	if (BSP_UART_init() != 0) {
		BSP_LED_on(LED2);
	}

	FIL f;
	FATFS fs;
	bool fs_ready = false;
	if (init_fatfs(&fs) == 0) {
		FRESULT rc = f_open(&f, "STM32.TXT", FA_WRITE|FA_CREATE_ALWAYS);
		printf("rc=%u  rc==FR_OK=%d\n", rc, rc==FR_OK);
		if (rc == FR_OK) fs_ready = true;
	}
	if (BSP_can_init() != 0) {
		BSP_LED_on(LED5);
	}

	char tx[] = "Hello World\n";
    while (1) {
		for (uint8_t i = 0; i < 0xF+1; i++) {
			for (uint8_t j = 0; j < 0xF+1; j++) {
				seven_seg_disp_num(i, j);
				// printf("%x, %x\n", i, j);
				// puts(tx);

				HAL_Delay(100*10);
				int can_rc = 0;

				// can_rc = can_send_hello();
				if (can_rc != 0) {
					printf("CAN tx ERR: %d\n", can_rc);
				}


				can_rc = can_recv_test();
				if (can_rc != 0) {
					printf("CAN ERR %d\n", can_rc);
					can_print_err();
					BSP_LED_toggle(LED4);
				} else {
					printf("CAN recv SUCCESS!\n");
				}


				if(!fs_ready) {
					BSP_LED_toggle(LED1);
				} else {
					char buf[64] = {'\0'};
					sprintf(buf, "i=%x, j=%x\n", i, j);

					unsigned bw;
					FRESULT rc = f_write(&f, buf, 64, &bw);
					if (rc != FR_OK) fs_ready = false;

					printf("rc=%d bw=%u\n", rc, bw);
					f_sync(&f);
				}
			}
		}
	}
}


void HAL_CAN_TxCpltCallback (CAN_HandleTypeDef *hcan) {
	printf("HAL_CAN_TxCpltCallback\n");
	can_print(hcan);
}

void HAL_CAN_RxCpltCallback (CAN_HandleTypeDef *hcan) {
	printf("HAL_CAN_RxCpltCallback\n");
	can_print(hcan);
}

void HAL_CAN_ErrorCallback (CAN_HandleTypeDef *hcan) {
	printf("HAL_CAN_ErrorCallback\n");
	can_print(hcan);
}
