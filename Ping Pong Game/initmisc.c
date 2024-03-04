#include <pic32mx.h>
#include <stdint.h>
#include <stdio.h>
#include "Defines.h"

// Global variables - Need to be defined in a non-header file
int score1 = 0;			// Player1 Score
int score2 = 0;			// Player2 Score
int lastScored = 1;		// Which Player last scored
int gameMode = 2;		// Selected Game Mode - 2 or 1 Player
int gameOverFlag = 0;	// Forces Game Over if set to 1 
Obj Ball;				// Pong Ball
Obj Paddle1;			// Left Pong Paddle
Obj Paddle2;			// Right Pong Paddle

/*
	Set Switches and Buttons to inputs
	Return 4 bits with states of BTN4:1

	Imported from lab - Written by Dana & Aron
*/
int getbtns(void)
{   
    TRISDSET = 0xfe0;     // set bits 11:5 to 1 in value of TRISD - SW4:1, BTN4:2
 	TRISFSET = 0x2;       // set bit 1 of TRISF - BTN1
	return ((PORTD >> 4 & 0xe)|((PORTF >> 1) & 0x1));	// return 4 bits with states of BTN4:1
}

/*
	Create a small delay by occupying the processor with nonsense calculations - length determined by input
	---Provided by Course Material---
*/ 
void delay(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/*
	Board initialization - consult manual
	---Provided by Course Material---
*/
void board_init(void){
	/* Some of these may be unnecessary for our purposes,
	but let's just leave them in. */

	/* Set up peripheral bus clock */
	OSCCON &= ~0x180000;
	OSCCON |= 0x080000;
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	
	/* Clear SPIROV*/
	SPI2STATCLR &= ~0x40;
	/* Set CKP = 1, MSTEN = 1; */
    SPI2CON |= 0x60;
	
	/* Turn on SPI */
	SPI2CONSET = 0x8000;
}

/*
	Display initialization - consult I/O Shield manual
	---Provided by Course Material---
*/
void display_init() {
	DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
	delay(10);
	DISPLAY_VDD_PORT &= ~DISPLAY_VDD_MASK;
	delay(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_RESET_PORT &= ~DISPLAY_RESET_MASK;
	delay(10);
	DISPLAY_RESET_PORT |= DISPLAY_RESET_MASK;
	delay(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_VBATT_PORT &= ~DISPLAY_VBATT_MASK;
	delay(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}
