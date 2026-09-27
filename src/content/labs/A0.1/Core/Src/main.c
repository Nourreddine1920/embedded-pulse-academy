/* USER CODE BEGIN Header */
/**
 * @file    main.c
 * @brief   A0.1 Integer lab: HAL version (NUCLEO-F446RE).
 *
 * Clock:  HSI 16 MHz, no PLL -> SYSCLK = HCLK = PCLK1 = PCLK2 = 16 MHz.
 * Output: USART2 (PA2 TX / PA3 RX) -> ST-LINK Virtual COM Port, 115200 8N1.
 * LED:    LD2 on PA5. Slow blink = all tests pass, fast blink = a test failed.
 */
/* USER CODE END Header */
#include "main.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include "int_lab.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PD */
#define LAB_UART_TIMEOUT_MS    (100U)  /**< Per-string TX timeout.          */
#define LED_PERIOD_PASS_MS     (500U)  /**< Half-period: 1 Hz blink.        */
#define LED_PERIOD_FAIL_MS     (100U)  /**< Half-period: 5 Hz blink.        */
/* USER CODE END PD */

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN 0 */
/** @brief Blocking string output on USART2 (used by the lab). */
static void Lab_UartWrite(const char *text)
{
    size_t length = strlen(text);

    /* HAL takes a uint16_t length: clamp instead of silently truncating. */
    if (length > UINT16_MAX)
    {
        length = UINT16_MAX;
    }
    (void)HAL_UART_Transmit(&huart2, (const uint8_t *)text, (uint16_t)length,
                            LAB_UART_TIMEOUT_MS);
}

/** @brief Enables the DWT cycle counter (Cortex-M3/M4/M7). */
static void Lab_CycleCounterInit(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; /* power the DWT block  */
    DWT->CYCCNT = 0U;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;           /* start counting       */
}

/** @brief Returns the current CPU cycle count. */
static uint32_t Lab_CycleCounterRead(void)
{
    return DWT->CYCCNT;
}
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    /* USER CODE BEGIN 2 */
    Lab_CycleCounterInit();
    const bool     allPassed = IntLab_Run(Lab_UartWrite, Lab_CycleCounterRead);
    const uint32_t halfPeriodMs = allPassed ? LED_PERIOD_PASS_MS : LED_PERIOD_FAIL_MS;
    /* USER CODE END 2 */

    while (1)
    {
        /* USER CODE BEGIN 3 */
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_Delay(halfPeriodMs);
        /* USER CODE END 3 */
    }
}

/**
 * @brief System clock: HSI 16 MHz straight to SYSCLK, all buses /1.
 *        At 16 MHz and VOS scale 3 the Flash needs 0 wait states.
 */
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

/** @brief USART2: 115200 baud, 8 data bits, no parity, 1 stop bit. */
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

/** @brief PA5 (LD2) as push-pull output, initially low. */
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

/** @brief Called on unrecoverable init errors: stop with interrupts off. */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
