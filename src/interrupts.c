#include <stm32f4xx_hal.h>
#include "BSP/devboard/devboard.h"
#include "BSP/devboard/sd.h"
#include "BSP/devboard/uart.h"
#include "BSP/devboard/can.h"

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

void NMI_Handler(void) {}

void HardFault_Handler(void) {
	while (1);
}

void MemManage_Handler(void) {
	while (1);
}

void BusFault_Handler(void) {
	while (1);
}

void UsageFault_Handler(void) {
	while (1);
}

void SVC_Handler(void) {}

void DebugMon_Handler(void) {}

void PendSV_Handler(void) {}

void SysTick_Handler(void) {
	HAL_IncTick();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) , for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f407xx.s).                                               */
/******************************************************************************/

void EXTI9_5_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(BUTTON_PIN[BUTTON1]);
	HAL_GPIO_EXTI_IRQHandler(BUTTON_PIN[BUTTON2]);
}

void EXTI15_10_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(BUTTON_PIN[BUTTON3]);
	HAL_GPIO_EXTI_IRQHandler(BUTTON_PIN[BUTTON4]);
	HAL_GPIO_EXTI_IRQHandler(BUTTON_PIN[BUTTON5]);
	HAL_GPIO_EXTI_IRQHandler(BUTTON_PIN[BUTTON6]);
	HAL_GPIO_EXTI_IRQHandler(BUTTON_PIN[BUTTON7]);
	HAL_GPIO_EXTI_IRQHandler(BUTTON_PIN[BUTTON8]);
}

void DMA2_Stream3_IRQHandler(void) {
	BSP_SD_DMA_Rx_IRQHandler();
}


void DMA2_Stream6_IRQHandler(void) {
	BSP_SD_DMA_Tx_IRQHandler();
}


void SDIO_IRQHandler(void) {
	BSP_SD_IRQHandler();
}

void USARTx_IRQHandler(void) {
	BSP_UARTx_IRQHandler();
}

void CAN1_RX0_IRQHandler(void) {
	BSP_can_irq_callback();
}

void CAN2_RX0_IRQHandler(void) {
    BSP_can_irq_callback();
}

void CAN1_RX1_IRQHandler(void) {
	BSP_can_irq_callback();
}

void CAN2_RX1_IRQHandler(void) {
	BSP_can_irq_callback();
}

void CAN1_TX_IRQHandler(void) {
	BSP_can_irq_callback();
}

void CAN2_TX_IRQHandler(void) {
	BSP_can_irq_callback();
}
