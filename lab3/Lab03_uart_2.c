//------------------------------------
// Lab 3 - Part 1: UART - Lab03_uart.c
//------------------------------------
//

#include "init.h"
void URAT_send();
void URAT_receive();

int alter = 0;
char wordsr;
char wordss;

UART_HandleTypeDef DISCO_UART;
UART_HandleTypeDef USB_UART0;
// Main Execution Loop
int main(void) {
	//Initialize the system
	Sys_Init();
	// Enable interrupt
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_EnableIRQ(USART6_IRQn);
	initUart(&DISCO_UART, 38400, USART6);
	HAL_UART_MspInit(&DISCO_UART);
	initUart(&USB_UART0, 115200, USART1);
	HAL_UART_MspInit(&USB_UART0);
	// First awaken function
	HAL_UART_Receive_IT(&DISCO_UART, (uint8_t *)input_d, 1);
	HAL_UART_Receive_IT(&USB_UART0, (uint8_t *)input_c, 1);


	while(1)
	{
		// no code here
	}

}

void URAT_send()
{
	wordss = uart_getchar(&USB_UART0, 1);
	if (wordss != 0)
	{
		uart_putchar(&DISCO_UART, &wordss);
	}
	if ((int)wordss == 27)
	{
		printf("Program halted.\r\n");
		while(1);
	}
	wordss = 0;
}

void URAT_receive()
{
	wordsr = uart_getchar(&DISCO_UART, 0);
	printf("The word is %c .\r\n", wordsr);
	if ((int)wordsr == 27)
	{
		printf("Program halted.\r\n");
		while(1);
	}
}

// handle 2 interrupts
void USART1_IRQHandler()
{
	HAL_UART_IRQHandler(&USB_UART0);
}

void USART6_IRQHandler()
{
	HAL_UART_IRQHandler(&DISCO_UART);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{

	///* dertermine which input needs to be handled
	// if it is from keyboard
	if (UartHandle->Instance == USART1)
	{
		uart_putchar(&DISCO_UART, &wordss);
		// esc condition
		if ((int)wordss == 27)
		{
			printf("\033[12;8HProgram halted.\r");
			fflush(stdout);
			while(1);
		}
		printf("\033[4;2HThe word from keyboard is %c .\r", wordss);
		fflush(stdout);
		// go into loops
		HAL_UART_Receive_IT(&USB_UART0, (uint8_t *)wordss, 1);
	}
	// if it is from other devices
	else if (UartHandle->Instance == USART6)
	{
		if ((int)wordsr == 27)
		{
			printf("\033[12;8HProgram halted.\r");
			fflush(stdout);
			while(1);
		}
		printf("\033[18;2HThe word from far away is %c .\r", wordsr);
		fflush(stdout);
		// go into loops
		HAL_UART_Receive_IT(&DISCO_UART, (uint8_t *)wordsr, 1);
	}
	//*/
}




