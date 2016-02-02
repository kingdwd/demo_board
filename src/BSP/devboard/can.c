#include <stm32f4xx_hal.h>

#include "can.h"

CAN_HandleTypeDef can_handle;

#define TIMOUT_TIME 100

#define IS_ODD(x)   ((x & 1) == 1)

static float get_sample_point(CAN_HandleTypeDef* hcan) {
	const int TSYNC_SEG = 1;
	const int TSEG1 = hcan->Init.BS1; // Prop_Seg + Phase_Seg1
	const int TSEG2 = hcan->Init.BS2; // Phase_Seg2

	const int TBit = TSYNC_SEG + TSEG1 + TSEG2;

	const float sample_point = (float)( TBit - TSEG2 ) / (float)TBit;
	return sample_point;
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan) {
	(void)hcan;
	__HAL_RCC_CAN1_CLK_ENABLE(); // Must always enable CAN1 clk even if we only use CAN2

	{
		__HAL_RCC_CAN1_CLK_ENABLE();
		__HAL_RCC_CAN2_CLK_ENABLE();
	}

	CANx_CLK_ENABLE();
	CANx_GPIO_CLK_ENABLE();

	HAL_GPIO_Init(CANx_TX_GPIO_PORT, &(GPIO_InitTypeDef){
		.Pin       = CANx_TX_PIN,
		.Mode      = GPIO_MODE_AF_PP,
		.Speed     = GPIO_SPEED_FAST,
		.Pull      = GPIO_NOPULL, //GPIO_PULLUP,
		.Alternate = CANx_TX_AF,
	});

	HAL_GPIO_Init(CANx_RX_GPIO_PORT, &(GPIO_InitTypeDef){
		.Pin       = CANx_RX_PIN,
		.Mode      = GPIO_MODE_AF_PP,
		.Speed     = GPIO_SPEED_FAST,
		.Pull      = GPIO_NOPULL, //GPIO_PULLUP,
		.Alternate = CANx_RX_AF,
	});

	HAL_NVIC_SetPriority(CANx_RX_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(CANx_RX_IRQn);

	// TODO: are these needed?
	HAL_NVIC_SetPriority(CAN2_RX1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn);
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan) {
	(void)hcan;
	CANx_FORCE_RESET();
	CANx_RELEASE_RESET();

	HAL_GPIO_DeInit(CANx_TX_GPIO_PORT, CANx_TX_PIN);
	HAL_GPIO_DeInit(CANx_RX_GPIO_PORT, CANx_RX_PIN);
}

int BSP_can_init(void) {
	BSP_can_deinit();
	HAL_CAN_MspInit(&can_handle);
	static CanTxMsgTypeDef TxMessage;
	static CanRxMsgTypeDef RxMessage;

	can_handle.Instance = CANx;
	can_handle.pTxMsg   = &TxMessage;
	can_handle.pRxMsg   = &RxMessage;

	can_handle.Init.TTCM      = DISABLE;
	can_handle.Init.ABOM      = DISABLE;
	can_handle.Init.AWUM      = DISABLE;
	can_handle.Init.NART      = DISABLE;
	can_handle.Init.RFLM      = DISABLE;
	can_handle.Init.TXFP      = DISABLE;
	can_handle.Init.Mode      = CAN_MODE_NORMAL; //CAN_MODE_NORMAL; //CAN_MODE_LOOPBACK;

	// 125k bitrate Sample point at 81.3%
	// can_handle.Init.SJW       = CAN_SJW_1TQ;
	// can_handle.Init.BS1       = CAN_BS1_12TQ; //CAN_BS1_11TQ; //CAN_BS1_12TQ; //CAN_BS1_11TQ;
	// can_handle.Init.BS2       = CAN_BS2_3TQ; //CAN_BS2_4TQ; //CAN_BS2_3TQ; //CAN_BS2_4TQ;
	// can_handle.Init.Prescaler = 24; //2; //60; //6;

	// // 125k bitrate Sample point at 75%
	can_handle.Init.SJW       = CAN_SJW_3TQ;
	can_handle.Init.BS1       = CAN_BS1_8TQ; //CAN_BS1_11TQ; //CAN_BS1_12TQ; //CAN_BS1_11TQ;
	can_handle.Init.BS2       = CAN_BS2_4TQ; //CAN_BS2_4TQ; //CAN_BS2_3TQ; //CAN_BS2_4TQ;
	can_handle.Init.Prescaler = 30; //2; //60; //6;

	// 125k bitrate Sample point at 87.5%
	// can_handle.Init.SJW       = CAN_SJW_1TQ;
	// can_handle.Init.BS1       = CAN_BS1_13TQ; //CAN_BS1_11TQ; //CAN_BS1_12TQ; //CAN_BS1_11TQ;
	// can_handle.Init.BS2       = CAN_BS2_2TQ; //CAN_BS2_4TQ; //CAN_BS2_3TQ; //CAN_BS2_4TQ;
	// can_handle.Init.Prescaler = 12; //2; //60; //6;

	if (HAL_CAN_Init(&can_handle) != HAL_OK) return -1;

	if (HAL_CAN_ConfigFilter(&can_handle, &(CAN_FilterConfTypeDef){
		.FilterNumber         = can_handle.Instance == CAN1 ? 0 : 14, //14, // CAN1 0..13, CAN 14..27
		.FilterMode           = CAN_FILTERMODE_IDMASK,
		.FilterScale          = CAN_FILTERSCALE_32BIT,
		.FilterIdHigh         = 0x0000,
		.FilterIdLow          = 0x0000,
		.FilterMaskIdHigh     = 0x0000,
		.FilterMaskIdLow      = 0x0000,
		.FilterFIFOAssignment = 0,
		.FilterActivation     = ENABLE,
		.BankNumber           = 14, // 0 gives all to CAN2, 14 = 50/50, 28 give all to CAN1
	}) != HAL_OK) return -1;

	return 0;
}

void BSP_can_deinit(void) {
	HAL_CAN_DeInit(&can_handle);
}

int can_send_hello(void) {
	can_handle.pTxMsg->StdId   = 0x11;
	can_handle.pTxMsg->ExtId   = 0x11;
	can_handle.pTxMsg->RTR     = CAN_RTR_DATA;
	can_handle.pTxMsg->IDE     = CAN_ID_STD;
	can_handle.pTxMsg->DLC     = 6;
	can_handle.pTxMsg->Data[0] = 'H';
	can_handle.pTxMsg->Data[1] = 'e';
	can_handle.pTxMsg->Data[2] = 'l';
	can_handle.pTxMsg->Data[3] = 'l';
	can_handle.pTxMsg->Data[4] = 'o';
	can_handle.pTxMsg->Data[5] = '\n';

	if (HAL_CAN_Transmit(&can_handle, TIMOUT_TIME) != HAL_OK) return -1;
	return (HAL_CAN_GetState(&can_handle) == HAL_CAN_STATE_READY) ? 0 : -1;
}

int can_recv_test(void) {
	if (HAL_CAN_Receive(&can_handle, CAN_FIFO0, TIMOUT_TIME) != HAL_OK) {
		if (HAL_CAN_Receive(&can_handle, CAN_FIFO1, TIMOUT_TIME) != HAL_OK) return -1;
	}
	if (HAL_CAN_GetState(&can_handle) != HAL_CAN_STATE_READY) return -2;

	// Print the recved message
	can_print(&can_handle);

	return 0;
}

void can_print(CAN_HandleTypeDef *hcan) {
	printf("CAN_recv: ");
	printf("|id %#x|",       hcan->pRxMsg->StdId);
	printf("|is_std_id %d|", hcan->pRxMsg->IDE == CAN_ID_STD);
	printf("|dlc %d|",       hcan->pRxMsg->DLC);
	for (size_t i = 0; i < hcan->pRxMsg->DLC; i++) {
		printf("%#x ", hcan->pRxMsg->Data[i]);
	}
	printf("\n");
}

void can_print_err(void) {
	uint32_t i = 0;
	uint32_t state = HAL_CAN_GetError(&can_handle);

	const char fmt[] = "CAN err:";
	while (state != 0) {
		switch(state & i) {
		case HAL_CAN_ERROR_EWG: printf("%s EWG\n", fmt); break;
		case HAL_CAN_ERROR_EPV: printf("%s EPV\n", fmt); break;
		case HAL_CAN_ERROR_BOF: printf("%s BOF\n", fmt); break;
		case HAL_CAN_ERROR_STF: printf("%s STUFF\n", fmt); break;
		case HAL_CAN_ERROR_FOR: printf("%s FORM\n", fmt); break;
		case HAL_CAN_ERROR_ACK: printf("%s Acknowledgment\n", fmt); break;
		case HAL_CAN_ERROR_BR: printf("%s Bit recessive\n", fmt); break;
		case HAL_CAN_ERROR_BD: printf("%s LEC dominant\n", fmt); break;
		case HAL_CAN_ERROR_CRC: printf("%s CRC LEC transfer error\n", fmt); break;

		case HAL_CAN_ERROR_NONE: default: break;         //printf("%s None\n", fmt); break;
		}
		state >>= 1;
	}
}

void BSP_can_irq_callback(void) {
	HAL_CAN_IRQHandler(&can_handle);
}
