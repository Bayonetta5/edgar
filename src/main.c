/* This tutorial demonstrates tile based collision
** detection. Pressing the arrow keys
** will sprite around the screen. Pressing escape
** or closing the window will exit the program
*/

#include "main.h"

extern void init(char *);
extern void cleanup(void);
extern void getInput(void);
extern void draw(void);
extern void doPlayer(void);
extern void delay(unsigned int);
extern void initPlayer(void);
extern void initGame(void);
extern void doEntities(void);
extern void doCollisions(void);
extern void loadResources(void);
extern void doGame(void);
extern void addBat(int, int);

int main(int argc, char *argv[])
{
	unsigned int frameLimit = SDL_GetTicks() + 16;
	int go;

	/* Start up SDL */

	init("The Legend of Edgar");

	/* Call the cleanup function when the program exits */

	atexit(cleanup);

	go = 1;

	/* Load the resources */
	
	loadResources();
	
	/* Initialise the player */

	initPlayer();
	
	/* Initialise the game variables */
	
	initGame();

	/* Loop indefinitely for messages */
	
	printf("Main loop\n");

	while (go == 1)
	{
		getInput();
		
		/* Do the game */
		
		doGame();

		/* Do the player, provided they still have enough lives left */
		
		doPlayer();
		
		/* Do the Entities */
		
		doEntities();
		
		/* Do collisions */
		
		doCollisions();

		/* Draw the map */

		draw();

		/* Sleep briefly to stop sucking up all the CPU time */

		delay(frameLimit);

		frameLimit = SDL_GetTicks() + 16;
	}

	/* Exit the program */

	exit(0);
}