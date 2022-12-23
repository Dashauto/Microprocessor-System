//----------------------------------
// Lab 2 - Timer Interrupts - Lab02.c
//----------------------------------
// Objective:
//   interrupt
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

int32_t randomNumber = 0;
uint32_t startTime = 0;
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
		HAL_Delay(1000);
		if (buttonPressed) // This is for task1 register GPIO interrupt
		{
			buttonPressed = 0; // clear the flag
			printf("D5 is changed, count: %d\r\n", count);
			count++;
			fflush(stdout);
		}
		if (elapsed) // This is the Task2 register timer interrupt
		{
			elapsed = 0; // clear the flag
			printf("0.1s has passed, count: %d\r\n", count);
			count++;
		}

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
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	// Enable clock to SYSCONFIG module to enable writing of EXTICRn registers
	RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// or __HAL_RCC_SYSCFG_CLK_ENABLE();

	// Delay after an RCC peripheral clock enabling
	asm ("nop");
	asm ("nop");

	// GPIO Interrupt
	// By default pin PA0 will trigger the interrupt, change EXTICR1 to route proper pin
	SYSCFG -> EXTICR[2] |= 0x00000002;	// make EXTI8 to PC8 -> D5 port

	// Set PC8 as input (button) with pull-down.
	GPIOC -> PUPDR |= 0x00020000;
	

	// Set interrupt enable for EXTI8.
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
	elapsed = 1;
}

// Non-HAL GPIO/EXTI Handler
void EXTI9_5_IRQHandler() {
	// Clear Interrupt Bit by setting it to 1.
	EXTI -> PR |= 0x00000100U;

	buttonPressed = 1;

	for (int ii=0; ii<10; ii++) {}
}

void EXTI0_IRQHandler() {
	// Clear Interrupt Bit by setting it to 1.
	EXTI -> PR |= 0x00000001;

	buttonPressed = 1;

	for (int ii=0; ii<10; ii++) {}
}

