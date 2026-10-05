/* USER CODE BEGIN Header */
/**
 * @file    main.c
 * @brief   A0.4 Register probe: HAL version (NUCLEO-F446RE).
 *
 * Shows that every HAL / CMSIS name is, in the end, an address: GPIOA is a
 * cast of 0x40020000, HAL_GetDEVID() reads 0xE0042000, and
 * HAL_GPIO_WritePin() is one store to 0x40020018.
 *
 * Clock:  HSI 16 MHz (as in A0.1).  Console: USART2 115200 8N1 (ST-LINK VCP).
 */
/* USER CODE END Header */
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stddef.h>
#include <string.h>
#include "devinfo.h"
#include "led_drv.h"
#include "mmio.h"
#include "mmio_selftest.h"
#include "probe.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PD */
#define LAB_UART_TIMEOUT_MS   (100U)
#define BLINK_MS              (500U)
#define LD2_PIN_NUMBER        (5U)
/* USER CODE END PD */

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN 0 */
/* The CMSIS struct layout matches the RM0390 offsets, checked by the compiler. */
_Static_assert(offsetof(GPIO_TypeDef, ODR) == GPIO_ODR_OFS, "ODR offset");
_Static_assert(offsetof(GPIO_TypeDef, BSRR) == GPIO_BSRR_OFS, "BSRR offset");
_Static_assert(offsetof(RCC_TypeDef, AHB1ENR) == RCC_AHB1ENR_OFS, "AHB1ENR offset");

static void Lab_UartWrite(const char *text)
{
    size_t length = strlen(text);

    if (length > UINT16_MAX)
    {
        length = UINT16_MAX;
    }
    (void)HAL_UART_Transmit(&huart2, (const uint8_t *)text, (uint16_t)length, LAB_UART_TIMEOUT_MS);
}

/** @brief Part 1: a CMSIS peripheral name is a pointer cast of a base address. */
static void Demo_NamesAreAddresses(void)
{
    Probe_Printf("GPIOA                = (GPIO_TypeDef *)0x%08lX\r\n", (unsigned long)(uintptr_t)GPIOA);
    Probe_Printf("&GPIOA->BSRR         = 0x%08lX\r\n", (unsigned long)(uintptr_t)&GPIOA->BSRR);
    Probe_Printf("sizeof(GPIO_TypeDef) = 0x%02lX bytes (10 registers)\r\n", (unsigned long)sizeof(GPIO_TypeDef));
}

/** @brief Part 2: HAL's ID functions read the same addresses as DevInfo_Read(). */
static void Demo_HalVsPointers(void)
{
    const DevInfo_Sources src = DEVINFO_SOURCES_STM32F4;
    DevInfo info;

    DevInfo_Read(&src, &info);
    Probe_PrintDevInfo(&info);

    Probe_Printf("HAL_GetDEVID() = 0x%03lX, HAL_GetREVID() = 0x%04lX, HAL_GetUIDw0() = 0x%08lX\r\n",
                 (unsigned long)HAL_GetDEVID(), (unsigned long)HAL_GetREVID(), (unsigned long)HAL_GetUIDw0());
    Probe_Printf("HAL and raw pointers agree: %s\r\n",
                 ((HAL_GetDEVID() == info.devId) && (HAL_GetREVID() == info.revId) &&
                  (HAL_GetUIDw0() == info.uid[0])) ? "yes" : "NO");
}

/** @brief Part 3: HAL_GPIO_WritePin() vs a raw store to BSRR: same effect. */
static void Demo_WritePinIsAStore(void)
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    Probe_Printf("after HAL_GPIO_WritePin(SET):    ODR = 0x%08lX\r\n", (unsigned long)GPIOA->ODR);

    REG32(LAB_GPIOA_BASE + GPIO_BSRR_OFS) = 1UL << (LD2_PIN_NUMBER + 16U);   /* BR5 */
    Probe_Printf("after REG32(0x40020018) = BR5:   ODR = 0x%08lX\r\n", (unsigned long)GPIOA->ODR);
}
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    /* USER CODE BEGIN 2 */
    Probe_Init(Lab_UartWrite);
    Probe_Printf("\r\n=== A0.4 Register probe (HAL) ===\r\n");
    (void)MmioSelfTest_Run();

    Probe_Printf("\r\n--- CMSIS names are addresses ---\r\n");
    Demo_NamesAreAddresses();

    Probe_Printf("\r\n--- Device identification: pointers vs HAL ---\r\n");
    Demo_HalVsPointers();

    Probe_Printf("\r\n--- HAL_GPIO_WritePin is one store to BSRR ---\r\n");
    Demo_WritePinIsAStore();

    /* The lab driver on the real port: CubeMX already made PA5 an output,
     * so LedDrv_Init() rewrites the same field values (no visible change). */
    LedDrv led;
    LedDrv_Init(&led, (volatile uint32_t *)LAB_GPIOA_BASE, LD2_PIN_NUMBER);
    Probe_Printf("\r\nLD2 now blinks at 1 Hz through LedDrv_Toggle().\r\n");
    /* USER CODE END 2 */

    while (1)
    {
        /* USER CODE BEGIN 3 */
        LedDrv_Toggle(&led);
        HAL_Delay(BLINK_MS);
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

/** @brief PA5 = LD2 output, low. */
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
