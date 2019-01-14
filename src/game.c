#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

#include <include/game.h>

stGame *game_init()
{	
	// init random 
	srand(time(NULL));

	stGame *game = NULL;

	game = malloc(sizeof(stGame));
	game->screenSize.x = 640;
	game->screenSize.y = 480; 

	game->pWindow = NULL;
	game->pRenderer = NULL;

	game->pFont = NULL;

	game->pTexPlayer = NULL;
	game->pTexFood = NULL;

	game->score = 0;
	game->playerPositionRect.x = game->screenSize.x / 2;
	game->playerPositionRect.y = game->screenSize.y / 2;
	game->playerPositionRect.w = 50;
	game->playerPositionRect.h = 60;

	game->foodPositionRect.w = 60;
	game->foodPositionRect.h = 60;
	game_spawnFood(game);

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "unable to init SDL : %s\n", SDL_GetError());
		game_destroy(game);
		return NULL;
	}

	// init TTF font
	if (TTF_Init() != 0) {
		fprintf(stderr, "unable to init TTF : %s\n", TTF_GetError());
		game_destroy(game);
		return NULL;		
	}

	// Load font
	game->pFont = TTF_OpenFont("fonts/arial.ttf", 25);

	if (!game->pFont)
	{
		fprintf(stderr, "unable to open TTF : %s\n", TTF_GetError());
		game_destroy(game);
		return NULL;	
	}

	// build window
	game->pWindow = SDL_CreateWindow("Mario", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, game->screenSize.x, game->screenSize.y, SDL_WINDOW_SHOWN);

	if (game->pWindow) {
		// create renderer
		game->pRenderer = SDL_CreateRenderer(game->pWindow, -1, SDL_RENDERER_ACCELERATED);
		
		if (!game->pRenderer) {
			fprintf(stderr, "unable to create renderer : %s\n", SDL_GetError());
			game_destroy(game);
			return NULL;
		}
	} else {
		fprintf(stderr, "unable to create window : %s\n", SDL_GetError());
		game_destroy(game);
		return NULL;
	}

	// load player texture
	SDL_Surface *surfaceMario = IMG_Load("images/Super_Mario.png");

	if (!surfaceMario) {
		fprintf(stderr, "unable to load image : %s\n", IMG_GetError());
		game_destroy(game);
		return NULL;	
	} else {
		game->pTexPlayer = SDL_CreateTextureFromSurface(game->pRenderer, surfaceMario);

		if (!game->pTexPlayer) {
			fprintf(stderr, "unable to handle texture : %s\n", SDL_GetError());
			game_destroy(game);
			return NULL;
		}

		SDL_FreeSurface(surfaceMario);
	}

	SDL_Surface *surfaceFood = IMG_Load("images/pomme.png");

	if (!surfaceFood) {
		fprintf(stderr, "unable to load image : %s\n", IMG_GetError());
		game_destroy(game);
		return NULL;	
	} else {
		game->pTexFood = SDL_CreateTextureFromSurface(game->pRenderer, surfaceFood);

		if (!game->pTexFood) {
			fprintf(stderr, "unable to handle texture : %s\n", SDL_GetError());
			game_destroy(game);
			return NULL;
		}

		SDL_FreeSurface(surfaceFood);
	}
	return game;
}

void game_destroy(stGame *game)
{	
	if (game) {

		if (game->pTexFood)
		{
			SDL_DestroyTexture(game->pTexFood);
		}

		if (game->pTexPlayer)
		{
			SDL_DestroyTexture(game->pTexPlayer);
		}

		if (game->pRenderer) {
			SDL_DestroyRenderer(game->pRenderer);
		}

		if (game->pWindow) {
			SDL_DestroyWindow(game->pWindow);
		}

		if (game->pFont)
		{
			TTF_CloseFont(game->pFont);
		}

		TTF_Quit();

		SDL_Quit();
		free(game);
	}
}


void game_draw(stGame * game)
{
	// clean screen
	SDL_SetRenderDrawColor(game->pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(game->pRenderer);
	
	//SDL_Delay(2000);

	// display player
	SDL_RenderCopy(game->pRenderer, game->pTexPlayer, NULL, &game->playerPositionRect);

	//display food
	SDL_RenderCopy(game->pRenderer, game->pTexFood, NULL, &game->foodPositionRect);
	
	SDL_Color textColor = {255, 255, 255, 255};
	char text[40];

	snprintf(text, 40, "Score : %d", game->score);
	SDL_Surface *textSurface = TTF_RenderText_Solid(game->pFont, text, textColor);

	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(game->pRenderer, textSurface);
	SDL_Rect destText = {0, 0, 100, 20};

	SDL_RenderCopy(game->pRenderer, textTexture, NULL, &destText);

	SDL_DestroyTexture(textTexture);
	SDL_FreeSurface(textSurface);


	// show renderer
	SDL_RenderPresent(game->pRenderer);

	//SDL_Delay(1000);
}

int game_event(stGame *game)
{
	int result = 0;

	SDL_Event e;


	if (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT) {
			result = -1;
		}

		else if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:
					result = -1;
					break;
				case SDLK_UP:
				case SDLK_DOWN:
				case SDLK_LEFT:
				case SDLK_RIGHT:
					game_movePlayer(game, e.key.keysym.sym);
					break;
				default :
				fprintf(stderr, "unknown key : %d\n", e.key.keysym.sym);
				break;
			}
		}
	}

	game_checkFood(game);
	return result;
}

void game_movePlayer(stGame *game, SDL_Keycode direction)
{
	if (direction == SDLK_UP){
		if (game->playerPositionRect.y > 0) {
			game->playerPositionRect.y -= 10;
		}
	} else if (direction == SDLK_DOWN) {
		if (game->playerPositionRect.y < (game->screenSize.y - game->playerPositionRect.h)) {
			game->playerPositionRect.y += 10;
		}
	} else if (direction == SDLK_LEFT) {
		if (game->playerPositionRect.x > 0) {
			game->playerPositionRect.x -= 10;
		}
	} else if (direction == SDLK_RIGHT) {
		if (game->playerPositionRect.x < (game->screenSize.x - game->playerPositionRect.w)) {
			game->playerPositionRect.x += 10;
		}
	} else {
		fprintf(stderr, "unknown direction\n");
	}
}

void game_checkFood(stGame *game)
{
	SDL_Rect result;

	if (SDL_IntersectRect(&game->playerPositionRect, &game->foodPositionRect, &result))
	{
		game->score += 1;
		game_spawnFood(game);
	}
}

void game_spawnFood(stGame *game)
{
	game->foodPositionRect.x = rand() % (game->screenSize.x - game->foodPositionRect.w);
	game->foodPositionRect.y = rand() % (game->screenSize.y - game->foodPositionRect.h);
}

