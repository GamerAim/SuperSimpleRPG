void loadAndDisplay(char *fileName)
{
	SDL_Rect rect;
	SDL_Surface *avatar;

	/*Set rect to cover screen*/
	rect.x = 0;
	rect.y = 0;
	rect.w = gWidth;
	rect.h = gHeight;

	/*Obliterate gSurface*/
	SDL_FillRect(gSurface, &rect, WHITE);

	/*Now for the Avatar*/
	avatar = IMG_Load(fileName);
	if (avatar == NULL)
		die("Failed to load: %d\n", fileName);
	SDL_BlitSurface(avatar, NULL, gSurface, NULL);
	SDL_UpdateWindowSurface(gWindow);

	/*Clean up*/
	SDL_FreeSurface(avatar);
}

void TitleScreen(void)
{
	/*Display Sebbu Avatar*/
	loadAndDisplay("assets/Sebbu_w_name.png");
	SDL_Delay(3000);

	/*Display Brandc Avatar*/
	loadAndDisplay("assets/Brandc.png");
	SDL_Delay(3000);

	/*Display HJ Avatar*/
	loadAndDisplay("assets/HJ.png");
	SDL_Delay(3000);
}








