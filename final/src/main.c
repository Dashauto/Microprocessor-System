#include "init.h"
#include <stdint.h>
#include <stdlib.h>
#include "usbh_conf.h"
#include "usbh_hid.h"
#include "usbh_core.h"
#include "ff_gen_drv.h"
#include "usbh_diskio.h"

USBH_HandleTypeDef husbh;
DAC_HandleTypeDef hDAC1;
DMA_HandleTypeDef dma1;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
extern uint16_t dow[];
extern uint16_t ree[];
extern uint16_t mii[];
extern uint16_t faa[];
extern uint16_t sol[];
extern uint16_t laa[];
extern uint16_t sii[];
//If device is connect it becomes 1.
uint8_t connect = 0; 
char input[1];

void Init_timer_HAL();
void configureDAC();
void USBH_UserProcess(USBH_HandleTypeDef *, uint8_t);

int main(void)
{
	// all the inital steps
	Sys_Init();
	configureDAC();
	Init_timer_HAL();
	HAL_TIM_Base_Start(&htim6);
	USBH_Init(&husbh, USBH_UserProcess, 0);
	USBH_RegisterClass(&husbh, USBH_HID_CLASS);
	USBH_Start(&husbh);
	printf("\033[2J");
				fflush(stdout);
				HAL_Delay(1000);
				printf("\033[1;24H-------------------------------- \r\n");
				fflush(stdout);
				printf("\033[2;24H------- Piano Simulation ------- \r\n");
				fflush(stdout);
				printf("\033[3;24H--------------------------------  \r\n");
				fflush(stdout);
				printf("\033[8;14H |      |      |      |      |      |      |      | \r\n");
				printf("\033[9;14H |      |      |      |      |      |      |      | \r\n");
				printf("\033[10;15H|      |      |      |      |      |      |      | \r\n");
				printf("\033[11;15H|      |      |      |      |      |      |      | \r\n");
				printf("\033[12;15H|      |      |      |      |      |      |      | \r\n");
				printf("\033[13;15H|      |      |      |      |      |      |      | \r\n");
				printf("\033[14;15H|      |      |      |      |      |      |      | \r\n");
				printf("\033[15;15H|  C4  |  D4  |  E4  |  F4  |  G4  |  A4  |  B4  | \r\n");
				printf("\033[16;15H|      |      |      |      |      |      |      | \r\n");
				printf("\033[17;15H|      |      |      |      |      |      |      | \r\n");
				printf("\033[18;15H|      |      |      |      |      |      |      | \r\n");
				printf("\033[19;15H|------|------|------|------|------|------|------| \r\n");
				fflush(stdout);
				printf("\033[22;15H Press Q, W, E, R, T, Y, U on keyboard to Play \r");
				fflush(stdout);
	while(1)
	{
		// always check for USB status
		USBH_Process(&husbh);
		while(connect)
		{
			USBH_Process(&husbh);
		}
	}
}

void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
	HID_KEYBD_Info_TypeDef *keybd_info;
	HID_KEYBD_Info_TypeDef *test_use;
	uint8_t keycode;
	uint8_t keyneed[6];
	HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;
	if (HID_Handle->Init == USBH_HID_KeybdInit)
	{
		keybd_info = USBH_HID_GetKeybdInfo(phost);
		// loop through the array to find out the last input
		for(int i = 0; i < 6; i++)
		{
			if (keybd_info->keys[i] == '\0')
			{
				break;
			}
			else
			{
				test_use->keys[0] = keybd_info->keys[i];
			}
		}
		// finally get the input
		keycode = USBH_HID_GetASCIICode(test_use);
		// start the DMA transport and DAC
		if(keycode == 'q')
		{
			HAL_DAC_Start_DMA(&hDAC1, DAC_CHANNEL_1, (uint32_t*)dow,25000, DAC_ALIGN_8B_R);
		}
		else if(keycode == 'w')
		{
			HAL_DAC_Start_DMA(&hDAC1, DAC_CHANNEL_1, (uint32_t*)ree,25000, DAC_ALIGN_8B_R);
		}
		else if(keycode == 'e')
		{
			HAL_DAC_Start_DMA(&hDAC1, DAC_CHANNEL_1, (uint32_t*)mii,25000, DAC_ALIGN_8B_R);
		}
		else if(keycode == 'r')
		{
			HAL_DAC_Start_DMA(&hDAC1, DAC_CHANNEL_1, (uint32_t*)faa,15000, DAC_ALIGN_8B_R);
		}
		else if(keycode == 't')
		{
			HAL_DAC_Start_DMA(&hDAC1, DAC_CHANNEL_1, (uint32_t*)sol,15000, DAC_ALIGN_8B_R);
		}
		else if(keycode == 'y')
		{
			HAL_DAC_Start_DMA(&hDAC1, DAC_CHANNEL_1, (uint32_t*)laa,15000, DAC_ALIGN_8B_R);
		}
		else if(keycode == 'u')
		{
			HAL_DAC_Start_DMA(&hDAC1, DAC_CHANNEL_1, (uint32_t*)sii,15000, DAC_ALIGN_8B_R);
		}

	}

}


