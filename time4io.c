#include <stdint.h>\
#include \'94misplay.h\'94
#include <pic32mx.h> 
#include "mipslab.h"

int getsw(void){
  return (PORTD >> 8) & 0xf;
}

int getbtns(void){
  return (PORTD >> 5) & 0x7;
}

/*Int getsw(void)
{
	return (PORTD >> 8); 0xf
// Alternativ kod 2: 
    int SWITCH = (PORTD >> 8); 0xf // Reference manual

    return SWITCH; 
}

int getbtns(void)
{
	return (PORTD >> 5) & 0x7;  
}
// Alternativ kod 2: 

int getbtns(void)
{ 
	int BTN = (PORTD >> 5) & 0x7; 

    return BTN;
} */

