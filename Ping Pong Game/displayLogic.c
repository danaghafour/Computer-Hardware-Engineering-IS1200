#include <pic32mx.h>
#include <stdint.h>
#include <stdio.h>
#include "Defines.h"

/* 
	Write a single data byte to display via SPI - consult manual
	---Provided by Course Material---
*/
uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 0x01));
	return SPI2BUF;
}

// uint8_t is an unsigned 8 bit/1 byte integer format - used for the display where each bit controls 1 pixel
/* Buffer made of a 2d byte array split into columns and pages. 
 Used for storing the next state of every pixel of the display.
 Call displayUpdate to write displayBuffer contents to display. */
uint8_t displayBuffer[128][4]; 

/**
 * displayBuffer[x][page_index] bestämmer vilket byte i displayBuffer vi ändrar på.
|= 0x1 << bit_index sätter en 1:a i byte:n på bit-nummer bit_index (bitvis OR med 00000001 vänsterskiftat med 0-7)
*/

/*
	Sets a single pixel in displayBuffer at coordinate (x, y) to 1.
	Uses displayBuffer

	Written by Aron
*/
void setPixel(uint8_t x, uint8_t y)     // Call every time we set a pixel in the displayBuffer
{
	if(y<0 || y>31 || x<0 || x>127){ return;}   // checks that the designated coordinate is on the display
	y = 31 - y;									// invert y-axis - y = 0 so that y starts at bottom of screen instead of top
    uint8_t bit_index = y % 8;                  // y modulo byte-size to get bit index within vertically aligned bytes 
    uint8_t page_index = (y - bit_index)/8;     // integer division without remainder: y / byte-size = page number
    displayBuffer[x][page_index] |= 0x1 << bit_index; // Set a 1 (pixel on) at bit-index in byte x
}

/**
	displayBuffer[x][page_index] bestämmer vilket byte i displayBuffer vi ändrar på.
	&= !(0x1 << bit_index) sätter en 0:a i byte:n på bit-nummer bit_index (bitvis AND med invers(00000001 vänsterskiftat med 0-7) - t.ex. 11111110)
*/

/*
	Sets a single pixel in displayBuffer at coordinate (x, y) to 0 (Clear)
	Uses displayBuffer

	Written by Aron
*/
void clearPixel(uint8_t x, uint8_t y)     // Call every time we clear a pixel in the displayBuffer
{
	if(y<0 || y>31 || x<0 || x>127){ return;}   // checks that the designated coordinate is on the display
	y = 31 - y;									// invert y-axis - y = 0 at bottom of screen instead of top
    uint8_t bit_index = y % 8;                  // y modulo byte-size to get bit index within vertically aligned bytes 
    uint8_t page_index = (y - bit_index)/8;     // integer division without remainder of y by byte-size to get page number
    displayBuffer[x][page_index] &= ~(0x1 << bit_index); // Set a 0 (pixel off) at bit-index in byte x
}

/* 	Call every time we want to update the screen with displayBuffer contents 
 	The only function used to update screen for consistency reasons

	Display:en är uppdelad i bytes och pages osv., och när vi uppdaterar displayen behöver vi ta hänsyn till det.
	Men att konstant ta hänsyn till hur Display:en är uppdelad varje gång vi gör något display-relaterat är ointuitivt och irriterande.
	Vi ändrar hellre på skärmen på ett sätt som vi förstår, x-y-koordinater, och låter setPixel/clearPixel konvertera mellan formaten åt oss.
  	current_v_byte är bara namnet på den byte vi skriver till skärmen från displayBuffer.
  	write-pointer var bara en benämning på vart i skärmen SPI håller på att skriva, som Program Counter eller Stack Pointer.
*/

