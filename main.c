#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*Official Third party libs*/
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

/*Global variables*/
SDL_Window  *gScreen;
SDL_Surface *gSurface;

/*Prototypes*/
void init(void);
void die(char *reason, ...);

/*Internal libs*/
#include "Title.c"


int main(int argc, char **argv)
{
	/*Prime libs*/
	init();

	/*This would be a handy place to put asset loading*/
	TitleScreen();

	/*Clean up!*/
	SDL_Quit();
	return 0;
}



/*===============================*/
/*Handy functions below this line*/
/*===============================*/

/*Universal death macro*/
void die(char *reason, ...)
{
	va_list args;

	va_start(args, reason);
	vfprintf(stderr, reason, args);
	va_end(args);

	exit(1);
}

/*initializations go here*/
void init(void)
{
	/*Init for SDL.h*/
	if (SDL_Init(SDL_INIT_EVERYTHING))
		die("Could not initialize SDL: %s\n", SDL_GetError());
	atexit(SDL_Quit);


	/*Init for SDL_image.h*/
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
		die("Could not initialize SDL_image: %s\n", IMG_GetError());
	atexit(IMG_Quit);
}









