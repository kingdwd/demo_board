#include <stm32f4xx_hal.h>

#include "dac.h"

static DAC_HandleTypeDef hdac = {.Instance = DAC};

#define ARR_LEN(x) (sizeof(x) / sizeof(x[0]))

void init_dac(void) {
	HAL_DAC_MspInit(&hdac);
	HAL_DAC_Init(&hdac);

	static DAC_ChannelConfTypeDef sconf = {
		.DAC_Trigger      = DAC_TRIGGER_NONE,//SOFTWARE,
		.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE,//ENABLE,
	};
	HAL_DAC_ConfigChannel(&hdac, &sconf, DAC_CHANNEL1);

	// const uint8_t aEscalator8bit[] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};
	// HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL1, (uint32_t*)aEscalator8bit, ARR_LEN(aEscalator8bit), DAC_ALIGN_8B_R);
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL1, DAC_ALIGN_8B_R, 0x00);
	HAL_DAC_Start(&hdac, DAC_CHANNEL1);
}

void dac_set(uint8_t value) {
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL1, DAC_ALIGN_8B_R, value);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac) {
	// Enable Clocks
	__HAL_RCC_DAC_CLK_ENABLE();
	DAC_CHANNEL1_GPIO_CLK_ENABLE();
	DAC_DMA_CLK_ENABLE();

	// Configure GPIO pins
	HAL_GPIO_Init(DAC_CHANNEL1_GPIO_PORT, &(GPIO_InitTypeDef) {
		.Pin  = DAC_CHANNEL1_PIN,
		.Mode = GPIO_MODE_ANALOG,
		.Pull = GPIO_NOPULL,
	});

	// Configure DMA
	static DMA_HandleTypeDef hdma_dac1 = {
		.Instance = DAC_DMA_STREAM1,
		.Init = {
			.Channel             = DAC_DMA_CHANNEL1,
			.Direction           = DMA_MEMORY_TO_PERIPH,
			.PeriphInc           = DMA_PINC_DISABLE,
			.MemInc              = DMA_MINC_ENABLE,
			.PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
			.MemDataAlignment    = DMA_MDATAALIGN_BYTE,
			.Mode                = DMA_CIRCULAR,
			.Priority            = DMA_PRIORITY_HIGH,
			.FIFOMode            = DMA_FIFOMODE_DISABLE,
			.FIFOThreshold       = DMA_FIFO_THRESHOLD_HALFFULL,
			.MemBurst            = DMA_MBURST_SINGLE,
			.PeriphBurst         = DMA_PBURST_SINGLE,
		},
	};
	HAL_DMA_Init(&hdma_dac1);
	__HAL_LINKDMA(hdac, DMA_Handle1, hdma_dac1);

	HAL_NVIC_SetPriority(DAC_DMA_IRQn1, 2, 0);
	HAL_NVIC_EnableIRQ(DAC_DMA_IRQn1);
}

void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac) {
	DAC_FORCE_RESET();
	DAC_RELEASE_RESET();

	HAL_GPIO_DeInit(DAC_CHANNEL1_GPIO_PORT, DAC_CHANNEL1_PIN);

	static DMA_HandleTypeDef hdma_dac1 = {.Instance = DAC_DMA_STREAM1};
	HAL_DMA_DeInit(&hdma_dac1);

	HAL_NVIC_DisableIRQ(DAC_DMA_IRQn1);
}
