//----------------------------------
// Lab 3 - Part 4: SPI - Lab03_spi.c
//----------------------------------
//

#include "init.h"
#include <stdio.h>
#include <stdlib.h>
#include<string.h>

void printchar();
void firmwareversion();
void temperature();
void clearterminal();
void changeID();
void exitfunction();
char key_getchar();

/*
 * For convenience, configure the SPI handler here
 */
char wordsr;
char input[1];
char output[1];
uint8_t tasknumber;
uint8_t versionhigh, versionlow;
uint8_t temphigh, templow;
int tempraw;
double temp;
uint8_t deviceID;
uint8_t exits;
int ii;

GPIO_InitTypeDef GPIO_InitStructure_A, GPIO_InitStructure_B;
SPI_HandleTypeDef SPI_2;
// See 769 Description of HAL drivers.pdf, Ch. 58.1 or stm32f7xx_hal_spi.c
void configureSPI() {
    SPI_2.Instance = SPI2;                   // Please use SPI2!
    SPI_2.Init.Mode = SPI_MODE_MASTER;       // Set master mode
    SPI_2.Init.TIMode = SPI_TIMODE_DISABLE;  // Use Motorola mode, not TI mode
    SPI_2.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_2.Init.CLKPolarity = SPI_POLARITY_LOW;
	SPI_2.Init.CLKPhase = SPI_PHASE_2EDGE;
	SPI_2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI_2.Init.NSS = SPI_NSS_HARD_OUTPUT;
	SPI_2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	SPI_2.Init.Direction = SPI_DIRECTION_2LINES;
	SPI_2.Init.FirstBit = SPI_FIRSTBIT_MSB;

    HAL_SPI_Init(&SPI_2);

}

/*
 * This is called upon SPI initialization. It handles the configuration
 * of the GPIO pins for SPI.
 */
// Do NOT change the name of an MspInit function; it needs to override a
// __weak function of the same name. It does not need a prototype in the header.
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
	__GPIOA_CLK_ENABLE();
	        __GPIOB_CLK_ENABLE();

	GPIO_InitStructure_A.Pin = GPIO_PIN_12; //SPI CLK
	GPIO_InitStructure_A.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure_A.Pull = GPIO_PULLUP;
	GPIO_InitStructure_A.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure_A.Alternate = GPIO_AF5_SPI2;
    // SPI GPIO initialization structure here
    GPIO_InitStructure_B.Pin = GPIO_PIN_14 | GPIO_PIN_15; // MISO and MOSI
    GPIO_InitStructure_B.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure_B.Pull = GPIO_PULLUP;
    GPIO_InitStructure_B.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure_B.Alternate = GPIO_AF5_SPI2;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure_A);
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure_B);

    GPIO_InitStructure_A.Pin = GPIO_PIN_11; // Chip Select
    GPIO_InitStructure_A.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure_A);
    if (hspi->Instance == SPI2) {
        __SPI2_CLK_ENABLE();
        HAL_NVIC_EnableIRQ(SPI2_IRQn);
    }
}

int main(void) {
    Sys_Init();

    // For convenience
    configureSPI();
	while (1)
	{
	    printf("\033[2J\033[;H\033[7h\033[0m"); // Erase screen & move cursor to home position
	    fflush(stdout);                         // Need to flush stdout after using printf that doesn't end in \n
		printf("choose task\r\n");
		printf("2. Receive terminal characters from the peripheral device.\r\n");
		printf("3. Read the peripheral’s firmware version.\r\n");
		printf("4. Trigger a temperature measurement.\r\n");
		printf("5. Clear or reset the peripheral terminal.\r\n");
		printf("6. Change and read the device ID of the peripheral.\r\n");
		printf("Press <ESC> to reselect funtions.\r\n");
		tasknumber = getchar();
		printf("\033[2J\033[;H\033[7h\033[0m"); // Erase screen & move cursor to home position
		fflush(stdout);                         // Need to flush stdout after using printf that doesn't end in \n
		if (tasknumber == 0x32){
			printchar();
		}
		if (tasknumber == 0x33){
			firmwareversion();
		}
		if(tasknumber == 0x34){
			temperature();
		}
		if(tasknumber == 0x35){
			clearterminal();
		}
		if(tasknumber == 0x36){
			changeID();
		}
	}
}


