#include <pic32mx.h>
#include <stdint.h>
#include <stdio.h>
#include "Defines.h" 


/*
	Change Ball velocity on Bounce - only inverts velocity
	Enter different parameter T depending on what type of bounce
	Uses Ball.vel_x, Ball.vel_y

	Written by Dana
*/
void BounceCalc(int T)		
{
	if(T == 1){
	Ball.vel_x *= -1;
	}
	if(T == 2){
	Ball.vel_y *= -1;
	}
}

/*
	Check if Ball is about to impact a Border or Paddle, or is past a Paddle and should Score. gameMode is changing
	Calls BounceCalc, gameOver, gameStart, scoreScreen
	Uses variables Ball, Paddle1/2 - min/max/vel_x/y, BORDER:s, gameMode, score1/2, lastScored

	Written by Dana & Aron
*/
void CollisionCheck(void)	
{	
	
	// Screen Border
	if(Ball.min_x + Ball.vel_x <= BORDER_LEFT) 
	BounceCalc(1);
	if(Ball.max_x + Ball.vel_x >= BORDER_RIGHT)
	BounceCalc(1);
	if(Ball.min_y + Ball.vel_y <= BORDER_BOTTOM)			
	BounceCalc(2);
	if(Ball.max_y + Ball.vel_y >= BORDER_TOP)			
	BounceCalc(2);

	// Paddle
	// Hit left Paddle from side
	// Checks if the next x and y position of the Ball will be in the current x and y range of the left Paddle
	if((Ball.min_x + Ball.vel_x <= Paddle1.max_x) && (Ball.min_y >= Paddle1.min_y && Ball.max_y <= Paddle1.max_y)){
		BounceCalc(1);
		if(gameMode == 1){score1++;}	// If 1Player, add point for parrying the ball.
	}
	// Hit right Paddle from side
	// Checks if the next x and y position of the Ball will be in the current x and y range of the right Paddle
	if((Ball.max_x + Ball.vel_x >= Paddle2.min_x) && (Ball.min_y >= Paddle2.min_y && Ball.max_y <=Paddle2.max_y)){
		BounceCalc(1);
	}

	
	// Passed Left Paddle - Score for Player 2
	// Checks if rightmost end of ball is past the leftmost end of the left Paddle
	if(Ball.max_x <= Paddle1.min_x){
		if(gameMode == 1){gameOver();}	// Game over in 1Player mode
		score2++;						// Increase Player2 Score
		lastScored=2;					// Set last scorer - affects starting ball Direction in next round 
		if(gameMode == 2){				// Check if 2Player mode
		gameStart();					// Reset Ball and Paddles
		scoreScreen(2);}				// Show Score Screen for Player2
	}
	// Passed Right Paddle - Score for Player 1
	// Checks if lefttmost end of ball is past the rightmost end of the right Paddle
	if(Ball.min_x >= Paddle2.max_x){
		if(gameMode == 2){				// Check if 2Player mode
		score1++;						// Increase Player1 Score
		lastScored=1;					// Set last scorer - affects starting ball Direction in next round
		gameStart();					// Reset Ball and Paddles
		scoreScreen(1);}				// Show Score Screen for Player1
	}
}

/* 
	Updates Ball Position after checking Ball for bouncing or scoring conditions,
	forces Ball back within borders if it's gone beyond them
	Calls CollisionCheck
	Uses Ball, min/max x/y, BALL_WIDTH, BORDER_LEFT/RIGHT/BOTTOM/TOP

	Written by Aron
*/
void UpdateBall(void)		
{
	// Collision Check may update ball velocity - do before setting next ball position
	CollisionCheck();
	// Update Ball position to its next position
	Ball.max_x += Ball.vel_x;
	Ball.max_y += Ball.vel_y;
	Ball.min_x += Ball.vel_x;
	Ball.min_y += Ball.vel_y;

	// Force ball back within borders if it's gone outside the borders - possibly unnecessary, but ball went out of screen sometimes in early development
	// If ball position P is beyond border B by distance D, set position P = B - D	
	// If it has gone beyond a border, it will be reflected relative to that border
	if(Ball.min_x < BORDER_LEFT)
	{Ball.min_x = BORDER_LEFT - (Ball.min_x - BORDER_LEFT); 
	Ball.max_x = Ball.min_x + BALL_WIDTH; }

	if(Ball.max_x > BORDER_RIGHT)	
	{Ball.max_x = BORDER_RIGHT - (Ball.max_x - BORDER_RIGHT); 
	Ball.min_x = Ball.max_x - BALL_WIDTH; }

	if(Ball.min_y < BORDER_BOTTOM)
	{Ball.min_y = BORDER_BOTTOM - (Ball.min_y - BORDER_BOTTOM);
	Ball.max_y = Ball.min_y + BALL_WIDTH; }

	if(Ball.max_y > BORDER_TOP)
	{Ball.max_y = BORDER_TOP - (Ball.max_y - BORDER_TOP); 
	Ball.min_y = Ball.max_y - BALL_WIDTH; }
}

