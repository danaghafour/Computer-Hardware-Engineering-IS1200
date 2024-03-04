#include <pic32mx.h>
#include <stdint.h>
#include <stdio.h>

/*
Defines used by Template/Lab Init functions and SPI - Copied to ensure those still work
---Provided by Course Material---
*/
#define DISPLAY_VDD PORTFbits.RF6
#define DISPLAY_VBATT PORTFbits.RF5
#define DISPLAY_COMMAND_DATA PORTFbits.RF4
#define DISPLAY_RESET PORTGbits.RG9
#define DISPLAY_VDD_PORT PORTF
#define DISPLAY_VDD_MASK 0x40
#define DISPLAY_VBATT_PORT PORTF
#define DISPLAY_VBATT_MASK 0x20
#define DISPLAY_COMMAND_DATA_PORT PORTF
#define DISPLAY_COMMAND_DATA_MASK 0x10
#define DISPLAY_RESET_PORT PORTG
#define DISPLAY_RESET_MASK 0x200
#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

/* ---Following is Custom Code--- */

/*
Screen arranged in vertical 1x8 bytes with 1 bit per pixel
Each page is 1 vertical byte high and 128 wide
The screen is 4 pages high and 1 page wide
These constants are not used.
*/
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define PAGE_HEIGHT 8

/**
motivera konstantvalen för allt ifrån VELOCITY_Y till MAX_SCORE
--- Framförallt arbiträrt och en smakfråga
Max_Score - Har bara stöd för att visa poäng med en enda siffra 0-9, så Max_score bör vara 0-9
Velocity_X/Y - Float-värde, helst mellan 0-1. Om Bollen rör sig mer än en pixel i något led per uppdatering 
kommer studsarna se skumma ut eller behöva skrivas på ett mycket mer komplicerat sätt
*/

// Game Object constants
#define VELOCITY_Y 1 		// Vertical starting velocity of Ball - Set between 0 and 1. If above 1, bounces may look odd or bug
#define VELOCITY_X 1 		// Horizontal starting velocity of Ball - Set between 0 and 1. If above 1, bounces may look odd or bug
#define PADDLE_DISTANCE 10	// Distance between edge of Paddle to game Border
#define PADDLE_WIDTH 1		// Width added to Paddle. Paddles will span 1 + PADDLE_WIDTH in x-coordinates
#define PADDLE_HEIGHT 10	// Height added to Paddle. Paddles will span 1 + PADDLE_HEIGHT in y-coordinates
#define BALL_WIDTH 1		// Width/radius added to Ball. Ball will span 1 + BALL_WIDTH in x-coordinates and y-coordinates

// Define game borders to make code easier to understand - allows shrinking game borders for debugging in case ball goes outside display  
#define BORDER_LEFT 1		// Left Border of game. Set to at least 0.
#define BORDER_RIGHT 126	// Right Border of game. Set to at most 127.
#define BORDER_BOTTOM 1		// Bottom Border of game. Set to at least 0.
#define BORDER_TOP 30		// Top Border of game. Set to at most 31.

// General game constants
#define GAME_SPEED 100000		// Length of delay between game updates. Increase to make game slower, decrease to make game faster.
#define MESSAGE_TIME 10000000	// Length of delay during which messages are shown on screen. Should be long enough to read, but short enough to not interrupt the flow of game.
#define MAX_SCORE 9				// Game will exit when score1/score2 exceed MAX_SCORE. Set between 0-9, or incorrect ASCII characters may be shown as score.

/**
Ville göra några globala variabler relaterade till spelet som lätt kan kommas åt av dess funktioner.
Score - Poäng som visas i hörnet. Finns en check i gameLoop tror jag som kollar om Score är >= MAX_SCORE och isåfall avslutar spelet.
LastScored - används i gameStart för att bestämma riktningen bollen ska börja i. 
Lite orättvist om bollen fortsätter köra åt samma håll oavsett vem som senast fick poäng.
GameMode - 1/2-Player-mode. Ändras i Menu. Finns några stycken enkla if-satser i spelfunktionerna som kollar dess värde.
gameOverFlag - En check i gameLoop avbryter genast spelet om den är 1. Sätts till 1 i några fall. Viktigt att återställa till 0. 
*/

/*
	Create custom type/datstructure for Game Objects like Ball and Paddles.
	Its 6 internal variables determine Game Object size and position.
	Game objects are rectangles spanning (min_x, min_y) to (max_x, max_y).
	
	Floats used - But only whole-integer values are actually used by default. Game could technically support complex speeds and bounces, but those are not implemented.
	Floats need to be explicitly casted to uint8_t when drawing Game Objects on Display.
*/
struct gameObject
{
	float min_x;	// leftmost x-coordinate
	float max_x;	// rightmost x-coordinate
	float min_y;	// bottommost y-coordinate
	float max_y;	// topmost y-coordinate
	float vel_x;	// horizontal velocity (Ball Only)
	float vel_y;	// vertical velocity (Ball Only)
};
typedef struct gameObject Obj;	// Create 'Obj' as synonym to 'struct gameObject', makes code shorter

// Declare Ball and Paddle as global gameObjects. Allows calling game functions without gameObjects as parameters.

extern Obj Ball;	
extern Obj Paddle1;	
extern Obj Paddle2;	

// Declare Global game variables - Define somewhere else

extern int score1;
extern int score2;
extern int lastScored;
extern int gameMode;
extern int gameOverFlag;

// Pre-Declarations of Custom functions - make available to all .c-files

void menu(void);
int getbtns(void);

void gameStart(void);
void gameLoop(void);
void scoreScreen(int Player);
void gameOver(void);

void WriteClearGameObject(Obj gameObject, int WriteType);
void UpdateBall(void);
void BounceCalc(int T);
void UpdatePaddles(void);

void displayUpdate(void);
void clearDisplayBuffer(void);
void displayString(int line, char *s);

void setPixel(uint8_t x, uint8_t y);
void clearPixel(uint8_t x, uint8_t y);


// Re-used Template Functions - make available to all .c-files

void delay(int cyc);
uint8_t spi_send_recv(uint8_t data);
void display_init();
void board_init(void);
extern const uint8_t const font[128*8];

// uint8_t is an unsigned 8 bit/1 byte integer format - used for the display where each bit controls 1 pixel

/* Buffer made of a 2d byte array split into columns and pages. 
 Used for storing the next state of every pixel of the display.
 Call displayUpdate to write displayBuffer contents to display.*/
extern uint8_t displayBuffer[128][4];   

