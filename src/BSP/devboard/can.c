#include <stm32f4xx_hal.h>

#include "can.h"

CAN_HandleTypeDef can_handle;

void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan) {
    (void)hcan;
    CANx_CLK_ENABLE();
    CANx_GPIO_CLK_ENABLE();

    HAL_GPIO_Init(CANx_TX_GPIO_PORT, &(GPIO_InitTypeDef){
        .Pin       = CANx_TX_PIN,
        .Mode      = GPIO_MODE_AF_PP,
        .Speed     = GPIO_SPEED_FAST,
        .Pull      = GPIO_PULLUP,
        .Alternate = CANx_TX_AF,
    });

    HAL_GPIO_Init(CANx_RX_GPIO_PORT, &(GPIO_InitTypeDef){
        .Pin       = CANx_RX_PIN,
        .Mode      = GPIO_MODE_AF_PP,
        .Speed     = GPIO_SPEED_FAST,
        .Pull      = GPIO_PULLUP,
        .Alternate = CANx_RX_AF,
    });
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan) {
    (void)hcan;
    CANx_FORCE_RESET();
    CANx_RELEASE_RESET();

    HAL_GPIO_DeInit(CANx_TX_GPIO_PORT, CANx_TX_PIN);
    HAL_GPIO_DeInit(CANx_RX_GPIO_PORT, CANx_RX_PIN);
}


int BSP_can_init(void) {
    static CanTxMsgTypeDef        TxMessage;
    static CanRxMsgTypeDef        RxMessage;

    can_handle.Instance = CANx;
    can_handle.pTxMsg   = &TxMessage;
    can_handle.pRxMsg   = &RxMessage;

    can_handle.Init.TTCM      = DISABLE;
    can_handle.Init.ABOM      = DISABLE;
    can_handle.Init.AWUM      = DISABLE;
    can_handle.Init.NART      = DISABLE;
    can_handle.Init.RFLM      = DISABLE;
    can_handle.Init.TXFP      = DISABLE;
    can_handle.Init.Mode      = CAN_MODE_LOOPBACK;
    can_handle.Init.SJW       = CAN_SJW_1TQ;
    can_handle.Init.BS1       = CAN_BS1_11TQ;
    can_handle.Init.BS2       = CAN_BS2_4TQ;
    can_handle.Init.Prescaler = 3;

    if (HAL_CAN_Init(&can_handle) != HAL_OK) return -1;

    if (HAL_CAN_ConfigFilter(&can_handle, &(CAN_FilterConfTypeDef){
        .FilterNumber         = 0,
        .FilterMode           = CAN_FILTERMODE_IDMASK,
        .FilterScale          = CAN_FILTERSCALE_32BIT,
        .FilterIdHigh         = 0x0000,
        .FilterIdLow          = 0x0000,
        .FilterMaskIdHigh     = 0x0000,
        .FilterMaskIdLow      = 0x0000,
        .FilterFIFOAssignment = 0,
        .FilterActivation     = ENABLE,
        .BankNumber           = 14,
    }) != HAL_OK) return -1;

    return 0;
}

int can_send_hello(void) {
    can_handle.pTxMsg->StdId   = 0x11;
    can_handle.pTxMsg->RTR     = CAN_RTR_DATA;
    can_handle.pTxMsg->IDE     = CAN_ID_STD;
    can_handle.pTxMsg->DLC     = 6;
    can_handle.pTxMsg->Data[0] = 'H';
    can_handle.pTxMsg->Data[1] = 'e';
    can_handle.pTxMsg->Data[2] = 'l';
    can_handle.pTxMsg->Data[3] = 'l';
    can_handle.pTxMsg->Data[4] = 'o';
    can_handle.pTxMsg->Data[5] = '\n';

    if (HAL_CAN_Transmit(&can_handle, 10) != HAL_OK) return -1;
    return (HAL_CAN_GetState(&can_handle) == HAL_CAN_STATE_READY) ? 0 : -1;
}

void BSP_can_irq_callback(void) {
    HAL_CAN_IRQHandler(&can_handle);
}
