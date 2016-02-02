#ifndef DAC_H
#define DAC_H

#include <stm32f4xx_hal.h>

#define DAC_CHANNEL1_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

#define DAC_FORCE_RESET()              __HAL_RCC_DAC_FORCE_RESET()
#define DAC_RELEASE_RESET()            __HAL_RCC_DAC_RELEASE_RESET()

/* Definition for DACx Channel1 Pin */
#define DAC_CHANNEL1_PIN                GPIO_PIN_4
#define DAC_CHANNEL1_GPIO_PORT          GPIOA

/* Definition for DACx's Channel1 */
#define DAC_CHANNEL1                    DAC_CHANNEL_1

/* Definition for DACx's DMA Channel1 */
#define DAC_DMA_CLK_ENABLE()            __HAL_RCC_DMA1_CLK_ENABLE()
#define DAC_DMA_CHANNEL1                DMA_CHANNEL_7
#define DAC_DMA_STREAM1                 DMA1_Stream5

/* Definition for DACx's NVIC */
#define DAC_DMA_IRQn1                   DMA1_Stream5_IRQn
#define DAC_DMA_IRQHandler1             DMA1_Stream5_IRQHandler

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac);
void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac);
void init_dac(void);
void dac_set(uint8_t value);

#endif /* DAC_H */