void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	switch(id)
	{
	case HOST_USER_SELECT_CONFIGURATION:
		break;

	case HOST_USER_CLASS_ACTIVE:
		// detect keyboard input
		if(phost->pActiveClass == USBH_HID_CLASS)
		{
			// user interface
			printf("\033[2J");
			fflush(stdout);
			HAL_Delay(1000);
			printf("\033[1;24H-------------------------------- \r\n");
			fflush(stdout);
			printf("\033[2;24H------- Piano Simulation ------- \r\n");
			fflush(stdout);
			printf("\033[3;24H--------------------------------  \r\n");
			fflush(stdout);
			printf("\033[8;14H |      |      |      |      |      |      |      | \r\n");
			printf("\033[9;14H |      |      |      |      |      |      |      | \r\n");
			printf("\033[10;15H|      |      |      |      |      |      |      | \r\n");
			printf("\033[11;15H|      |      |      |      |      |      |      | \r\n");
			printf("\033[12;15H|      |      |      |      |      |      |      | \r\n");
			printf("\033[13;15H|      |      |      |      |      |      |      | \r\n");
			printf("\033[14;15H|      |      |      |      |      |      |      | \r\n");
			printf("\033[15;15H|  C4  |  D4  |  E4  |  F4  |  G4  |  A4  |  B4  | \r\n");
			printf("\033[16;15H|      |      |      |      |      |      |      | \r\n");
			printf("\033[17;15H|      |      |      |      |      |      |      | \r\n");
			printf("\033[18;15H|      |      |      |      |      |      |      | \r\n");
			printf("\033[19;15H|------|------|------|------|------|------|------| \r\n");
			fflush(stdout);
			printf("\033[22;15H Press Q, W, E, R, T, Y, U on keyboard to Play \r");
			fflush(stdout);
		}
		break;

	case HOST_USER_CLASS_SELECTED:
		break;

	case HOST_USER_CONNECTION:
		// if a USB device is connected
		connect =1;
		break;

	case HOST_USER_DISCONNECTION:
		// if it is disconnected
		connect =0;
		break;

	case HOST_USER_UNRECOVERED_ERROR:
		break;
	}
}


void DMA1_Stream5_IRQHandler()
{
	HAL_DMA_IRQHandler(&dma1);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
	// stop to receive other outputs
	HAL_DAC_Stop_DMA(&hDAC1, DAC_CHANNEL_1);
}


void Init_timer_HAL(){
	TIM_MasterConfigTypeDef sMasterConfig;

	//Enable clock
	__HAL_RCC_TIM6_CLK_ENABLE();

	htim6.Instance = TIM6;
	htim6.Init.Prescaler =0;
	htim6.Init.Period = 2000;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	//Initiate the timer
	HAL_TIM_Base_Init(&htim6);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);


	HAL_Init();
	HAL_TIM_Base_MspInit(&htim7);

	htim7.Instance = TIM7;
	htim7.Init.Prescaler =2249;
	htim7.Init.Period = 999;

	//Enable clock
	__HAL_RCC_TIM7_CLK_ENABLE();

	//Set priority and enable IRQ
	HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);

	//Initiate the timer
	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start_IT(&htim7);
}

void configureDAC(){
	__HAL_RCC_DAC_CLK_ENABLE();

	hDAC1.Instance = DAC;
	HAL_DAC_Init(&hDAC1);

	DAC_ChannelConfTypeDef dacchan;
	dacchan.DAC_Trigger = DAC_TRIGGER_T6_TRGO;//The DAC event triggered by TIM6
	dacchan.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	HAL_DAC_ConfigChannel(&hDAC1, &dacchan, DAC_CHANNEL_1);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
    if(hdac->Instance == DAC){
    	// Enable GPIO Clocks
       	__HAL_RCC_GPIOA_CLK_ENABLE();

    	// Initialize Pin
    	GPIO_InitStruct.Pin       = GPIO_PIN_4;
        GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    	GPIO_InitStruct.Pull      = GPIO_NOPULL;
    	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    	//Set the DMA and connect to DAC
    	 __HAL_RCC_DMA1_CLK_ENABLE();
    	 dma1.Instance = DMA1_Stream5;
    	 dma1.Init.Channel = DMA_CHANNEL_7;
    	 dma1.Init.Direction = DMA_MEMORY_TO_PERIPH;
         dma1.Init.PeriphInc = DMA_PINC_DISABLE;
    	 dma1.Init.MemInc = DMA_MINC_ENABLE;
    	 dma1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    	 dma1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    	 dma1.Init.Mode = DMA_CIRCULAR;
    	 dma1.Init.Priority = DMA_PRIORITY_LOW;
    	 dma1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    	 HAL_DMA_Init(&dma1);

    	 __HAL_LINKDMA(hdac, DMA_Handle1,dma1);

    	 HAL_NVIC_SetPriority(DMA1_Stream5_IRQn,0,0);
    	 HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    }
  }

void TIM7_IRQHandler(){
	HAL_TIM_IRQHandler(&htim7);
}


