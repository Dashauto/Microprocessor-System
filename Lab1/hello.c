//------------------------------------------------------------------------------------
// Hello.c
//------------------------------------------------------------------------------------
//
// Test program to demonstrate serial port I/O.  This program writes a message on
// the console using the printf() function, and reads characters using the getchar()
// function.  An ANSI escape sequence is used to clear the screen if a '2' is typed.
// A '1' repeats the message and the program responds to other input characters with
// an appropriate message.
//
// Any valid keystroke turns on the green LED on the board; invalid entries turn it off
//

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "hello.h"

#include <stdint.h>

#include "stm32f769xx.h"

void Task1();
void Task2();
void Task3_R();
void Task3_HAL();
void Task4();

char choice;
//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)

{
    Sys_Init();               // This always goes at the top of main (defined in init.c)
    printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout);           // Need to flush stdout after using printf that doesn't end in \n
    //printf("\033[1;30HPlease choose the task\r\n");

    while (1)
    {
        /*choose task, 1 is for Task 1, 2 is for Task 2, r is for Task 3 build with Register,
        h is for Task 3 build with HAL driver, other input would go to Task 4*/
    	Task4();
    	break;
    }
}

/* Task 1 is a program that outputs character is pressed on the terminal keyboard. It halts when <ESC> or ^[ is pressed.
 */
void Task1()
{
    printf("\033[2;32HKeyboard Checker\r\n");
    printf("\033[3;25HEscape by \035ESC key or ^[ is Pressed\r\n");
    char input;
    while (1)
    {
        input = getchar();
        if ((int)input == 27) // <ESC> or ^[ is pressed
        {
            break;
        }
        printf("The keyboard character is %c\r\n", input);
    }
}

/* Task 2 is a program that outputs character is pressed on the terminal keyboard. When non-printable characters are pressed,
the program would beep and show a warning message. It halts when <ESC> or ^[ is pressed.
Note: The terminal is assumed 80x24.
*/
void Task2()
{
    printf("\033[2J\033[;H\033[7h\033[0m"); // Erase screen & move cursor to home position
    fflush(stdout);                         // Need to flush stdout after using printf that doesn't end in \n
    printf("\033[33;44m\n");
    for (int i = 0; i < 80 * 24; i++) // print the terminal to blue
    {
        printf(" ");
    }
    fflush(stdout);
    printf("\033[H");
    printf("\033[2;25HEscape by \035ESC key or ^[ is Pressed\r\n");
    int first = 1;
    fflush(stdout);
    char input, correct_input;
    int correct = 0;
    int count = 0;
    while (1)
    {
        input = getchar();
        if ((int)input == 27) // <ESC> or ^[ is pressed
        { 
            break;
        }
        else if ((int)input < 32 || (int)input > 126) // non-printable characters are pressed
        {
            count += 1;
            if (count > 13) //all previous messages would be moved up, with the oldest message being removed
            {
                //print the new warning message
                printf("\033D\033[24;0H\033[33;44;5mThe keyboard character is $%X \033[4m\033[44;5m\'not printable\'\033[0m\033[44;5;33m. count is %d.\a", input, count);
                printf("\033[0m");
                printf("\033[33;44m");
                printf("\033[H");
                fflush(stdout);
                //scrolling procedure, move line 1 to 2, line 5 to 6, erase oldest warning message(line 12)
                for (int i = 0; i < 80; i++)
                {
                    printf(" "); // erase line 1
                }
                printf("\033[2;25HEscape by \035ESC key or ^[ is Pressed\r\n");
                for (int i = 0; i < 80 * 9; i++)
                {
                    printf(" "); // erase line 5 and 12
                }
                if (correct) // if there is a correct input before
                {
                    printf("\033[6;0H\033[33mThe keyboard character is \033[31m%c\r\n", correct_input); // reprint the correct message shown before
                }
                printf("\033[23;0H\n");
                fflush(stdout);
            }
            else if (count == 13)
            {
                printf("\033[33;44;5mThe keyboard character is $%X \033[4m\033[44;5m\'not printable\'\033[0m\033[44;5;33m. count is %d.\r\a", input, count);
            }
            else
            {
                if (first)
                {
                    printf("[\033[12;0H"); 
                    printf("\033[0m");
                    printf("\033[33;44m");
                    fflush(stdout);
                }
                //print the new warning message
                printf("\033[44;5mThe keyboard character is $%X \033[4m\033[44;5m\'not printable\'\033[0m\033[44;5;33m. count is %d.\r\a\n", input, count);
                first = 0;
                fflush(stdout);
            }
        }
        else
        {
            // print correct input message
            printf("\033[6;0H\033[33mThe keyboard character is \033[31m%c\r\n", input);
            correct_input = input;
            correct = 1;
        }
        fflush(stdout);
    }
}

