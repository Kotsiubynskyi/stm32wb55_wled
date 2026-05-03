#include "stm32wbxx_hal.h"
#include "stm32wbxx_hal_dma.h"
#include "stm32wbxx_hal_rcc.h"
#include "stm32wbxx_hal_tim.h"

#include "ws2812b.h"
#include "ws2812b_animations.h"

void GPIO_PA0_Init();
void SystemClock_Config();
void startWledTimer(uint32_t *ledData);
void Error_Handler(void);

int main() {
  HAL_Init();
  SystemClock_Config();

  GPIO_PA0_Init();

  uint32_t *dma_array = ws2812b_init(HAL_Delay);
  startWledTimer(dma_array);
  ws2812b_allOff();
  ws2812b_allOn(0x00ff00);
  HAL_Delay(1000);
  while (1) {
    // HAL_Delay(3000);
    // HAL_Delay(3000);
    // ws2812b_allOff();
    // delay(300);
    // comet(5, 0xff010f, 5);
    // glowing(0x00000f, 8);
    rainbow();
  }

  return 0;
}

// Half Transfer Complete callback
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) {
    ws2812b_dma_half_callback();
  }
}

// Full Transfer Complete callback
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) {
    ws2812b_dma_complete_callback();
  }
}

// Init WS2812B data pin in Alternate Function mode with Timer 3
void GPIO_PA0_Init() {
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// Start timer on 4MHz in PWM+DMA mode with period 1us
TIM_HandleTypeDef htim2;
void startWledTimer(uint32_t *ledData) {
  __HAL_RCC_TIM2_CLK_ENABLE();

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 16 - 1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4 - 1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    Error_Handler();

  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    Error_Handler();

  HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, ledData, 2 * COLOR_BITS);
}

// Configure and Init DMA; also setup DMA interrupts: half and full transfer
// complete events
DMA_HandleTypeDef hdma_tim2_ch1;
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {

  if (htim->Instance != TIM2)
    return;

  __HAL_RCC_TIM2_CLK_ENABLE();
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  hdma_tim2_ch1.Instance = DMA1_Channel1;
  hdma_tim2_ch1.Init.Request = DMA_REQUEST_TIM2_CH1;
  hdma_tim2_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_tim2_ch1.Init.MemDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tim2_ch1.Init.Mode = DMA_CIRCULAR;
  hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_HIGH;
  if (HAL_DMA_Init(&hdma_tim2_ch1) != HAL_OK)
    Error_Handler();
  __HAL_LINKDMA(htim, hdma[TIM_DMA_ID_CC1], hdma_tim2_ch1);

  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    Error_Handler();

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    Error_Handler();
}

void Error_Handler(void) {
  __disable_irq();
  while (1)
    ;
}
