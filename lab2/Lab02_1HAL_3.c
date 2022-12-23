//----------------------------------
// Lab 2 - Timer Interrupts - Lab02.c
//----------------------------------
// Objective:
//   Use interrupt
//

//
//
// -- Imports ---------------
//
#include "init.h"

//
//
// -- Prototypes ------------
//
void Init_GPIO();
void Init_Timer();

//
//
// -- Code Body -------------
//
volatile uint8_t timeUpdated = 0;
volatile uint8_t buttonPressed = 0;
volatile uint8_t buttonReleased = 0;
volatile uint32_t elapsed = 0;

int32_t randomNumber = 0;
uint32_t startTime = 0;
float averageScore = 0;
unsigned int iterations = 0;
int flag = 0;
int count = 0;
int t_i = 0;
TIM_HandleTypeDef htim7;

int main()
{
	Sys_Init();
	Init_Timer();
	Init_GPIO();

	while (1)
	{
		if (flag) //EXTI0 interrupt triggered
		{
			printf("pushbutton is pressed.\r\n");
			flag = 0; //reset the signal
		}
		if (t_i) //Timer7 interrupt triggered
		{
			printf("%d tenth seconds has passed\r\n", count);
			t_i = 0;
		}
	}
}

//
//
// -- Init Functions ----------
//
void Init_Timer()
{
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = 21599; // 108MHz/21.6k = 5kHz
	htim7.Init.Period = 499;	  // 5kHz / 0.5k = 10Hz = 0.1s
	__HAL_RCC_TIM7_CLK_ENABLE();  // Enable the TIM6 peripheral

	HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM7_IRQn); // Enable the peripheral IRQ

	HAL_TIM_Base_Init(&htim7);	   // Configure the timer
	HAL_TIM_Base_Start_IT(&htim7); // Start the timer
}

void Init_GPIO()
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef pin_init_A_in_0; // A0
	pin_init_A_in_0.Pin = GPIO_PIN_0;
	pin_init_A_in_0.Mode = GPIO_MODE_INPUT | GPIO_MODE_IT_RISING_FALLING;
	pin_init_A_in_0.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &pin_init_A_in_0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn); // EXTI0

	// Delay after an RCC peripheral clock enabling
	HAL_Delay(1000);
	asm("nop");
	asm("nop");
}

//
//
// -- ISRs (IRQs) -------------
//

void TIM7_IRQHandler()
{
	HAL_TIM_IRQHandler(&htim7);
}

// This callback is automatically called by the HAL on the UEV event
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM7) //provided through the timer handle passed to the callback function
	{
		count += 1;
		t_i = 1; //set up a signal
	}
}

// HAL - GPIO/EXTI Handler
void EXTI0_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0); // clear the interrupt flag
	asm("nop");
	asm("nop");
	flag = 1; //set up a signal
}
