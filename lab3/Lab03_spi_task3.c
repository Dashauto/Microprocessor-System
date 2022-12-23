//----------------------------------
// Lab 3 - Part 3: SPI - Lab03_spi.c
//----------------------------------
//

#include "init.h"
char key_getchar();

// If needed:
//#include <stdio.h>
//#include <stdlib.h>

/*
 * For convenience, configure the SPI handler here
 */
char wordsr[1];
char wordss[1];
//char wordss[1];

GPIO_InitTypeDef GPIO_InitStructure;
SPI_HandleTypeDef SPI_2;
// See 769 Description of HAL drivers.pdf, Ch. 58.1 or stm32f7xx_hal_spi.c
void configureSPI() {
    //
    //    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    //
    //
    //    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, ENABLE);
    //    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, DISABLE);
	__SPI2_CLK_ENABLE();
    // HAL driver 900
    SPI_2.Instance = SPI2;                   // Please use SPI2!
    SPI_2.Init.Mode = SPI_MODE_MASTER;       // Set master mode
	SPI_2.Init.Direction = SPI_DIRECTION_2LINES;
	SPI_2.Init.TIMode = SPI_TIMODE_DISABLE; // Use Motorola mode, not TI mode
	SPI_2.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_2.Init.NSS = SPI_NSS_HARD_OUTPUT;
	SPI_2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI_2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    /*
	// HAL driver 62.3 912
    SPI_2.Init.DataSize = SPI_DATASIZE_8BIT;
    // HAL driver 62.3 911
	SPI_2.Init.CLKPolarity = SPI_POLARITY_HIGH;
    // HAL driver 62.3 911
	SPI_2.Init.CLKPhase = SPI_PHASE_1EDGE;
    // HAL driver 62.3 912
	SPI_2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    // HAL driver 62.3 916
	SPI_2.Init.NSS = SPI_NSS_HARD_OUTPUT;
    // HAL driver 62.3 911
	SPI_2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    // HAL driver 62.3 912
	SPI_2.Init.Direction = SPI_DIRECTION_2LINES;
	*/



    HAL_SPI_Init(&SPI_2);
    //
    // Note: HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
    //
    // HAL_SPI_Init() will call HAL_SPI_MspInit() after reading the properties of
    // the passed SPI_HandleTypeDef. After finishing the MspInit call, it will set
    // the SPI property bits. This is how all HAL_[peripheral]_Init() functions work.
}

/*
 * This is called upon SPI initialization. It handles the configuration
 * of the GPIO pins for SPI.
 */
// Do NOT change the name of an MspInit function; it needs to override a
// __weak function of the same name. It does not need a prototype in the header.
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
    // SPI GPIO initialization structure here
    // Discovery_Manual Appendix A 37, D11, D12
	__GPIOB_CLK_ENABLE();
    GPIO_InitStructure.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    if (hspi->Instance == SPI2)
    {
        //        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
        // Enable SPI GPIO port clocks, set HAL GPIO init structure's values for each
        HAL_NVIC_EnableIRQ(SPI2_IRQn);
        // SPI-related port pin (SPI port pin configuration), enable SPI IRQs (if applicable), etc.
    }
}

int main(void) {
    Sys_Init();

    // For convenience
    configureSPI();

    // See 769 Description of HAL drivers.pdf, Ch. 58.2.3 or stm32f7xx_hal_spi.c
    //
    //	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
    //
	while (1)
	{
		printf("\033c");
		wordss[0] = key_getchar();
		if ((int)wordss[0] > 32 && (int)wordss[0] < 126)
		{
			// received from keyboard
            printf("\033[HFrom Key: The word is %c .\r", wordss[0]);
			fflush(stdout);

			// Transmit and receive function
            HAL_SPI_TransmitReceive(&SPI_2,(uint8_t*) wordss ,(uint8_t*)wordsr , 1, 1000);

            // received from SPI
			printf("\033[12;0HFrom SPI: The word is %c .\r", wordsr[0]);
			fflush(stdout);

		}

	}
}

// keyboard capture function from UART
char key_getchar()
{
	char input[1];
    HAL_UART_Receive(&USB_UART, (uint8_t *)input, 1, 1000);
    return (char)input[0];
}

