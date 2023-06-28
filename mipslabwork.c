 
/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

#include <stdio.h>
#include <math.h>
#define PORTE_ADDR ((volatile int*) 0xbf886110)
volatile int* initPORTE = PORTE_ADDR;
#define TRISE_ADDR (volatile int*) 0xbf886100;

char textstring[] = "text, more text, and even more text!";

// Danas: make install TTYDEV=/dev/tty.usbserial-3
// Adalats: make install TTYDEV=/dev/tty.usbserial-5

/* Interrupt Service Routine */
void user_isr( void )
{
  IFSCLR(0) = 0x100; /* 
  För att återställa interruptflaggan för interrupt nummer 0x100, dvs 0001 0000 0000
  indikerar vi att bara interrupt 8 ska återställas. 
  Genom att sätta flaggan till noll kommer inte ISR att köras igen 
  för samma händelse, och CPU:n kommer att fortsätta köra programmet 
  från där det lämnades när interruptet inträffade.*/
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  // Här initierar vi TRISE så att bit 7:0 konfigureras till outputs
  volatile int* initTRISE = TRISE_ADDR
  *initTRISE = ~0b11111111
  // Vi initierar PORTE till 0
  *initPORTE = 0;

  enable_interrupt();
  TRISDSET = 0x0FE0; // The button is enabled for switch input. 
  *trise &= ~0xff; // Clear bits 0-7
  *porte = 0x1;
  TRISD = 0x0fe0; // bit-ORring bits 5 to 11, to input. 
    // TRISD används för att knapparna ska användas.  Vi sätter den sista knappen
    // Dvs, BTN4 som enable. 
    // Vi skriver stora bokstäver för registret, för att datan finns i Macro. 

  // Initializing Timer 2: 
  TMR2 = 0; // reset timer. 
  T2CON = 0; 
  PR2 =  ((80000000/256/10)); 
  //IFS(0) &= ~(0x100 | 0x800); // We are clearing the bits 11 to 7. 
  //IEC(0) = 0x100; 
  T2CONSET = 0x8070; 
  IEC(0) = 0x100;
  IPC(2) = 0X1C;
}
// Behöver vi koden för switches?
/*int main (void) { 
  int switches; 

  TRISD = 0xFF00; */ // We set the RD [7:0] to output and [11:8] to input. 

  //
 /*/ while(1) {      // We read and mask switches from RD[11:8], 
    switches = (PORTD >> 8 ) & 0xF; 
    PORTD = switches; //and display them on the LEDS.
  } */
/* This function is called repetitively from the main program */
  void labwork(void) 
  {
    prime = nextprime (prime); 
    display_string(0, itoaconv (prime));
    display_update(); 

  // change the labwork function so it never calls delay. Instead 
  // - the labwork function should test out the timeout event flag. 
  // If the flag indicates a timeout event => it should reset the flag and call upon 
  //display string, display update, time2string and tick function. Make these functions conditional, so that
  //they are called only incase of this event. 
  // Call getbtns and check which button is pressed
  /*int btns = getbtns();
  int sw = 0; 
  if (btns) {
    sw = getsw();
    if (btns & 0x1) {
      // BTN2 is pressed
      mytime = (mytime & 0xff0f) | (sw << 4);
    } 
     if (btns & 0x2) {
      // BTN3 is pressed
      mytime = (mytime & 0xf0ff) | (sw << 8);
    } 
     if (btns & 0x4) {
      // BTN4 is pressed
      mytime = (mytime & 0x0fff) | (sw << 12); 
     }
  }
   if(IFS(0) & 0x100) 
      {
        timeoutcount++; 
        IFSCLR(0) = 0x100;
      } 
  
  //delay(1000); //Instead of delay function, we implement a condition along with function calls. 
  if (timeoutcount==10) {    
  timeoutcount = 0;  
    static int ticks = 0;
   ticks++; 
   *porte &= ~0xff;
   *porte |= (ticks & 0xff); 
                        // Only call the functions during time-out events. 
  }
  display_image(96, icon); */
    // Clearing the flags to then return to isr
  }
  // Efter att ha bortkommenterat delay funktionen, får vi: 
  // volatile pointer: 
  //int Timer2_(void) {}


