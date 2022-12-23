//----------------------------------
// Lab 2 - Timer Interrupts - Lab02.c
//----------------------------------
// Objective:
//   Build a small game that records user's reaction time.
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
void blinkScreen();
void Init_GPIO();
void Init_Timer();
//void EXTI9_5_IRQHandler();
//
//
// -- Code Body -------------
//
volatile uint8_t timeUpdated = 0;
volatile uint8_t buttonPressed = 0;
volatile uint8_t buttonReleased = 0;
volatile uint32_t elapsed = 0;

uint8_t finish = 0;
uint8_t end_time = 0;
uint8_t endcheck = 0;
int32_t digit = 0;
int32_t number = 0;
int32_t randomNumber = 0;
uint32_t releasedtime = 0;
float averageScore = 0;
unsigned int iterations = 0;
int flag = 0;
int count = 0;
int main() {
	Sys_Init();
	Init_Timer();
	Init_GPIO();
	while (1)
	{
		// nothing in the while(1) loop
	}
}

//
//
// -- Utility Functions ------
//
void blinkScreen(){
	printf("\033[30;47m");
	// Clear and redraw display (flash it & sound the bell).
	printf("\a\033[s\033[2J\033[u");
	fflush(stdout);
	HAL_Delay(100);

	printf("\033[37;40m");
	// Clear and redraw display (flash it).
	printf("\033[s\033[2J\033[u");
	fflush(stdout);
}


//
//
// -- Init Functions ----------
//
void Init_Timer()
{
	// Enable the TIM6 interrupt.
	// Looks like HAL hid this little gem, this register isn't mentioned in
	//   the STM32F7 ARM Reference Manual....
	NVIC->ISER[54/32] = (uint32_t) 1 << (54%32);

	// Enable TIM6 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	asm ( "nop" );
	asm ( "nop" );

	// Set pre-scaler to slow down ticlks
	TIM6->PSC = 10800 - 1;

	// Set the Auto-reload Value for 10Hz overflow
	TIM6->ARR = 1000 - 1;

	// Generate update events to auto reload.
	TIM6->EGR |= 1;

	// Enable Update Interrupts.
	TIM6->DIER |= 1;

	// Start the timer.
	TIM6->CR1 = TIM_CR1_CEN;
}

void Init_GPIO() {
	// PJ0(EXTI0) -> D4 and PC8(EXTI8) -> D5

	// Enable the clock for GPIO
//	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOJEN;

	// Enable clock to SYSCONFIG module to enable writing of EXTICRn registers
	RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// or __HAL_RCC_SYSCFG_CLK_ENABLE();

	// Delay after an RCC peripheral clock enabling
	asm ("nop");
	asm ("nop");

	// Set Pin 13/5 to output. (LED1 and LED2)
	//GPIOJ->MODER
	// Since the default is input mode, not need to set input here

	// GPIO Interrupt
	// By default pin PA0 will trigger the interrupt, change EXTICR1 to route proper pin
	//SYSCFG -> EXTICR[0] &= 0x00000000;	// make EXTI0 to PA0 -> Pushbotton
	SYSCFG -> EXTICR[2] |= 0x00000002;	// make EXTI8 to PC8 -> D5 port

	// Set PJ0 and PC8 as input (button) with pull-down.
	GPIOA -> PUPDR |= 0x00000002;
	GPIOC -> PUPDR |= 0x00020000;


	// Set interrupt enable for EXTI0.
	NVIC -> ISER[6/32] = (uint32_t) 1 << (6%32);
	NVIC -> ISER[23/32] = (uint32_t) 1 << (23%32);

	// Unmask interrupt.
	EXTI -> IMR |= 0x00000101;

	// Register for rising edge.
	EXTI -> RTSR |= 0x00000101;

	// And register for the falling edge.
	EXTI -> FTSR |= 0x00000101;

}

//
//
// -- ISRs (IRQs) -------------
//
void TIM6_DAC_IRQHandler() {
	// Clear Interrupt Bit
	TIM6->SR &= ~TIM_SR_UIF;
	// Other code here:
	if (flag) //button pressed start counting
	{
		count += 1; // count for pressing time
//		printf("hold, count: %d\r\n", count);
	}
	else if (end_time) // button released start counting
	{
		if (!endcheck) // if wait time not exceed 1s; if it exceeds 1s, close this gate
		{
			releasedtime += 1; // user finished a digit, start counting 1s.
//			printf("wait, releasedtime: %d\r\n", releasedtime);
			endcheck = 0;  // we only want to check this once not many times
		}

	}
	if (releasedtime > 10) // if it exceeds 1s, the digit is entered, add it to the number
	{
		releasedtime = 0; // clear release time counter
		endcheck = 1; // have already done with 1s check, stop releasedtime increment
		number = number * 10 + digit; // add the digit to the number
//		printf("done for number, number: %d\r\n", number);
		digit = 0;
	}


}

// Non-HAL GPIO/EXTI Handler
void EXTI9_5_IRQHandler() {
	// Clear Interrupt Bit by setting it to 1.
	EXTI -> PR |= 0x00000100;

	buttonPressed = 1;

	for (int ii=0; ii<10; ii++) {}
}

void EXTI0_IRQHandler() // calls when button pressed
{
	// Clear Interrupt Bit by setting it to 1.
	EXTI -> PR |= 0x00000001;
	for (int ii=0; ii<10; ii++) {}
	if (!flag) // button pressed.
	{
		releasedtime = 0;
		count = 0;
		flag = 1;
		end_time = 0;
//		printf("button pressed. ヽ(・∀・)ﾉ\r\n");
	}
	else if (flag) // button released.
	{
		flag = 0;
		end_time = 1;
		endcheck = 0;
		//printf("button released. ヽ(*・ω・)ﾉ\r\n");
		if (count / 10 >= 3) // press longer than 1s
		{
			if(finish) // if there are numbers not printed
			{
				printf("number: %d\r\n", number);
				number = 0;
			}
			printf("program halts\r\n");
			while(1); // end the program
		}
		else if (count / 10 >= 1) // press longer than 1s
		{
			printf("number: %d\r\n", number);
			number = 0;
			finish = 0;
		}
		else if (count / 10 < 1) // press less than 1s
		{
			releasedtime = 0;
			digit += 1;
			if (digit == 10) // out of the range
			{
				digit = 0; // we can not have 10 for a decimal digit
			}
			finish = 1;
//			printf("digit: %d\r\n", digit);
		}
		count = 0; // clear count number

	}



}

//HAL - GPIO/EXTI Handler
void xxx_IRQHandler() {
	//HAL_GPIO_EXTI_IRQHandler(???);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	// ISR code here.
}


// For the HAL timer interrupts, all of the associated Callbacks need to exist,
// otherwise during assembly, they will generate compiler errors as missing symbols
// Below are the ones that are not used.

// void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim){};
// void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim){};
