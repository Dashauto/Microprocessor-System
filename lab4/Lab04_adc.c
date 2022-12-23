//--------------------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//

#include "init.h"
#include <stdint.h>
#include <stdlib.h>

ADC_HandleTypeDef ADC_h3;

void configureADC();
uint16_t checkvoltage();
void Init_GPIO_interrup();

int switchon = 0;
uint16_t read_value = 0;
// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	configureADC();
	Init_GPIO_interrup();
	// Code goes here
	
	int samples = 0;
	float voltage = 0;
	float total = 0;
	float high = 0;
	float low = 3.3;
	while(1)
	{
		if (switchon)
		{
			switchon = 0;
			voltage = 3.3 * read_value / 4096;
			total = total + voltage;
			samples += 1;
			if (voltage > high)
				high = voltage;
			if (voltage < low)
				low = voltage;
			printf("\033[0;0H\033[2J");
			printf("raw: %x\r\ndecimal: %10.6f\r\nhigh: %10.6f\r\nlow: %10.6f\r\navg:%10.6f\r\n",read_value,voltage,high,low,total / samples);
		}

	}
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

void Init_GPIO_interrup()
{
	HAL_Init();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	
	GPIO_InitTypeDef pin_init_A_in_0; // A0
	pin_init_A_in_0.Pin = GPIO_PIN_0;
	pin_init_A_in_0.Mode = GPIO_MODE_IT_RISING;
	pin_init_A_in_0.Pull = GPIO_PULLDOWN;
	pin_init_A_in_0.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &pin_init_A_in_0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn); // EXTI0

	// Delay after an RCC peripheral clock enabling
	HAL_Delay(100);
}

// HAL - GPIO/EXTI Handler
void EXTI0_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0); // clear the interrupt flag
	read_value =  checkvoltage(); // get the ADC value
	switchon = 1; //set up a signal
}


