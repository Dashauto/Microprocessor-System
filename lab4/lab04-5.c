#include "init.h"
#include <stdint.h>
#include <stdlib.h>

ADC_HandleTypeDef ADC_h3, ADC_h1;
DAC_HandleTypeDef dac1;
TIM_HandleTypeDef htim6;

uint16_t f2 = 0;
uint16_t f1 = 0;

float x = 0;
float xminus1 = 0;
float xminus2 = 0;
float xminus3 = 0;
float xminus4 = 0;
float y = 0;
float yminus1 = 0;
float yminus2 = 0;
float yminus3 = 0;
float yminus4 = 0;
float sum = 0;

uint16_t adc_state = 0; // 0:sampling  1:adc1 interrupt triggered  2: adc1 gets value and adc3 interrupt triggered  3: adc3 gets value

void configureDAC();
void configureADC();
void Init_timer_HAL();
float analogtodigital(uint16_t a);
uint16_t digitaltoanalog(float a);
void filter();

// Main Execution Loop
int main(void)
{
	// Initialize the system
	Sys_Init();
	configureDAC();
	HAL_DAC_Start(&dac1, DAC_CHANNEL_1);
	configureADC();

	HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ADC_IRQn);

	Init_timer_HAL();

	while (1)
	{
	}
}

void Init_timer_HAL()
{
	HAL_Init();
	HAL_TIM_Base_MspInit(&htim6);

	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 9; // 108MHz / 10 = 10.8MHz
	htim6.Init.Period = 99;	  // 10.8MHz / 100 = 108kHz

	__HAL_RCC_TIM6_CLK_ENABLE();

	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	HAL_TIM_Base_Init(&htim6);
	HAL_TIM_Base_Start_IT(&htim6);
}

void TIM6_DAC_IRQHandler()
{
	TIM6->SR &= 0xFFFFFFFE; // clear flag

	if (adc_state == 0)
	{
		adc_state = 1;
		HAL_ADC_Start_IT(&ADC_h1);
	}
}

void configureDAC()
{
	__HAL_RCC_DAC_CLK_ENABLE();
	dac1.Instance = DAC;
	HAL_DAC_Init(&dac1);
	DAC_ChannelConfTypeDef dacchan;
	dacchan.DAC_Trigger = DAC_TRIGGER_NONE;
	dacchan.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	HAL_DAC_ConfigChannel(&dac1, &dacchan, DAC_CHANNEL_1);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if (hdac->Instance == DAC)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_4;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}

void configureADC()
{
	__HAL_RCC_ADC3_CLK_ENABLE();
	ADC_h3.Instance = ADC3;
	ADC_h3.Init.Resolution = ADC_RESOLUTION_12B;
	ADC_h3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
	ADC_h3.Init.ScanConvMode = DISABLE;
	ADC_h3.Init.NbrOfConversion = 1;
	ADC_h3.Init.ContinuousConvMode = DISABLE;
	ADC_h3.Init.DiscontinuousConvMode = DISABLE;
	ADC_h3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	ADC_h3.Init.DMAContinuousRequests = DISABLE;
	ADC_h3.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	ADC_h3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	HAL_ADC_Init(&ADC_h3);

	ADC_ChannelConfTypeDef chanconfig3;
	chanconfig3.Channel = ADC_CHANNEL_8;
	chanconfig3.Rank = ADC_REGULAR_RANK_1;
	chanconfig3.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&ADC_h3, &chanconfig3);

	__HAL_RCC_ADC1_CLK_ENABLE();
	ADC_h1.Instance = ADC1;
	ADC_h1.Init.Resolution = ADC_RESOLUTION_12B;
	ADC_h1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
	ADC_h1.Init.ScanConvMode = DISABLE;
	ADC_h1.Init.NbrOfConversion = 1;
	ADC_h1.Init.ContinuousConvMode = DISABLE;
	ADC_h1.Init.DiscontinuousConvMode = DISABLE;
	ADC_h1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	ADC_h1.Init.DMAContinuousRequests = DISABLE;
	ADC_h1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	ADC_h1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	HAL_ADC_Init(&ADC_h1);

	ADC_ChannelConfTypeDef chanconfig3;
	chanconfig3.Channel = ADC_CHANNEL_12;
	chanconfig3.Rank = ADC_REGULAR_RANK_1;
	chanconfig3.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&ADC_h1, &chanconfig3);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if (hadc->Instance == ADC3)
	{
		__HAL_RCC_GPIOF_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	}

	if (hadc->Instance == ADC1)
	{
		__HAL_RCC_GPIOC_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	}

	HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_IRQHandler()
{
	if (adc_state == 1)
	{
		HAL_ADC_IRQHandler(&ADC_h1);
	}
	if (adc_state == 2)
	{
		HAL_ADC_IRQHandler(&ADC_h3);
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == ADC1)
	{
		f1 = HAL_ADC_GetValue(&ADC_h1);
		adc_state = 2;
		HAL_ADC_Start_IT(&ADC_h3);
	}
	if (hadc->Instance == ADC3)
	{
		f2 = HAL_ADC_GetValue(&ADC_h3);
		adc_state = 3;
		filter();
	}
}

float analogtodigital(uint16_t a)
{
	float b = 3.3 * a / 4096 return b;
}

uint16_t digitaltoanalog(float a)
{
	uint16_t b = a / 3.3 * 4096;
	return b;
}

void filter()
{
	if (adc_state == 3)
	{ // ADC1 and ADC3 interrupt are triggered
		adc_state = 0;

		xminus4 = xminus3;
		xminus3 = xminus2;
		xminus2 = xminus1;
		xminus1 = x;
		x = rtof(f2) * rtof(f1);

		yminus4 = yminus3;
		yminus3 = yminus2;
		yminus2 = yminus1;
		yminus1 = y;
		y = 0.001 * x - 0.002 * xminus2 + 0.001 * xminus4 + 3.166 * yminus1 - 4.418 * yminus2 + 3.028 * yminus3 - 0.915 * yminus4;
		sum = ftor(y+1);
		HAL_DAC_SetValue(&dac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint16_t)sum);
	}
}