/* Task 3 is a program that reads input from D0-D3, and show it on the LED1-LED4. Program is written in both
register and HAL driver.
*/
// LD1: J13   LD2: J5   LD3: A12   LD4: D4
// D0: C7   D1: C6   D2: J1   D3: F6
void Task3_R()
{
    //open GPIO clock in Port J, A, D, C, F
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;

    GPIOJ->MODER |= 0x04000400U;        //initalize GPIO J Pin 13 and Pin 5 to output
    GPIOA->MODER |= 0x01000000U;        //initalize GPIO A Pin 12 to output
    GPIOD->MODER |= 0x00000100U;        //initalize GPIO D Pin 4 to output


    uint32_t wave1, wave2, wave3;
    GPIOC->PUPDR = 0x00005000U;     //initalize GPIO C Pin 7 and Pin 6 with pull-up resistors
    GPIOJ->PUPDR = 4U;              //initalize GPIO J Pin 1 with pull-up resistors
    GPIOF->PUPDR = 0x00001000U;     //initalize GPIO F Pin 6 with pull-up resistors
    while (1)
    {
        // Read signals from D0-D3's ports
        wave1 = GPIOC->IDR;
        wave2 = GPIOJ->IDR;
        wave3 = GPIOF->IDR;

        if ((wave1 >> 7) % 2) // Read D0
        {

            GPIOJ->BSRR |= (uint16_t)GPIO_PIN_13; // Turn on LED1
        }
        else
        {
            GPIOJ->BSRR |= ~(uint16_t)GPIO_PIN_13; // Turn off LED1
        }
        if ((wave1 >> 6) % 2) // Read D1
        {

            GPIOJ->BSRR |= (uint16_t)GPIO_PIN_5; // Turn on LED2
        }
        else
        {
            GPIOJ->BSRR |= ~(uint16_t)GPIO_PIN_5; // Turn off LED2
        }
        if ((wave2 >> 1) % 2) // Read D2
        {
            GPIOA->BSRR |= (uint16_t)GPIO_PIN_12; // Turn on LED3
        }
        else
        {
            GPIOA->BSRR |= ~(uint16_t)GPIO_PIN_12; // Turn off LED3
        }
        if ((wave3 >> 6) % 2) // Read D3
        {
            GPIOD->BSRR |= ~(uint16_t)GPIO_PIN_4; // Turn on LED4
        }
        else
        {
            GPIOD->BSRR |= (uint16_t)GPIO_PIN_4; // Turn off LED4
        }
    }
}

