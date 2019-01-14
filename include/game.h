#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct 
{	
	// SDL variables
	SDL_Point screenSize;
	SDL_Window *pWindow;
	SDL_Renderer *pRenderer;

	// TTF font
	TTF_Font *pFont;


	// Texture
	SDL_Texture *pTexPlayer;
	SDL_Texture *pTexFood;

	// Game
	int score;
	SDL_Rect playerPositionRect;
	SDL_Rect foodPositionRect;


} stGame;

stGame *game_init();
void game_destroy(stGame *game);
void game_draw(stGame *game);
int game_event(stGame *game);
void game_movePlayer(stGame *game, SDL_Keycode direction);
void game_checkFood(stGame *game);
void game_spawnFood(stGame *game);