//--------------------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//
#include "init.h"
#include <stdint.h>
#include <stdlib.h>

void configureDAC();

DAC_HandleTypeDef dac1;
DAC_ChannelConfTypeDef daccctd;

uint16_t counter = 0;


ADC_HandleTypeDef ADC_h3;

void configureADC();
uint16_t checkvoltage();

int switchon = 0;
uint16_t read_value = 0;
float voltage = 0;

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	configureDAC();
	HAL_DAC_MspInit(&dac1);
	HAL_DAC_Start(&dac1, DAC_CHANNEL_1);

	configureADC();

	while(1){
		read_value =  checkvoltage();
		// voltage = 3.3 * read_value / 4096;
//		if(counter == 4096){
//			counter = 0;
//		}
//		counter++;
		HAL_DAC_SetValue(&dac1, DAC_CHANNEL_1, DAC_CHANNEL_1, read_value); //HAL_and_LL_Drivers p204
	}

	// Code goes here
}

void configureDAC()
{
	// Enable the ADC Clock.
	__HAL_RCC_DAC_CLK_ENABLE();

	dac1.Instance = DAC;
	HAL_DAC_Init(&dac1);
	// HAL_ADC_Init(&dac1);

	daccctd.DAC_Trigger = DAC_TRIGGER_NONE; //HAL_and_LL_Drivers p198
	daccctd.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; //HAL_and_LL_Drivers p198
	HAL_DAC_ConfigChannel(&dac1, &daccctd, DAC_CHANNEL_1); //HAL_and_LL_Drivers p204

}


void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(hdac -> Instance == DAC){
		__HAL_RCC_GPIOF_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_4; //HAL_and_LL_Drivers p199
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
// GPIO init

}


uint16_t checkvoltage()
{
	HAL_ADC_Start(&ADC_h3);
	HAL_ADC_PollForConversion(&ADC_h3,100);
	return HAL_ADC_GetValue(&ADC_h3);
}

void configureADC()
{
	// Enable the ADC Clock.
	//A3 of PF10 (ADC3_IN8) STM32F769NI MCU - User manual P. 25
	// HAL and Low-layer drivers P. 710
	__HAL_RCC_ADC3_CLK_ENABLE();
	ADC_h3.Instance = ADC3;
	// HAL and Low-layer drivers P. 88/106
	ADC_h3.Init.Resolution = ADC_RESOLUTION_12B;
	// HAL and Low-layer drivers P. 88/101
	ADC_h3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	// HAL and Low-layer drivers P. 88
	ADC_h3.Init.ScanConvMode = DISABLE;
	// HAL and Low-layer drivers P. 89
	ADC_h3.Init.NbrOfConversion = 1;
	// HAL and Low-layer drivers P. 89
	ADC_h3.Init.ContinuousConvMode = ENABLE;
	// HAL and Low-layer drivers P. 89
	ADC_h3.Init.DiscontinuousConvMode = DISABLE;
	// HAL and Low-layer drivers P. 88
	ADC_h3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	// HAL and Low-layer drivers P. 88
	ADC_h3.Init.DMAContinuousRequests = DISABLE;
	// HAL and Low-layer drivers P. 89
	ADC_h3.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	// HAL and Low-layer drivers P. 89
	ADC_h3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	HAL_ADC_Init(&ADC_h3); // Initialize the ADC


	/* Available sampling times:

		ADC_SAMPLETIME_3CYCLES
	  ADC_SAMPLETIME_15CYCLES
		ADC_SAMPLETIME_28CYCLES
		ADC_SAMPLETIME_56CYCLES
		ADC_SAMPLETIME_84CYCLES
		ADC_SAMPLETIME_112CYCLES
		ADC_SAMPLETIME_144CYCLES
		ADC_SAMPLETIME_480CYCLES
	*/

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
	GPIO_InitTypeDef  GPIO_InitStruct;
    if(hadc -> Instance == ADC3)
	{
    	// Enable GPIO Clocks
       	__HAL_RCC_GPIOF_CLK_ENABLE();

    	// Initialize Pin
    	GPIO_InitStruct.Pin       = GPIO_PIN_10;
        GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    	GPIO_InitStruct.Pull      = GPIO_PULLUP;
    	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    }


}