/*
	Update Paddles by polling BTN4:1 - Call this function repeatedly 
	Calls getbtns
	Uses Paddle1, Paddle2, min_y, max_y, BORDER_BOTTOM/TOP

	Written by Aron & Dana
*/
void UpdatePaddles(void)
{
	// Move Paddles with BTN4:1 as inputs
	int btns = getbtns();
	if (btns)   // any button being pressed will save a non-0 int in btn, only 0 acts as a false
  		{
     	if ((btns & 0x1) && (Paddle2.max_y < BORDER_TOP)) { // BTN1 is pressed AND Paddle not out of Border
				Paddle2.min_y++;
				Paddle2.max_y++;
    		} 
		if ((btns & 0x2) && (Paddle2.min_y > BORDER_BOTTOM)) { // BTN2 is pressed AND Paddle not out of Border
				Paddle2.min_y--;
				Paddle2.max_y--;
			} 
		if ((btns & 0x4) && (Paddle1.max_y < BORDER_TOP)) { // BTN3 is pressed AND Paddle not out of Border
				Paddle1.min_y++;
				Paddle1.max_y++;
			}
		if ((btns & 0x8) && (Paddle1.min_y > BORDER_BOTTOM)) { // BTN4 is pressed AND Paddle not out of Border
				Paddle1.min_y--;
				Paddle1.max_y--;
			}
  		}
}

/*
	Sets size and starting values for Ball and Paddles, clears displayBuffer
	Calls clearDisplayBuffer
	Uses min/max/vel x/y, lastScored, gameMode, BALL/PADDLE_WIDTH/HEIGHT/DISTANCE, VELOCITY_X/Y, BORDER_LEFT/RIGHT

	Written by Dana & Aron
*/
void gameStart(void)
{
	clearDisplayBuffer();
	//Set variables for Ball - reflect x-direction depending on who last scored
	Ball.min_x = 63;
	Ball.max_x = Ball.min_x + BALL_WIDTH;
	Ball.min_y = 15;
	Ball.max_y = Ball.min_y + BALL_WIDTH;
	if(lastScored == 2){	// Assuming x-velocity positive, send Ball toward Player 2 if Player 2 last scored
	Ball.vel_x = VELOCITY_X;
	Ball.vel_y = VELOCITY_Y;}
	if(lastScored == 1){	// Assuming x-velocity positive, send Ball toward Player 1 if Player 1 last scored
	Ball.vel_x = -VELOCITY_X;
	Ball.vel_y = VELOCITY_Y;}

	//Set variables for Paddle1
	Paddle1.min_x = BORDER_LEFT + PADDLE_DISTANCE;
	Paddle1.max_x = Paddle1.min_x + PADDLE_WIDTH;
	Paddle1.min_y = 12;
	Paddle1.max_y = Paddle1.min_y + PADDLE_HEIGHT;

	//Set variables for Paddle2
	if(gameMode == 2){		// Only if 2Player
	Paddle2.max_x = BORDER_RIGHT - PADDLE_DISTANCE;
	Paddle2.min_x = Paddle2.max_x - PADDLE_WIDTH;
	Paddle2.min_y = 12;
	Paddle2.max_y = Paddle2.min_y + PADDLE_HEIGHT;
	}
	
	if(gameMode != 2){		// If 1Player, make Paddle 2 a dot far outside of screen
	Paddle2.min_x = 200;
	Paddle2.max_x = 200;
	Paddle2.min_y = 200;
	Paddle2.max_y = 200;
	}
}

/*
gameMode uppdateras endast via knapptryck i Menu, score ökar i CollisonCheck och återställs i slutet av gameLoop
*/