void printchar() {
	while(1){
	    char words = key_getchar();
	    if ((int)words == 27) // when ESC is pressed, go to menu
	    {
	        break;
	    }
		// read STS_REG #1
	    int b = 1;
		HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
		for(ii = 0; ii < 160; ii++){};
		HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&b, (uint8_t *)output, 1, 1000);
		for(ii = 0; ii < 160; ii++){};
		HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&b, (uint8_t *)output, 1, 1000);
		for(ii = 0; ii < 160; ii++){};
		HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high
		for(ii = 0; ii < 160; ii++){};
		uint8_t check;
		check  = output[0];
		// NCHBF1 or NCHBF0 is high
		if(((check & 0b01100000) == 0b01100000)
			|| ((check & 0b01000000) == 0b01000000)
			|| ((check & 0b00100000) == 0b00100000)){
			// read CH_BUG #5
			HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
			for(ii = 0; ii < 160; ii++){};
			int c = 5;
			HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&c, (uint8_t *)output, 1, 1000);
			for(ii = 0; ii < 160; ii++){};
			int d = 0;
			HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&d, (uint8_t *)output, 1, 1000);
			for(ii = 0; ii < 160; ii++){};
			HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high
			for(ii = 0; ii < 160; ii++){};
			wordsr = output[0];
		}
		// print out character
		if ((int)wordsr > 32 && (int)wordsr < 126){
			printf("From SPI: The word is %c .\r\n", wordsr);
			wordsr = 0;
		}
	}
}

void firmwareversion(){
	// read V_MIN #8
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
	for(ii = 0; ii < 160; ii++){};
	int a = 8;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&a, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	int b = 7;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&b, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high
	for(ii = 0; ii < 160; ii++){};
	versionlow = output[0];

	// read V_MAJ #7
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
	for(ii = 0; ii < 160; ii++){};
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&b, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&a, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high
	versionhigh  = output[0];
	printf("firmware: %d.%d\r\n",versionhigh, versionlow);
	exitfunction();
}

void temperature(){
	// Write RDTMP in CTL_REG high
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
	for(ii = 0; ii < 160; ii++){};
	int a = 0;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&a, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	int b = 2;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&b, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high
	for(ii = 0; ii < 160; ii++){};


	b = 1;
	while(1){
		// Read STS_REG (Reg #1)
		HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
		for(ii = 0; ii < 160; ii++){};
		HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&b, (uint8_t *)output, 1, 1000);
		for(ii = 0; ii < 160; ii++){};
		HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&b, (uint8_t *)output, 1, 1000);
		for(ii = 0; ii < 160; ii++){};
		HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high
		for(ii = 0; ii < 160; ii++){};
		uint8_t check;
		check  = output[0];
		if((check & 0b00001000) == 0b00001000){break;} // TRDY is high
	}
	// Read TMP_LO (Reg #3)
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
	for(ii = 0; ii < 160; ii++){};
	a = 3;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&a, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&a, (uint8_t *)output, 1, 1000);
	templow = output[0];
	for(ii = 0; ii < 160; ii++){};
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high
	for(ii = 0; ii < 160; ii++){};

	// Read TMP_HI (Reg #4)
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
	for(ii = 0; ii < 160; ii++){};
	a = 4;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&a, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&a, (uint8_t *)output, 1, 1000);
	temphigh = output[0];
	for(ii = 0; ii < 160; ii++){};
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high

	tempraw = (temphigh << 8) + templow;
	temp = 	357.6 - 0.187*(double)tempraw;
	printf("temperature high: %d\r\n",temphigh);
	printf("temperature low: %d\r\n",templow);
	printf("temperature in raw: %d\r\n", tempraw);
	printf("temperature in celcius: %lf\r\n", temp);
	exitfunction();
}

void clearterminal(){
	// Write TRMCLR in CTL_REG high
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
	for(ii = 0; ii < 160; ii++){};
	int a = 0;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&a, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	int b = 4;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&b, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high

	printf("Slave terminal is cleared.\r\n");
	exitfunction();
}

void changeID(){
	// Write ULKDID in CTL_REG high
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
	for(ii = 0; ii < 160; ii++){};
	int a = 0;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&a, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	int b = 128;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&b, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high
	for(ii = 0; ii < 160; ii++){};

	// Write 4 into DEVID (Reg# 9)
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
	for(ii = 0; ii < 160; ii++){};
	a = 9;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&a, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	b = 4;
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&b, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high
	for(ii = 0; ii < 160; ii++){};

	// Read DEVID (Reg# 9)
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the pin low
	for(ii = 0; ii < 160; ii++){};
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&a, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	HAL_SPI_TransmitReceive(&SPI_2, (uint8_t *)&b, (uint8_t *)output, 1, 1000);
	for(ii = 0; ii < 160; ii++){};
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the pin high

	deviceID = output[0];
	printf("deviceID: %d\r\n",deviceID);
	exitfunction();
}

void exitfunction(){
	while(1){
		exits = getchar();
		if ((int)exits == 27){
			break;
		}
	}
}

char key_getchar()
{
    char input[1];
    HAL_UART_Receive(&USB_UART, (uint8_t *)input, 1, 1000);
    return (char)input[0];
}
