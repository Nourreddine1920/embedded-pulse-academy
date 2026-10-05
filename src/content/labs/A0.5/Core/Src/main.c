/* USER CODE BEGIN Header */
/**
 * @file    main.c
 * @brief   A0.5 Register maps & layouts: HAL version (NUCLEO-F446RE).
 *
 * The HAL drives the board through CMSIS's GPIO_TypeDef / USART_TypeDef.
 * This program lays our own my_gpio_t / my_usart_t (regmap.h) over the same
 * addresses and proves both views agree: at compile time (regmap_check.h)
 * and at run time (same addresses, same register values).
 * Then it prints the layout report, and on every press of B1 it encodes a
 * sensor frame at an odd address and decodes it again.
 *
 * Clock:  HSI 16 MHz (as in A0.1).  Console: USART2 115200 8N1 (ST-LINK VCP).
 */
/* USER CODE END Header */
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h>
#include "regmap.h"
#include "regmap_check.h"      /* compile-time: my_*_t == CMSIS *_TypeDef */
#include "layout_report.h"
#include "sensor_frame.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PD */
#define LAB_UART_TIMEOUT_MS   (100U)
#define BUTTON_SAMPLE_MS      (10U)
#define LED_PIN               (5U)
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

/** @brief Pointer -> integer for printing an address (A0.4). */
static unsigned long addr_of(const volatile void *p)
{
    return (unsigned long)(uintptr_t)p;
}

/** @brief CMSIS view vs our view of the same registers. */
static void Demo_TwoViews(void)
{
    Report_Printf("\r\n--- Two views of the same silicon ---\r\n");
    Report_Printf("GPIOA           = 0x%08lX   MY_GPIOA           = 0x%08lX\r\n",
                  addr_of(GPIOA), addr_of(MY_GPIOA));
    Report_Printf("&GPIOA->AFR[1]  = 0x%08lX   &MY_GPIOA->AFR[1]  = 0x%08lX\r\n",
                  addr_of(&GPIOA->AFR[1]), addr_of(&MY_GPIOA->AFR[1]));
    Report_Printf("&huart2.Instance->BRR = 0x%08lX   &MY_USART2->BRR = 0x%08lX\r\n",
                  addr_of(&huart2.Instance->BRR), addr_of(&MY_USART2->BRR));

    /* Same bus reads, two spellings. HAL computed BRR = 0x8B for 115200 @ 16 MHz. */
    Report_Printf("huart2.Instance->BRR = 0x%08lX   MY_USART2->BRR  = 0x%08lX\r\n",
                  (unsigned long)huart2.Instance->BRR, (unsigned long)MY_USART2->BRR);
    Report_Printf("GPIOA->MODER    = 0x%08lX   MY_GPIOA->MODER    = 0x%08lX\r\n",
                  (unsigned long)GPIOA->MODER, (unsigned long)MY_GPIOA->MODER);

    /* Write through our struct, read back through the HAL. */
    MY_GPIOA->BSRR = 1UL << LED_PIN;
    Report_Printf("MY_GPIOA->BSRR = 1 << 5  ->  HAL_GPIO_ReadPin(LD2) = %u\r\n",
                  (unsigned)HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin));
    MY_GPIOA->BSRR = 1UL << (LED_PIN + 16U);
    Report_Printf("MY_GPIOA->BSRR = 1 << 21 ->  HAL_GPIO_ReadPin(LD2) = %u\r\n",
                  (unsigned)HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin));
}

/** @brief Encode a reading at an odd address, decode it, print it. */
static void Demo_FrameOnPress(uint8_t seq)
{
    _Alignas(4) uint8_t rx[1U + SENSOR_FRAME_SIZE];
    sensor_reading_t    out = {0};
    const sensor_reading_t in = {
        .seq = seq, .temp_cdeg = 2150, .humidity_cpct = 4012U,
        .timestamp_ms = HAL_GetTick(),
        .status = (uint8_t)(SENSOR_STATUS_VALID | (3U << SENSOR_STATUS_ID_POS)),
    };

    SensorFrame_Encode(&in, &rx[1]);
    const bool ok = SensorFrame_DecodeBytes(&rx[1], &out);
    Report_Printf("frame #%u: %s  temp=%d cdeg  hum=%u c%%  t=%lu ms\r\n",
                  (unsigned)out.seq, ok ? "ok " : "BAD", (int)out.temp_cdeg,
                  (unsigned)out.humidity_cpct, (unsigned long)out.timestamp_ms);
}
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    /* USER CODE BEGIN 2 */
    Report_Init(Lab_UartWrite);
    Report_Printf("\r\n=== A0.5 Register maps & layouts (HAL) ===\r\n");
    Demo_TwoViews();
    Report_RegisterMaps();
    Report_Padding();
    Report_Bitfields();
    const uint32_t failures = Report_Frames();
    Report_Printf("\r\nframe checks failed: %lu\r\n", (unsigned long)failures);
    Report_Printf("\r\nPress B1: each press sends one frame through the encoder/decoder.\r\n");

    GPIO_PinState previous = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
    uint8_t       seq      = 0U;
    /* USER CODE END 2 */

    while (1)
    {
        /* USER CODE BEGIN 3 */
        const GPIO_PinState now = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

        if ((now != previous) && (now == GPIO_PIN_RESET))      /* B1 is active low */
        {
            seq++;
            MY_GPIOA->BSRR = ((MY_GPIOA->ODR & (1UL << LED_PIN)) != 0U)
                               ? (1UL << (LED_PIN + 16U))      /* on  -> reset */
                               : (1UL << LED_PIN);             /* off -> set   */
            Demo_FrameOnPress(seq);
        }
        previous = now;
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
