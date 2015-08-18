#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*Official Third party libs*/
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

/*Global variables*/
SDL_Window  *gWindow;
SDL_Surface *gSurface;

int gWidth;
int gHeight;

/*Global defs*/
#define WHITE 0xFFFFFFFF
#define BLACK 0x00000000

/*Prototypes*/
void init_libs(void);
void init_globals(void);
void die(char *reason, ...);

/*Internal libs*/
#include "Title.c"
#include "LoadMap.c"


int main(int argc, char **argv)
{
	/*Prime libs*/
	init_libs();

	/*Prime globals*/
	init_globals();

	/*Display logos*/
	TitleScreen();

	/*Clean up!*/
	IMG_Quit();
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
void init_libs(void)
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

void init_globals(void)
{
	/*Current desired window dimensions*/
	gWidth  = 460;
	gHeight = 460;

	/*Window creation*/
	gWindow = SDL_CreateWindow(
		"Title Screen",		 /*windows title*/
		SDL_WINDOWPOS_UNDEFINED, /*X*/
		SDL_WINDOWPOS_UNDEFINED, /*Y*/
		gWidth,			 /*Width*/
		gHeight,		 /*Height*/
		SDL_WINDOW_SHOWN |
		  SDL_WINDOW_RESIZABLE);
	if (gWindow == NULL)
		die("Failed to create title screen: %s\n", SDL_GetError());

	/*Get the window's double buffered surface*/
	gSurface = SDL_GetWindowSurface(gWindow);
}








