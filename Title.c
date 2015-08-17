void loadAndDisplay(char *fileName, SDL_Window *window, SDL_Surface *wSurface)
{
	SDL_Surface *avatar;

	/*Now for the Avatar*/
	avatar = IMG_Load(fileName);
	SDL_BlitSurface(avatar, NULL, wSurface, NULL);
	SDL_UpdateWindowSurface(window);

	/*Clean up*/
	SDL_FreeSurface(avatar);
}

void TitleScreen(void)
{
	SDL_Window *title;
	SDL_Surface *surface;

	/*Window creation*/
	title = SDL_CreateWindow(
		"Title Screen",		 /*windows title*/
		SDL_WINDOWPOS_UNDEFINED, /*X*/
		SDL_WINDOWPOS_UNDEFINED, /*Y*/
		460,			 /*Width*/
		460,			 /*Height*/
		SDL_WINDOW_SHOWN);
	if (title == NULL)
		die("Failed to create title screen: %s\n", SDL_GetError());

	/*It's double buffered*/
	surface = SDL_GetWindowSurface(title);

	/*Display Sebbu Avatar*/
	loadAndDisplay("Sebbu_w_name.png", title, surface);
	SDL_Delay(3000);

	/*Display Brandc Avatar*/
	loadAndDisplay("Brandc.png", title, surface);
	SDL_Delay(3000);

	/*Clean up!*/
	SDL_FreeSurface(surface);
	SDL_DestroyWindow(title);
}