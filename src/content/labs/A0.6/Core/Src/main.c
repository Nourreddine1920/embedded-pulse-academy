/* USER CODE BEGIN Header */
/**
 * @file    main.c
 * @brief   A0.6 Hygiene bench: HAL version (NUCLEO-F446RE).
 *
 * 1. Runs the hygiene bench (macro bugs vs static inline fixes, static
 *    lifetimes, reentrancy, enums, static_assert) and prints the results.
 * 2. Prints where the event_counter module's objects live in memory.
 * 3. Loop: every press of B1 (PC13) is counted by the event_counter module,
 *    LD2 (PA5) toggles, and a 1 s tick is counted in the background.
 *
 * Clock:  HSI 16 MHz (as in A0.1).  Console: USART2 115200 8N1 (ST-LINK VCP).
 */
/* USER CODE END Header */
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h>
#include "console.h"
#include "event_counter.h"
#include "hygiene.h"
#include "hygiene_bench.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PD */
#define LAB_UART_TIMEOUT_MS   (100U)
#define BUTTON_SAMPLE_MS      (10U)
#define TICK_PERIOD_MS        (1000U)
/* USER CODE END PD */

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
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

/**
 * @brief  true once per press of B1 (active low).
 * @note   'previous' is a function-local static: it keeps its value between
 *         calls, lives in .data (initialised to "released"), and is visible
 *         only here. It also makes the function non-reentrant: call it from
 *         the main loop only, never from an ISR as well.
 */
static bool Button_PressedEdge(void)
{
    static GPIO_PinState previous = GPIO_PIN_SET;       /* released */
    const GPIO_PinState  now      = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
    const bool           pressed  = (previous == GPIO_PIN_SET) && (now == GPIO_PIN_RESET);

    previous = now;
    return pressed;
}
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    /* USER CODE BEGIN 2 */
    Console_Init(Lab_UartWrite);
    Console_Printf("\r\n=== A0.6 Hygiene bench (HAL) ===\r\n");
    Console_Printf("event_counter v0x%08lX\r\n", (unsigned long)g_eventCounterVersion);
    (void)HygieneBench_Run();

    Console_Printf("\r\n--- Where the module's objects live ---\r\n");
    EventCounter_PrintStorage();
    Console_Printf("\r\nPress B1: each press is counted by event_counter.c and toggles LD2.\r\n");

    uint32_t lastTick = HAL_GetTick();
    /* USER CODE END 2 */

    while (1)
    {
        /* USER CODE BEGIN 3 */
        if ((HAL_GetTick() - lastTick) >= TICK_PERIOD_MS)      /* wrap-safe (A0.1) */
        {
            lastTick += TICK_PERIOD_MS;
            EventCounter_Record(EVENT_TICK);
        }

        if (Button_PressedEdge())
        {
            EventCounter_Record(EVENT_BUTTON);
            HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
            Console_Printf("%s #%lu at %s %lu s\r\n",
                           EventCounter_Name(EVENT_BUTTON), (unsigned long)EventCounter_Get(EVENT_BUTTON),
                           EventCounter_Name(EVENT_TICK), (unsigned long)EventCounter_Get(EVENT_TICK));
        }
        HAL_Delay(BUTTON_SAMPLE_MS);
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

/** @brief PA5 = LD2 output, PC13 = B1 input with pull-up. */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin  = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

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
