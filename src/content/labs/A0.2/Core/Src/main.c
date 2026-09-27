/* USER CODE BEGIN Header */
/**
 * @file    main.c
 * @brief   A0.2 Register X-ray: HAL version (NUCLEO-F446RE).
 *
 * Uses the HAL GPIO API and prints what each call really does to the GPIO
 * registers. Button B1 (PC13, active low) drives LED LD2 (PA5).
 *
 * Clock:  HSI 16 MHz (as in A0.1).  Console: USART2 115200 8N1 (ST-LINK VCP).
 */
/* USER CODE END Header */
#include "main.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include "bits.h"
#include "bits_selftest.h"
#include "xray.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PD */
#define LAB_UART_TIMEOUT_MS   (100U)
#define BUTTON_SAMPLE_MS      (10U)   /**< Sampling period: hides most bounce (A5.5). */
#define PIN5_MODER_POS        (5U * 2U)
#define PIN5_OSPEEDR_POS      (5U * 2U)
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

/** @brief Part 1: what do HAL_GPIO_WritePin / TogglePin do to ODR? */
static void Demo_HalWrites(void)
{
    uint32_t before;

    Xray_Printf("\r\n--- HAL_GPIO_WritePin / TogglePin -> GPIOA->ODR ---\r\n");
    Xray_Ruler();

    before = GPIOA->ODR;
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);      /* BSRR = 1 << 5  */
    Xray_Diff("WritePin SET", before, GPIOA->ODR);

    before = GPIOA->ODR;
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);                    /* BSRR from ODR  */
    Xray_Diff("TogglePin", before, GPIOA->ODR);

    /* BSRR is write-only: reading it always returns 0 (RM0390, GPIO registers). */
    Xray_Show("GPIOA->BSRR", GPIOA->BSRR);
}

/** @brief Part 2: re-initialise PA5 with each speed and watch the 2-bit field. */
static void Demo_SpeedField(void)
{
    static const uint32_t speeds[] = {
        GPIO_SPEED_FREQ_LOW, GPIO_SPEED_FREQ_MEDIUM, GPIO_SPEED_FREQ_HIGH, GPIO_SPEED_FREQ_VERY_HIGH
    };
    GPIO_InitTypeDef init = {0};

    Xray_Printf("\r\n--- HAL_GPIO_Init(Speed) -> GPIOA->OSPEEDR[11:10] ---\r\n");
    Xray_Ruler();

    init.Pin  = LD2_Pin;
    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Pull = GPIO_NOPULL;

    for (uint32_t i = 0U; i < (sizeof speeds / sizeof speeds[0]); i++)
    {
        const uint32_t before = GPIOA->OSPEEDR;

        init.Speed = speeds[i];
        HAL_GPIO_Init(LD2_GPIO_Port, &init);
        Xray_Diff("OSPEEDR", before, GPIOA->OSPEEDR);
        Xray_Printf("%*sspeed field = %lu\r\n", 34, "",
                    (unsigned long)field_get(GPIOA->OSPEEDR, FIELD_MASK(2U, PIN5_OSPEEDR_POS), PIN5_OSPEEDR_POS));
    }

    Xray_Printf("PA5 mode field (MODER[11:10]) = %lu (1 = general-purpose output)\r\n",
                (unsigned long)field_get(GPIOA->MODER, FIELD_MASK(2U, PIN5_MODER_POS), PIN5_MODER_POS));
}
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    /* USER CODE BEGIN 2 */
    Xray_Init(Lab_UartWrite);
    Xray_Printf("\r\n=== A0.2 Register X-ray (HAL) ===\r\n");
    (void)BitsSelfTest_Run();
    Demo_HalWrites();
    Demo_SpeedField();
    Xray_Printf("\r\nPress B1: the LED follows the button, each edge prints GPIOC->IDR.\r\n");

    GPIO_PinState previous = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
    uint32_t      presses  = 0U;
    /* USER CODE END 2 */

    while (1)
    {
        /* USER CODE BEGIN 3 */
        const GPIO_PinState now = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

        if (now != previous)
        {
            const bool pressed = (now == GPIO_PIN_RESET);   /* B1 is active low */

            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, pressed ? GPIO_PIN_SET : GPIO_PIN_RESET);
            if (pressed)
            {
                presses++;
            }
            Xray_Printf("%s #%lu\r\n", pressed ? "PRESS  " : "RELEASE", (unsigned long)presses);
            Xray_Show("GPIOC->IDR", GPIOC->IDR);
            previous = now;
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
