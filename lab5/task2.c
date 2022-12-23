#include "init.h"
#include <stdint.h>
#include<stdlib.h>


char rece_buffer[50];
char send_buffer[50];
int i = 0;
UART_HandleTypeDef uart_handle;
SPI_HandleTypeDef SPI_2;
DMA_HandleTypeDef RX_DMA;
DMA_HandleTypeDef TX_DMA;
void configureSPI();
void DMA_init();
void DMA1_Stream3_IRQHandler();
char key_getchar();

int main()
{
	Sys_Init();
	HAL_Init();
    SystemClock_Config();

    configureSPI();
	DMA_init();

	char receive;
	while(1)
	{
		/*
		user_s[0] = key_getchar();
		printf("\033[2J\033[;H");
		fflush(stdout);
		printf("From computer:\r\n%c",user_s[0]);
		fflush(stdout);

		HAL_SPI_TransmitReceive_DMA(&SPI_2, (uint8_t *)user_s,(uint8_t *)user_m,1);
		*/
		printf("\033[5;0H\033[2K"); fflush(stdout);
		printf("\033[4;0HThe word from keyboard is: \r\n");
		while((int)receive != 13)
		{
			receive = getchar();
			printf("%c", receive);
			fflush(stdout);
			send_buffer[i] = receive;
			i ++;
		}
		HAL_SPI_TransmitReceive_DMA(&SPI_2, (uint8_t *)send_buffer, (uint8_t *)rece_buffer, i + 1);
		HAL_Delay(50);
		receive = 0;
		i = 0;
	}
}

void DMA_init()
{

	__HAL_RCC_DMA1_CLK_ENABLE();
	// Reference_Manual P. 249
	RX_DMA.Instance = DMA1_Stream3;
	RX_DMA.Init.Channel = DMA_CHANNEL_0;
	// HAL_and_LL_Drivers P.275/282
	RX_DMA.Init.Direction = DMA_PERIPH_TO_MEMORY;
	RX_DMA.Init.PeriphInc = DMA_PINC_DISABLE;
	RX_DMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	RX_DMA.Init.MemInc = DMA_MINC_ENABLE;
	RX_DMA.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	RX_DMA.Init.Mode = DMA_NORMAL;
	RX_DMA.Init.Priority = DMA_PRIORITY_MEDIUM;
	// "hdmarx" in HAL_and_LL_Drivers P.901
	__HAL_LINKDMA(&SPI_2,hdmarx,RX_DMA);
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

	HAL_DMA_DeInit(&RX_DMA);
	HAL_DMA_Init(&RX_DMA);


	// Reference_Manual P. 249
	TX_DMA.Instance = DMA1_Stream4;
	TX_DMA.Init.Channel = DMA_CHANNEL_0;
	// HAL_and_LL_Drivers P.275/282
	TX_DMA.Init.Direction = DMA_MEMORY_TO_PERIPH;
	TX_DMA.Init.PeriphInc = DMA_PINC_DISABLE;
	TX_DMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	TX_DMA.Init.MemInc = DMA_MINC_ENABLE;
	TX_DMA.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	TX_DMA.Init.Mode = DMA_NORMAL;
	TX_DMA.Init.Priority = DMA_PRIORITY_MEDIUM;
	// "hdmatx" in HAL_and_LL_Drivers P.901
	__HAL_LINKDMA(&SPI_2,hdmatx,TX_DMA);
	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);


	HAL_DMA_DeInit(&TX_DMA);
	HAL_DMA_Init(&TX_DMA);

}

void DMA1_Stream3_IRQHandler()
{
	HAL_DMA_IRQHandler(&RX_DMA);
	printf("\033[16;0HThe word from SPI is: \r\n");
	printf("\033[2K");
	fflush(stdout);
	for(int index = 0; index <= i; index++)
	{
		printf("%c", rece_buffer[index]);
		fflush(stdout);
	}
	//rece_buffer[0] = 0;
}

void DMA1_Stream4_IRQHandler()
{
	HAL_DMA_IRQHandler(&TX_DMA);
	//send_buffer[0] = 0;
}

void configureSPI()
{
	__SPI2_CLK_ENABLE();

	SPI_2.Instance = SPI2;
	SPI_2.Init.Mode = SPI_MODE_MASTER;
	SPI_2.Init.Direction = SPI_DIRECTION_2LINES;
	SPI_2.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_2.Init.CLKPolarity = SPI_POLARITY_HIGH;
	SPI_2.Init.CLKPhase = SPI_PHASE_2EDGE;
	SPI_2.Init.NSS = SPI_NSS_HARD_OUTPUT;
	SPI_2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	SPI_2.Init.FirstBit = SPI_FIRSTBIT_LSB;
	SPI_2.Init.TIMode = SPI_TIMODE_DISABLE;
	SPI_2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	HAL_SPI_Init(&SPI_2);

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio_init;
	gpio_init.Pin = GPIO_PIN_14 | GPIO_PIN_15;
	gpio_init.Mode = GPIO_MODE_AF_PP;
	gpio_init.Pull = GPIO_PULLUP;
	gpio_init.Speed = GPIO_SPEED_HIGH;
    gpio_init.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &gpio_init);

    gpio_init.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOA, &gpio_init);
}

char key_getchar()
{
    char input[1];
    HAL_UART_Receive(&USB_UART, (uint8_t *)input, 1, 1000);
    return (char)input[0];
}
