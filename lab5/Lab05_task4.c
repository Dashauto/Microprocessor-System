#include "init.h"
#include <stdint.h>
#include <stdlib.h>

void configureDAC();
void DMA_init();
void DMA2_Stream0_IRQHandler();
float IIRfilter_asm();
float IIRfilter_c();
void Init_Timer();

DAC_HandleTypeDef dac1;
DAC_ChannelConfTypeDef daccctd;
DMA_HandleTypeDef ADC_DMA;
DMA_HandleTypeDef DAC_DMA;
TIM_HandleTypeDef htim6;

uint32_t ADC_buffer[1000];

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
    Init_Timer();
	HAL_DAC_MspInit(&dac1);
	HAL_DAC_Start(&dac1, DAC_CHANNEL_1);

	HAL_ADC_Start_DMA(&ADC_h3, (uint32_t*)ADC_buffer, 1000);
	while (1)
	{
	}
}



void DMA2_Stream0_IRQHandler(){
	HAL_DMA_IRQHandler(&ADC_DMA);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	//Shift the value by one unit time leftward.
	// yminus1 = IIRfilter_c();
    for (int i = 0; i < 999; i++){
        xminus2 = xminus1;
        xminus1 = x;
        x = ADC_buffer[i];
        yminus1 = a1 * x + a2 * xminus1 + a1 * xminus2 + a3 * yminus1;
        HAL_DAC_SetValue(&dac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint16_t)yminus1); // p204
    }

}

void configureDAC()
{
	// Enable the ADC Clock.
	__HAL_RCC_DAC_CLK_ENABLE();

	dac1.Instance = DAC;
	HAL_DAC_Init(&dac1);
	// HAL_ADC_Init(&dac1);

	daccctd.DAC_Trigger = DAC_TRIGGER_T6_TRGO;				   // p198
	daccctd.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;	   // p198
	HAL_DAC_ConfigChannel(&dac1, &daccctd, DAC_CHANNEL_1); // p204
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if (hdac->Instance == DAC)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_4; // p199 A1
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		
		__HAL_RCC_DMA1_CLK_ENABLE();
		// Reference_Manual P. 249
		DAC_DMA.Instance = DMA1_Stream5;
		DAC_DMA.Init.Channel = DMA_CHANNEL_7;  //DAC
		// HAL_and_LL_Drivers P.275/282
		DAC_DMA.Init.Direction = DMA_MEMORY_TO_PERIPH;
		DAC_DMA.Init.PeriphInc = DMA_PINC_ENABLE;
		DAC_DMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		DAC_DMA.Init.MemInc = DMA_MINC_ENABLE;
		DAC_DMA.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		DAC_DMA.Init.Mode = DMA_CIRCULAR; //我不知道对不对
		DAC_DMA.Init.Priority = DMA_PRIORITY_MEDIUM;
		// "hdmatx" in HAL_and_LL_Drivers P.198
		__HAL_LINKDMA(&dac1,DMA_Handle1,DAC_DMA);
		HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

		HAL_DMA_Init(&DAC_DMA);
	}
	// GPIO init
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
    ADC_h3.Init.ExternalTrigConv = ADC_EXTERNALTRIG0_T6_TRGO;

	ADC_h3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	HAL_ADC_Init(&ADC_h3); // Initialize the ADC

	ADC_ChannelConfTypeDef chanconfig;
	// Configure the ADC channel
	// HAL and Low-layer drivers P. 90
	chanconfig.Channel = ADC_CHANNEL_8;
	chanconfig.Rank = ADC_REGULAR_RANK_1;
	chanconfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
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
		GPIO_InitStruct.Pin = GPIO_PIN_10; // A3
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

		
		__HAL_RCC_DMA2_CLK_ENABLE();
		// Reference_Manual P. 249
		ADC_DMA.Instance = DMA2_Stream0;
		ADC_DMA.Init.Channel = DMA_CHANNEL_2; //ADC3
		// HAL_and_LL_Drivers P.275/282
		ADC_DMA.Init.Direction = DMA_PERIPH_TO_MEMORY;
		ADC_DMA.Init.PeriphInc = DMA_PINC_DISABLE;
		ADC_DMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		ADC_DMA.Init.MemInc = DMA_MINC_ENABLE;
		ADC_DMA.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		ADC_DMA.Init.Mode = DMA_CIRCULAR; // circular
		ADC_DMA.Init.Priority = DMA_PRIORITY_MEDIUM;
		ADC_DMA.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		// "DMA_Handle" in HAL_and_LL_Drivers P.91
		HAL_DMA_Init(&ADC_DMA);
		__HAL_LINKDMA(hadc,DMA_Handle,ADC_DMA);  

		HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);


	}
	
}

void Init_timer_HAL()
{
	HAL_TIM_Base_MspInit(&htim6);

	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 9; // 108MHz / 10 = 10.8MHz
	htim6.Init.Period = 9;	  // 10.8MHz / 100 = 1080kHz

	__HAL_RCC_TIM6_CLK_ENABLE();

	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	HAL_TIM_Base_Init(&htim6);
	HAL_TIM_Base_Start_IT(&htim6);
}

void TIM6_DAC_IRQHandler()
{
	TIM6->SR &= 0xFFFFFFFE; // clear flag

}

// This callback is automatically called by the HAL on the UEV event
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6) //provided through the timer handle passed to the callback function
	{
	}
}