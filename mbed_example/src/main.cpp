#include <mbed.h>
#include <MMA7455.h>
#include <LM75B.h>
#include <display.h>

typedef enum {JLT = 0, JRT, JUP, JDN, JCR} btnId_t;					// Initialise Joystick   
static DigitalIn jsBtns[] = {P5_0, P5_4, P5_2, P5_1, P5_3}; // LFT, RGHT, UP, DWN, CTR
bool jsPrsdAndRlsd(btnId_t b);

Display *screen = Display::theDisplay();										//This is how you call a static method of class Display
																														//Returns a pointer to an object that manages the display screen 

																														
void timerHandler(); 																				//Timer interrupt and handler
int tickCt = 0;																							//prototype of handler function
																														
int x = 240, y = 262, dx = 0, dy = 0, bx = 0, by = 0;				//Drawing coordinates

int randomrange(int from, int to){
    int range = to-from;
    return from  + rand()%(range+1);
	}

int currentBalls = 0;
int score = 0;	
int gameStart = 0;
int topHit = 1;
int scoreAdd = 1;
int magicScoreAdd;
int continueGame = 0;
int obLength;
int magicTime = 0;	
	
int main() {
  
  screen -> fillScreen(WHITE);															// Initialise display
  screen -> setTextColor(BLACK, WHITE);
  
  Ticker ticktock;																					//Initialise ticker and install interrupt handler
  ticktock.attach( & timerHandler, 1);

  screen -> drawRect(0, 1, 480, 272, BLACK);
  screen -> drawRect(0, 1, 480, 30, BLACK);

  while (true) {

    int randNumberX; 																				//Balls X axis
    int randNumberY; 																				//Balls Y axis
    int ballStart;
    int ballDirectionLeft; 																	// Bounces ball left
    int ballDirectionRight;																	// Bounces ball right
    int ballDirectionUp; 																		// Bounces ball Up
    int ballDirectionDown; 																	// Bounces ball down 

    /***DISPLAYS SCORE AND LIVES***/

    screen -> setCursor(6, 8);															// balls left score location
    screen -> printf("Balls Left = %d", currentBalls);			// display balls left

    screen -> setCursor(300, 8);														// score location
    screen -> printf("Score = %d    ", score);							// display score

    if (jsPrsdAndRlsd(JCR)) { 															// STARTS/RESTARTS GAME 
      if (currentBalls >= 1 && continueGame == 0) {					// checks if balls are left to restart
        score = 0;
        gameStart = 0;
        screen -> setCursor(100, 136);
        screen -> printf("Game Restarted");
        wait(1);
        screen -> fillRect(1, 31, 478, 240, WHITE);					// Playing area
        screen -> fillRect(x, y, 40, 4, WHITE);							// Score area
        x = 240;
        currentBalls = 5;
        gameStart = 1;
        ballStart = 1;
      }

      if (currentBalls >= 1 && continueGame == 1) {					// checks if balls are left to restart
        continueGame = 0;
        gameStart = 1;
        screen -> fillRect(1, 31, 478, 240, WHITE);					// Playing area
        screen -> fillRect(x, y, 40, 4, WHITE);							// Score area
        x = 240;
        ballStart = 1;
      }

      if (currentBalls <= 0) {															// checks if balls are left to restart
        score = 0;
        currentBalls = 5;
        gameStart = 1;
        ballStart = 1;
        scoreAdd = 1;																				// base score increment
        topHit = 0;
        screen -> fillRect(1, 31, 478, 240, WHITE);					// Playing area
        screen -> fillRect(x, y, 40, 4, WHITE);							// Score area
        x = 240;
      }
    }

    if (gameStart == 1) { 																		//RUNS GAME  

      if (ballStart == 1) {
        randNumberX = rand() % (380 + 1 - 0) + 50;
        randNumberY = rand() % (50 + 1 - 0) + 40;
        bx = 1;
        by = 1;
        ballStart--;
      }

      /***BALL COLLISION DETECTION***/
   
      if (randNumberX > 470) {			 													//Checks for bounce on right wall
        ballDirectionLeft = 1;
        ballDirectionRight = 0;
      }

      if (randNumberX < 10) { 																//Checks for bounce on left wall
        ballDirectionRight = 1;
        ballDirectionLeft = 0;
      }

      if (randNumberX > x - 1 && randNumberX < x + 41) { 			//Checks for bounce on bat 
        if (randNumberY > y - 5) {
          ballDirectionUp = 1;
          ballDirectionDown = 0;
        }
      }

      if (randNumberY > 277) { 																//Checks if ball off screen
        randNumberX = rand() % (380 + 1 - 0) + 50;
        randNumberY = rand() % (50 + 1 - 0) + 40;
        bx = 2;
        scoreAdd = 1;
        topHit = 0;
        currentBalls--;
        continueGame = 1;
        gameStart = 0;
        screen -> fillRect(1, 31, 478, 240, WHITE);
        screen -> fillRect(x, y, 40, 4, WHITE);
        x = 240;
      }


      /***BALL BOUNCE DIRECTION***/

      if (ballDirectionLeft == 1) {
        bx = -1;
        by = 1;
      }

      if (ballDirectionRight == 1) {
        bx = 1;
        by = 1;
      }
      if (ballDirectionUp == 1) {
        by = -1;
      }
      if (ballDirectionDown == 1) {
        by = 1;
      }
			
      /***EXTRAS***/

      obLength = rand() % (200 + 1 - 40) + 40;

      screen -> fillRect(100, 100, obLength, 2, GREEN);

      if (randNumberY < 100 && randNumberY > 98) { 					//Hits of the bottom of the obstacle
        if (randNumberX > 99 && randNumberX < 301) {
          ballDirectionUp = 0;
          ballDirectionDown = 1;
        }
      }

      if (randNumberY < 97 && randNumberY > 95) { 					//Hits of the top of the obstacle
        if (randNumberX > 99 && randNumberX < 301) {
          ballDirectionUp = 1;
          ballDirectionDown = 0;
        }
      }


      /***GAME SCORE AND GAME END***/

      if (currentBalls == 0) {
        screen -> fillRect(1, 31, 478, 240, WHITE);
        screen -> setCursor(100, 136);
        screen -> printf("GAME OVER! PRESS JOYSTICK CENTER TO RESTART");
        continueGame = 0;
        gameStart = 0;
        score = 0;
      }

      screen -> setCursor(100, 60);
      screen -> printf("Increment: %d", scoreAdd);

      if (randNumberY == 40 && ballDirectionUp == 1) { 				//increments score when ball hits 40 and is moving up 
        ballDirectionUp = 0;
        ballDirectionDown = 1;
        if (magicTime == 1) { 																//if magic time is activated
          magicScoreAdd = (scoreAdd * 2); 										//double score
          score = score + magicScoreAdd; 											//add double score
        } else {
          score = score + scoreAdd; 													//add normal score
        }
        if (topHit < 4  ) { 																	//if ball has hit top less than five times score increment is 1
          topHit++;
        } else { 																							//if ball has hit top 5 times in a row increment increases by 1
          scoreAdd++;
          topHit = 0;
        }
      }

      /***DRAW OBJECTS ON SCREEN AND BAT CONTROLS***/

      if (magicTime == 0) {
        screen -> fillCircle(randNumberX, randNumberY, 5, WHITE);
        randNumberX += bx;
        randNumberY += by;
        screen -> fillCircle(randNumberX, randNumberY, 5, BLUE);
      } else {
        screen -> fillCircle(randNumberX, randNumberY, 5, WHITE);
        randNumberX += bx;
        randNumberY += by;
        screen -> fillCircle(randNumberX, randNumberY, 5, RED);
      }

      screen -> fillRect(x, y, 40, 4, WHITE);
      x += dx;
      y += dy;
      screen -> fillRect(x, y, 40, 4, BLACK);

      if (jsPrsdAndRlsd(JLT)) {
        dx = 2;
        dy = 0;
      } else if (jsPrsdAndRlsd(JRT)) {
        dx = -2;
        dy = 0;
      }

      if (x > 440) {
        dx = 0;
        x = 439;
      }

      if (x < 0) {
        dx = 0;
        x = 1;
      }
      wait(0.005); 																						//5 milliseconds
    }
  }
}

void timerHandler() {																					//Definition of timer interrupt handler

}

/* Definition of Joystick press capture function
 * b is one of JLEFT, JRIGHT, JUP, JDOWN - from enum, 0, 1, 2, 3
 * Returns true if this Joystick pressed then released, false otherwise.
 *
 * If the value of the button's pin is 0 then the button is being pressed,
 * just remember this in savedState.
 * If the value of the button's pin is 1 then the button is released, so
 * if the savedState of the button is 0, then the result is true, otherwise
 * the result is false. */
bool jsPrsdAndRlsd(btnId_t b) {
  bool result = false;
  uint32_t state;
  static uint32_t savedState[4] = {														//initially all 1s: nothing pressed
    1,
    1,
    1,
    1
  };
  
  state = jsBtns[b].read();
  if ((savedState[b] == 0) && (state == 1)) {
    result = true;
  }
  savedState[b] = state;
  return result;
}