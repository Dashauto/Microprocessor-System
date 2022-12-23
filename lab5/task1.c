#include "init.h" // Always need init.h, otherwise nothing will work.
#include<stdint.h>
#include<stdlib.h>

DMA_HandleTypeDef handle;
uint8_t start_buffer[1000];
uint8_t end_buffer[1000];
uint16_t start_buffer_16[1000];
uint16_t end_buffer_16[1000];
uint32_t start_buffer_32[1000];
uint32_t end_buffer_32[1000];
uint32_t cycles = 0;
int dma_size = 0;
int dma_test = 0;

int main(void)
{
	Sys_Init();
	DMA_init();
	// Enable the DWT_CYCCNT register
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->LAR = 0xC5ACCE55;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	for(int i = 0; i < 1000; i++)
	{
		//make 1000 array buffer
		start_buffer[i] = i;
		start_buffer_16[i] = i;
		start_buffer_32[i] = i;
	}

	fflush(stdout);
	printf("software:\n\r");
//--------------------------------------------------------------------------------------
//-------------------------- uint8_t test ----------------------------------------------
//--------------------------------------------------------------------------------------

	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int i = 0; i < 10; i++) // 10 uint8_t case
	{
		end_buffer[i] = start_buffer[i];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("uint8_t test: memory length:\t%d\tcycle:\t%u\n\r", 10, cycles);

	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int i = 0; i < 100; i++) // 100 uint8_t case
	{
		
		end_buffer[i] = start_buffer[i];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("uint8_t test: memory length:\t%d\tcycle:\t%u\n\r", 100, cycles);

	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int i = 0; i < 1000; i++) // 1000 uint8_t case
	{
		
		end_buffer[i] = start_buffer[i];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("uint8_t test: memory length:\t%d\tcycle:\t%u\n\r", 1000, cycles);

//--------------------------------------------------------------------------------------
//-------------------------- uint16_t test ----------------------------------------------
//--------------------------------------------------------------------------------------

	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int i = 0; i < 10; i++) // 10 uint16_t case
	{
		
		end_buffer_16[i] = start_buffer_16[i];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("uint16_t test: memory length:\t%d\tcycle:\t%u\n\r", 10, cycles);

	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int i = 0; i < 100; i++) // 100 uint16_t case
	{
		
		end_buffer_16[i] = start_buffer_16[i];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("uint16_t test: memory length:\t%d\tcycle:\t%u\n\r", 100, cycles);

	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int i = 0; i < 1000; i++) // 1000 uint16_t case
	{
		
		end_buffer_16[i] = start_buffer_16[i];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("uint16_t test: memory length:\t%d\tcycle:\t%u\n\r", 1000, cycles);

//--------------------------------------------------------------------------------------
//-------------------------- uint32_t test ----------------------------------------------
//--------------------------------------------------------------------------------------	

	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int i = 0; i < 10; i++) // 10 uint32_t case
	{
		
		end_buffer_32[i] = start_buffer_32[i];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("uint32_t test: memory length:\t%d\tcycle:\t%u\n\r", 10, cycles);

	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int i = 0; i < 100; i++) // 100 uint32_t case
	{
		
		end_buffer_32[i] = start_buffer_32[i];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("uint32_t test: memory length:\t%d\tcycle:\t%u\n\r", 100, cycles);

	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int i = 0; i < 1000; i++) // 1000 uint32_t case
	{
		
		end_buffer_32[i] = start_buffer_32[i];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("uint32_t test: memory length:\t%d\tcycle:\t%u\n\r", 1000, cycles);


	printf("\nDMA:\n\r");
	printf("uint8_t test:\n\r");
	DWT->CYCCNT = 0; // Clear the cycle counter
	dma_size = 10;
	HAL_DMA_Start_IT(&handle, start_buffer, end_buffer, dma_size);
	HAL_Delay(100);
	dma_size = 100;
	DWT->CYCCNT = 0; // Clear the cycle counter
	HAL_DMA_Start_IT(&handle, start_buffer, end_buffer, dma_size);
	HAL_Delay(100);
	dma_size = 1000;
	DWT->CYCCNT = 0; // Clear the cycle counter
	HAL_DMA_Start_IT(&handle, start_buffer, end_buffer, dma_size);
	HAL_Delay(100);

	printf("uint16_t test:\n\r");
	DWT->CYCCNT = 0; // Clear the cycle counter
	dma_size = 10;
	HAL_DMA_Start_IT(&handle, start_buffer_16, end_buffer_16, dma_size);
	HAL_Delay(100);
	dma_size = 100;
	DWT->CYCCNT = 0; // Clear the cycle counter
	HAL_DMA_Start_IT(&handle, start_buffer_16, end_buffer_16, dma_size);
	HAL_Delay(100);
	dma_size = 1000;
	DWT->CYCCNT = 0; // Clear the cycle counter
	HAL_DMA_Start_IT(&handle, start_buffer_16, end_buffer_16, dma_size);
	HAL_Delay(100);

	printf("uint32_t test:\n\r");
	DWT->CYCCNT = 0; // Clear the cycle counter
	dma_size = 10;
	HAL_DMA_Start_IT(&handle, start_buffer_32, end_buffer_32, dma_size);
	HAL_Delay(100);
	dma_size = 100;
	DWT->CYCCNT = 0; // Clear the cycle counter
	HAL_DMA_Start_IT(&handle, start_buffer_32, end_buffer_32, dma_size);
	HAL_Delay(100);
	dma_size = 1000;
	DWT->CYCCNT = 0; // Clear the cycle counter
	HAL_DMA_Start_IT(&handle, start_buffer_32, end_buffer_32, dma_size);
	HAL_Delay(100);



	while(1);
}

void DMA_init()
{
	__HAL_RCC_DMA2_CLK_ENABLE();
	// Reference_Manual P. 249
	handle.Instance = DMA2_Stream0;
	handle.Init.Channel = DMA_CHANNEL_0;
	// HAL_and_LL_Drivers P.275/282
	handle.Init.Direction = DMA_MEMORY_TO_MEMORY;
	// HAL_and_LL_Drivers P.275/284
	handle.Init.PeriphInc = DMA_PINC_ENABLE;
	// HAL_and_LL_Drivers P.275/284
	handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	// HAL_and_LL_Drivers P.275/284
	handle.Init.MemInc = DMA_MINC_ENABLE;
	// HAL_and_LL_Drivers P.275/284
	handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	// HAL_and_LL_Drivers P.275/284
	handle.Init.Mode = DMA_NORMAL;
	handle.Init.Priority = DMA_PRIORITY_MEDIUM;

	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	HAL_DMA_DeInit(&handle);
	HAL_DMA_Init(&handle);

}

void DMA2_Stream0_IRQHandler()
{
	HAL_DMA_IRQHandler(&handle);
	uint32_t cyclesd = DWT->CYCCNT;
	printf("memory length:\t%d\tcycle:\t%u\n\r",dma_size,cyclesd);

}
