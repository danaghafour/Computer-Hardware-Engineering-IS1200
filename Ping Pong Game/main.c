#include <pic32mx.h>
#include <stdint.h>
#include <stdio.h>
#include "Defines.h"

/*
	Menu - Select a GameMode with BTN2 and BTN3, start Game with BTN4
	Calls clearDisplayBuffer, displayString, displayUpdate, delay, getbtns, gameStart, gameLoop

	Written by Dana & Aron
*/
void menu(void)
{
	clearDisplayBuffer();					// Clear Buffer
	while(1){								// Loop forever until broken
    if(gameMode == 2){
	displayString(0, "      Pong      "); 
	displayString(1, "> Two Player    ");  
    displayString(2, "  One Player    "); 
	}
	if(gameMode == 1){
	displayString(0, "      Pong      ");
    displayString(1, "  Two Player    ");  
    displayString(2, "> One Player    ");
	}
	displayUpdate();				// Update Display
	delay(MESSAGE_TIME / 100);		// Delay between button checks. No delay may cause instability or flickering.
	int btns = getbtns();
	if (btns)   // Any button being pressed will save a non-0 int in btn, only 0 acts as a false
  		{
    	if ((btns & 0x2) && (gameMode > 1)) { // BTN2 is pressed, not at Bottom Option
		gameMode--;
    	} 
     	if ((btns & 0x4) && (gameMode < 2)) { // BTN3 is pressed, not at Top Option
		gameMode++;
    	}
		if ((btns & 0x8)) { // BTN4 is pressed
		break;	// Break menu loop
    	} 
		
		}
	}
	gameStart();	// Game setup - set values for Ball and Paddles
	gameLoop();		// Game loop. Will continue until gameOverFlag == 1.
}

/*
	Main - Calls Menu forever

	Written by Dana & Aron
*/
int main(void) {

	/*	Init functions were already present in lab and template 
	Consult manual when changing */
	
	/*	Board initialization - do not remove */
	board_init(); 
	
	/*	Display initialization - do not remove */
	display_init(); 

	// Call Menu
	while(1){menu();}

	// Do nothing
	for(;;) ;
	return 0;
}

