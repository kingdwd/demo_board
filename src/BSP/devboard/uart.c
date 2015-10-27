#include <stm32f4xx_hal.h>
#include <stdio.h>

#include "uart.h"

UART_HandleTypeDef UartHandle;

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    (void)huart;

    USARTx_TX_GPIO_CLK_ENABLE();
    USARTx_RX_GPIO_CLK_ENABLE();
    USARTx_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init = {
        .Pin       = USARTx_TX_PIN,
        .Mode      = GPIO_MODE_AF_PP,
        .Pull      = GPIO_PULLUP,
        .Speed     = GPIO_SPEED_FAST,
        .Alternate = USARTx_TX_AF,
    };
    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &gpio_init);
    gpio_init.Pin       = USARTx_RX_PIN;
    gpio_init.Alternate = USARTx_RX_AF;
    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &gpio_init);

    // Insert rx / tx DMA here

    HAL_NVIC_SetPriority(USARTx_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USARTx_IRQn);
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart) {
    (void)huart;

    USARTx_FORCE_RESET();
    USARTx_RELEASE_RESET();

    HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
    HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);
}

int BSP_UART_init(void) {
    UartHandle.Instance = USARTx;

    UartHandle.Init.BaudRate     = 115200;
    UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits     = UART_STOPBITS_1;
    UartHandle.Init.Parity       = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode         = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

    return HAL_UART_Init(&UartHandle) == HAL_OK ? 0 : -1;
}

int BSP_UARTx_transmit(uint8_t* buf, size_t n) {
    return HAL_UART_Transmit(&UartHandle, buf, n, 0xFFFF) == HAL_OK ? 0 : -1;
}

// Retargets the C library printf function to the USART.
// int fputc(int ch, FILE *f) {
//     if (ch == '\n') fputc('\r', f);
//     return BSP_UARTx_transmit((uint8_t *)&ch, 1) == 0 ? ch : EOF;
// }

// int _write(int file, char *ptr, int len) {
//     (void)file;
//     return BSP_UARTx_transmit((uint8_t*)ptr, len) == 0 ? len : -1;
// }
//
// int isatty(int file) {
//     return 1;
// }

void BSP_UARTx_IRQHandler(void) {
    HAL_UART_IRQHandler(&UartHandle);
}