/*
	Updates I/O Display with contents of displayBuffer (Needs to be called every time a visible change on the Display is desired)
	Calls spi_send_recv()
	Uses displayBuffer

	Written by Aron, SPI-section from lab
*/
void displayUpdate(void)	// Update display to show contents of displayBuffer
{
    uint8_t page;               
    uint8_t column;
    uint8_t current_v_byte;

    for (page = 0; page < 4; page++)    // Loop through pages
    {
	    {  // SPI-subroutine - changes display page to 'page' variable and sets SPI write-pointer at the left end of the screen
            DISPLAY_CHANGE_TO_COMMAND_MODE;
		    spi_send_recv(0x22);
		    spi_send_recv(page);
		
		    spi_send_recv(0x0);
		    spi_send_recv(0x10);
		    DISPLAY_CHANGE_TO_DATA_MODE; 
        }
        for(column = 0; column < 128; column++)			// Loop through columns (x-coordinate)
			{
		    current_v_byte = displayBuffer[column][page]; 	// Set current byte in displayBuffer as next byte to be written to display
            spi_send_recv(current_v_byte);               	// Write selected byte to the display and move SPI write-pointer right
			}
    }  
}

/* 
	Clear displayBuffer by setting all of its bytes to 0. 
	displayBuffer is called before displayUpdate to clear display.
	This uses displayBuffer.

	Written by Dana
*/
void clearDisplayBuffer(void)
{
    uint8_t page;               
    uint8_t column;
	// Loop through each byte in buffer, set to 0
    for (page = 0; page < 4; page++)    			// Loop through pages
    	{
        for(column = 0; column < 128; column++)		// Loop through columns
        	{
    	   	displayBuffer[column][page] = 0x00;		// Set selected byte to 0
        	}
    	}  
}

/*
	In displayBuffer at page specified as parameter, write the pixel representation 
	of a 16-char string specified as parameter, starting at address 'str' 
	(128x8 pixel page fits 16 character-pixel-representations of 8x8 pixels each) 
	Uses displayBuffer

	Written by Aron, some of it's reused from the lab display functions
*/
void displayString(int page, char* str) 
{
	/* For example, if a call displayString(1, "A") is made, the following section will fetch the 8x8 pixel
	representation of A from textfont.c and write it to the left end of page 1 in displayBuffer. 
	To then actually show the A on the display, a call displayUpdate(); will need to be made.  */
	int c_index, byte_index, c_int;

	// Write strings to displayBuffer - partially recycled from display_string/display_update function from labs
	if(page < 0 || page >= 4){return;}	// If the specified page to write to is out of range, do nothing	
	if(!str){return;}					// If no string received, do nothing
	
	for(c_index = 0; c_index < 16; c_index++) // loop through char-array/string
	{
		if(*str) {					// Check if there's a character in current string pointer position
		c_int = *str;				// Set int c_int to current characters ASCII's value 
		str++;	}					// Increase string pointer - go to next char in String
		else {c_int = ' ';}			// If no character at pointer position - set c_int to ASCII for empty space
		if(c_int & 0x80){continue;}	// Check that ASCII char is in a certain range???
			for(byte_index = 0; byte_index < 8; byte_index++)	// loop through 8 bytes per char font 
			{displayBuffer[c_index*8 + byte_index][page] = (font[c_int*8 + byte_index]);}	
			// Set each byte in displayBuffer to pixel representation of every character in string
			// The pixel representation of each character is 8 bytes
	}
}

/*
	Writes a rectangle along Border. Unused, but saved for debugging.
*/
/*
void writeBorder(void)
{
	uint8_t x;
	uint8_t y;
	for(x = BORDER_LEFT; x<=BORDER_RIGHT; x++)
	{
		y = BORDER_BOTTOM;
		setPixel(x, y);
		y = BORDER_TOP;
		setPixel(x, y);
	}
	
	for(y = BORDER_BOTTOM; y<=BORDER_TOP; y++)
	{
		x = BORDER_LEFT;
		setPixel(x, y);
		x = BORDER_RIGHT;
		setPixel(x, y);
	}
}
*/