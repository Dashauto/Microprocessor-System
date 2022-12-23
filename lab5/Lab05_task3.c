#include "init.h"
#include <stdint.h>
#include <stdlib.h>

void configureDAC();
void DMA_init();
void DMA2_Stream0_IRQHandler();
float IIRfilter_asm();
float IIRfilter_c();

DAC_HandleTypeDef dac1;
DAC_ChannelConfTypeDef daccctd;
DMA_HandleTypeDef ADC_DMA;
DMA_HandleTypeDef DAC_DMA;

uint32_t ADC_buffer[1];

uint32_t xminus2 = 0;
uint32_t xminus1 = 0;
uint32_t x = 0;
float yminus1 = 0;

float a1 = 0.3125;
float a2 = 0.240385;
float a3 = 0.296875;
float sum, test;

ADC_HandleTypeDef ADC_h3;

void configureADC();

int switchon = 0;
float voltage = 0;

// Main Execution Loop
int main(void)
{
	// Initialize the system
	Sys_Init();
	configureDAC();
	configureADC();
	HAL_DAC_MspInit(&dac1);
	HAL_DAC_Start(&dac1, DAC_CHANNEL_1);

	HAL_ADC_Start_DMA(&ADC_h3, (uint32_t *)ADC_buffer, 1);
	while (1)
	{
	}
}

void DMA2_Stream0_IRQHandler()
{
	HAL_DMA_IRQHandler(&ADC_DMA);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	// IIR filter
	xminus2 = xminus1;
	xminus1 = x;
	x = ADC_buffer[0];
	yminus1 = a1 * x + a2 * xminus1 + a1 * xminus2 + a3 * yminus1;
	HAL_DAC_SetValue(&dac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint16_t)yminus1); // p204
}

void configureDAC()
{
	// Enable the ADC Clock.
	__HAL_RCC_DAC_CLK_ENABLE();

	dac1.Instance = DAC;
	HAL_DAC_Init(&dac1);

	daccctd.DAC_Trigger = DAC_TRIGGER_NONE;				   // p198
	daccctd.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;	   // p198
	HAL_DAC_ConfigChannel(&dac1, &daccctd, DAC_CHANNEL_1); // p204
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if (hdac->Instance == DAC)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_4; // p199 PA1
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}

void configureADC()
{
	// Enable the ADC Clock.
	// A3 of PF10 (ADC3_IN8) STM32F769NI MCU - User manual P. 25
	// HAL and Low-layer drivers P. 710
	__HAL_RCC_ADC3_CLK_ENABLE();
	ADC_h3.Instance = ADC3;
	// HAL and Low-layer drivers P. 88/106
	ADC_h3.Init.Resolution = ADC_RESOLUTION_12B;
	// HAL and Low-layer drivers P. 88/101
	ADC_h3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	// HAL and Low-layer drivers P. 88
	ADC_h3.Init.ScanConvMode = ADC_SCAN_DISABLE;
	// HAL and Low-layer drivers P. 89
	ADC_h3.Init.NbrOfConversion = 1;
	// HAL and Low-layer drivers P. 89
	ADC_h3.Init.ContinuousConvMode = ENABLE;
	// HAL and Low-layer drivers P. 89
	ADC_h3.Init.DiscontinuousConvMode = DISABLE;
	// HAL and Low-layer drivers P. 88
	ADC_h3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	// HAL and Low-layer drivers P. 88
	ADC_h3.Init.DMAContinuousRequests = ENABLE;
	// HAL and Low-layer drivers P. 89
	ADC_h3.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	// HAL and Low-layer drivers P. 89
	ADC_h3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	HAL_ADC_Init(&ADC_h3); // Initialize the ADC

	ADC_ChannelConfTypeDef chanconfig;
	// Configure the ADC channel
	// HAL and Low-layer drivers P. 90
	chanconfig.Channel = ADC_CHANNEL_8;
	chanconfig.Rank = ADC_REGULAR_RANK_1;
	chanconfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
	HAL_ADC_ConfigChannel(&ADC_h3, &chanconfig);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if (hadc->Instance == ADC3)
	{
		// Enable GPIO Clocks
		__HAL_RCC_GPIOF_CLK_ENABLE();

		// Initialize Pin
		GPIO_InitStruct.Pin = GPIO_PIN_10; // PA3
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

		__HAL_RCC_DMA2_CLK_ENABLE();
		// Reference_Manual P. 249
		ADC_DMA.Instance = DMA2_Stream0;
		ADC_DMA.Init.Channel = DMA_CHANNEL_2; // ADC3
		// HAL_and_LL_Drivers P.275/282
		ADC_DMA.Init.Direction = DMA_PERIPH_TO_MEMORY;
		ADC_DMA.Init.PeriphInc = DMA_PINC_DISABLE;

		ADC_DMA.Init.MemInc = DMA_MINC_ENABLE;
		ADC_DMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		ADC_DMA.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		ADC_DMA.Init.Mode = DMA_CIRCULAR; // circular
		ADC_DMA.Init.Priority = DMA_PRIORITY_MEDIUM;
		ADC_DMA.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		// "DMA_Handle" in HAL_and_LL_Drivers P.91
		HAL_DMA_Init(&ADC_DMA);
		__HAL_LINKDMA(&ADC_h3, DMA_Handle, ADC_DMA); // link DMA to ADC

		HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	}
}