void Task3_HAL()
{
    // Need to enable clock for peripheral bus on GPIO Port
    __HAL_RCC_GPIOA_CLK_ENABLE(); // Through HAL on GPIO Port A
    __HAL_RCC_GPIOC_CLK_ENABLE(); // Through HAL on GPIO Port C
    __HAL_RCC_GPIOD_CLK_ENABLE(); // Through HAL on GPIO Port D
    __HAL_RCC_GPIOJ_CLK_ENABLE(); // Through HAL on GPIO Port J

    //initialize GPIO A Pin 12(LED3) to be an outout
    GPIO_InitTypeDef pin_init_A_12; // LD3
    pin_init_A_12.Pin = GPIO_PIN_12;
    pin_init_A_12.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &pin_init_A_12);

    //initialize GPIO D Pin 4(LED4) to be an output
    GPIO_InitTypeDef pin_init_D_4; // LD4
    pin_init_D_4.Pin = GPIO_PIN_4;
    pin_init_D_4.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOD, &pin_init_D_4);

    //initialize GPIO J Pin 5(LED2) to be an output
    GPIO_InitTypeDef pin_init_J_5; // LD2
    pin_init_J_5.Pin = GPIO_PIN_5;
    pin_init_J_5.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOJ, &pin_init_J_5);

    //initialize GPIO J Pin 1(LED1) to be an output
    GPIO_InitTypeDef pin_init_J_13; // LD1
    pin_init_J_13.Pin = GPIO_PIN_13;
    pin_init_J_13.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOJ, &pin_init_J_13);

    //initialize GPIO C Pin 7(D0) to be an input
    GPIO_InitTypeDef pin_init_C_in_7; // D0
    pin_init_C_in_7.Pin = GPIO_PIN_7;
    pin_init_C_in_7.Mode = GPIO_MODE_INPUT;
    pin_init_C_in_7.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &pin_init_C_in_7);

    //initialize GPIO C Pin 6(D1) to be an input
    GPIO_InitTypeDef pin_init_C_in_6; // D1
    pin_init_C_in_6.Pin = GPIO_PIN_6;
    pin_init_C_in_6.Mode = GPIO_MODE_INPUT;
    pin_init_C_in_6.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &pin_init_C_in_6);

    //initialize GPIO J Pin 1(D2) to be an input
    GPIO_InitTypeDef pin_init_J_in_1; // D2
    pin_init_J_in_1.Pin = GPIO_PIN_1;
    pin_init_J_in_1.Mode = GPIO_MODE_INPUT;
    pin_init_J_in_1.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOJ, &pin_init_J_in_1);

    //initialize GPIO F Pin 6(D3) to be an input
    GPIO_InitTypeDef pin_init_F_in_6; // D3
    pin_init_F_in_6.Pin = GPIO_PIN_6;
    pin_init_F_in_6.Mode = GPIO_MODE_INPUT;
    pin_init_F_in_6.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &pin_init_F_in_6);

    HAL_Delay(1000);

    while (1)
    {
        int state_0 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7); //read GPIO C Pin 0(D0)
        int state_1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6); //read GPIO C Pin 6(D1)
        int state_2 = HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_1); //read GPIO J Pin 1(D2)
        int state_3 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6); //read GPIO F Pin 6(D3)

        if (state_0)
        {
            HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET); // Turn on LED1
        }
        else
        {
            HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET); // Turn off LED1
        }

        if (state_1)
        {
            HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_SET); // Turn on LED2
        }
        else
        {
            HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_RESET); // Turn off LED2
        }

        if (state_2)
        {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); // Turn on LED3
        }
        else
        {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); // Turn on LED3
        }

        if (state_3)
        {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET); // Turn on LED4
        }
        else
        {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET); // Turn on LED4
        }

        fflush(stdout);
    }
}