/*
	Main Game Loop - Shows score, draws/erases Game Objects, updates Ball with speed, updates Paddles with button inputs, checks for  Game Over conditions
	Calls clearDisplayBuffer, displayString, WriteClearGameObject, displayUpdate, delay, UpdateBall, UpdatePaddles, gameOver
	Uses variables score1/2, Ball, Paddle1/2, GAME_SPEED, MAX_SCORE, gameOverFlag

	Written by Dana & Aron
*/
void gameLoop(void)
{
	clearDisplayBuffer();
	char scoreString [16] = "0              0";

	while(1){
	// Update scoreString with Player Scores
	scoreString[0] = 48 + score1; // 48 is ASCII value for 0, add score on top
	scoreString[15] = 48 + score2;
	// Write scoreString to top page in displayBuffer
	displayString(0, scoreString);
	
	// Write Game Objects to displayBuffer
	WriteClearGameObject(Ball, 1);
	WriteClearGameObject(Paddle1, 1);
	WriteClearGameObject(Paddle2, 1);

	//Update Display with the contents of displayBuffer
	displayUpdate();
	// Wait for a duration set by game_speed - the game is still during this time
	delay(GAME_SPEED);
	
	// Erase Game Objects from displayBuffer
	clearDisplayBuffer();

	// Update Ball Position
	UpdateBall();
	// Update Paddles position with current input from buttons - button polling
	UpdatePaddles();

	if ((score1 > MAX_SCORE) || (score2 > MAX_SCORE)){gameOver();}	// If either Player Score has exceeded maximum, call gameOver()
	if (gameOverFlag){												// If GameOverFlag set (gameOver() was called)
	// Reset score and gameOverFlag. Allows starting another game from Menu.
	score1 = 0;			
	score2 = 0;		
	gameOverFlag = 0;
	// Break game-loop
	return;
	}
}
	return;	// After breaking game loop, return to Menu
}

/*
	---GAME-DISPLAY interaction functions---
*/

/*
	Writes 1:s or 0:s to displayBuffer in the position of a specified gameObject,
	depending on the value entered for WriteType
	Calls setPixel or clearPixel
	Uses the min/max x/y variables of the gameObject as range for where to write

	Written by Aron
*/
void WriteClearGameObject(Obj gameObject, int WriteType)	
{					
uint8_t x;
uint8_t y;
	// Cast Ball float coordinates to unsigned byte so that it can be written to Display - will floor values
	uint8_t pmin_x = (uint8_t)gameObject.min_x;		// leftmost x-coordinate
	uint8_t pmax_x = (uint8_t)gameObject.max_x;		// rightmost x-coordinate		
	uint8_t pmin_y = (uint8_t)gameObject.min_y;		// bottommost y-coordinate
	uint8_t pmax_y = (uint8_t)gameObject.max_y;		// topmost y-coordinate
	for (y = pmin_y; y <= pmax_y; y++)				// Loop through specified y-coordinate range
		{
		for (x = pmin_x; x <= pmax_x; x++)		// Loop through specified x-coordinate range
			{
				if(WriteType == 1){setPixel(x, y);}		// Set the pixel in the buffer
				if(WriteType == 0){clearPixel(x, y);}	// Clear the pixel in the buffer
			}	
		}

}

/**
MESSAGE_TIME Anger hur länge Game Over/Player Win visas. Helst tillräckligt länge för att läsa meddelandet.
gameOverFlag uppdateras här och i Menu, gameMode uppdateras endast via knapptryck i Menu, score ökar i CollisonCheck och återställs i gameLoop
*/

/* 
	Makes and shows a Game Over Screen -
	Calls displayString, displayUpdate, delay
	Uses variables gameMode, score1/score2, MESSAGE_TIME, changes gameOverFlag

	Written by Dana
*/
void gameOver(void)
{
	displayString(1, "   Game  Over");	// Add a string indicating the current Game is over
	if(gameMode==2){					
		if(score1 > score2){
			displayString(2, "  Player1 Win!");	// Add a second string specifying who won
		}
		if(score2 > score1){
			displayString(2, "  Player2 Win!");	// Add a second string specifying who won
		}
	}
	displayUpdate();		// Update the Display
	delay(MESSAGE_TIME);	// Time during which the Game Over Screen is showed 
	gameOverFlag = 1;		// Set a Game Over Flag for breaking out of the game back to Menu
}

/**
Skärmen clearas genom att först anropa clearDisplayBuffer (för att rensa buffern), och sedan displayUpdate (för att skriva/visa Buffern på skärmen).
Det finns dock inget intressant i att se en svart skärm, så varför inte också stoppa in vad vi vill att den faktiskt ska visa, innan vi anropar displayUpdate?
Funktionen nedan anropas när någon får ett poäng, där Player bara är parametern för vilken spelare som nyss fick poäng
*/

/*
	Makes and shows a Score Screen - Uses a int parameter specifying which Player scored
	Calls clearDisplayBuffer, displayString, displayUpdate, delay
	Uses variable MESSAGE_TIME

	Written by Dana
*/
void scoreScreen(int Player)
{
	clearDisplayBuffer();	// Clear Buffer
	if(Player == 1){displayString(1, " Player1 Score! ");}	// Write who scored
	if(Player == 2){displayString(1, " Player2 Score! ");}
	displayUpdate();		// Update Display with message
	delay(MESSAGE_TIME);	// Wait a bit
	clearDisplayBuffer();	// Clear Buffer
}
