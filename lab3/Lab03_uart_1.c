//------------------------------------
// Lab 3 - Part 1: UART - Lab03_uart.c
//------------------------------------
//

#include "init.h"
void URAT_send();
void URAT_receive();


char wordsr;
char wordss;
UART_HandleTypeDef DISCO_UART;
UART_HandleTypeDef USB_UART0;
// Main Execution Loop
int main(void) {
	//Initialize the system
	Sys_Init();
	// configure UART
	initUart(&DISCO_UART, 38400, USART6);
	HAL_UART_MspInit(&DISCO_UART);
	initUart(&USB_UART0, 115200, USART1);
	HAL_UART_MspInit(&USB_UART0);

	while(1)
	{
		URAT_send();
		URAT_receive();
	}

}

void URAT_send()
{
	//receive characters from keyboard
	wordss = uart_getchar(&USB_UART0, 1);
	if (wordss != 0)
	{
		//transmit the received charater
		uart_putchar(&DISCO_UART, &wordss);
	}
	// esc condtion
	if ((int)wordss == 27)
	{
		printf("Program halted.\r\n");
		while(1);
	}
	wordss = 0;
}

void URAT_receive()
{
	//receive characters from other devices
	wordsr = uart_getchar(&DISCO_UART, 0);
	printf("The word is %c .\r\n", wordsr);
	// esc condtion
	if ((int)wordsr == 27)
	{
		printf("Program halted.\r\n");
		while(1);
	}
}



