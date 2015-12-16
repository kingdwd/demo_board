#ifndef CAN_H
#define CAN_H

#include <stm32f4xx_hal.h>

#define CANx                            CAN2
#define CANx_CLK_ENABLE()               __HAL_RCC_CAN2_CLK_ENABLE()
#define CANx_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()

#define CANx_FORCE_RESET()              __HAL_RCC_CAN2_FORCE_RESET()
#define CANx_RELEASE_RESET()            __HAL_RCC_CAN2_RELEASE_RESET()

#define CANx_TX_PIN                    GPIO_PIN_6
#define CANx_TX_GPIO_PORT              GPIOB
#define CANx_TX_AF                     GPIO_AF9_CAN2
#define CANx_RX_PIN                    GPIO_PIN_5
#define CANx_RX_GPIO_PORT              GPIOB
#define CANx_RX_AF                     GPIO_AF9_CAN2

#define CANx_RX_IRQn                   CAN2_RX0_IRQn
#define CANx_RX_IRQHandler             CAN2_RX0_IRQHandler

int BSP_can_init(void);
void BSP_can_irq_callback(void);
int can_send_hello(void);
int can_recv_test(void);
void can_print(CAN_HandleTypeDef *hcan);
void BSP_can_deinit(void);
void can_print_err(void);

#endif /* CAN_H */
