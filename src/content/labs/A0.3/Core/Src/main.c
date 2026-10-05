/* USER CODE BEGIN Header */
/**
 * @file    main.c
 * @brief   A0.3 volatile & const lab: HAL version (NUCLEO-F446RE).
 *
 * TIM6 (CubeMX: PSC 0, ARR 799, update interrupt on) runs at 20 kHz and its
 * HAL callback calls VcLab_TickIsr(). LD2 blinks at 1 Hz if every fixed
 * variant passed, 5 Hz if not.
 *
 * Clock:  HSI 16 MHz (as in A0.1).  Console: USART2 115200 8N1 (ST-LINK VCP).
 */
/* USER CODE END Header */
#include "main.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include "vc_lab.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PD */
#define LAB_UART_TIMEOUT_MS   (100U)
#define BLINK_PASS_MS         (500U)
#define BLINK_FAIL_MS         (100U)
#define FLASH_START           (0x08000000UL)
#define FLASH_END             (0x08080000UL)   /**< 512 KB */
#define SRAM_START            (0x20000000UL)
#define SRAM_END              (0x20020000UL)   /**< 128 KB */
/* USER CODE END PD */

TIM_HandleTypeDef  htim6;
UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN 0 */
static void Lab_UartWrite(const char *text)
{
    size_t length = strlen(text);

    if (length > UINT16_MAX)
    {
        length = UINT16_MAX;
    }
    (void)HAL_UART_Transmit(&huart2, (const uint8_t *)text, (uint16_t)length, LAB_UART_TIMEOUT_MS);
}

static void Lab_FastTick(bool enable)
{
    if (enable)
    {
        __HAL_TIM_SET_COUNTER(&htim6, 0U);
        (void)HAL_TIM_Base_Start_IT(&htim6);
    }
    else
    {
        (void)HAL_TIM_Base_Stop_IT(&htim6);
        __DSB();
        __ISB();
    }
}

static const char *Lab_RegionOf(const void *address)
{
    const uintptr_t a = (uintptr_t)address;

    if ((a >= FLASH_START) && (a < FLASH_END))
    {
        return "Flash";
    }
    if ((a >= SRAM_START) && (a < SRAM_END))
    {
        return "SRAM";
    }
    return "other";
}

/** @brief Called by HAL_TIM_IRQHandler() from TIM6_DAC_IRQHandler (stm32f4xx_it.c). */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        VcLab_TickIsr();
    }
}
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM6_Init();
    MX_USART2_UART_Init();

    /* USER CODE BEGIN 2 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;   /* DWT cycle counter */
    DWT->CYCCNT = 0U;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    const VcLab_Platform platform = {
        .write        = Lab_UartWrite,
        .cycleCounter = &DWT->CYCCNT,
        .cyclesPerMs  = HAL_RCC_GetHCLKFreq() / 1000U,
        .fastTick     = Lab_FastTick,
        .regionOf     = Lab_RegionOf,
        .cpuid        = &SCB->CPUID,
    };

    const uint32_t blinkMs = VcLab_Run(&platform) ? BLINK_PASS_MS : BLINK_FAIL_MS;
    /* USER CODE END 2 */

    while (1)
    {
        /* USER CODE BEGIN 3 */
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_Delay(blinkMs);
        /* USER CODE END 3 */
    }
}

/** @brief HSI 16 MHz straight to SYSCLK, all buses /1, 0 wait states. */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                       RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/** @brief TIM6: 16 MHz / (0 + 1) / (799 + 1) = 20 kHz update interrupt. */
static void MX_TIM6_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim6.Instance               = TIM6;
    htim6.Init.Prescaler         = 0;
    htim6.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim6.Init.Period            = 799;
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_USART2_UART_Init(void)
{
    huart2.Instance          = USART2;
    huart2.Init.BaudRate     = 115200;
    huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    huart2.Init.StopBits     = UART_STOPBITS_1;
    huart2.Init.Parity       = UART_PARITY_NONE;
    huart2.Init.Mode         = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

/** @brief PA5 = LD2 output. */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = LD2_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
