#include "uart.h"

UART_HandleTypeDef USB_UART;

// Initialize Hardware Resources
// Peripheral's clock enable
// Peripheral's GPIO Configuration
void HAL_UART_MspInit(UART_HandleTypeDef *huart){
	GPIO_InitTypeDef  GPIO_InitStruct;

	if (huart->Instance == USART1) {
		// Enable GPIO Clocks
		__GPIOA_CLK_ENABLE();

		// Initialize TX Pin
		GPIO_InitStruct.Pin       = GPIO_PIN_9;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //TX Config

		// Initialize RX Pin
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //RX Config

		// Enable UART Clocking
		__USART1_CLK_ENABLE();

	} else if (huart->Instance == USART6) {
		// Enable GPIO Clocks
		__GPIOC_CLK_ENABLE();

		// Initialize TX Pin
		GPIO_InitStruct.Pin       = GPIO_PIN_6;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); //TX Config

		// Initialize RX Pin
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); //RX Config

		// Enable UART Clocking
		__USART6_CLK_ENABLE();

	}
}

//UART Initialization
void initUart(UART_HandleTypeDef* Uhand, uint32_t Baud, USART_TypeDef* Tgt) {
	Uhand->Instance        = Tgt;

	Uhand->Init.BaudRate   = Baud;
	Uhand->Init.WordLength = UART_WORDLENGTH_8B;
	Uhand->Init.StopBits   = UART_STOPBITS_1;
	Uhand->Init.Parity     = UART_PARITY_NONE;
	Uhand->Init.Mode       = UART_MODE_TX_RX;
	Uhand->Init.HwFlowCtl  = UART_HWCONTROL_NONE;

	HAL_UART_Init(Uhand);
}

/* ============================================================================

Reassigning _write and _read to USB_UART by default.

Note the following for an explanation of syntax:

HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

============================================================================= */


// Make printf(), putchar(), etc. default to work over USB UART
int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&USB_UART, (uint8_t*) ptr, len, 1000);
	return len;
}

// Make scanf(), getchar(), etc. default to work over USB UART
int _read(int file, char *ptr, int len) {
	*ptr = 0x00; // Clear the character buffer because scanf() is finicky
	len = 1; // Again because of scanf's finickiness, len must = 1
	HAL_UART_Receive(&USB_UART, (uint8_t*) ptr, len, HAL_MAX_DELAY);
	return len;
}

/* ============================================================================

Custom UART functions that can be mapped to any UART of your choice when called

Note the following for an explanation of syntax:

HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

============================================================================= */

// Print a string on the specified UART. (Basically a redirectable puts)
void uart_print(UART_HandleTypeDef *huart, char* string) {
    HAL_UART_Transmit(huart, (uint8_t*) string, strlen((const char*)string), 1000);
}

// Get one character
// 'echo' means enable (1) or disable (0) echoing of characters
char uart_getchar(UART_HandleTypeDef *huart, uint8_t echo) {
	char input[1];
	HAL_UART_Receive(huart, (uint8_t *)input, 1, HAL_MAX_DELAY);
	if (echo) HAL_UART_Transmit(huart, (uint8_t*) input, 1, 1000);
	return (char)input[0];
}

// Send one character
void uart_putchar(UART_HandleTypeDef *huart, char * input) {
	HAL_UART_Transmit(huart, (uint8_t*) input, 1, 1000);
}

// Collects characters until size limit or an endline is recieved
// Returns number of characters collected
// 'max size' should match the size of the array or be smaller
int uart_getline(UART_HandleTypeDef *huart, char * input, int max_size)
{
  char single;
  int i = 0;

	while(1)
	{
		if (i > max_size)
      {
				break;
			}

		single = uart_getchar(huart, 1); // Get one character

  	if (single == '\n' || single == '\r') // Unix & co. use \n, Windows uses \r\n
			break;
  	else
			input[i] = single;
  	i++;
	}
  return i; // The way this is set up, it will always report the wrong size
}