/*
This program is to generate a pre-set maze to let user play it. In this maze, starting position is in the left,
ending is in the right, all 4 sides are surrounded with walls. WASD is used to move the player. When the player
reaches ending, LED1 will light up to indicate finish. User pushbutton can be used to reset the maze, please
press it while input a movement. The shape of the maze is followed.

    ███████████
    █     █   █
    █ ██  ██
    █ █ █    ██
    █   █ ██  █
    ███ █   █
    █ █  ██ █ █
    █ █   █ █ █
    █   █   █ █
    ███████████
*/
void Task4()
{
    // Need to enable clock for peripheral bus on GPIO Port
    __HAL_RCC_GPIOJ_CLK_ENABLE(); // Through HAL on GPIO Port J
    __HAL_RCC_GPIOA_CLK_ENABLE(); // Through HAL on GPIO Port A

    //initialize GPIO A Pin 0(pushbutton) to be an input
    GPIO_InitTypeDef pin_init_A_in_0; // D0
    pin_init_A_in_0.Pin = GPIO_PIN_0;
    pin_init_A_in_0.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOA, &pin_init_A_in_0);

    //initialize GPIO J Pin 13(LED1) to be an output
    GPIO_InitTypeDef pin_init_J_13; // LD1
    pin_init_J_13.Pin = GPIO_PIN_13;
    pin_init_J_13.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOJ, &pin_init_J_13);

    int reset = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

    //build-up the maze
    uint8_t maze[10][11];
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            maze[i][j] = 0;
        }
    }
    maze[1][1] = 1;
    maze[1][2] = 1;
    maze[1][3] = 1;
    maze[1][4] = 1;
    maze[1][5] = 1;
    maze[1][7] = 1;
    maze[1][8] = 1;
    maze[1][9] = 1;
    maze[2][1] = 1;
    maze[2][4] = 1;
    maze[2][5] = 1;
    maze[2][8] = 1;
    maze[2][9] = 1;
    maze[2][10] = 1;
    maze[3][1] = 1;
    maze[3][3] = 1;
    maze[3][5] = 1;
    maze[3][6] = 1;
    maze[3][7] = 1;
    maze[3][8] = 1;
    maze[4][1] = 1;
    maze[4][2] = 1;
    maze[4][3] = 1;
    maze[4][5] = 1;
    maze[4][8] = 1;
    maze[4][9] = 1;
    maze[5][0] = 1;
    maze[5][1] = 1;
    maze[5][5] = 1;
    maze[5][7] = 1;
    maze[5][8] = 1;
    maze[5][9] = 1;
    maze[6][1] = 1;
    maze[6][3] = 1;
    maze[6][4] = 1;
    maze[6][7] = 1;
    maze[6][9] = 1;
    maze[7][1] = 1;
    maze[7][3] = 1;
    maze[7][4] = 1;
    maze[7][5] = 1;
    maze[7][7] = 1;
    maze[7][9] = 1;
    maze[8][1] = 1;
    maze[8][2] = 1;
    maze[8][3] = 1;
    maze[8][5] = 1;
    maze[8][6] = 1;
    maze[8][7] = 1;
    maze[8][9] = 1;
    printf("      use WASD to play the Maze game!\n\r");
    char input;
    char last_input;
    int row = 5;
    int col = 0;
    for (int i = 0; i < 10; i++) //print maze
    {
        for (int j = 0; j < 11; j++)
        {
            if (maze[i][j] == 0)
            {
                printf("█");
                //█ ★
            }
            else
            {
                printf(" ");
            }
        }
        printf("\r\n");
    }
    printf("\033[7;1H\033[31;43m★\033[D\033[s"); //print user
    printf("\033[0m");
    fflush(stdout);

    //starts playing
    while (1)
    {
        if (row == 2 && col == 10) // player reaches ending
        {
            HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET); // LED1 lights up to indicate finish
        }
        else
        {
            HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);
        }
        input = getchar();
        if (input == 'w') // user goes up
        {
            if (maze[row - 1][col] == 1)
            {
                row -= 1;

                printf("\033[u \033[u");
                printf("\033[A\033[31;43m★\033[D");
                printf("\033[0m");
                printf("\033[s\033[H");
                fflush(stdout);
            }
        }
        else if (input == 'a') // user goes left
        {
            if (maze[row][col - 1] == 1)
            {
                col -= 1;

                printf("\033[u \033[u");
                printf("\033[D\033[31;43m★\033[D");
                printf("\033[0m");
                printf("\033[s\033[H");
                fflush(stdout);
            }
        }
        else if (input == 's') // user goes down
        {
            if (maze[row + 1][col] == 1)
            {
                row += 1;

                printf("\033[u \033[u");
                printf("\033[B\033[31;43m★\033[D");
                printf("\033[0m");
                printf("\033[s\033[H");
                fflush(stdout);
            }
        }
        else if (input == 'd') // user goes right
        {
            if (maze[row][col + 1] == 1)
            {
                col += 1;

                printf("\033[u \033[u");
                printf("\033[C\033[31;43m★\033[D");
                printf("\033[0m");
                printf("\033[s\033[H");
                fflush(stdout);
            }
        }
        else
        {
            printf("Invalid input, Try again.\n\r");
        }

        reset = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
        // If user pushbutton is pressed, reprint user to starting location
        if (reset) 
        {
            printf("\033[u ");
            printf("\033[7;1H\033[31;43m★\033[D\033[s");
            printf("\033[0m");
            fflush(stdout);
            int row = 5;
            int col = 0;
        }
    }
}

//------------------------------------------------------------------------------------
// Extra thing to consider...
//------------------------------------------------------------------------------------
void serial_print_things(void)
{
    // Input Buffer
    char input[2];
    input[0] = 0;
    input[1] = 0;

    // Initialize the system
    Sys_Init();
    initUart(&Second_UART, 9600, USART6); // Allow printing over USART6 (Arduino pins D0 - Rx and D1 - TX)
    uart_print(&USB_UART, "\033[2J\033[;H");
    uart_print(&Second_UART, "\033[2J\033[;H");
    uart_print(&USB_UART, "Hello World: This is the USB Serial Connection\r\n");
    uart_print(&Second_UART, "Hello World: This is the other UART Connection\r\n");
    uart_print(&USB_UART, "In order to send newlines to this terminal,\r\n");
    uart_print(&USB_UART, "Press <ESC> and type \"[20h\" (without quotes)\r\n");
    uart_print(&USB_UART, "To get out of newline mode and back to line feed mode,\r\n");
    uart_print(&USB_UART, "Press <ESC> and type \"[20l\" (without quotes)\r\n");
    uart_print(&USB_UART, "\"Thanks for Playing!\"\r\n");

    printf("THIS SENTENCE USES PRINTF!!!\r\n");
    // Don't forget to end printf with newline or run fflush(stdout) after it!

    while (1)
    {
        input[0] = uart_getchar(&USB_UART, 0);
        uart_print(&USB_UART, input);
        uart_print(&Second_UART, input);
    }

    while (1)
        ; // HALT AND CATCH FIRE
}
